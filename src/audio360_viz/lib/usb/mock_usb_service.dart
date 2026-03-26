import 'dart:async';
import 'dart:math';
import '../models/packet.dart';
import '../models/enums.dart';
import '../base_data_service.dart';

/// Mock USB service for testing without hardware.
/// Simulates packet data with random classifications and directions.
final class MockUsbService extends BaseDataService {
  Timer? _timer;
  int _packetCount = 0;

  final Random _random = Random();

  MockUsbService({required super.onPacket, required super.onStatus});

  @override
  Future<void> initialize() async {
    // Simulate connection delay
    await Future.delayed(const Duration(milliseconds: 500));
    onStatus("Connected (Mock Mode)");

    // Start sending mock packets every 2 seconds
    _timer = Timer.periodic(const Duration(seconds: 2), (_) {
      _sendMockPacket();
    });

    // Send first packet immediately
    _sendMockPacket();
  }

  void _sendMockPacket() {
    // Cycle labels predictably so the main project classes all appear.
    const classifications = <Classification>[
      Classification.someoneTalking,
      Classification.siren,
      Classification.smokeAlarm,
    ];
    final classification =
        classifications[_packetCount % classifications.length];

    // Keep direction changes predictable as well.
    const quadrants = <Quadrant>[
      Quadrant.east,
      Quadrant.northEast,
      Quadrant.south,
      Quadrant.west,
    ];
    final quadrant = quadrants[_packetCount % quadrants.length];

    // Cycle faults predictably so mock mode exercises the fault banner too.
    const faultSequence = <SystemFault>[
      SystemFault.none,
      SystemFault.hardware,
      SystemFault.none,
      SystemFault.classification,
      SystemFault.none,
      SystemFault.doa,
    ];
    final systemFault = faultSequence[_packetCount % faultSequence.length];
    _packetCount++;

    // Random priority (1-5)
    final priority = _random.nextInt(5) + 1;

    final packet = Packet(
      classification: classification,
      quadrant: quadrant,
      systemFault: systemFault,
      priority: priority,
    );

    onPacket(packet);
  }

  @override
  void close() {
    _timer?.cancel();
    _timer = null;
    onStatus("Disconnected");
  }
}
