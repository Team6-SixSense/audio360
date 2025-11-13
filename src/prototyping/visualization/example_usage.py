"""
Example usage of the Sound Visualization GUI

This script demonstrates how to use the visualization GUI with
classification and direction data.
"""

import tkinter as tk
from sound_visualization_gui import SoundVisualizationGUI
import numpy as np
import time
import random


def example_manual_input():
    """Example: Manual input through GUI."""
    root = tk.Tk()
    app = SoundVisualizationGUI(root)
    root.mainloop()


def example_programmatic_updates():
    """Example: Programmatic updates simulating real-time data."""
    root = tk.Tk()
    app = SoundVisualizationGUI(root)
    
    # Simulate sound detections
    classifications = ["doorbell", "phone", "alarm", "knock", "voice"]
    
    def simulate_detection():
        """Simulate a sound detection event."""
        classification = random.choice(classifications)
        direction = random.uniform(0, 2 * np.pi)  # Random direction in radians
        
        print(f"Detected: {classification} at {np.degrees(direction):.1f}°")
        app.update_from_external(classification, direction)
        
        # Schedule next detection
        root.after(3000, simulate_detection)  # Every 3 seconds
    
    # Start simulation
    root.after(1000, simulate_detection)
    root.mainloop()


def example_integration_template():
    """
    Template for integrating with existing classification and direction analysis code.
    
    Replace the placeholder functions with your actual classification and
    direction analysis functions.
    """
    root = tk.Tk()
    app = SoundVisualizationGUI(root)
    
    def your_classification_function(audio_data):
        """
        Placeholder for your classification function.
        
        Args:
            audio_data: Your audio input data
            
        Returns:
            str: Classification result (e.g., "doorbell", "phone", "alarm")
        """
        # TODO: Replace with your actual classification code
        # Example:
        # return classify_sound(audio_data)
        return "doorbell"
    
    def your_direction_analysis_function(audio_data):
        """
        Placeholder for your direction analysis function.
        
        Args:
            audio_data: Your audio input data (multi-channel)
            
        Returns:
            float: Direction angle in radians
        """
        # TODO: Replace with your actual direction analysis code
        # Example:
        # return calculate_direction_of_arrival(audio_data)
        return np.pi / 4  # 45 degrees
    
    def process_audio_and_update(audio_data):
        """Process audio and update visualization."""
        # Get classification
        classification = your_classification_function(audio_data)
        
        # Get direction
        direction = your_direction_analysis_function(audio_data)
        
        # Update GUI
        app.update_from_external(classification, direction)
    
    # Example: Simulate processing audio data
    # In your actual code, you would call process_audio_and_update()
    # whenever you have new audio data to process
    
    root.mainloop()


if __name__ == "__main__":
    # Run manual input example
    print("Starting GUI - you can manually enter classification and direction")
    example_manual_input()
    
    # Uncomment to run programmatic updates example:
    # example_programmatic_updates()
    
    # Uncomment to see integration template:
    # example_integration_template()



