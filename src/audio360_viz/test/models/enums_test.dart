import 'package:audio360_viz/models/enums.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  test('classification enum stays aligned with embedded classifier values', () {
    expect(classificationFromInt(0), Classification.unknown);
    expect(classificationFromInt(1), Classification.someoneTalking);
    expect(classificationFromInt(2), Classification.siren);
    expect(classificationFromInt(3), Classification.smokeAlarm);
  });

  test('invalid classification values fall back to unknown', () {
    expect(classificationFromInt(-1), Classification.unknown);
    expect(classificationFromInt(99), Classification.unknown);
  });
}
