import 'package:flutter/material.dart';

import '../models/packet.dart';
import '../widgets/sound_detection_hud.dart';

/// Main visualization screen optimized for a fixed upper-right HUD.
class VisualizationScreen extends StatelessWidget {
  final Packet? packet;
  final String status;

  const VisualizationScreen({
    super.key,
    required this.packet,
    required this.status,
  });

  HudConnectionState _connectionStateFor(String status) {
    final normalized = status.trim().toLowerCase();
    if (normalized.contains('disconnected')) {
      return HudConnectionState.disconnected;
    }
    if (normalized.contains('connected')) {
      return HudConnectionState.connected;
    }
    return HudConnectionState.disconnected;
  }

  @override
  Widget build(BuildContext context) {
    final connectionState = _connectionStateFor(status);
    const hudMargin = 32.0;

    return Scaffold(
      backgroundColor: Colors.black,
      body: SafeArea(
        child: LayoutBuilder(
          builder: (context, constraints) {
            final availableWidth = constraints.maxWidth - (hudMargin * 2);
            final availableHeight = constraints.maxHeight - (hudMargin * 2);
            final horizontalScale =
                (availableWidth / SoundDetectionHud.layoutWidth)
                    .clamp(0.0, 1.0)
                    .toDouble();
            final verticalScale =
                (availableHeight / SoundDetectionHud.layoutHeight)
                    .clamp(0.0, 1.0)
                    .toDouble();
            final fittedScale = horizontalScale < verticalScale
                ? horizontalScale
                : verticalScale;
            final hudWidth = SoundDetectionHud.layoutWidth * fittedScale;
            final hudHeight = SoundDetectionHud.layoutHeight * fittedScale;

            return Stack(
              children: [
                Positioned(
                  bottom: hudMargin,
                  right: hudMargin,
                  child: SizedBox(
                    width: hudWidth,
                    height: hudHeight,
                    child: FittedBox(
                      alignment: Alignment.bottomRight,
                      fit: BoxFit.contain,
                      child: AnimatedSwitcher(
                        duration: const Duration(milliseconds: 220),
                        child: SoundDetectionHud(
                          key: ValueKey(
                            'hud-${connectionState.name}-${packet?.systemFault.name ?? 'none'}-${packet?.classification.name ?? 'none'}-${packet?.quadrant.name ?? 'none'}',
                          ),
                          packet: packet,
                          connectionState: connectionState,
                        ),
                      ),
                    ),
                  ),
                ),
              ],
            );
          },
        ),
      ),
    );
  }
}
