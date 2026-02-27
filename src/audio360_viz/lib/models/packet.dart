import 'enums.dart';

/// Represents a single packet from microcontroller.
class Packet {
  /// Classfication of main audio source.
  final Classification classification;

  /// Quadrant that the audio source is coming from.
  final Quadrant quadrant;

  /// The current system fualt.
  final SystemFault systemFault;

  // The priority of the audio source.
  final int priority;

   Packet({
    required this.classification,
    required this.quadrant,
    required this.systemFault,
    required this.priority,
  });
}