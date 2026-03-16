import 'package:flutter/material.dart';

import '../models/enums.dart';

/// Compact fault banner that takes priority over normal HUD content.
class SystemFaultBanner extends StatelessWidget {
  final SystemFault fault;

  const SystemFaultBanner({super.key, required this.fault});

  IconData _iconFor(SystemFault fault) {
    switch (fault) {
      case SystemFault.hardware:
        return Icons.memory_outlined;
      case SystemFault.classification:
        return Icons.hearing_disabled_outlined;
      case SystemFault.doa:
        return Icons.explore_off_outlined;
      case SystemFault.none:
        return Icons.report_problem_outlined;
    }
  }

  String _titleFor(SystemFault fault) {
    switch (fault) {
      case SystemFault.hardware:
        return 'HARDWARE FAULT';
      case SystemFault.classification:
        return 'CLASSIFICATION FAULT';
      case SystemFault.doa:
        return 'DIRECTION FAULT';
      case SystemFault.none:
        return 'SYSTEM FAULT';
    }
  }

  String _messageFor(SystemFault fault) {
    switch (fault) {
      case SystemFault.hardware:
        return 'Check sensors and controller link.';
      case SystemFault.classification:
        return 'Audio classification is unavailable.';
      case SystemFault.doa:
        return 'Direction analysis is unavailable.';
      case SystemFault.none:
        return 'Safety features are limited.';
    }
  }

  @override
  Widget build(BuildContext context) {
    const color = Color(0xFF7CFF7C);

    return Container(
      constraints: const BoxConstraints(maxWidth: 290),
      padding: const EdgeInsets.symmetric(horizontal: 14, vertical: 12),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha: 0.68),
        borderRadius: BorderRadius.circular(14),
        border: Border.all(color: color.withValues(alpha: 0.9), width: 1.4),
        boxShadow: [
          BoxShadow(
            color: color.withValues(alpha: 0.16),
            blurRadius: 10,
            spreadRadius: 0.8,
          ),
        ],
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(_iconFor(fault), color: color, size: 18),
          const SizedBox(width: 10),
          Flexible(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              mainAxisSize: MainAxisSize.min,
              children: [
                Text(
                  _titleFor(fault),
                  style: const TextStyle(
                    color: color,
                    fontSize: 13,
                    fontWeight: FontWeight.bold,
                    letterSpacing: 1.0,
                  ),
                ),
                const SizedBox(height: 2),
                Text(
                  _messageFor(fault),
                  style: TextStyle(
                    color: color.withValues(alpha: 0.82),
                    fontSize: 11,
                    letterSpacing: 0.2,
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
