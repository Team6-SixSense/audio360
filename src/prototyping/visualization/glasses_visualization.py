import numpy as np
from typing import Optional, Dict
from PyQt6 import QtWidgets

from .sound_visualization_gui import SoundVisualizationGUI


# Global GUI state
_glasses_gui: Optional[SoundVisualizationGUI] = None
_app: Optional[QtWidgets.QApplication] = None


def _extract_direction_from_doa(doa_results: Dict) -> Optional[float]:
    """
    Extract direction from DOA results.

    Prefers FRIDA, then SRP-PHAT, then MUSIC, mirroring your original logic. :contentReference[oaicite:2]{index=2}

    Args:
        doa_results: Dictionary of DOA algorithm results

    Returns:
        Direction in radians, or None if no valid result
    """
    priority_order = ["FRIDA", "SRP-PHAT", "MUSIC"]

    for algo_name in priority_order:
        if algo_name in doa_results:
            doa = doa_results[algo_name]
            if getattr(doa, "azimuth_recon", None) is not None and len(doa.azimuth_recon) > 0:
                return float(doa.azimuth_recon[0])  # assumed radians

    return None


def init_glasses_gui():
    """
    Initialize the PyQt6 glasses GUI.

    This mirrors the old Tkinter entry point but uses a QApplication and
    a QMainWindow instead. It does NOT call app.exec_(); instead,
    visualize_glasses_continuous() pumps events with processEvents().
    """
    global _glasses_gui, _app

    if _glasses_gui is not None:
        return

    _app = QtWidgets.QApplication.instance()
    if _app is None:
        _app = QtWidgets.QApplication([])

    _glasses_gui = SoundVisualizationGUI()
    _glasses_gui.show()

    # Make sure the window appears immediately
    _app.processEvents()


def _init_glasses_gui():
    """Internal alias for backwards compatibility."""
    init_glasses_gui()


def visualize_glasses_continuous(
    doa_results: Dict,
    classification: str,
    mic_positions: Optional[np.ndarray] = None,
    true_source_position: Optional[np.ndarray] = None,
):
    """
    Real-time glasses visualization with **instant** updates.

    - Classification: uses the current prediction directly (no moving average).
    - Direction: uses the latest DOA estimate directly (no exponential smoothing).

    Args:
        doa_results: Dictionary of DOA algorithm results (from analyze_doa_continuous)
        classification: Current classification prediction (from classify_audio)
        mic_positions: Unused here (kept for API compatibility)
        true_source_position: Unused here (kept for API compatibility)
    """
    global _glasses_gui, _app

    # Ensure GUI exists
    if _glasses_gui is None:
        _init_glasses_gui()
    if _glasses_gui is None:
        return

    # Instant classification
    display_classification = classification

    # Instant direction from DOA (fallback to 0 rad if missing)
    direction = _extract_direction_from_doa(doa_results)
    if direction is None:
        direction = 0.0

    # Normalize angle to [0, 2π)
    direction = direction % (2 * np.pi)

    try:
        # Update HUD immediately
        _glasses_gui.update_from_external(display_classification, direction)

        # Pump Qt events so the window stays responsive
        if _app is not None:
            _app.processEvents()

    except Exception:
        # If the window was closed or other GUI errors occur, don't crash the pipeline
        pass


def reset_visualization_state():
    """
    Reset visualization-related state (kept for API compatibility).

    Since this version does not maintain smoothing history, this is effectively a no-op,
    but we keep it so any existing calls in your codebase don't break.
    """
    # No internal history to clear in the instant-update version.
    pass
