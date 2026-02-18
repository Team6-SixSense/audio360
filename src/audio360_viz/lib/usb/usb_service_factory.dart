import '../models/packet.dart';
import 'usb_service.dart';
import 'mock_usb_service.dart';

/// Factory to create USB service based on environment.
/// Uses mock service in debug mode or when USE_MOCK_USB is true.
class UsbServiceFactory {
  static const bool useMock = false;

  static dynamic createUsbService({
    required Function(Packet) onPacket,
    required Function(String) onStatus,
  }) {
    // Use mock in debug mode or if explicitly requested
    if (useMock) {
      return MockUsbService(
        onPacket: onPacket,
        onStatus: onStatus,
      );
    } else {
      return UsbService(
        onPacket: onPacket,
        onStatus: onStatus,
      );
    }
  }
}
