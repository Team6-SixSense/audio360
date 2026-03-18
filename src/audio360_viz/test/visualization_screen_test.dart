import 'package:audio360_viz/models/enums.dart';
import 'package:audio360_viz/models/packet.dart';
import 'package:audio360_viz/screens/visualization_screen.dart';
import 'package:audio360_viz/widgets/direction_reticle.dart';
import 'package:audio360_viz/widgets/sound_detection_hud.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  Packet buildPacket({
    Classification classification = Classification.siren,
    Quadrant quadrant = Quadrant.northEast,
    SystemFault systemFault = SystemFault.none,
    int priority = 1,
  }) {
    return Packet(
      classification: classification,
      quadrant: quadrant,
      systemFault: systemFault,
      priority: priority,
    );
  }

  Future<void> pumpScreen(
    WidgetTester tester, {
    Packet? packet,
    required String status,
  }) async {
    await tester.pumpWidget(
      MaterialApp(
        home: VisualizationScreen(packet: packet, status: status),
      ),
    );
    await tester.pumpAndSettle();
  }

  void setWideViewport(WidgetTester tester) {
    tester.view.physicalSize = const Size(1920, 1080);
    tester.view.devicePixelRatio = 1.0;
    addTearDown(() {
      tester.view.resetPhysicalSize();
      tester.view.resetDevicePixelRatio();
    });
  }

  testWidgets('shows bluetooth disconnect notice when link drops', (
    tester,
  ) async {
    await pumpScreen(tester, status: 'Bluetooth disconnected');

    expect(
      find.byKey(const ValueKey('bluetooth-disconnected')),
      findsOneWidget,
    );
    expect(find.byKey(const ValueKey('notification-panel')), findsNothing);
    expect(find.byKey(const ValueKey('fault-banner')), findsNothing);
    expect(find.byType(DirectionReticle), findsNothing);
  });

  testWidgets(
    'treats searching status as disconnected for bluetooth icon state',
    (tester) async {
      await pumpScreen(tester, status: 'Searching for device');

      expect(
        find.byKey(const ValueKey('bluetooth-disconnected')),
        findsOneWidget,
      );
      expect(find.byKey(const ValueKey('bluetooth-connected')), findsNothing);
      expect(find.byKey(const ValueKey('notification-panel')), findsNothing);
    },
  );

  testWidgets('prioritizes system fault content over live detection content', (
    tester,
  ) async {
    setWideViewport(tester);
    await pumpScreen(
      tester,
      packet: buildPacket(systemFault: SystemFault.hardware),
      status: 'Bluetooth connected',
    );

    expect(find.text('HARDWARE FAULT'), findsOneWidget);
    expect(find.byKey(const ValueKey('fault-banner')), findsOneWidget);
    expect(find.byKey(const ValueKey('bluetooth-connected')), findsNothing);
    expect(find.byKey(const ValueKey('notification-panel')), findsNothing);
    expect(find.byKey(const ValueKey('classification-display')), findsNothing);
    expect(find.byType(DirectionReticle), findsNothing);

    final screenSize = tester.view.physicalSize / tester.view.devicePixelRatio;
    final hudRect = tester.getRect(find.byType(SoundDetectionHud));
    expect(hudRect.top, equals(32));
    expect(hudRect.right, equals(screenSize.width - 32));
  });

  testWidgets('renders the live detection hud in the upper right corner', (
    tester,
  ) async {
    setWideViewport(tester);
    await pumpScreen(
      tester,
      packet: buildPacket(
        classification: Classification.carHorn,
        quadrant: Quadrant.east,
        priority: 2,
      ),
      status: 'Bluetooth connected',
    );

    expect(
      find.byKey(const ValueKey('classification-display')),
      findsOneWidget,
    );
    expect(find.text('SOUND DETECTED'), findsOneWidget);
    expect(find.text('Car Horn'), findsOneWidget);
    expect(find.byType(DirectionReticle), findsOneWidget);
    expect(find.byKey(const ValueKey('bluetooth-connected')), findsOneWidget);
    expect(find.byKey(const ValueKey('notification-panel')), findsOneWidget);

    final screenSize = tester.view.physicalSize / tester.view.devicePixelRatio;
    final hudRect = tester.getRect(find.byType(SoundDetectionHud));
    final panelRect = tester.getRect(
      find.byKey(const ValueKey('notification-panel')),
    );
    final bluetoothRect = tester.getRect(
      find.byKey(const ValueKey('bluetooth-connected')),
    );
    final subjectIconRect = tester.getRect(
      find.byKey(const ValueKey('subject-icon')),
    );
    final reticleRect = tester.getRect(find.byType(DirectionReticle));
    final titleRect = tester.getRect(find.text('SOUND DETECTED'));
    final classificationRect = tester.getRect(
      find.byKey(const ValueKey('classification-display')),
    );
    final textBlockCenterY =
        (titleRect.center.dy + classificationRect.center.dy) / 2;
    expect(hudRect.top, closeTo(32, 0.01));
    expect(hudRect.right, closeTo(screenSize.width - 32, 0.01));
    expect(hudRect.width, closeTo(SoundDetectionHud.layoutWidth, 0.01));
    expect(panelRect.width, closeTo(SoundDetectionHud.panelWidth, 0.01));
    expect(panelRect.height, closeTo(SoundDetectionHud.panelHeight, 0.01));
    expect(bluetoothRect.width, closeTo(SoundDetectionHud.bluetoothSize, 0.01));
    expect(
      (subjectIconRect.center.dy - classificationRect.center.dy).abs(),
      lessThan(6),
    );
    expect(reticleRect.center.dx, greaterThan(classificationRect.center.dx));
    expect((reticleRect.center.dy - textBlockCenterY).abs(), lessThan(8));
  });

}
