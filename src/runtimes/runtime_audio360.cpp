/**
 ******************************************************************************
 * @file    runtime_audio360.cpp
 * @brief   FFT runtime main source code.
 ******************************************************************************
 */

#include "runtime_audio360.hpp"

#include <cstdint>

#include "classification.h"
#include "doa.h"
#include "embedded_mic.h"
#include "logging.hpp"
#include "packet.h"
#include "peripheral.h"
#include "usb_host.h"
#include "usbh_aoa.h"
#include "utils.h"

// Microphone definitions.
static embedded_mic_t* micA1 = nullptr;
static embedded_mic_t* micB1 = nullptr;
static embedded_mic_t* micA2 = nullptr;
static embedded_mic_t* micB2 = nullptr;

// Larger buffer to hold microphone audio data for post-processing.
static int32_t micA1Buffer[MIC_BUFFER_SIZE];
static int32_t micB1Buffer[MIC_BUFFER_SIZE];
static int32_t micA2Buffer[MIC_BUFFER_SIZE];
static int32_t micB2Buffer[MIC_BUFFER_SIZE];

static int micBufferStartPos{0};
static uint8_t micMainHalf{0}, micMainFull{0}, micDummyHalf{0}, micDummyFull{0};

// Audio360 features.
static DOA doa{DOA_SAMPLES};
static Classification classifier{MIC_BUFFER_SIZE / 2, NUM_MEL_FILTERS,
                                 NUM_DCT_COEFF, NUM_PCA_COMPONENTS,
                                 NUM_CLASSES};

static bool checkSame = false;

void mainAudio360() {
  INFO("Running Audio360.");

  INFO("Initializing microphones.");
  micA1 = embedded_mic_get(MIC_A1);
  micB1 = embedded_mic_get(MIC_B1);
  micA2 = embedded_mic_get(MIC_A2);
  micB2 = embedded_mic_get(MIC_B2);

  // Start DMA (Non-blocking).
  embedded_mic_start(micA1);
  embedded_mic_start(micB1);
  embedded_mic_start(micA2);
  embedded_mic_start(micB2);

  VisualizationPacket vizPacket{};
  vizPacket.classification = ClassificationLabel::CarHorn;
  vizPacket.direction = DirectionLabel::North;
  vizPacket.priority = 3U;

  while (1) {
    MX_USB_HOST_Process();

    if (Is_AOA_Connected() == 1) {
      INFO("Audio360 loop start.");

      // Extract microphone data if ready.
      INFO("Microphone data extraction.");
      bool newData = extractMicData();

      INFO("Running DoA estimation.");
      float angle_rad = runDoA(newData);
      INFO("DoA angle: %f rad.", angle_rad);

      std::string prediction = runClassification(newData);
      INFO("Classification: %s", prediction.c_str());

      vizPacket.classification = StringToClassification(prediction);

      vizPacket.direction = angleToDirection(angle_rad);

      std::array<uint8_t, PACKET_BYTE_SIZE> packet = createPacket(vizPacket);

      USBH_AOA_Transmit(packet.data(), packet.size());

      // Reset half and full bool flags.
      if (micMainFull == 1U) {
        micMainHalf = 0U;
        micMainFull = 0U;
      }
    }
    INFO("Audio360 loop end.");
  }
}

bool extractMicData() {
  bool newData{false};

  // Check that microphone buffers are ready to be read in. Since microphones
  // are synched to same clock, use dummy checks for other mics.
  check_mic_buffers(micA1, &micMainHalf, &micMainFull);
  check_mic_buffers(micB1, &micDummyHalf, &micDummyFull);
  check_mic_buffers(micA2, &micDummyHalf, &micDummyFull);
  check_mic_buffers(micB2, &micDummyHalf, &micDummyFull);

  // Store microphone data in larger buffer when dma buffer is full.
  for (int step = 0; step < 2; step++) {
    bool process = false;
    int offset = 0;

    if (micMainHalf || micMainFull) {
      process = true;
      micBufferStartPos =
          (micBufferStartPos + MIC_HALF_BUFFER_SIZE) % MIC_BUFFER_SIZE;
      offset = (step == 0) ? 0 : MIC_HALF_BUFFER_SIZE;
    }

    if (process) {
      newData = true;

      int32_t* A1Data = &micA1->pBuffer[offset];
      int32_t* B1Data = &micB1->pBuffer[offset];
      int32_t* A2Data = &micA2->pBuffer[offset];
      int32_t* B2Data = &micB2->pBuffer[offset];

      size_t numBytes = MIC_HALF_BUFFER_SIZE * 4;
      size_t startPos = micBufferStartPos + offset;

      // Invalidate Source Cache (CPU reads from RAM updated by DMA).
      SCB_InvalidateDCache_by_Addr((uint32_t*)A1Data, numBytes);
      SCB_InvalidateDCache_by_Addr((uint32_t*)B1Data, numBytes);
      SCB_InvalidateDCache_by_Addr((uint32_t*)A2Data, numBytes);
      SCB_InvalidateDCache_by_Addr((uint32_t*)B2Data, numBytes);

      // Copy audio data from dynamic memory buffer to larger audio buffer.
      memcpy(&micA1Buffer[startPos], A1Data, numBytes);
      memcpy(&micB1Buffer[startPos], B1Data, numBytes);
      memcpy(&micA2Buffer[startPos], A2Data, numBytes);
      memcpy(&micB2Buffer[startPos], B2Data, numBytes);

      // Clean Destination Cache (USB DMA reads from RAM updated by CPU). Ensure
      // the data we just wrote to larger audio buffer is flushed from Cache to
      // RAM.
      SCB_CleanDCache_by_Addr((uint32_t*)micA1Buffer[startPos], numBytes);
      SCB_CleanDCache_by_Addr((uint32_t*)micB1Buffer[startPos], numBytes);
      SCB_CleanDCache_by_Addr((uint32_t*)micA2Buffer[startPos], numBytes);
      SCB_CleanDCache_by_Addr((uint32_t*)micB2Buffer[startPos], numBytes);
    }
  }

  return newData;
}

float runDoA(bool newData) {
  if (!newData) {
    INFO("There is no new data. Skipping...");
    return -1.0;
  }

  // Extract the most recent microphone data.
  size_t start = micBufferStartPos;
  size_t end = micBufferStartPos + MIC_HALF_BUFFER_SIZE;
  if (micMainFull == 1U) {
    start += MIC_HALF_BUFFER_SIZE;
    end += MIC_HALF_BUFFER_SIZE;
  }

  std::vector<float> mic1Data(MIC_HALF_BUFFER_SIZE);
  std::vector<float> mic2Data(MIC_HALF_BUFFER_SIZE);
  std::vector<float> mic3Data(MIC_HALF_BUFFER_SIZE);
  std::vector<float> mic4Data(MIC_HALF_BUFFER_SIZE);

  for (size_t i = 0; i < MIC_HALF_BUFFER_SIZE; i++) {
    mic1Data[i] = static_cast<float>(micA1Buffer[start + i]);
    mic2Data[i] = static_cast<float>(micA2Buffer[start + i]);
    mic3Data[i] = static_cast<float>(micB2Buffer[start + i]);
    mic4Data[i] = static_cast<float>(micB1Buffer[start + i]);
  }

  float angle = doa.calculateDirection(mic1Data, mic2Data, mic3Data, mic4Data,
                                       DOA_Algorithms::GCC_PHAT);

  return angle;
}

std::string runClassification(bool newData) {
  if (!newData) {
    // INFO("There is no new data. Skipping...");
    return classifier.getClassificationLabel();
  }

  // Extract the most recent microphone data.
  size_t start = micBufferStartPos;
  if (micMainFull == 1U) {
    start += MIC_HALF_BUFFER_SIZE;
  }

  const int32_t* raw = &micA1Buffer[start];

  // --- SAME-BUFFER CHECK ---
  static uint32_t lastHash = 0;
  uint32_t h = fnv1a_hash32(raw, MIC_HALF_BUFFER_SIZE);
  if (h == lastHash) {
    checkSame = true;
    return classifier.getClassificationLabel();
  }
  if (!checkSame) {
    lastHash = h;
  }
  // -----------------------

  std::vector<float> mic1Data(MIC_HALF_BUFFER_SIZE);

  for (size_t i = 0; i < MIC_HALF_BUFFER_SIZE; i++) {
    mic1Data[i] = static_cast<float>(micA1Buffer[start + i]);
  }

  classifier.Classify(mic1Data);

  return classifier.getClassificationLabel();
}
