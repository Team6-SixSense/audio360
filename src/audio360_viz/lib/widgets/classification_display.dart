import 'package:flutter/material.dart';
import '../models/enums.dart';

/// A widget that displays classification information optimized for smart glasses.
/// Shows icon, text, and color coding for different audio classifications.
class ClassificationDisplay extends StatelessWidget {
  final Classification classification;
  final int priority;

  const ClassificationDisplay({
    super.key,
    required this.classification,
    required this.priority,
  });

  /// Get icon for classification
  IconData _getIcon(Classification classification) {
    switch (classification) {
      case Classification.fire:
        return Icons.local_fire_department;
      case Classification.engine:
        return Icons.hearing;
      case Classification.truckReversing:
        return Icons.local_shipping;
      case Classification.unKnown:
        return Icons.help_outline;
    }
  }

  /// Get color for classification (green monochrome display)
  /// Uses different brightness levels of green to differentiate
  Color _getColor(Classification classification) {
    switch (classification) {
      case Classification.fire:
        return const Color(0xFFFF4444); // High alert
      case Classification.engine:
        return const Color(0xFF66FF66); // Medium-bright green
      case Classification.truckReversing:
        return const Color(0xFF33FFCC); // Vehicle motion
      case Classification.unKnown:
        return const Color(0xFF00AA00); // Dim green
    }
  }

  /// Get display name for classification
  String _getName(Classification classification) {
    switch (classification) {
      case Classification.fire:
        return 'FIRE';
      case Classification.engine:
        return 'ENGINE';
      case Classification.truckReversing:
        return 'TRUCK REVERSING';
      case Classification.unKnown:
        return 'UNKNOWN';
    }
  }

  @override
  Widget build(BuildContext context) {
    final color = _getColor(classification);
    final icon = _getIcon(classification);
    final name = _getName(classification);

    return Container(
      constraints: const BoxConstraints(maxWidth: 190),
      padding: const EdgeInsets.symmetric(horizontal: 14, vertical: 10),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha: 0.58),
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: color, width: 1.4),
        boxShadow: [
          BoxShadow(
            // Keep bloom controlled for optical displays.
            color: color.withValues(alpha: 0.16),
            blurRadius: 8,
            spreadRadius: 0.6,
          ),
        ],
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(icon, color: color, size: 18),
          const SizedBox(width: 8),
          Flexible(
            child: Text(
              name,
              overflow: TextOverflow.ellipsis,
              style: TextStyle(
                color: color,
                fontSize: 14,
                fontWeight: FontWeight.bold,
                letterSpacing: 1.0,
                shadows: const [Shadow(color: Colors.black, blurRadius: 3)],
              ),
            ),
          ),
        ],
      ),
    );
  }
}
