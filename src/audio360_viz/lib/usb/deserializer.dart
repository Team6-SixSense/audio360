import 'dart:typed_data';
import '../models/enums.dart';
import '../models/packet.dart';

/// Deserializes incoming data packet.
Packet deserializePacket(Uint8List data) {
  final bd = ByteData.sublistView(data);
  int offset = 0;

  final int classificationInt = bd.getUint8(offset);
  offset += 1;

  final int quadrantInt = bd.getUint8(offset);
  offset += 1;

  final int priority = bd.getUint8(offset);

  return Packet(
    classification: classificationFromInt(classificationInt),
    quadrant: quadrantFromInt(quadrantInt),
    priority: priority);
}
