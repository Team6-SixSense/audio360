import 'dart:math' as math;
import 'package:flutter/material.dart';
import '../models/enums.dart';

/// Transverse-plane (floor) direction reticle for monochrome-green optics.
///
/// - Draws a foreshortened ellipse (flat disk viewed at an angle)
/// - Concentric rings + crosshairs + cardinal ticks
/// - Adds a darker center "matte" so the arrow reads clearly on-lens
class DirectionReticle extends StatelessWidget {
  final Quadrant quadrant;
  final double size;

  /// 0.0..1.0. Lower = more flattened (more transverse-plane look).
  final double yCompression;

  const DirectionReticle({
    super.key,
    required this.quadrant,
    this.size = 240,
    this.yCompression = 0.55,
  });

  double _quadrantToAzimuthDeg(Quadrant q) {
    switch (q) {
      case Quadrant.north:
        return 0;
      case Quadrant.northEast:
        return 45;
      case Quadrant.east:
        return 90;
      case Quadrant.southEast:
        return 135;
      case Quadrant.south:
        return 180;
      case Quadrant.southWest:
        return 225;
      case Quadrant.west:
        return 270;
      case Quadrant.northWest:
        return 315;
      case Quadrant.none:
        return 0;
    }
  }

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: size,
      height: size,
      child: CustomPaint(
        painter: _ReticlePainter(
          azimuthDeg: _quadrantToAzimuthDeg(quadrant),
          hasDirection: quadrant != Quadrant.none,
          yCompression: yCompression,
        ),
      ),
    );
  }
}

class _ReticlePainter extends CustomPainter {
  final double azimuthDeg;
  final bool hasDirection;
  final double yCompression;

  _ReticlePainter({
    required this.azimuthDeg,
    required this.hasDirection,
    required this.yCompression,
  });

  static const _gBright = Color(0xFF00FF00);
  static const _gDim = Color(0xFF00AA00);

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final radius = size.width * 0.38;

    final ellipseRect = Rect.fromCenter(
      center: center,
      width: radius * 2,
      height: radius * 2 * yCompression,
    );

    // Slight vignette to help the reticle read on-lens
    final vignettePaint = Paint()
      ..shader = RadialGradient(
        colors: [
          Colors.black.withOpacity(0.55),
          Colors.black.withOpacity(0.0),
        ],
      ).createShader(Rect.fromCircle(center: center, radius: radius * 1.35));
    canvas.drawRect(Offset.zero & size, vignettePaint);

    _drawConcentricRings(canvas, center, radius);
    _drawCrosshairs(canvas, center, radius);
    _drawCardinalTicks(canvas, center, radius);

    // Darker center matte so the arrow doesn’t get lost
    final matte = Paint()..color = Colors.black.withOpacity(0.55);
    canvas.drawOval(
      Rect.fromCenter(
        center: center,
        width: radius * 0.9,
        height: radius * 0.9 * yCompression,
      ),
      matte,
    );

    // Main outer ellipse, slightly brighter
    final outline = Paint()
      ..color = _gBright.withOpacity(0.9)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2.5;
    canvas.drawOval(ellipseRect, outline);

    if (hasDirection) {
      _drawArrow(canvas, center, radius);
    } else {
      final dot = Paint()..color = _gDim.withOpacity(0.6);
      canvas.drawCircle(center, 3, dot);
    }
  }

  void _drawConcentricRings(Canvas canvas, Offset center, double radius) {
    for (int i = 1; i <= 4; i++) {
      final r = radius * (i / 4);
      final rect = Rect.fromCenter(
        center: center,
        width: r * 2,
        height: r * 2 * yCompression,
      );

      final opacity = 0.20 + (0.10 * (4 - i));
      final p = Paint()
        ..color = _gDim.withOpacity(opacity)
        ..style = PaintingStyle.stroke
        ..strokeWidth = i == 1 ? 1.5 : 1.0;
      canvas.drawOval(rect, p);
    }
  }

  void _drawCrosshairs(Canvas canvas, Offset center, double radius) {
    final v = Paint()
      ..color = _gBright.withOpacity(0.85)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2.0;
    final h = Paint()
      ..color = _gBright.withOpacity(0.75)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.6;

    final extend = radius * 1.25;
    canvas.drawLine(
      Offset(center.dx, center.dy - extend),
      Offset(center.dx, center.dy + extend),
      v,
    );
    canvas.drawLine(
      Offset(center.dx - extend, center.dy),
      Offset(center.dx + extend, center.dy),
      h,
    );

    final c = Paint()..color = _gBright;
    canvas.drawCircle(center, 2.5, c);
  }

  void _drawCardinalTicks(Canvas canvas, Offset center, double radius) {
    const angles = <double>[0, 90, 180, 270];
    final tickPaint = Paint()
      ..color = _gBright.withOpacity(0.9)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2.0;

    final tickLen = 12.0;
    for (final deg in angles) {
      final a = deg * (math.pi / 180);
      final x = center.dx + radius * math.cos(a);
      final y = center.dy - radius * math.sin(a) * yCompression;

      // Perpendicular tick
      final perp = a + math.pi / 2;
      final s = Offset(
        x + tickLen * 0.2 * math.cos(perp),
        y - tickLen * 0.2 * math.sin(perp),
      );
      final e = Offset(
        x + tickLen * 1.3 * math.cos(perp),
        y - tickLen * 1.3 * math.sin(perp),
      );
      canvas.drawLine(s, e, tickPaint);
    }
  }

  void _drawArrow(Canvas canvas, Offset center, double radius) {
    final a = azimuthDeg * (math.pi / 180);
    final len = radius * 0.62;

    final tip = Offset(
      center.dx + len * math.cos(a),
      center.dy - len * math.sin(a) * yCompression,
    );

    // Arrow shaft (thicker + slight glow)
    final shaft = Paint()
      ..color = _gBright.withOpacity(0.95)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 4.0
      ..strokeCap = StrokeCap.round;
    canvas.drawLine(center, tip, shaft);

    final glow = Paint()
      ..color = _gBright.withOpacity(0.25)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 10
      ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 10);
    canvas.drawLine(center, tip, glow);

    // Arrowhead: account for yCompression when computing angle
    final ang = math.atan2((center.dy - tip.dy) / yCompression, tip.dx - center.dx);
    const head = 14.0;
    final p = Path()
      ..moveTo(tip.dx, tip.dy)
      ..lineTo(
        tip.dx - head * math.cos(ang - math.pi / 6),
        tip.dy + head * math.sin(ang - math.pi / 6),
      )
      ..lineTo(
        tip.dx - head * 0.75 * math.cos(ang),
        tip.dy + head * 0.75 * math.sin(ang),
      )
      ..lineTo(
        tip.dx - head * math.cos(ang + math.pi / 6),
        tip.dy + head * math.sin(ang + math.pi / 6),
      )
      ..close();
    final headPaint = Paint()..color = _gBright;
    canvas.drawPath(p, headPaint);

    // Tip marker (bright + glow)
    final tipGlow = Paint()
      ..color = _gBright.withOpacity(0.35)
      ..style = PaintingStyle.fill
      ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 12);
    canvas.drawCircle(tip, 14, tipGlow);
    canvas.drawCircle(tip, 5.5, Paint()..color = _gBright);
  }

  @override
  bool shouldRepaint(covariant _ReticlePainter oldDelegate) {
    return oldDelegate.azimuthDeg != azimuthDeg ||
        oldDelegate.hasDirection != hasDirection ||
        oldDelegate.yCompression != yCompression;
  }
}

