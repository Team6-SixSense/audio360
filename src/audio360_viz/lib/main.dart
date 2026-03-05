import 'package:audio360_viz/base_data_service.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'ble/bluetooth_service.dart';
import 'models/packet.dart';
import 'usb/usb_service_factory.dart';
import 'screens/visualization_screen.dart';

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
  late BaseDataService dataService; // Can be UsbService or MockUsbService or BluetoothLEService
  Packet? _packet;
  String _status = "Disconnected";

  bool useBluetooth = true;

  void onPacket (Packet packet) {
  setState(() => _packet = packet);
  }

  void onStatus (String status) {
  setState(() => _status = status);
  }


  @override
  void initState() {

    super.initState();
    if(!useBluetooth){
      dataService = UsbServiceFactory.createUsbService(
        onPacket: onPacket,
        onStatus: onStatus,
      );
    } else {
      dataService= BluetoothLEService(p: onPacket,
          s: onStatus);
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
    return VisualizationScreen(
      packet: _packet,
      status: _status,
    );
  }
}
