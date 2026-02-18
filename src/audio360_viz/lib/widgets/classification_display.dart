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
      case Classification.siren:
        return Icons.warning;
      case Classification.carHorn:
        return Icons.directions_car;
      case Classification.jackHammer:
        return Icons.build;
      case Classification.unKnown:
        return Icons.help_outline;
    }
  }

  /// Get color for classification (green monochrome display)
  /// Uses different brightness levels of green to differentiate
  Color _getColor(Classification classification) {
    switch (classification) {
      case Classification.siren:
        return const Color(0xFF00FF00); // Bright green - highest priority
      case Classification.carHorn:
        return const Color(0xFF66FF66); // Medium-bright green
      case Classification.jackHammer:
        return const Color(0xFF33FF33); // Medium green
      case Classification.unKnown:
        return const Color(0xFF00AA00); // Dim green
    }
  }

  /// Get display name for classification
  String _getName(Classification classification) {
    switch (classification) {
      case Classification.siren:
        return 'SIREN';
      case Classification.carHorn:
        return 'CAR HORN';
      case Classification.jackHammer:
        return 'JACKHAMMER';
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
      padding: const EdgeInsets.symmetric(horizontal: 18, vertical: 12),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha:0.65),
        borderRadius: BorderRadius.circular(14),
        border: Border.all(
          color: color,
          width: 1.8,
        ),
        boxShadow: [
          BoxShadow(
            // Keep bloom controlled for optical displays
            color: color.withValues(alpha:0.22),
            blurRadius: 10,
            spreadRadius: 1,
          ),
        ],
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          // Icon
          Icon(
            icon,
            color: color,
            size: 26,
          ),
          const SizedBox(width: 10),
          // Text
          Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            mainAxisSize: MainAxisSize.min,
            children: [
              Text(
                name,
                style: TextStyle(
                  color: color,
                  fontSize: 18,
                  fontWeight: FontWeight.bold,
                  letterSpacing: 1.2,
                  shadows: [
                    Shadow(
                      color: Colors.black,
                      blurRadius: 4,
                    ),
                  ],
                ),
              ),
              if (priority > 0)
                Text(
                  'Priority: $priority',
                  style: TextStyle(
                    color: color.withValues(alpha:0.8), // Use classification color
                    fontSize: 11,
                    shadows: [
                      Shadow(
                        color: Colors.black,
                        blurRadius: 2,
                      ),
                    ],
                  ),
                ),
            ],
          ),
        ],
      ),
    );
  }
}
