#include "embedded_mic.h"
#include "peripheral.h"
#include <string.h>

#ifdef STM_BUILD

static embedded_mic_t mics[4];

DMA_HandleTypeDef hdma_sai1_a;
DMA_HandleTypeDef hdma_sai1_b;
DMA_HandleTypeDef hdma_sai2_a;
DMA_HandleTypeDef hdma_sai2_b;


void init_mic_a1();
void init_mic_a2();
void init_mic_b1();
void init_mic_b2();


void embedded_mic_init() {
  memset(mics, 0, sizeof(mics));

  init_mic_a1();
  init_mic_a2();
  init_mic_b1();
  init_mic_b2();

}

embedded_mic_t* embedded_mic_get(const embedded_mic_index index) {
    return &mics[index];
}

void embedded_mic_start(embedded_mic_t* mic_handle) {
  // if (mic_handle != NULL && mic_handle->hsai_block != NULL && mic_handle->pBuffer != NULL) {
  //   HAL_SAI_Receive_DMA(mic_handle->hsai_block, (uint8_t*)mic_handle->pBuffer, mic_handle->BufferSize);
  // }
}

void init_mic_a1() {
  mics[MIC_A1] = embedded_mic_t();
  mics[MIC_A1].index = MIC_A1;

  //we use DMA2 Stream 1
  mics[MIC_A1].irq = DMA2_Stream1_IRQn;
  mics[MIC_A1].hdma_sai = &hdma_sai1_a;
  SAI_HandleTypeDef& hsai_BlockA1 = mics[MIC_A1].hsai_block;
  // SAI 1 has 2 subblocks: A and B. Set A with the master clock, so B can sync
  // with A.

  // Block A initialization.

  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_RX;
  hsai_BlockA1.Init.DataSize = SAI_DATASIZE_24;
  hsai_BlockA1.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockA1.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_OUTBLOCKA_ENABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA1.FrameInit.FrameLength = 64;
  hsai_BlockA1.FrameInit.ActiveFrameLength = 32;
  hsai_BlockA1.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  hsai_BlockA1.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockA1.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;
  hsai_BlockA1.SlotInit.FirstBitOffset = 0;
  hsai_BlockA1.SlotInit.SlotSize = SAI_SLOTSIZE_32B;
  hsai_BlockA1.SlotInit.SlotNumber = 2;
  hsai_BlockA1.SlotInit.SlotActive = 0x00000001;

  if (HAL_SAI_Init(&hsai_BlockA1) != HAL_OK) {
    Error_Handler();
  }

}

void init_mic_b1() {

  mics[MIC_B1] = embedded_mic_t();
  mics[MIC_B1].index = MIC_B1;

  // we use dma 2 stream 0
  mics[MIC_B1].hdma_sai = &hdma_sai1_b;
  mics[MIC_B1].irq = DMA2_Stream0_IRQn;
  SAI_HandleTypeDef& hsai_BlockB1 = mics[MIC_B1].hsai_block;

  // Block B initialization.
  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockB1.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockB1.Init.DataSize = SAI_DATASIZE_24;
  hsai_BlockB1.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockB1.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  hsai_BlockB1.Init.Synchro = SAI_SYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_OUTBLOCKA_ENABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  hsai_BlockB1.FrameInit.FrameLength = 64;
  hsai_BlockB1.FrameInit.ActiveFrameLength = 32;
  hsai_BlockB1.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  hsai_BlockB1.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockB1.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;
  hsai_BlockB1.SlotInit.FirstBitOffset = 0;
  hsai_BlockB1.SlotInit.SlotSize = SAI_SLOTSIZE_32B;
  hsai_BlockB1.SlotInit.SlotNumber = 2;
  hsai_BlockB1.SlotInit.SlotActive = 0x00000001;

  if (HAL_SAI_Init(&hsai_BlockB1) != HAL_OK) {
    Error_Handler();
  }
}

void init_mic_a2() {
  mics[MIC_A2] = embedded_mic_t();
  mics[MIC_A2].index = MIC_A2;

  // we use dma2 stream 2
  mics[MIC_A2].hdma_sai = &hdma_sai2_a;
  mics[MIC_A2].irq = DMA2_Stream2_IRQn;

  SAI_HandleTypeDef& hsai_BlockA2 = mics[MIC_A2].hsai_block;

  // Block A initialization.
  hsai_BlockA2.Instance = SAI2_Block_A;
  hsai_BlockA2.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockA2.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockA2.Init.DataSize = SAI_DATASIZE_24;
  hsai_BlockA2.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockA2.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  hsai_BlockA2.Init.Synchro = SAI_SYNCHRONOUS_EXT_SAI1;
  hsai_BlockA2.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA2.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA2.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA2.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA2.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  hsai_BlockA2.FrameInit.FrameLength = 64;
  hsai_BlockA2.FrameInit.ActiveFrameLength = 32;
  hsai_BlockA2.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  hsai_BlockA2.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockA2.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;
  hsai_BlockA2.SlotInit.FirstBitOffset = 0;
  hsai_BlockA2.SlotInit.SlotSize = SAI_SLOTSIZE_32B;
  hsai_BlockA2.SlotInit.SlotNumber = 2;
  hsai_BlockA2.SlotInit.SlotActive = 0x00000001;

  if (HAL_SAI_Init(&hsai_BlockA2) != HAL_OK) {
    Error_Handler();
  }
}

void init_mic_b2() {
  mics[MIC_B2] = embedded_mic_t();
  mics[MIC_B2].index = MIC_B2;

  // we use dma 2 stream 6
  mics[MIC_B2].hdma_sai = &hdma_sai2_b;
  mics[MIC_B2].irq = DMA2_Stream6_IRQn;
  SAI_HandleTypeDef& hsai_BlockB2 = mics[MIC_B2].hsai_block;

  // Block B initialization.
  hsai_BlockB2.Instance = SAI2_Block_B;
  hsai_BlockB2.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockB2.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockB2.Init.DataSize = SAI_DATASIZE_24;
  hsai_BlockB2.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockB2.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  hsai_BlockB2.Init.Synchro = SAI_SYNCHRONOUS;
  hsai_BlockB2.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB2.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB2.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB2.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB2.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  hsai_BlockB2.FrameInit.FrameLength = 64;
  hsai_BlockB2.FrameInit.ActiveFrameLength = 32;
  hsai_BlockB2.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  hsai_BlockB2.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockB2.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;
  hsai_BlockB2.SlotInit.FirstBitOffset = 0;
  hsai_BlockB2.SlotInit.SlotSize = SAI_SLOTSIZE_32B;
  hsai_BlockB2.SlotInit.SlotNumber = 2;
  hsai_BlockB2.SlotInit.SlotActive = 0x00000001;

  if (HAL_SAI_Init(&hsai_BlockB2) != HAL_OK) {
    Error_Handler();
  }
}


#endif
