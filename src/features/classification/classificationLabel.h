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
  SomeoneTalking = 1,
  Siren = 2,
  SmokeAlarm = 3,
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
    case ClassificationLabel::SomeoneTalking:
      return "someone_talking";
    case ClassificationLabel::Siren:
      return "siren";
    case ClassificationLabel::SmokeAlarm:
      return "smoke_alarm";
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
  if (classification == "someone_talking")
    return ClassificationLabel::SomeoneTalking;
  if (classification == "siren") return ClassificationLabel::Siren;
  if (classification == "smoke_alarm") return ClassificationLabel::SmokeAlarm;
  return ClassificationLabel::Unknown;
}
