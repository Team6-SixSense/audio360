import 'dart:typed_data';
import 'package:flutter/services.dart';
import 'package:usb_serial/usb_serial.dart';
import '../models/packet.dart';
import './deserializer.dart';

typedef PacketCallback = void Function(Packet packet);
typedef StatusCallback = void Function(String status);

class UsbService {
  UsbPort? _port;
  Uint8List _buffer = Uint8List(0);
  static const int packetSize = 4;

  final PacketCallback onPacket;
  final StatusCallback onStatus;

  static const EventChannel _channel = EventChannel('com.audio360.aoa/stream');

  UsbService({required this.onPacket, required this.onStatus});

  Future<void> connect() async {

    _channel.receiveBroadcastStream().listen((dynamic event) {
      final Uint8List data = event;

      _onDataReceived(data);
      onStatus("Connected");
    });
  }

void _onDataReceived(Uint8List data) {
  _buffer = Uint8List.fromList(_buffer + data);

  while (_buffer.length >= packetSize) {
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

  void dispose() {
    _port?.close();
  }
}
