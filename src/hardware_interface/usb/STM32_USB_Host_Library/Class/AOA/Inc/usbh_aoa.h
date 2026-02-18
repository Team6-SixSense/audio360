/**
  ******************************************************************************
  * @file    usbh_aoa.h
  * @brief   This file contains all the prototypes for the usbh_aoa.c
  *          firmware driver.
  ******************************************************************************
  */

#ifndef AUDIO360_USBH_AOA_H
#define AUDIO360_USBH_AOA_H
#include <stdint.h>

#include "usbh_def.h"
#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief  AOA State Machine Structure
 */
typedef enum {
  AOA_STATE_IDLE = 0,               /*!< Idle state */
  AOA_STATE_SEND_AOA_PROTOCOL_REQ,  /*!< Send AOA Protocol Request */
  AOA_STATE_SEND_APP_DESCRIPTOR,    /*!< Send Application Descriptor */
  AOA_STATE_WAIT_REBOOT_REQ,        /*!< Wait for Reboot Request */
  AOA_SEND_REBOOT_REQ,              /*!< Send Reboot Request */
  AOA_STATE_WAIT_FOR_NEXT_ENUM,     /*!< Wait for Next Enumeration */
  AOA_STATE_CONNECTED               /*!< Connected state */
} AOA_State_t;

/* Handle Structure */
/**
 * @brief  AOA Handle Structure
 */
typedef struct {
  uint8_t              InPipe;   /*!< IN Pipe number */
  uint8_t              OutPipe;  /*!< OUT Pipe number */
  uint8_t              OutEp;    /*!< OUT Endpoint address */
  uint8_t              InEp;     /*!< IN Endpoint address */
  AOA_State_t          state;    /*!< Current AOA State */
  uint32_t             timer;    /*!< Timer for timeouts */
} AOA_HandleTypeDef;

extern USBH_ClassTypeDef  AOA_Class;
#define USBH_AOA_CLASS    &AOA_Class

/**
 * @brief  Transmits data over AOA interface
 * @param  pbuff: Pointer to byte data buffer
 * @param  length: Length of data to be sent in bytes.
 * @retval USBH Status
 */
USBH_StatusTypeDef USBH_AOA_Transmit(uint8_t *pbuff, uint16_t length);

uint8_t Is_AOA_Connected();
#ifdef __cplusplus
 }
#endif
#endif  // AUDIO360_USBH_AOA_H
