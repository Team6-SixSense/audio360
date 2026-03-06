  import 'package:flutter/services.dart';
import 'package:usb_serial/usb_serial.dart';
import '../base_data_service.dart';



final class UsbService extends BaseDataService {
  UsbPort? _port;

  static const EventChannel _channel = EventChannel('com.audio360.aoa/stream');

  UsbService({required PacketCallback p, required StatusCallback s}) : super(onPacket: p, onStatus: s);

  @override
  Future<void> initialize() async {

    _channel.receiveBroadcastStream().listen((dynamic event) {
      final Uint8List data = event;

      super.onDataReceived(data);
      onStatus("Connected");
    });
  }


  @override
  void close() {
    _port?.close();
  }
}
