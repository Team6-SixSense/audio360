import 'dart:async';
import 'package:flutter/foundation.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';

import '../base_data_service.dart';

final class BluetoothLEService extends BaseDataService {
  // Bluetooth device has FFE0 service and FFE1 characteristic.
  static const String hm10ServiceUUID = 'FFE0';
  static const String hm10CharacteristicUUID = 'FFE1';

  BluetoothCharacteristic? stm32Pipe;
  StreamSubscription<List<ScanResult>>? _scanSubscription;
  StreamSubscription<List<int>>? _dataSubscription;
  StreamSubscription<BluetoothConnectionState>? _connectionSubscription;
  BluetoothDevice? stm32dev;

  BluetoothLEService({required PacketCallback p, required StatusCallback s})
    : super(onPacket: p, onStatus: s);

  @override
  Future<void> initialize() async {
    await scanAndConnectToSTM32();
  }

  Future<void> scanAndConnectToSTM32() async {
    onStatus('Bluetooth scanning');
    await _scanSubscription?.cancel();
    await FlutterBluePlus.stopScan();

    _scanSubscription = FlutterBluePlus.scanResults.listen((results) async {
      for (final result in results) {
        if (kDebugMode) {
          print(
            '${result.device.remoteId}: '
            '"${result.advertisementData.advName}" found!',
          );
        }

        if (result.advertisementData.advName.contains('BT05')) {
          await FlutterBluePlus.stopScan();
          await _scanSubscription?.cancel();
          _scanSubscription = null;
          await connectToDevice(result.device);
          break;
        }
      }
    });

    unawaited(FlutterBluePlus.startScan(timeout: const Duration(seconds: 4)));
  }

  Future<void> connectToDevice(BluetoothDevice device) async {
    onStatus('Bluetooth connecting');

    try {
      await _connectionSubscription?.cancel();
      await _dataSubscription?.cancel();

      _connectionSubscription = device.connectionState.listen((state) async {
        if (state == BluetoothConnectionState.connected) {
          onStatus('Bluetooth connected');
          return;
        }

        stm32Pipe = null;
        stm32dev = null;
        await _dataSubscription?.cancel();
        _dataSubscription = null;
        onStatus('Bluetooth disconnected');

        if (kDebugMode) {
          print('Bluetooth disconnected: ${device.disconnectReason}');
        }
      });
      device.cancelWhenDisconnected(_connectionSubscription!, delayed: true);

      await device.connect();
      stm32dev = device;

      if (kDebugMode) {
        print('Connected to ${device.platformName}');
      }

      final services = await device.discoverServices();

      // Find the characteristic and service we want.
      final targetService = services.firstWhere(
        (service) =>
            service.serviceUuid.toString().toUpperCase() == hm10ServiceUUID,
      );

      stm32Pipe = targetService.characteristics.firstWhere(
        (characteristic) =>
            characteristic.uuid.toString().toUpperCase() ==
            hm10CharacteristicUUID,
      );

      // We get notified every time a new value is published to the bluetooth
      // byte stream.
      await stm32Pipe!.setNotifyValue(true);
      _dataSubscription = stm32Pipe!.lastValueStream.listen((value) {
        final packet = Uint8List.fromList(value);
        onDataReceived(packet);
      });

      onStatus('Bluetooth connected');
    } catch (e) {
      onStatus('Bluetooth disconnected');
      stm32Pipe = null;
      stm32dev = null;

      if (kDebugMode) {
        print('Bluetooth connection error: $e');
      }
    }
  }

  // Disconnect from bluetooth and terminate subscription.
  @override
  Future<void> close() async {
    try {
      onStatus('Bluetooth disconnected');
      await _scanSubscription?.cancel();
      await _dataSubscription?.cancel();
      await _connectionSubscription?.cancel();
      await FlutterBluePlus.stopScan();
      await stm32dev?.disconnect();

      _scanSubscription = null;
      _dataSubscription = null;
      _connectionSubscription = null;
      stm32Pipe = null;
      stm32dev = null;

      if (kDebugMode) {
        print('Disconnected from STM32');
      }
    } catch (e) {
      if (kDebugMode) {
        print('Error during disconnect: $e');
      }
    }
  }
}
