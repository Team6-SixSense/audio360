/**
 ******************************************************************************
 * @file    classificationLabel.h
 * @brief   Classification label header code
 ******************************************************************************
 */

#pragma once

#include <string>

/** @brief Enum holding all classification labels. */
enum class ClassificationLabel {
  Fire = 0,
  Screech = 1,
  TruckReversing = 2,
  Unknown = 3
};

/**
 * @brief Converts a classification enum value into a string label.
 *
 * @param classification Enum value to convert.
 * @return String label for the enum value.
 */
inline const char* ClassificationClassToString(
    ClassificationLabel classification) {
  switch (classification) {
    case ClassificationLabel::Fire:
      return "fire";
    case ClassificationLabel::Screech:
      return "screech";
    case ClassificationLabel::TruckReversing:
      return "truck_reversing";
    case ClassificationLabel::Unknown:
      return "unknown";
  }
  return "unknown";
}

/**
 * @brief Maps a string label to the matching classification enum value.
 *
 * @param classification Input string label (e.g. "siren").
 * @return Enum value corresponding to the input label.
 */
inline ClassificationLabel StringToClassification(std::string classification) {
  if (classification == "fire") return ClassificationLabel::Fire;
  if (classification == "screech") return ClassificationLabel::Screech;
  if (classification == "truck_reversing") return ClassificationLabel::TruckReversing;
  return ClassificationLabel::Unknown;
}
