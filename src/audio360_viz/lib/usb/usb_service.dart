import 'dart:typed_data';
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

  UsbService({required this.onPacket, required this.onStatus});

  Future<void> connect() async {
    final devices = await UsbSerial.listDevices();

    if (devices.isEmpty) {
      onStatus("No USB devices found");
      return;
    }

    _port = await devices.first.create();

    if (!await _port!.open()) {
      onStatus("Failed to open USB");
      return;
    }

    await _port!.setPortParameters(
      115200,
      UsbPort.DATABITS_8,
      UsbPort.STOPBITS_1,
      UsbPort.PARITY_NONE,
    );

    _port!.inputStream!.listen(_onDataReceived);
    onStatus("Connected");
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
