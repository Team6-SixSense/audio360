/// Classification by the classification module.
/// Must be in sync with src/features/classifcation/classifcationLabel.h
enum Classification {
  unKnown,
  siren,
  jackHammer,
  carHorn
}

/// Quadrant by the DoA module.
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

/// System fault by the System Fault Manager.
/// Must be in sync with src/features/diagnostics/system_fault_states.h
enum SystemFault {
  none,
  hardware,
  classification,
  doa, 
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

/// Returns the system fault enum value from int value.
SystemFault systemFaultFromInt(int value) {
  if (value < 0 || value >= SystemFault.values.length) {
    return SystemFault.none;
  }
  return SystemFault.values[value];
}
