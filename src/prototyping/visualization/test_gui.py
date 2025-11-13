"""Simple test to verify GUI works."""
import sys
import os

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    print("Importing tkinter...")
    import tkinter as tk
    print("  [OK] tkinter imported")
    
    print("Importing matplotlib...")
    import matplotlib
    print("  [OK] matplotlib imported")
    
    print("Importing sound_visualization_gui...")
    from sound_visualization_gui import SoundVisualizationGUI
    print("  [OK] SoundVisualizationGUI imported")
    
    print("\nCreating GUI window...")
    root = tk.Tk()
    app = SoundVisualizationGUI(root)
    print("  [OK] GUI created")
    
    print("\nGUI window should be visible now!")
    print("Press Ctrl+C to exit or close the window.\n")
    
    root.mainloop()
    
except Exception as e:
    print(f"\nERROR: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

