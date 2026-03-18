import 'package:flutter/material.dart';

import '../models/enums.dart';
import '../models/packet.dart';
import 'direction_reticle.dart';

enum HudConnectionState { connected, disconnected }

class SoundDetectionHud extends StatelessWidget {
  static const double uiScale = 1.18;

  static const double bluetoothSize = 44 * uiScale;
  static const double bluetoothTopOffset = 10 * uiScale;
  static const double bluetoothGap = 48 * uiScale;
  static const double panelWidth = 513 * uiScale;
  static const double panelHeight = 153 * uiScale;
  static const double subjectTop = 68.5 * uiScale;
  static const double subjectIconSize = 44 * uiScale;
  static const double reticleSize = 210 * uiScale;
  static const double reticleHorizontalShift = 28 * uiScale;
  static const double textBlockCenterY = (subjectTop + subjectIconSize) / 2;
  static const double reticleTop = textBlockCenterY - (reticleSize / 2);
  static const double layoutWidth = bluetoothSize + bluetoothGap + panelWidth;
  static const double layoutHeight = panelHeight;

  static const Color _green = Color(0xFF29FF66);
  static const Color _faultGreen = Color(0xFF6FFF8F);

  final Packet? packet;
  final HudConnectionState connectionState;

  const SoundDetectionHud({
    super.key,
    required this.packet,
    required this.connectionState,
  });

  bool get _hasFault =>
      connectionState == HudConnectionState.connected &&
      packet != null &&
      packet!.systemFault != SystemFault.none;

  @override
  Widget build(BuildContext context) {
    if (_hasFault) {
      return SizedBox(
        key: const ValueKey('sound-detection-hud'),
        width: layoutWidth,
        height: layoutHeight,
        child: Align(
          alignment: Alignment.topRight,
          child: _FaultBanner(fault: packet!.systemFault),
        ),
      );
    }

    final signalColor = switch (connectionState) {
      HudConnectionState.connected => _green,
      HudConnectionState.disconnected => _green.withValues(alpha: 0.3),
    };

    return SizedBox(
      key: const ValueKey('sound-detection-hud'),
      width: layoutWidth,
      height: layoutHeight,
      child: RepaintBoundary(
        child: Row(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Padding(
              padding: const EdgeInsets.only(top: bluetoothTopOffset),
              child: _BluetoothIndicator(connectionState: connectionState),
            ),
            const SizedBox(width: bluetoothGap),
            _NotificationPanel(
              packet: packet,
              connectionState: connectionState,
              signalColor: signalColor,
            ),
          ],
        ),
      ),
    );
  }
}

class _NotificationPanel extends StatelessWidget {
  final Packet? packet;
  final HudConnectionState connectionState;
  final Color signalColor;

  const _NotificationPanel({
    required this.packet,
    required this.connectionState,
    required this.signalColor,
  });

  bool get _hasDetection =>
      connectionState == HudConnectionState.connected &&
      packet != null &&
      packet!.systemFault == SystemFault.none;

  @override
  Widget build(BuildContext context) {
    if (!_hasDetection) {
      return const SizedBox(
        key: ValueKey('notification-panel-empty'),
        width: SoundDetectionHud.panelWidth,
        height: SoundDetectionHud.panelHeight,
      );
    }

    return SizedBox(
      key: const ValueKey('notification-panel'),
      width: SoundDetectionHud.panelWidth,
      height: SoundDetectionHud.panelHeight,
      child: Stack(
        clipBehavior: Clip.none,
        children: [
          Positioned(
            left: 0,
            top: 0,
            child: Text(
              'SOUND DETECTED',
              style: TextStyle(
                color: signalColor,
                fontSize: 36 * SoundDetectionHud.uiScale,
                fontWeight: FontWeight.w700,
                letterSpacing: 0.8,
                height: 1,
                shadows: _textGlow(signalColor, 0.58),
              ),
            ),
          ),
          Positioned(
            left: 0,
            top: SoundDetectionHud.subjectTop,
            child: Row(
              crossAxisAlignment: CrossAxisAlignment.center,
              children: [
                Text(
                  _subjectLabel(packet!.classification),
                  key: const ValueKey('classification-display'),
                  style: TextStyle(
                    color: signalColor,
                    fontSize: 40 * SoundDetectionHud.uiScale,
                    fontWeight: FontWeight.w400,
                    height: 1,
                    shadows: _textGlow(signalColor, 0.46),
                  ),
                ),
                SizedBox(width: 14 * SoundDetectionHud.uiScale),
                Transform.translate(
                  offset: Offset(0, -1 * SoundDetectionHud.uiScale),
                  child: SizedBox(
                    key: const ValueKey('subject-icon'),
                    width: SoundDetectionHud.subjectIconSize,
                    height: SoundDetectionHud.subjectIconSize,
                    child: Center(
                      child: Icon(
                        _subjectIcon(packet!.classification),
                        size: SoundDetectionHud.subjectIconSize,
                        color: signalColor,
                        shadows: _iconGlow(signalColor, 0.42),
                      ),
                    ),
                  ),
                ),
              ],
            ),
          ),
          if (packet!.quadrant != Quadrant.none)
            Positioned(
              left:
                  SoundDetectionHud.panelWidth - SoundDetectionHud.reticleSize,
              top: SoundDetectionHud.reticleTop,
              child: Transform.translate(
                offset: const Offset(
                  SoundDetectionHud.reticleHorizontalShift,
                  0,
                ),
                child: DirectionReticle(
                  key: const ValueKey('direction-reticle'),
                  quadrant: packet!.quadrant,
                  size: SoundDetectionHud.reticleSize,
                  yCompression: 0.48,
                ),
              ),
            ),
        ],
      ),
    );
  }

  List<Shadow> _textGlow(Color color, double opacity) {
    return [
      Shadow(
        color: color.withValues(alpha: opacity),
        blurRadius: 18 * SoundDetectionHud.uiScale,
      ),
      Shadow(
        color: color.withValues(alpha: opacity * 0.7),
        blurRadius: 8 * SoundDetectionHud.uiScale,
      ),
    ];
  }

  List<Shadow> _iconGlow(Color color, double opacity) {
    return [
      Shadow(
        color: color.withValues(alpha: opacity),
        blurRadius: 14 * SoundDetectionHud.uiScale,
      ),
      Shadow(
        color: color.withValues(alpha: opacity * 0.6),
        blurRadius: 7 * SoundDetectionHud.uiScale,
      ),
    ];
  }

  String _subjectLabel(Classification classification) {
    return switch (classification) {
      Classification.siren => 'Siren',
      Classification.carHorn => 'Car Horn',
      Classification.jackHammer => 'Jackhammer',
      Classification.unKnown => 'Unknown',
    };
  }

  IconData _subjectIcon(Classification classification) {
    return switch (classification) {
      Classification.siren => Icons.warning_rounded,
      Classification.carHorn => Icons.directions_car_filled_rounded,
      Classification.jackHammer => Icons.construction_rounded,
      Classification.unKnown => Icons.hearing_rounded,
    };
  }
}

class _FaultBanner extends StatelessWidget {
  final SystemFault fault;

  const _FaultBanner({required this.fault});

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      key: const ValueKey('fault-banner'),
      width: SoundDetectionHud.panelWidth,
      height: SoundDetectionHud.panelHeight,
      child: Stack(
        children: [
          Positioned(
            left: 0,
            top: 0,
            child: Text(
              _titleFor(fault),
              style: TextStyle(
                color: SoundDetectionHud._faultGreen,
                fontSize: 36 * SoundDetectionHud.uiScale,
                fontWeight: FontWeight.w700,
                letterSpacing: 0.8,
                height: 1,
                shadows: [
                  Shadow(
                    color: SoundDetectionHud._faultGreen.withValues(alpha: 0.5),
                    blurRadius: 18 * SoundDetectionHud.uiScale,
                  ),
                ],
              ),
            ),
          ),
          Positioned(
            left: 0,
            top: 74 * SoundDetectionHud.uiScale,
            child: Row(
              children: [
                Icon(
                  _iconFor(fault),
                  size: 36 * SoundDetectionHud.uiScale,
                  color: SoundDetectionHud._faultGreen,
                  shadows: [
                    Shadow(
                      color: SoundDetectionHud._faultGreen.withValues(
                        alpha: 0.35,
                      ),
                      blurRadius: 12 * SoundDetectionHud.uiScale,
                    ),
                  ],
                ),
                SizedBox(width: 14 * SoundDetectionHud.uiScale),
                Text(
                  _messageFor(fault),
                  style: TextStyle(
                    color: SoundDetectionHud._faultGreen.withValues(
                      alpha: 0.94,
                    ),
                    fontSize: 24 * SoundDetectionHud.uiScale,
                    fontWeight: FontWeight.w400,
                    height: 1.1,
                    shadows: [
                      Shadow(
                        color: SoundDetectionHud._faultGreen.withValues(
                          alpha: 0.3,
                        ),
                        blurRadius: 10 * SoundDetectionHud.uiScale,
                      ),
                    ],
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }

  String _titleFor(SystemFault fault) {
    return switch (fault) {
      SystemFault.hardware => 'HARDWARE FAULT',
      SystemFault.classification => 'CLASSIFICATION FAULT',
      SystemFault.doa => 'DIRECTION FAULT',
      SystemFault.none => 'SYSTEM FAULT',
    };
  }

  String _messageFor(SystemFault fault) {
    return switch (fault) {
      SystemFault.hardware => 'Check sensors and controller link.',
      SystemFault.classification => 'Audio classification unavailable.',
      SystemFault.doa => 'Direction analysis unavailable.',
      SystemFault.none => 'Safety features limited.',
    };
  }

  IconData _iconFor(SystemFault fault) {
    return switch (fault) {
      SystemFault.hardware => Icons.memory_rounded,
      SystemFault.classification => Icons.hearing_disabled_rounded,
      SystemFault.doa => Icons.explore_off_rounded,
      SystemFault.none => Icons.warning_amber_rounded,
    };
  }
}

class _BluetoothIndicator extends StatelessWidget {
  final HudConnectionState connectionState;

  const _BluetoothIndicator({required this.connectionState});

  static const Color _green = SoundDetectionHud._green;

  @override
  Widget build(BuildContext context) {
    final active = connectionState == HudConnectionState.connected;
    final color = switch (connectionState) {
      HudConnectionState.connected => _green,
      HudConnectionState.disconnected => _green.withValues(alpha: 0.3),
    };
    final icon = switch (connectionState) {
      HudConnectionState.connected => Icons.bluetooth_rounded,
      HudConnectionState.disconnected => Icons.bluetooth_disabled_rounded,
    };

    return SizedBox(
      key: ValueKey('bluetooth-${connectionState.name}'),
      width: SoundDetectionHud.bluetoothSize,
      height: SoundDetectionHud.bluetoothSize,
      child: Icon(
        icon,
        color: color,
        size: SoundDetectionHud.bluetoothSize,
        shadows: [
          if (active)
            Shadow(
              color: color.withValues(alpha: 0.9),
              blurRadius: 20 * SoundDetectionHud.uiScale,
            ),
        ],
      ),
    );
  }
}
