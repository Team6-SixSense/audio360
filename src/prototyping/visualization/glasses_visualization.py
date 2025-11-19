"""
Glasses visualization with moving average for classification and direction smoothing.
Integrates with existing classification and DOA analysis code.
"""

import numpy as np
from collections import deque
from typing import Optional, Dict
import threading
import tkinter as tk
from .sound_visualization_gui import SoundVisualizationGUI


# Global state for visualization
_glasses_gui = None
_gui_thread = None
_gui_lock = threading.Lock()

# Moving average state
_classification_history = deque(maxlen=5)  # Track last 5 classifications
_current_displayed_classification = None
_direction_history = deque(maxlen=10)  # Track last 10 directions for smoothing
_smoothed_direction = None


def _extract_direction_from_doa(doa_results: Dict) -> Optional[float]:
    """
    Extract direction from DOA results.
    Prefers FRIDA, then SRP-PHAT, then MUSIC.
    
    Args:
        doa_results: Dictionary of DOA algorithm results
        
    Returns:
        Direction in radians, or None if no valid result
    """
    # Priority order: FRIDA > SRP-PHAT > MUSIC
    priority_order = ['FRIDA', 'SRP-PHAT', 'MUSIC']
    
    for algo_name in priority_order:
        if algo_name in doa_results:
            doa = doa_results[algo_name]
            if doa.azimuth_recon is not None and len(doa.azimuth_recon) > 0:
                # Use the first source's azimuth
                azimuth_rad = doa.azimuth_recon[0]
                return azimuth_rad
    
    return None


def _get_smoothed_classification(classification: str, threshold: float = 0.8) -> Optional[str]:
    """
    Use moving average approach to determine displayed classification.
    Only display if threshold fraction of recent predictions match.
    
    Args:
        classification: Current classification prediction
        threshold: Fraction needed to display (e.g., 0.8 = 4/5)
        
    Returns:
        Classification to display, or None if not stable enough
    """
    global _current_displayed_classification
    
    # Add current classification to history
    _classification_history.append(classification)
    
    # Need at least some history before making decisions
    if len(_classification_history) < 3:
        return _current_displayed_classification
    
    # Count occurrences of each classification in history
    classification_counts = {}
    for cls in _classification_history:
        classification_counts[cls] = classification_counts.get(cls, 0) + 1
    
    # Find the most common classification
    most_common = max(classification_counts.items(), key=lambda x: x[1])
    most_common_class, count = most_common
    
    # Calculate fraction
    fraction = count / len(_classification_history)
    
    # If fraction meets threshold, update displayed classification
    if fraction >= threshold:
        # Only update if it's different from current (to avoid flickering)
        if most_common_class != _current_displayed_classification:
            _current_displayed_classification = most_common_class
        return _current_displayed_classification
    
    # If threshold not met but we have a current classification, keep it
    # This prevents flickering when there are occasional misclassifications
    if _current_displayed_classification is not None:
        return _current_displayed_classification
    
    # If no stable classification yet, return the most common one anyway (for initial display)
    return most_common_class


def _get_smoothed_direction(new_direction: Optional[float], alpha: float = 0.3) -> Optional[float]:
    """
    Smooth direction using exponential moving average.
    
    Args:
        new_direction: New direction measurement in radians
        alpha: Smoothing factor (0-1), lower = more smoothing
        
    Returns:
        Smoothed direction in radians
    """
    global _smoothed_direction
    
    if new_direction is None:
        return _smoothed_direction
    
    # Normalize to [0, 2π)
    new_direction = new_direction % (2 * np.pi)
    
    if _smoothed_direction is None:
        _smoothed_direction = new_direction
        return _smoothed_direction
    
    # Handle wrap-around in angles (e.g., 350° to 10°)
    # Find shortest path between angles
    diff = new_direction - _smoothed_direction
    
    # Normalize to [-π, π]
    if diff > np.pi:
        diff -= 2 * np.pi
    elif diff < -np.pi:
        diff += 2 * np.pi
    
    # Apply exponential moving average
    _smoothed_direction = (_smoothed_direction + alpha * diff) % (2 * np.pi)
    
    return _smoothed_direction


def init_glasses_gui():
    """Initialize the glasses GUI in the main thread. Call this before starting the main loop."""
    global _glasses_gui
    
    if _glasses_gui is not None:
        return
    
    # Create GUI in the current thread (should be main thread)
    root = tk.Tk()
    _glasses_gui = SoundVisualizationGUI(root)
    # Show the window and process initial events
    root.update_idletasks()
    root.deiconify()  # Ensure window is visible
    root.update()  # Process initial window creation events to make it visible


def _init_glasses_gui():
    """Internal alias for init_glasses_gui() for backward compatibility."""
    init_glasses_gui()


def visualize_glasses_continuous(doa_results: Dict,
                                  classification: str,
                                  mic_positions: Optional[np.ndarray] = None,
                                  true_source_position: Optional[np.ndarray] = None):
    """
    Real-time glasses visualization with moving average for classification and direction smoothing.
    
    This function integrates with existing classification and DOA analysis code.
    It uses a moving average approach for classification (e.g., display if 4/5 detections match)
    and exponential moving average for direction smoothing to reduce volatility.
    
    Args:
        doa_results: Dictionary of DOA algorithm results (from analyze_doa_continuous)
        classification: Current classification prediction (from classify_audio)
        mic_positions: Optional microphone positions (not used for glasses display)
        true_source_position: Optional true source position (not used for glasses display)
    """
    global _glasses_gui
    
    # Initialize GUI if needed
    if _glasses_gui is None:
        _init_glasses_gui()
    
    # Wait for GUI to be ready
    if _glasses_gui is None:
        return
    
    # Extract direction from DOA results
    direction = _extract_direction_from_doa(doa_results)
    
    # Get smoothed classification (moving average approach)
    display_classification = _get_smoothed_classification(classification, threshold=0.8)
    
    # Get smoothed direction
    smoothed_direction = _get_smoothed_direction(direction, alpha=0.3)
    
    # Update GUI (should be called from main thread)
    try:
        if _glasses_gui is not None and hasattr(_glasses_gui, 'root'):
            root = _glasses_gui.root
            
            # Update the display
            # Always show something if we have classification, even if direction is missing
            if display_classification is not None:
                if smoothed_direction is not None:
                    _glasses_gui.update_from_external(display_classification, smoothed_direction)
                else:
                    # Show classification even without direction
                    _glasses_gui.update_from_external(display_classification, 0.0)
            
            # Process pending GUI events (non-blocking)
            # Use update_idletasks() for non-blocking updates, but occasionally call update() for full refresh
            root.update_idletasks()
    except Exception:
        # Silently handle GUI update errors (GUI might be closing)
        pass


def reset_visualization_state():
    """Reset the moving average state (useful for testing or restarting)."""
    global _classification_history, _current_displayed_classification
    global _direction_history, _smoothed_direction
    
    _classification_history.clear()
    _current_displayed_classification = None
    _direction_history.clear()
    _smoothed_direction = None

