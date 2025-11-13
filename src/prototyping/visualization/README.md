# Sound Visualization GUI

A Python GUI application for visualizing sound classification and direction of arrival (DoA) results.

## Features

- **Polar Plot Visualization**: Displays sound direction on a circular polar plot with cardinal directions (N, E, S, W)
- **Classification Display**: Shows the sound classification label (e.g., "doorbell", "phone", "alarm")
- **Direction Arrow: Points in the direction of the sound source
- **History Tracking**: Shows recent detections as gray dots
- **Real-time Updates**: Can be updated programmatically from external code

## Requirements

- Python 3.7+
- tkinter (usually included with Python)
- matplotlib
- numpy

## Installation

```bash
pip install matplotlib numpy
```

## Usage

### Basic Usage (Manual Input)

Run the GUI and manually enter classification and direction:

```bash
python sound_visualization_gui.py
```

### Programmatic Usage

```python
import tkinter as tk
from sound_visualization_gui import SoundVisualizationGUI
import numpy as np

root = tk.Tk()
app = SoundVisualizationGUI(root)

# Update with classification and direction
classification = "doorbell"
direction = np.pi / 4  # 45 degrees in radians
app.update_from_external(classification, direction)

root.mainloop()
```

### Integration with Existing Code

```python
from sound_visualization_gui import SoundVisualizationGUI
import tkinter as tk

# Your existing functions
def classify_sound(audio_data):
    # Your classification code here
    return "doorbell"

def calculate_direction(audio_data):
    # Your direction analysis code here
    return 1.57  # radians

# Setup GUI
root = tk.Tk()
app = SoundVisualizationGUI(root)

# Process audio and update GUI
audio_data = get_audio_data()  # Your audio capture function
classification = classify_sound(audio_data)
direction = calculate_direction(audio_data)
app.update_from_external(classification, direction)

root.mainloop()
```

## Input Format

- **Classification**: String (e.g., "doorbell", "phone", "alarm", "knock", "voice")
- **Direction**: Float in radians (0 to 2π)
  - 0 radians = North (forward)
  - π/2 radians = East (right)
  - π radians = South (backward)
  - 3π/2 radians = West (left)

## Coordinate System

The visualization uses a polar coordinate system where:
- 0 radians (0°) points to the North (forward direction)
- Angles increase clockwise
- The coordinate system matches the glasses frame coordinate system described in the SRS

## Examples

See `example_usage.py` for complete examples including:
- Manual input
- Programmatic updates
- Integration templates



