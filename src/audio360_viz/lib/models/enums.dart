/// Classification by the classification module.
/// Must be in sync with src/features/classifcation/classifcationLabel.h
enum Classification {
  unKnown,
  siren,
  jackHammer,
  carHorn
}

/// Classification by the DoA module.
/// Must be in sync with src/features/doa/directionLabel.h
enum Quadrant {
  none,
  north,
  northWest,
  west,
  southWest,
  south,
  southEast,
  east,
  northEast,
}

/// Returns the classification enum value from int value.
Classification classificationFromInt(int value) {
  if (value < 0 || value >= Classification.values.length) {
    return Classification.unKnown;
  }
  return Classification.values[value];
}

/// Returns the quadrant enum value from int value.
Quadrant quadrantFromInt(int value) {
  if (value < 0 || value >= Quadrant.values.length) {
    return Quadrant.none;
  }
  return Quadrant.values[value];
}
