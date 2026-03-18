import 'dart:math' as math;

import 'package:flutter/material.dart';

import '../models/enums.dart';

/// Transverse-plane direction reticle for monochrome-green optics.
class DirectionReticle extends StatelessWidget {
  final Quadrant quadrant;
  final double size;

  /// Lower values flatten the reticle for a more lens-like perspective.
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

  static const _gBright = Color(0xFF29FF66);
  static const _gDim = Color(0xFF0FA83A);

  @override
  void paint(Canvas canvas, Size size) {
    final scale = (size.width / 240).clamp(0.45, 1.0).toDouble();
    final center = Offset(size.width / 2, size.height / 2);
    final radius = size.width * 0.36;

    final ellipseRect = Rect.fromCenter(
      center: center,
      width: radius * 2,
      height: radius * 2 * yCompression,
    );

    final vignettePaint = Paint()
      ..shader = RadialGradient(
        colors: [
          Colors.black.withValues(alpha: 0.28),
          Colors.black.withValues(alpha: 0.0),
        ],
      ).createShader(Rect.fromCircle(center: center, radius: radius * 1.35));
    canvas.drawRect(Offset.zero & size, vignettePaint);

    _drawConcentricRings(canvas, center, radius);
    _drawCrosshairs(canvas, center, radius);
    _drawCardinalTicks(canvas, center, radius);

    final matte = Paint()..color = Colors.black.withValues(alpha: 0.34);
    canvas.drawOval(
      Rect.fromCenter(
        center: center,
        width: radius * 0.88,
        height: radius * 0.88 * yCompression,
      ),
      matte,
    );

    final outline = Paint()
      ..color = _gBright.withValues(alpha: 0.78)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2.0 * scale;
    canvas.drawOval(ellipseRect, outline);

    if (hasDirection) {
      _drawArrow(canvas, center, radius);
    } else {
      final dot = Paint()..color = _gDim.withValues(alpha: 0.5);
      canvas.drawCircle(center, 2.2 * scale, dot);
    }
  }

  void _drawConcentricRings(Canvas canvas, Offset center, double radius) {
    final scale = (radius / 86).clamp(0.45, 1.0).toDouble();

    for (int i = 1; i <= 4; i++) {
      final r = radius * (i / 4);
      final rect = Rect.fromCenter(
        center: center,
        width: r * 2,
        height: r * 2 * yCompression,
      );

      final opacity = (0.18 + (0.1 * (4 - i))) * 0.8;
      final paint = Paint()
        ..color = _gDim.withValues(alpha: opacity)
        ..style = PaintingStyle.stroke
        ..strokeWidth = (i == 1 ? 1.2 : 0.85) * scale;
      canvas.drawOval(rect, paint);
    }
  }

  void _drawCrosshairs(Canvas canvas, Offset center, double radius) {
    final scale = (radius / 86).clamp(0.45, 1.0).toDouble();
    final vertical = Paint()
      ..color = _gBright.withValues(alpha: 0.72)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.6 * scale;
    final horizontal = Paint()
      ..color = _gBright.withValues(alpha: 0.62)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.35 * scale;

    final verticalExtend = radius * yCompression;
    final horizontalExtend = radius * 1.04;
    canvas.drawLine(
      Offset(center.dx, center.dy - verticalExtend),
      Offset(center.dx, center.dy + verticalExtend),
      vertical,
    );
    canvas.drawLine(
      Offset(center.dx - horizontalExtend, center.dy),
      Offset(center.dx + horizontalExtend, center.dy),
      horizontal,
    );

    canvas.drawCircle(center, 2.0 * scale, Paint()..color = _gBright);
  }

  void _drawCardinalTicks(Canvas canvas, Offset center, double radius) {
    final scale = (radius / 86).clamp(0.45, 1.0).toDouble();
    const angles = <double>[0, 90, 180, 270];
    final tickPaint = Paint()
      ..color = _gBright.withValues(alpha: 0.72)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.4 * scale;

    final tickLength = 8.0 * scale;
    for (final deg in angles) {
      final angle = deg * (math.pi / 180);
      final x = center.dx + radius * math.cos(angle);
      final y = center.dy - radius * math.sin(angle) * yCompression;

      final perpendicular = angle + math.pi / 2;
      final start = Offset(
        x + tickLength * 0.2 * math.cos(perpendicular),
        y - tickLength * 0.2 * math.sin(perpendicular),
      );
      final end = Offset(
        x + tickLength * 1.2 * math.cos(perpendicular),
        y - tickLength * 1.2 * math.sin(perpendicular),
      );
      canvas.drawLine(start, end, tickPaint);
    }
  }

  void _drawArrow(Canvas canvas, Offset center, double radius) {
    final scale = (radius / 86).clamp(0.45, 1.0).toDouble();
    final angle = (azimuthDeg - 90) * (math.pi / 180);
    final length = radius * 0.68;

    final tip = Offset(
      center.dx + length * math.cos(angle),
      center.dy + length * math.sin(angle) * yCompression,
    );

    final glow = Paint()
      ..color = _gBright.withValues(alpha: 0.2)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 6.0 * scale
      ..strokeCap = StrokeCap.round
      ..maskFilter = MaskFilter.blur(BlurStyle.normal, 7 * scale);
    final shaft = Paint()
      ..color = _gBright.withValues(alpha: 0.9)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2.7 * scale
      ..strokeCap = StrokeCap.round;

    canvas.drawLine(center, tip, glow);
    canvas.drawLine(center, tip, shaft);

    final arrowAngle = math.atan2(
      (center.dy - tip.dy) / yCompression,
      tip.dx - center.dx,
    );
    final head = 11.0 * scale;
    final path = Path()
      ..moveTo(tip.dx, tip.dy)
      ..lineTo(
        tip.dx - head * math.cos(arrowAngle - math.pi / 6),
        tip.dy + head * math.sin(arrowAngle - math.pi / 6),
      )
      ..lineTo(
        tip.dx - head * 0.75 * math.cos(arrowAngle),
        tip.dy + head * 0.75 * math.sin(arrowAngle),
      )
      ..lineTo(
        tip.dx - head * math.cos(arrowAngle + math.pi / 6),
        tip.dy + head * math.sin(arrowAngle + math.pi / 6),
      )
      ..close();
    canvas.drawPath(path, Paint()..color = _gBright);

    final tipGlow = Paint()
      ..color = _gBright.withValues(alpha: 0.24)
      ..style = PaintingStyle.fill
      ..maskFilter = MaskFilter.blur(BlurStyle.normal, 8 * scale);
    canvas.drawCircle(tip, 8.5 * scale, tipGlow);
    canvas.drawCircle(tip, 3.8 * scale, Paint()..color = _gBright);
  }

  @override
  bool shouldRepaint(covariant _ReticlePainter oldDelegate) {
    return oldDelegate.azimuthDeg != azimuthDeg ||
        oldDelegate.hasDirection != hasDirection ||
        oldDelegate.yCompression != yCompression;
  }
}
