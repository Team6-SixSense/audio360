import '../base_data_service.dart';
import '../models/packet.dart';
import 'usb_service.dart';
import 'mock_usb_service.dart';

/// Factory to create USB service based on environment.
class UsbServiceFactory {
  static BaseDataService createUsbService({
    required Function(Packet) onPacket,
    required Function(String) onStatus,
    bool useMock = false,
  }) {
    if (useMock) {
      return MockUsbService(onPacket: onPacket, onStatus: onStatus);
    } else {
      return UsbService(p: onPacket, s: onStatus);
    }
  }
}
