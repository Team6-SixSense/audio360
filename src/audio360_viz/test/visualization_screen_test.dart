import 'package:audio360_viz/models/enums.dart';
import 'package:audio360_viz/models/packet.dart';
import 'package:audio360_viz/screens/visualization_screen.dart';
import 'package:audio360_viz/widgets/direction_reticle.dart';
import 'package:audio360_viz/widgets/system_fault_banner.dart';
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

    expect(find.text('BLUETOOTH DISCONNECTED'), findsNWidgets(2));
    expect(find.byType(DirectionReticle), findsNothing);
  });

  testWidgets('prioritizes system fault banner over live HUD content', (
    tester,
  ) async {
    setWideViewport(tester);
    await pumpScreen(
      tester,
      packet: buildPacket(systemFault: SystemFault.hardware),
      status: 'Bluetooth connected',
    );

    expect(find.text('HARDWARE FAULT'), findsOneWidget);
    expect(find.byKey(const ValueKey('classification-display')), findsNothing);
    expect(find.byType(DirectionReticle), findsNothing);

    final screenSize = tester.view.physicalSize / tester.view.devicePixelRatio;
    final bannerRect = tester.getRect(find.byType(SystemFaultBanner));
    expect(bannerRect.top, greaterThan(screenSize.height * 0.06));
    expect(bannerRect.left, greaterThan(screenSize.width * 0.05));
    expect(bannerRect.right, lessThan(screenSize.width * 0.28));
  });

  testWidgets('renders a smaller live reticle for non-intrusive HUD mode', (
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
    expect(find.byType(DirectionReticle), findsOneWidget);

    final reticleSize = tester.getSize(find.byType(DirectionReticle));
    final screenSize = tester.view.physicalSize / tester.view.devicePixelRatio;
    final reticleRect = tester.getRect(find.byType(DirectionReticle));
    final classificationRect = tester.getRect(
      find.byKey(const ValueKey('classification-display')),
    );
    expect(reticleSize.width, lessThan(170));
    expect(reticleRect.left, greaterThan(screenSize.width * 0.05));
    expect(reticleRect.right, lessThan(screenSize.width * 0.24));
    expect(
      (reticleRect.center.dx - classificationRect.center.dx).abs(),
      lessThan(8),
    );
  });
}
