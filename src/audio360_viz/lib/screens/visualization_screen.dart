import 'package:flutter/material.dart';
import '../models/packet.dart';
import '../models/enums.dart';
import '../widgets/direction_reticle.dart';
import '../widgets/classification_display.dart';

/// Main visualization screen optimized for smart glasses.
/// Displays directionality and classification in an AR-friendly layout.
class VisualizationScreen extends StatelessWidget {
  final Packet? packet;
  final String status;

  const VisualizationScreen({
    super.key,
    required this.packet,
    required this.status,
  });

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.black,
      body: SafeArea(
        child: Stack(
          children: [
            // Center reticle
            Align(
              alignment: Alignment.center,
              child: DirectionReticle(
                quadrant: packet?.quadrant ?? Quadrant.none,
                size: 260,
                yCompression: 0.55,
              ),
            ),

            // Top status (small, out of the way)
            Align(
              alignment: Alignment.topCenter,
              child: Padding(
                padding: const EdgeInsets.only(top: 12),
                child: _buildStatusIndicator(status),
              ),
            ),

            // Bottom classification (compact)
            Align(
              alignment: Alignment.bottomCenter,
              child: Padding(
                padding: const EdgeInsets.only(bottom: 22),
                child: packet == null
                    ? _buildWaitingChip()
                    : ClassificationDisplay(
                        classification: packet!.classification,
                        priority: packet!.priority,
                      ),
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildStatusIndicator(String status) {
    final isConnected = status.toLowerCase().contains('connected');
    // Use different green brightness for connected vs disconnected
    final greenColor = isConnected 
        ? const Color(0xFF00FF00) // Bright green for connected
        : const Color(0xFF00AA00); // Dim green for disconnected
    
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
      decoration: BoxDecoration(
        color: greenColor.withValues(alpha:0.2),
        borderRadius: BorderRadius.circular(20),
        border: Border.all(
          color: greenColor,
          width: 1,
        ),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Container(
            width: 8,
            height: 8,
            decoration: BoxDecoration(
              shape: BoxShape.circle,
              color: greenColor,
            ),
          ),
          const SizedBox(width: 8),
          Text(
            status.toUpperCase(),
            style: TextStyle(
              color: greenColor,
              fontSize: 12,
              fontWeight: FontWeight.bold,
              letterSpacing: 1,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildWaitingChip() {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 18, vertical: 10),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha:0.65),
        borderRadius: BorderRadius.circular(14),
        border: Border.all(
          color: const Color(0xFF00AA00).withValues(alpha:0.8),
          width: 1.5,
        ),
      ),
      child: Text(
        'WAITING',
        style: TextStyle(
          color: const Color(0xFF00AA00).withValues(alpha:0.9),
          fontSize: 14,
          fontWeight: FontWeight.bold,
          letterSpacing: 2,
        ),
      ),
    );
  }
}
