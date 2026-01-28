import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'models/packet.dart';
import 'usb/usb_service_factory.dart';
import 'screens/visualization_screen.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  
  // Set preferred orientations for smart glasses (landscape)
  SystemChrome.setPreferredOrientations([
    DeviceOrientation.landscapeLeft,
    DeviceOrientation.landscapeRight,
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
  dynamic usbService; // Can be UsbService or MockUsbService
  Packet? _packet;
  String _status = "Disconnected";



  @override
  void initState() {
    super.initState();
    usbService = UsbServiceFactory.createUsbService(
      onPacket: (Packet packet) {
        setState(() => _packet = packet);
      },
      onStatus: (String status) {
        setState(() => _status = status);
      },
    );
    // Auto-connect on startup
    usbService.connect();
  }

  @override
  void dispose() {
    usbService.dispose();
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
