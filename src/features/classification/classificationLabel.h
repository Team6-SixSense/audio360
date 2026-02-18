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
  Unknown = 0,
  Siren = 1,
  Jackhammer = 2,
  CarHorn = 3,
  Clapping = 4,
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
    case ClassificationLabel::Siren:
      return "siren";
    case ClassificationLabel::Jackhammer:
      return "jackhammer";
    case ClassificationLabel::CarHorn:
      return "car_horn";
    case ClassificationLabel::Clapping:
      return "clapping";
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
  if (classification == "siren") {
    return ClassificationLabel::Siren;
  }
  if (classification == "jackhammer") {
    return ClassificationLabel::Jackhammer;
  }
  if (classification == "car_horn") {
    return ClassificationLabel::CarHorn;
  }
  if (classification == "clapping") {
    return ClassificationLabel::Clapping;
  }
  return ClassificationLabel::Unknown;
}
