import 'dart:async';
import 'dart:convert';
import 'package:flutter/services.dart';

import '../models/packet.dart';
import '../usb/deserializer.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import '../baseDataService.dart';


final class BluetoothLEService extends BaseDataService {

  static const String hm10ServiceUUID = "FFE0";
  static const String hm10CharacteristicUUID = "FFE1";

  BluetoothCharacteristic? stm32Pipe;
  StreamSubscription? _dataSubscription;

  BluetoothDevice? stm32dev;


  BluetoothLEService({required PacketCallback p , required StatusCallback s}) : super(onPacket: p, onStatus: s);

  @override
  Future<void> initialize() async {
    scanAndConnectToSTM32();
  }



void scanAndConnectToSTM32()
  {
    FlutterBluePlus.startScan(timeout: Duration(seconds: 4));

    FlutterBluePlus.scanResults.listen((results) {
      for (ScanResult r in results) {
        print('${r.device.remoteId}: "${r.advertisementData.advName}" found!');
        // Check if this is the device we want
        if (r.advertisementData.advName.contains("BT05")) {
          FlutterBluePlus.stopScan();
          connectToDevice(r.device);
        }
      }
    });
  }

  Future<void> connectToDevice(BluetoothDevice device) async {
    await device.connect();
    print("Connected to ${device.platformName}");

    List<BluetoothService> services = await device.discoverServices();

    BluetoothService targetService = services.firstWhere(
            (s) => s.serviceUuid.toString().toUpperCase() == hm10ServiceUUID
    );

    stm32Pipe = targetService.characteristics.firstWhere(
            (c) => c.uuid.toString().toUpperCase() == hm10CharacteristicUUID
    );

    await stm32Pipe!.setNotifyValue(true);

    onStatus("Connected");

    stm32dev = device;

    _dataSubscription = stm32Pipe!.lastValueStream.listen((value) {
      Uint8List packet = Uint8List.fromList(value);
      onDataReceived(packet);
    });
  }

  @override
  Future<void> close() async {
    try {
      onStatus("Disconnected");
      await _dataSubscription?.cancel();
      await stm32dev?.disconnect();
      print("Disconnected from STM32");
    } catch (e) {
      print("Error during disconnect: $e");
    }
  }

}