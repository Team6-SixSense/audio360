import 'package:flutter/material.dart';
import 'models/packet.dart';
import 'usb/usb_service.dart';

void main() {
  runApp(const UsbSerialApp());
}

class UsbSerialApp extends StatelessWidget {
  const UsbSerialApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      home: UsbPage(),
    );
  }
}

class UsbPage extends StatefulWidget {
  const UsbPage({super.key});

  @override
  State<UsbPage> createState() => _UsbPageState();
}

class _UsbPageState extends State<UsbPage> {
  late UsbService usbService;
  Packet? _packet;
  String _status = "Disconnected";

  @override
  void initState() {
    super.initState();
    usbService = UsbService(
      onPacket: (packet) => setState(() => _packet = packet),
      onStatus: (status) => setState(() => _status = status),
    );
    usbService.connect();
  }

  @override
  void dispose() {
    usbService.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("USB Serial Viewer")),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text("Status: $_status"),
            const SizedBox(height: 12),
            if (_packet != null) ...[
              Text(
                "Classification: ${_packet!.classification.name}",
                style: Theme.of(context).textTheme.titleMedium,
              ),
              const SizedBox(height: 8),

              Text("Quadrant: ${_packet!.quadrant.name}"),
              const SizedBox(height: 4),

              Text("Priority: ${_packet!.priority}"),
            ],
          ],
        ),
      ),
    );
  }
}
