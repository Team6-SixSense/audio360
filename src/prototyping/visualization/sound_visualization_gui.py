import sys
import numpy as np
from typing import Optional

from PyQt6 import QtWidgets, QtGui, QtCore


class HUDWidget(QtWidgets.QWidget):
    """
    Central widget that draws the glasses frame and HUD overlay using QPainter.
    """

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setAutoFillBackground(True)

        # Current data
        self.current_classification: Optional[str] = None
        self.current_direction: Optional[float] = 0.0

        # Colors
        self.bg_color = QtGui.QColor("#1a1a1a")
        self.frame_fill = QtGui.QColor("#000000")
        self.frame_outline = QtGui.QColor("#CCCCCC")
        self.header_color = QtGui.QColor("#00FF00")
        self.text_color = QtGui.QColor("#FFFFFF")

        # Fonts
        self.header_font = QtGui.QFont("Arial", 9)
        self.classification_font = QtGui.QFont("Arial", 14, QtGui.QFont.Weight.Bold)
        self.degrees_font = QtGui.QFont("Arial", 11, QtGui.QFont.Weight.Bold)

    # ==================================================================
    # Public API
    # ==================================================================

    def set_data(self, classification: str, direction: float):
        """Update HUD with new classification and direction (radians)."""
        self.current_classification = classification
        self.current_direction = direction % (2 * np.pi)
        self.update()

    def clear(self):
        """Clear the HUD display."""
        self.current_classification = None
        self.current_direction = 0.0
        self.update()

    # ==================================================================
    # Layout helpers
    # ==================================================================

    def _compute_layout(self):
        w, h = self.width(), self.height()
        if w < 10 or h < 10:
            w, h = 1200, 800

        cx, cy = w / 2, h / 2

        lens_w = 400
        lens_h = 280
        bridge = 50
        thick = 8
        radius = 25
        temple = 200

        left_x = cx - lens_w - bridge / 2
        left_y = cy - lens_h / 2

        right_x = cx + bridge / 2
        right_y = left_y

        left_rect = QtCore.QRectF(
            left_x - thick / 2,
            left_y - thick / 2,
            lens_w + thick,
            lens_h + thick,
        )
        right_rect = QtCore.QRectF(
            right_x - thick / 2,
            right_y - thick / 2,
            lens_w + thick,
            lens_h + thick,
        )

        pad = 10
        left_hud = QtCore.QPointF(left_x + pad, left_y + lens_h - pad)
        right_hud = QtCore.QPointF(right_x + pad, right_y + lens_h - pad)

        return {
            "lens_w": lens_w,
            "lens_h": lens_h,
            "thick": thick,
            "radius": radius,
            "center_y": cy,
            "left_rect": left_rect,
            "right_rect": right_rect,
            "left_x": left_x,
            "right_x": right_x,
            "bridge": bridge,
            "temple": temple,
            "left_hud": left_hud,
            "right_hud": right_hud,
        }

    # ==================================================================
    # Drawing helpers
    # ==================================================================

    def _draw_rounded(self, painter, rect, radius, fill, outline, width):
        path = QtGui.QPainterPath()
        path.addRoundedRect(rect, radius, radius)
        painter.setBrush(fill)
        painter.setPen(QtGui.QPen(outline, width))
        painter.drawPath(path)

    def _draw_frame(self, painter, L):
        # Lenses
        self._draw_rounded(
            painter,
            L["left_rect"],
            L["radius"],
            self.frame_fill,
            self.frame_outline,
            L["thick"],
        )
        self._draw_rounded(
            painter,
            L["right_rect"],
            L["radius"],
            self.frame_fill,
            self.frame_outline,
            L["thick"],
        )

        # Bridge
        brect = QtCore.QRectF(
            (L["left_rect"].right() + L["right_rect"].left()) / 2 - L["bridge"] / 2,
            L["center_y"] - 15,
            L["bridge"],
            30,
        )
        painter.setBrush(self.frame_fill)
        painter.setPen(QtGui.QPen(self.frame_outline, L["thick"]))
        painter.drawRect(brect)

        # Temples
        px = L["left_x"]
        painter.drawLine(
            QtCore.QPointF(px, L["center_y"]),
            QtCore.QPointF(px - L["temple"], L["center_y"]),
        )

        px = L["right_x"] + L["lens_w"]
        painter.drawLine(
            QtCore.QPointF(px, L["center_y"]),
            QtCore.QPointF(px + L["temple"], L["center_y"]),
        )

    def _draw_hud(self, painter, pos, classification, direction):
        """
        Draw header, classification, arrow and degrees near the bottom-left
        of a lens, but with good padding and no overlap.
        """
        x0, y0 = pos.x(), pos.y()

        # Lift everything up from the very bottom
        BASE_OFFSET = 55         # how far above the bottom edge
        BASE_Y = y0 - BASE_OFFSET

        # ----- Header -----
        painter.setFont(self.header_font)
        painter.setPen(self.header_color)
        header = "Sound Detected"
        header_metrics = QtGui.QFontMetrics(self.header_font)
        header_h = header_metrics.height()
        painter.drawText(int(x0), int(BASE_Y - header_h - 25), header)

        # ----- Classification -----
        painter.setFont(self.classification_font)
        painter.setPen(self.text_color)
        ctext = (classification or "--").upper()
        fm = QtGui.QFontMetrics(self.classification_font)
        h2 = fm.height()

        class_y = BASE_Y
        painter.drawText(int(x0), int(class_y), ctext)

        # ----- Arrow positioning -----
        cwidth = fm.horizontalAdvance(ctext)

        ARROW_X_PADDING = 28         # spacing to the right of the text
        ARROW_Y_OFFSET = -h2 * 0.35  # vertically center arrow vs text

        ax = x0 + cwidth + ARROW_X_PADDING
        ay = class_y + ARROW_Y_OFFSET

        # Arrow geometry
        angle = -direction + np.pi / 2.0
        L = 24  # arrow length
        ex = ax + L * np.cos(angle)
        ey = ay - L * np.sin(angle)

        painter.setPen(QtGui.QPen(self.text_color, 2))
        painter.drawLine(QtCore.QPointF(ax, ay), QtCore.QPointF(ex, ey))

        # Arrowhead
        hlen = 7
        a1 = angle + np.radians(25)
        a2 = angle - np.radians(25)
        painter.drawLine(
            QtCore.QPointF(ex, ey),
            QtCore.QPointF(ex - hlen * np.cos(a1), ey + hlen * np.sin(a1)),
        )
        painter.drawLine(
            QtCore.QPointF(ex, ey),
            QtCore.QPointF(ex - hlen * np.cos(a2), ey + hlen * np.sin(a2)),
        )

        # ----- Degrees -----
        painter.setFont(self.degrees_font)
        deg = f"{(np.degrees(direction) % 360):.0f}°"
        dfm = QtGui.QFontMetrics(self.degrees_font)
        dw = dfm.horizontalAdvance(deg)

        DEGREE_PADDING = 42  # distance below the arrow

        painter.drawText(
            int(ax - dw / 2),
            int(ay + DEGREE_PADDING),
            deg,
        )

    # ==================================================================
    # Paint event
    # ==================================================================

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        painter.setRenderHint(QtGui.QPainter.RenderHint.Antialiasing)

        # Background
        painter.fillRect(self.rect(), self.bg_color)

        # Draw frame + HUD on each lens
        L = self._compute_layout()
        self._draw_frame(painter, L)

        direction = self.current_direction or 0.0
        self._draw_hud(painter, L["left_hud"], self.current_classification, direction)
        self._draw_hud(painter, L["right_hud"], self.current_classification, direction)


class SoundVisualizationGUI(QtWidgets.QMainWindow):
    """
    Main HUD window with NO control panel.
    """

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("SixSense - Smart Glasses HUD (PyQt6)")
        self.resize(1200, 800)

        self.hud = HUDWidget(self)
        self.setCentralWidget(self.hud)

    # External API (used by glasses_visualization.py)
    def update_from_external(self, classification: str, direction: float):
        self.hud.set_data(classification, direction)

    def clear_display(self):
        self.hud.clear()


def main():
    app = QtWidgets.QApplication(sys.argv)
    win = SoundVisualizationGUI()
    win.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
