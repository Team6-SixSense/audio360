import 'package:flutter/services.dart';
import '../models/packet.dart';
import './usb/deserializer.dart';

typedef PacketCallback = void Function(Packet packet);
typedef StatusCallback = void Function(String status);

/// Base class to provide data packet streaming functionality from
/// external sources.
base class BaseDataService {
  static const int packetSize = 4;

  final PacketCallback onPacket;
  final StatusCallback onStatus;

  Uint8List _buffer = Uint8List(0);

  BaseDataService({required this.onPacket, required this.onStatus});

  /// Initialize the service. Do setup here.
  Future<void> initialize()
  {
    throw UnimplementedError();
  }

  /// *
  /// Terminate the service. Implement your cleanup here.
  void close()
  {
    throw UnimplementedError();

  }

  /// This is a common method provided to process a list of raw bytes
  void onDataReceived(Uint8List data) {
    _buffer = Uint8List.fromList(_buffer + data);

    while (_buffer.length >= BaseDataService.packetSize) {
      // First byte must be 0xAA.
      if (_buffer[0] != 0xAA) {
        // Resync.
        _buffer = _buffer.sublist(1);
        continue;
      }

      final payload = _buffer.sublist(1, 5);
      _buffer = _buffer.sublist(5);

      final packet = deserializePacket(payload);
      onPacket(packet);
    }
  }


}