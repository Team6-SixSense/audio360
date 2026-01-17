enum Classification {
  UnKnown,
  Siren,
  JackHammer,
  CarHorn
}

enum Quadrant {
  None,
  North,
  NorthWest,
  West,
  SouthWest,
  South,
  SouthEast,
  East,
  NorthEast,
}

/// Returns the classification enum value from int value.
Classification classificationFromInt(int value) {
  if (value < 0 || value >= Classification.values.length) {
    return Classification.UnKnown;
  }
  return Classification.values[value];
}

/// Returns the quadrant enum value from int value.
Quadrant quadrantFromInt(int value) {
  if (value < 0 || value >= Quadrant.values.length) {
    return Quadrant.None;
  }
  return Quadrant.values[value];
}
