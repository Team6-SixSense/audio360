import 'package:audio360_viz/base_data_service.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import 'ble/bluetooth_service.dart';
import 'models/packet.dart';
import 'screens/visualization_screen.dart';
import 'usb/usb_service_factory.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();

  // Set preferred orientations for smart glasses (landscape)
  SystemChrome.setPreferredOrientations([
    DeviceOrientation.portraitUp,
    DeviceOrientation.portraitDown,
  ]);

  // Set system UI overlay style for immersive experience
  SystemChrome.setEnabledSystemUIMode(SystemUiMode.immersiveSticky);

  runApp(const Audio360VizApp());
}

class Audio360VizApp extends StatelessWidget {
  const Audio360VizApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Audio360 Visualization',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        brightness: Brightness.dark,
        primarySwatch: Colors.red,
        scaffoldBackgroundColor: Colors.black,
        fontFamily: 'Roboto',
      ),
      home: const MainVisualizationPage(),
    );
  }
}

class MainVisualizationPage extends StatefulWidget {
  const MainVisualizationPage({super.key});

  @override
  State<MainVisualizationPage> createState() => _MainVisualizationPageState();
}

class _MainVisualizationPageState extends State<MainVisualizationPage> {
  static const bool _useMockMode = bool.fromEnvironment(
    'USE_MOCK_MODE',
    defaultValue: false,
  );
  static const bool _useBluetooth = bool.fromEnvironment(
    'USE_BLUETOOTH',
    defaultValue: true,
  );

  // Can be UsbService or BluetoothLEService.
  late BaseDataService dataService;
  Packet? _packet;
  String _status = 'Bluetooth disconnected';

  bool _isConnectedStatus(String status) {
    final normalized = status.trim().toLowerCase();
    return normalized.contains('connected') &&
        !normalized.contains('disconnected') &&
        !normalized.contains('connecting');
  }

  void onPacket(Packet packet) {
    setState(() => _packet = packet);
  }

  void onStatus(String status) {
    setState(() {
      _status = status;
      if (!_isConnectedStatus(status)) {
        _packet = null;
      }
    });
  }

  @override
  void initState() {
    super.initState();
    if (_useMockMode) {
      dataService = UsbServiceFactory.createUsbService(
        onPacket: onPacket,
        onStatus: onStatus,
        useMock: true,
      );
    } else if (!_useBluetooth) {
      dataService = UsbServiceFactory.createUsbService(
        onPacket: onPacket,
        onStatus: onStatus,
      );
    } else {
      dataService = BluetoothLEService(p: onPacket, s: onStatus);
    }

    // Auto-connect on startup
    dataService.initialize();
  }

  @override
  void dispose() {
    dataService.close();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return VisualizationScreen(packet: _packet, status: _status);
  }
}
