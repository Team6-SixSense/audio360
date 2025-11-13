"""
Sound Visualization GUI - Smart Glasses HUD Style

A minimal GUI application that visualizes sound classification and direction
in a smart glasses HUD format.
"""

import tkinter as tk
from tkinter import ttk, messagebox
import numpy as np
from typing import Optional, Tuple


class SoundVisualizationGUI:
    """GUI for visualizing sound classification and direction in HUD style."""
    
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("SixSense - Smart Glasses HUD")
        # Full screen or large window to simulate glasses view
        self.root.geometry("1200x800")
        self.root.configure(bg='#1a1a1a')  # Dark gray background
        
        # Data storage
        self.current_classification: Optional[str] = None
        self.current_direction: Optional[float] = None  # in radians
        
        # Create HUD display
        self._create_hud_display()
        
        # Create minimal control panel (can be hidden/minimized)
        self._create_control_panel()
        
    def _create_hud_display(self):
        """Create the HUD overlay display with glasses frame."""
        # Main HUD canvas - full screen overlay
        self.hud_canvas = tk.Canvas(
            self.root, 
            bg='#1a1a1a',  # Dark gray background
            highlightthickness=0
        )
        self.hud_canvas.pack(fill=tk.BOTH, expand=True)
        
        # Bind resize event to redraw glasses frame
        self.hud_canvas.bind('<Configure>', self._on_canvas_resize)
        
        # Draw glasses frame after a short delay to ensure canvas is sized
        self.root.after(100, self._draw_glasses_frame)
        
        # Create HUD displays on both lenses (for eye comfort - images will merge)
        # Wait for frame to be drawn first, then position HUD elements
        self.root.after(150, self._create_hud_displays)
        
    def _on_canvas_resize(self, event=None):
        """Handle canvas resize - redraw glasses frame and reposition HUD."""
        self._draw_glasses_frame()
        # Reposition HUD elements after frame is redrawn
        self.root.after(50, self._reposition_hud_elements)
    
    def _reposition_hud_elements(self):
        """Reposition HUD elements based on current lens positions."""
        padding = 10
        if hasattr(self, 'left_lens_x') and hasattr(self, 'left_lens_y') and hasattr(self, 'left_lens_height'):
            # Reposition left lens HUD to bottom-left corner
            if hasattr(self, 'classification_label_left'):
                left_frame = self.classification_label_left.master.master
                if isinstance(left_frame, tk.Frame):
                    x_pos = self.left_lens_x + padding
                    y_pos = self.left_lens_y + self.left_lens_height - padding
                    left_frame.place(x=x_pos, y=y_pos, anchor='sw')
        
        if hasattr(self, 'right_lens_x') and hasattr(self, 'right_lens_y') and hasattr(self, 'right_lens_height'):
            # Reposition right lens HUD to bottom-left corner
            if hasattr(self, 'classification_label_right'):
                right_frame = self.classification_label_right.master.master
                if isinstance(right_frame, tk.Frame):
                    x_pos = self.right_lens_x + padding
                    y_pos = self.right_lens_y + self.right_lens_height - padding
                    right_frame.place(x=x_pos, y=y_pos, anchor='sw')
    
    def _draw_glasses_frame(self, event=None):
        """Draw a glasses frame on the canvas."""
        # Clear previous frame
        self.hud_canvas.delete("glasses_frame")
        
        # Get canvas dimensions
        canvas_width = self.hud_canvas.winfo_width()
        canvas_height = self.hud_canvas.winfo_height()
        
        # If canvas not yet sized, use default
        if canvas_width < 10:
            canvas_width = 1200
            canvas_height = 800
        
        # Center the glasses frame
        center_x = canvas_width / 2
        center_y = canvas_height / 2
        
        # Glasses dimensions - realistic rectangular lenses
        lens_width = 400  # Wider than tall
        lens_height = 280  # Realistic proportions
        corner_radius = 25  # Rounded corners
        bridge_width = 50
        frame_thickness = 8
        temple_length = 200
        
        # Left lens position
        left_lens_x = center_x - lens_width - bridge_width / 2
        left_lens_y = center_y - lens_height / 2
        
        # Right lens position
        right_lens_x = center_x + bridge_width / 2
        right_lens_y = center_y - lens_height / 2
        
        # Store lens dimensions
        self.left_lens_x = left_lens_x
        self.left_lens_y = left_lens_y
        self.left_lens_width = lens_width
        self.left_lens_height = lens_height
        
        self.right_lens_x = right_lens_x
        self.right_lens_y = right_lens_y
        self.right_lens_width = lens_width
        self.right_lens_height = lens_height
        
        # Draw frame outline with rounded corners - realistic glasses
        # Left lens frame (rounded rectangle)
        self._draw_rounded_rectangle(
            left_lens_x - frame_thickness/2, left_lens_y - frame_thickness/2,
            left_lens_x + lens_width + frame_thickness/2, left_lens_y + lens_height + frame_thickness/2,
            corner_radius, '#CCCCCC', frame_thickness, '#000000', "glasses_frame"
        )
        
        # Right lens frame (rounded rectangle)
        self._draw_rounded_rectangle(
            right_lens_x - frame_thickness/2, right_lens_y - frame_thickness/2,
            right_lens_x + lens_width + frame_thickness/2, right_lens_y + lens_height + frame_thickness/2,
            corner_radius, '#CCCCCC', frame_thickness, '#000000', "glasses_frame"
        )
        
        # Bridge
        bridge_y = center_y
        self.hud_canvas.create_rectangle(
            center_x - bridge_width/2, bridge_y - 15,
            center_x + bridge_width/2, bridge_y + 15,
            outline='#CCCCCC', width=frame_thickness, fill='#000000',
            tags="glasses_frame"
        )
        
        # Left temple
        self.hud_canvas.create_line(
            left_lens_x, bridge_y,
            left_lens_x - temple_length, bridge_y,
            fill='#CCCCCC', width=frame_thickness,
            tags="glasses_frame"
        )
        
        # Right temple
        self.hud_canvas.create_line(
            right_lens_x + lens_width, bridge_y,
            right_lens_x + lens_width + temple_length, bridge_y,
            fill='#CCCCCC', width=frame_thickness,
            tags="glasses_frame"
        )
        
        # Store lens positions for HUD elements
        self.left_lens_center_x = left_lens_x + lens_width / 2
        self.left_lens_center_y = left_lens_y + lens_height / 2
        self.right_lens_center_x = right_lens_x + lens_width / 2
        self.right_lens_center_y = right_lens_y + lens_height / 2
    
    def _draw_rounded_rectangle(self, x1, y1, x2, y2, radius, outline_color, outline_width, fill_color, tags):
        """Draw a rounded rectangle on the canvas."""
        # Draw filled rounded rectangle (background)
        # Main rectangle parts
        self.hud_canvas.create_rectangle(
            x1 + radius, y1,
            x2 - radius, y2,
            outline=fill_color, width=0, fill=fill_color,
            tags=tags
        )
        self.hud_canvas.create_rectangle(
            x1, y1 + radius,
            x2, y2 - radius,
            outline=fill_color, width=0, fill=fill_color,
            tags=tags
        )
        
        # Rounded corner fills (pies)
        self.hud_canvas.create_arc(
            x1, y1, x1 + 2*radius, y1 + 2*radius,
            start=90, extent=90, outline=fill_color, width=0,
            fill=fill_color, style=tk.PIESLICE, tags=tags
        )
        self.hud_canvas.create_arc(
            x2 - 2*radius, y1, x2, y1 + 2*radius,
            start=0, extent=90, outline=fill_color, width=0,
            fill=fill_color, style=tk.PIESLICE, tags=tags
        )
        self.hud_canvas.create_arc(
            x1, y2 - 2*radius, x1 + 2*radius, y2,
            start=180, extent=90, outline=fill_color, width=0,
            fill=fill_color, style=tk.PIESLICE, tags=tags
        )
        self.hud_canvas.create_arc(
            x2 - 2*radius, y2 - 2*radius, x2, y2,
            start=270, extent=90, outline=fill_color, width=0,
            fill=fill_color, style=tk.PIESLICE, tags=tags
        )
        
        # Draw outline
        # Horizontal lines
        self.hud_canvas.create_line(
            x1 + radius, y1, x2 - radius, y1,
            fill=outline_color, width=outline_width, tags=tags
        )
        self.hud_canvas.create_line(
            x1 + radius, y2, x2 - radius, y2,
            fill=outline_color, width=outline_width, tags=tags
        )
        # Vertical lines
        self.hud_canvas.create_line(
            x1, y1 + radius, x1, y2 - radius,
            fill=outline_color, width=outline_width, tags=tags
        )
        self.hud_canvas.create_line(
            x2, y1 + radius, x2, y2 - radius,
            fill=outline_color, width=outline_width, tags=tags
        )
        # Rounded corner arcs
        self.hud_canvas.create_arc(
            x1, y1, x1 + 2*radius, y1 + 2*radius,
            start=90, extent=90, outline=outline_color, width=outline_width,
            style=tk.ARC, tags=tags
        )
        self.hud_canvas.create_arc(
            x2 - 2*radius, y1, x2, y1 + 2*radius,
            start=0, extent=90, outline=outline_color, width=outline_width,
            style=tk.ARC, tags=tags
        )
        self.hud_canvas.create_arc(
            x1, y2 - 2*radius, x1 + 2*radius, y2,
            start=180, extent=90, outline=outline_color, width=outline_width,
            style=tk.ARC, tags=tags
        )
        self.hud_canvas.create_arc(
            x2 - 2*radius, y2 - 2*radius, x2, y2,
            start=270, extent=90, outline=outline_color, width=outline_width,
            style=tk.ARC, tags=tags
        )
    
    def _create_hud_displays(self):
        """Create HUD displays on both lenses after frame is drawn."""
        # Left lens: Full HUD (classification + direction)
        self._create_left_lens_display()
        
        # Right lens: Full HUD (classification + direction) - same as left
        self._create_right_lens_display()
    
    def _create_left_lens_display(self):
        """Create full HUD display on left lens (classification + direction)."""
        # Container frame positioned in left corner of left lens
        left_frame = tk.Frame(self.hud_canvas, bg='#000000')
        
        # Calculate position based on actual lens coordinates
        # Position in bottom-left corner of left lens with small padding
        padding = 10
        if hasattr(self, 'left_lens_x') and hasattr(self, 'left_lens_y') and hasattr(self, 'left_lens_height'):
            # Use absolute positioning based on lens coordinates
            x_pos = self.left_lens_x + padding
            y_pos = self.left_lens_y + self.left_lens_height - padding
            left_frame.place(x=x_pos, y=y_pos, anchor='sw')
        else:
            # Fallback to relative positioning
            left_frame.place(relx=0.15, rely=0.62, anchor='sw')
        
        # Header
        header_label = tk.Label(
            left_frame,
            text="Sound Detected",
            font=("Arial", 9, "normal"),
            fg='#00FF00',  # Green text
            bg='#000000'
        )
        header_label.pack(anchor='w', pady=(0, 3))
        
        # Horizontal container for classification and direction (parallel)
        content_frame = tk.Frame(left_frame, bg='#000000')
        content_frame.pack(anchor='w')
        
        # Classification label (left side)
        self.classification_label_left = tk.Label(
            content_frame,
            text="--",
            font=("Arial", 14, "bold"),
            fg='#FFFFFF',  # White text
            bg='#000000'
        )
        self.classification_label_left.pack(side=tk.LEFT, padx=(0, 15))
        
        # Direction container (right side)
        direction_frame = tk.Frame(content_frame, bg='#000000')
        direction_frame.pack(side=tk.LEFT)
        
        # Arrow canvas for direction
        self.arrow_canvas_left = tk.Canvas(
            direction_frame,
            width=50,
            height=50,
            bg='#000000',
            highlightthickness=0
        )
        self.arrow_canvas_left.pack()
        
        # Initialize arrow
        self._draw_arrow_left(0)
        
        # Degrees label below arrow
        self.degrees_label_left = tk.Label(
            direction_frame,
            text="0°",
            font=("Arial", 11, "bold"),
            fg='#FFFFFF',  # White text
            bg='#000000'
        )
        self.degrees_label_left.pack(pady=(2, 0))
    
    def _create_right_lens_display(self):
        """Create full HUD display on right lens (classification + direction)."""
        # Container frame positioned in left corner of right lens
        right_frame = tk.Frame(self.hud_canvas, bg='#000000')
        
        # Calculate position based on actual lens coordinates
        # Position in bottom-left corner of right lens with small padding
        padding = 10
        if hasattr(self, 'right_lens_x') and hasattr(self, 'right_lens_y') and hasattr(self, 'right_lens_height'):
            # Use absolute positioning based on lens coordinates
            x_pos = self.right_lens_x + padding
            y_pos = self.right_lens_y + self.right_lens_height - padding
            right_frame.place(x=x_pos, y=y_pos, anchor='sw')
        else:
            # Fallback to relative positioning
            right_frame.place(relx=0.65, rely=0.62, anchor='sw')
        
        # Header
        header_label = tk.Label(
            right_frame,
            text="Sound Detected",
            font=("Arial", 9, "normal"),
            fg='#00FF00',  # Green text
            bg='#000000'
        )
        header_label.pack(anchor='w', pady=(0, 3))
        
        # Horizontal container for classification and direction (parallel)
        content_frame = tk.Frame(right_frame, bg='#000000')
        content_frame.pack(anchor='w')
        
        # Classification label (left side)
        self.classification_label_right = tk.Label(
            content_frame,
            text="--",
            font=("Arial", 14, "bold"),
            fg='#FFFFFF',  # White text
            bg='#000000'
        )
        self.classification_label_right.pack(side=tk.LEFT, padx=(0, 15))
        
        # Direction container (right side)
        direction_frame = tk.Frame(content_frame, bg='#000000')
        direction_frame.pack(side=tk.LEFT)
        
        # Arrow canvas for direction
        self.arrow_canvas_right = tk.Canvas(
            direction_frame,
            width=50,
            height=50,
            bg='#000000',
            highlightthickness=0
        )
        self.arrow_canvas_right.pack()
        
        # Initialize arrow
        self._draw_arrow_right(0)
        
        # Degrees label below arrow
        self.degrees_label_right = tk.Label(
            direction_frame,
            text="0°",
            font=("Arial", 11, "bold"),
            fg='#FFFFFF',  # White text
            bg='#000000'
        )
        self.degrees_label_right.pack(pady=(2, 0))
        
    def _draw_arrow_left(self, direction: float):
        """Draw a minimal arrow pointing in the given direction (in radians) on left lens."""
        self.arrow_canvas_left.delete("all")
        
        # Canvas center (adjusted for smaller canvas)
        center_x, center_y = 25, 25
        arrow_length = 18
        
        # Convert direction: 0 radians = North (pointing up), clockwise
        # In canvas coordinates: 0 degrees = right, counter-clockwise
        # So we need: canvas_angle = -direction + π/2
        canvas_angle = -direction + np.pi / 2
        
        # Calculate arrow endpoint
        end_x = center_x + arrow_length * np.cos(canvas_angle)
        end_y = center_y - arrow_length * np.sin(canvas_angle)  # Negative because y increases downward
        
        # Draw arrow line (white color)
        self.arrow_canvas_left.create_line(
            center_x, center_y, end_x, end_y,
            fill='#FFFFFF',  # White
            width=2,
            arrow=tk.LAST,
            arrowshape=(6, 8, 2)
        )
    
    def _draw_arrow_right(self, direction: float):
        """Draw a minimal arrow pointing in the given direction (in radians) on right lens."""
        self.arrow_canvas_right.delete("all")
        
        # Canvas center (adjusted for smaller canvas)
        center_x, center_y = 25, 25
        arrow_length = 18
        
        # Convert direction: 0 radians = North (pointing up), clockwise
        # In canvas coordinates: 0 degrees = right, counter-clockwise
        # So we need: canvas_angle = -direction + π/2
        canvas_angle = -direction + np.pi / 2
        
        # Calculate arrow endpoint
        end_x = center_x + arrow_length * np.cos(canvas_angle)
        end_y = center_y - arrow_length * np.sin(canvas_angle)  # Negative because y increases downward
        
        # Draw arrow line (white color)
        self.arrow_canvas_right.create_line(
            center_x, center_y, end_x, end_y,
            fill='#FFFFFF',  # White
            width=2,
            arrow=tk.LAST,
            arrowshape=(6, 8, 2)
        )
        
    def _create_control_panel(self):
        """Create minimal control panel for input (can be toggled)."""
        # Control panel frame (initially minimized)
        self.control_frame = tk.Frame(self.root, bg='#1a1a1a', relief=tk.RAISED, bd=2)
        self.control_frame.place(relx=0.5, rely=0.95, anchor='s')
        
        # Minimize/Expand button
        self.panel_visible = False
        self.toggle_btn = tk.Button(
            self.control_frame,
            text="⚙ Controls",
            command=self._toggle_controls,
            bg='#333333',
            fg='#FFFFFF',
            font=("Arial", 10),
            relief=tk.FLAT,
            padx=10,
            pady=5
        )
        self.toggle_btn.pack()
        
        # Control panel content (initially hidden)
        self.controls_content = tk.Frame(self.control_frame, bg='#1a1a1a', padx=20, pady=10)
        
        # Classification input
        input_frame1 = tk.Frame(self.controls_content, bg='#1a1a1a')
        input_frame1.pack(fill=tk.X, pady=5)
        tk.Label(input_frame1, text="Classification:", bg='#1a1a1a', fg='#FFFFFF', font=("Arial", 10)).pack(side=tk.LEFT, padx=5)
        self.classification_var = tk.StringVar()
        classification_entry = tk.Entry(input_frame1, textvariable=self.classification_var, width=15, bg='#333333', fg='#FFFFFF', insertbackground='#FFFFFF')
        classification_entry.pack(side=tk.LEFT, padx=5)
        
        # Direction input
        input_frame2 = tk.Frame(self.controls_content, bg='#1a1a1a')
        input_frame2.pack(fill=tk.X, pady=5)
        tk.Label(input_frame2, text="Direction (rad):", bg='#1a1a1a', fg='#FFFFFF', font=("Arial", 10)).pack(side=tk.LEFT, padx=5)
        self.direction_var = tk.StringVar()
        direction_entry = tk.Entry(input_frame2, textvariable=self.direction_var, width=15, bg='#333333', fg='#FFFFFF', insertbackground='#FFFFFF')
        direction_entry.pack(side=tk.LEFT, padx=5)
        
        # Buttons
        button_frame = tk.Frame(self.controls_content, bg='#1a1a1a')
        button_frame.pack(pady=10)
        tk.Button(
            button_frame,
            text="Update",
            command=self.update_visualization,
            bg='#00AA00',
            fg='#FFFFFF',
            font=("Arial", 10, "bold"),
            padx=15,
            pady=5
        ).pack(side=tk.LEFT, padx=5)
        tk.Button(
            button_frame,
            text="Clear",
            command=self.clear_display,
            bg='#AA0000',
            fg='#FFFFFF',
            font=("Arial", 10),
            padx=15,
            pady=5
        ).pack(side=tk.LEFT, padx=5)
        
    def _toggle_controls(self):
        """Toggle control panel visibility."""
        if self.panel_visible:
            self.controls_content.pack_forget()
            self.toggle_btn.config(text="⚙ Controls")
            self.panel_visible = False
        else:
            self.controls_content.pack()
            self.toggle_btn.config(text="▼ Hide")
            self.panel_visible = True
    
    def update_visualization(self):
        """Update the visualization with new classification and direction."""
        try:
            classification = self.classification_var.get().strip()
            direction_str = self.direction_var.get().strip()
            
            if not classification:
                messagebox.showwarning("Warning", "Please enter a classification.")
                return
            
            if not direction_str:
                messagebox.showwarning("Warning", "Please enter a direction (in radians).")
                return
            
            direction = float(direction_str)
            
            # Normalize direction to [0, 2π)
            direction = direction % (2 * np.pi)
            
            # Update display
            self._update_display(classification, direction)
            
        except ValueError:
            messagebox.showerror("Error", "Invalid direction value. Please enter a number (in radians).")
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {str(e)}")
    
    def _update_display(self, classification: str, direction: float):
        """Update HUD display with new data on both lenses."""
        # Update classification on both lenses
        self.current_classification = classification
        self.classification_label_left.config(text=classification.upper())
        self.classification_label_right.config(text=classification.upper())
        
        # Update arrow on both lenses
        self.current_direction = direction
        self._draw_arrow_left(direction)
        self._draw_arrow_right(direction)
        
        # Update degrees on both lenses
        degrees = np.degrees(direction)
        self.degrees_label_left.config(text=f"{degrees:.0f}°")
        self.degrees_label_right.config(text=f"{degrees:.0f}°")
    
    def clear_display(self):
        """Clear the HUD display on both lenses."""
        self.current_classification = None
        self.current_direction = None
        self.classification_label_left.config(text="--")
        self.classification_label_right.config(text="--")
        self.degrees_label_left.config(text="0°")
        self.degrees_label_right.config(text="0°")
        self._draw_arrow_left(0)
        self._draw_arrow_right(0)
        self.classification_var.set("")
        self.direction_var.set("")
    
    def update_from_external(self, classification: str, direction: float):
        """
        Update visualization from external code.
        
        Args:
            classification: Sound classification string (e.g., "doorbell", "phone", "alarm")
            direction: Direction angle in radians
        """
        # Normalize direction
        direction = direction % (2 * np.pi)
        
        # Update directly without needing input fields
        self._update_display(classification, direction)
        
        # Also update input fields if they exist (for manual editing)
        if hasattr(self, 'classification_var'):
            self.classification_var.set(classification)
        if hasattr(self, 'direction_var'):
            self.direction_var.set(str(direction))


def main():
    """Main entry point for the GUI application."""
    root = tk.Tk()
    app = SoundVisualizationGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
