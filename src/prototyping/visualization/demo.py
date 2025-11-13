"""
Quick demo script that shows the GUI with some example data.
"""

import tkinter as tk
from sound_visualization_gui import SoundVisualizationGUI
import numpy as np
import time


def demo():
    """Run a quick demo with example classifications and directions."""
    root = tk.Tk()
    app = SoundVisualizationGUI(root)
    
    # Example data: (classification, direction_in_radians)
    examples = [
        ("doorbell", np.pi / 4),      # 45 degrees (Northeast)
        ("phone", np.pi / 2),          # 90 degrees (East)
        ("alarm", np.pi),              # 180 degrees (South)
        ("knock", 3 * np.pi / 2),      # 270 degrees (West)
        ("voice", 0),                  # 0 degrees (North)
        ("doorbell", 5 * np.pi / 4),   # 225 degrees (Southwest)
    ]
    
    print("Demo: Updating HUD with example sound detections...")
    print("The smart glasses HUD should be visible with updates every 2 seconds.")
    print("Look for the classification in the top-left and compass in the top-right.\n")
    
    def update_with_example(index=0):
        """Update GUI with next example."""
        if index < len(examples):
            classification, direction = examples[index]
            degrees = np.degrees(direction)
            print(f"  [{index+1}/{len(examples)}] {classification} at {degrees:.1f}° ({direction:.3f} rad)")
            app.update_from_external(classification, direction)
            root.after(2000, lambda: update_with_example(index + 1))
        else:
            print("\nDemo complete! You can continue using the GUI manually.")
    
    # Start updates after 1 second
    root.after(1000, lambda: update_with_example(0))
    
    print("Starting GUI...")
    root.mainloop()


if __name__ == "__main__":
    demo()



