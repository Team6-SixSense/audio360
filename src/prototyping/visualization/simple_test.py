"""Minimal test to verify GUI window appears."""
import sys
import os

# Ensure we can import from current directory
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Set matplotlib backend before any other imports
import matplotlib
matplotlib.use('TkAgg')

import tkinter as tk

print("=" * 50)
print("Testing GUI Window Creation")
print("=" * 50)

try:
    print("\n1. Creating root window...")
    root = tk.Tk()
    root.title("Test Window")
    root.geometry("300x200")
    
    label = tk.Label(root, text="If you see this window,\nGUI is working!", 
                     font=("Arial", 14))
    label.pack(expand=True)
    
    print("   [SUCCESS] Root window created")
    print("\n2. Starting mainloop...")
    print("   Window should appear now!")
    print("   Close the window to continue...\n")
    
    root.mainloop()
    
    print("\n[SUCCESS] Window was closed. GUI is working!")
    
except Exception as e:
    print(f"\n[ERROR] Failed: {e}")
    import traceback
    traceback.print_exc()
    input("\nPress Enter to exit...")
    sys.exit(1)

