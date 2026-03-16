import 'dart:math' as math;

import 'package:flutter/material.dart';

import '../models/enums.dart';
import '../models/packet.dart';
import '../widgets/classification_display.dart';
import '../widgets/direction_reticle.dart';
import '../widgets/system_fault_banner.dart';

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

  _HudConnectionState _connectionStateFor(String status) {
    final normalized = status.trim().toLowerCase();
    if (normalized.contains('disconnected')) {
      return _HudConnectionState.disconnected;
    }
    if (normalized.contains('connected')) {
      return _HudConnectionState.connected;
    }
    if (normalized.contains('scan') ||
        normalized.contains('search') ||
        normalized.contains('connect')) {
      return _HudConnectionState.searching;
    }
    return _HudConnectionState.disconnected;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.black,
      body: SafeArea(
        child: LayoutBuilder(
          builder: (context, constraints) {
            final connectionState = _connectionStateFor(status);
            final activeFault = packet?.systemFault ?? SystemFault.none;
            final showFault =
                connectionState == _HudConnectionState.connected &&
                activeFault != SystemFault.none;
            final showLiveHud =
                connectionState == _HudConnectionState.connected &&
                packet != null &&
                activeFault == SystemFault.none;
            final horizontalInset = math.min(
              132.0,
              math.max(28.0, constraints.maxWidth * 0.08),
            );
            final verticalInset = math.min(
              96.0,
              math.max(22.0, constraints.maxHeight * 0.08),
            );
            final reticleSize = math.min(
              148.0,
              math.max(104.0, constraints.biggest.shortestSide * 0.24),
            );
            final hudLaneWidth = math.min(
              260.0,
              math.max(210.0, constraints.maxWidth * 0.18),
            );

            return Padding(
              padding: EdgeInsets.fromLTRB(
                horizontalInset,
                verticalInset,
                horizontalInset,
                verticalInset,
              ),
              child: Stack(
                children: [
                  Align(
                    alignment: Alignment.topLeft,
                    child: SizedBox(
                      width: hudLaneWidth,
                      child: Align(
                        alignment: Alignment.topCenter,
                        child: _buildStatusIndicator(status, connectionState),
                      ),
                    ),
                  ),
                  Align(
                    alignment: Alignment.bottomLeft,
                    child: SizedBox(
                      width: hudLaneWidth,
                      child: AnimatedSwitcher(
                        duration: const Duration(milliseconds: 220),
                        child: Column(
                          key: ValueKey(
                            'hud-${connectionState.name}-${activeFault.name}-${packet?.classification.name ?? 'none'}',
                          ),
                          mainAxisSize: MainAxisSize.min,
                          children: [
                            if (showFault)
                              SizedBox(
                                width: hudLaneWidth,
                                child: SystemFaultBanner(fault: activeFault),
                              ),
                            if (showLiveHud) ...[
                              DirectionReticle(
                                quadrant: packet!.quadrant,
                                size: reticleSize,
                                yCompression: 0.48,
                              ),
                              const SizedBox(height: 14),
                            ],
                            _buildFooterCard(
                              connectionState: connectionState,
                              showFault: showFault,
                            ),
                          ],
                        ),
                      ),
                    ),
                  ),
                ],
              ),
            );
          },
        ),
      ),
    );
  }

  Widget _buildStatusIndicator(
    String status,
    _HudConnectionState connectionState,
  ) {
    final greenColor = switch (connectionState) {
      _HudConnectionState.connected => const Color(0xFF00FF00),
      _HudConnectionState.searching => const Color(0xFF66FF66),
      _HudConnectionState.disconnected => const Color(0xFF00AA00),
    };
    final icon = switch (connectionState) {
      _HudConnectionState.connected => Icons.bluetooth_connected,
      _HudConnectionState.searching => Icons.bluetooth_searching,
      _HudConnectionState.disconnected => Icons.bluetooth_disabled,
    };

    return Container(
      constraints: const BoxConstraints(maxWidth: 240),
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 7),
      decoration: BoxDecoration(
        color: greenColor.withValues(alpha: 0.14),
        borderRadius: BorderRadius.circular(18),
        border: Border.all(color: greenColor.withValues(alpha: 0.92), width: 1),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(icon, color: greenColor, size: 13),
          const SizedBox(width: 6),
          Flexible(
            child: Text(
              status.toUpperCase(),
              overflow: TextOverflow.ellipsis,
              style: TextStyle(
                color: greenColor,
                fontSize: 11,
                fontWeight: FontWeight.bold,
                letterSpacing: 0.9,
              ),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildFooterCard({
    required _HudConnectionState connectionState,
    required bool showFault,
  }) {
    if (showFault) {
      return const SizedBox.shrink();
    }

    if (connectionState == _HudConnectionState.disconnected) {
      return _buildNoticeChip(
        key: const ValueKey('bluetooth-disconnected'),
        icon: Icons.bluetooth_disabled,
        label: 'BLUETOOTH DISCONNECTED',
      );
    }

    if (connectionState == _HudConnectionState.searching) {
      return _buildNoticeChip(
        key: const ValueKey('bluetooth-searching'),
        icon: Icons.bluetooth_searching,
        label: 'SEARCHING FOR DEVICE',
      );
    }

    if (packet == null) {
      return _buildNoticeChip(
        key: const ValueKey('waiting-audio'),
        icon: Icons.hearing_outlined,
        label: 'WAITING FOR AUDIO',
      );
    }

    return ClassificationDisplay(
      key: const ValueKey('classification-display'),
      classification: packet!.classification,
      priority: packet!.priority,
    );
  }

  Widget _buildNoticeChip({
    required Key key,
    required IconData icon,
    required String label,
  }) {
    return Container(
      key: key,
      constraints: const BoxConstraints(maxWidth: 210),
      padding: const EdgeInsets.symmetric(horizontal: 14, vertical: 10),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha: 0.58),
        borderRadius: BorderRadius.circular(12),
        border: Border.all(
          color: const Color(0xFF00AA00).withValues(alpha: 0.8),
          width: 1.2,
        ),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(
            icon,
            size: 16,
            color: const Color(0xFF00AA00).withValues(alpha: 0.9),
          ),
          const SizedBox(width: 8),
          Flexible(
            child: Text(
              label,
              style: TextStyle(
                color: const Color(0xFF00AA00).withValues(alpha: 0.9),
                fontSize: 12,
                fontWeight: FontWeight.bold,
                letterSpacing: 1.1,
              ),
            ),
          ),
        ],
      ),
    );
  }
}

enum _HudConnectionState { connected, searching, disconnected }
