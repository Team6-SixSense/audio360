import 'package:flutter/services.dart';
import 'package:usb_serial/usb_serial.dart';
import '../models/packet.dart';
import './usb/deserializer.dart';

typedef PacketCallback = void Function(Packet packet);
typedef StatusCallback = void Function(String status);

base class BaseDataService {
  static const int packetSize = 4;

  final PacketCallback onPacket;
  final StatusCallback onStatus;

  Uint8List _buffer = Uint8List(0);

  BaseDataService({required this.onPacket, required this.onStatus});

  Future<void> initialize()
  {
    throw UnimplementedError();
  }

  void close()
  {
    throw UnimplementedError();

  }

  void onDataReceived(Uint8List data) {
    _buffer = Uint8List.fromList(_buffer + data);

    while (_buffer.length >= BaseDataService.packetSize) {
      // First byte must be 0xAA.
      if (_buffer[0] != 0xAA) {
        // Resync.
        _buffer = _buffer.sublist(1);
        continue;
      }

      final payload = _buffer.sublist(1, 4);
      _buffer = _buffer.sublist(4);

      final packet = deserializePacket(payload);
      onPacket(packet);
    }
  }


}