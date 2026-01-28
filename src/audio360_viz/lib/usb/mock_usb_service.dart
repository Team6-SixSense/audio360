import 'dart:async';
import 'dart:math';
import '../models/packet.dart';
import '../models/enums.dart';

typedef PacketCallback = void Function(Packet packet);
typedef StatusCallback = void Function(String status);

/// Mock USB service for testing without hardware.
/// Simulates packet data with random classifications and directions.
class MockUsbService {
  Timer? _timer;
  final PacketCallback onPacket;
  final StatusCallback onStatus;
  final Random _random = Random();

  MockUsbService({required this.onPacket, required this.onStatus});

  Future<void> connect() async {
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
    // Random classification (excluding unknown for better testing)
    final classifications = [
      Classification.siren,
      Classification.carHorn,
      Classification.jackHammer,
    ];
    final classification = classifications[_random.nextInt(classifications.length)];

    // Random quadrant (excluding none)
    final quadrants = [
      Quadrant.north,
      Quadrant.northEast,
      Quadrant.east,
      Quadrant.southEast,
      Quadrant.south,
      Quadrant.southWest,
      Quadrant.west,
      Quadrant.northWest,
    ];
    final quadrant = quadrants[_random.nextInt(quadrants.length)];

    // Random priority (1-5)
    final priority = _random.nextInt(5) + 1;

    final packet = Packet(
      classification: classification,
      quadrant: quadrant,
      priority: priority,
    );

    onPacket(packet);
  }

  void dispose() {
    _timer?.cancel();
    _timer = null;
    onStatus("Disconnected");
  }
}
