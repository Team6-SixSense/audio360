//
// Created by omera on 2026-01-24.
//

#ifndef AUDIO360_USBH_AOA_H
#define AUDIO360_USBH_AOA_H
#include <stdint.h>

#include "usbh_def.h"

typedef enum{
  AOA_STATE_IDLE = 0,
  AOA_STATE_SEND_AOA_PROTOCOL_REQ,
  AOA_STATE_SEND_APP_DESCRIPTOR,
  AOA_SEND_REBOOT_REQ,
  AOA_STATE_WAIT_FOR_NEXT_ENUM,
  AOA_STATE_CONNECTED
} AOA_State_t;

/* Handle Structure */
typedef struct {
  uint8_t              InPipe;
  uint8_t              OutPipe;
  uint8_t              OutEp;
  uint8_t              InEp;
  AOA_State_t     state;
} AOA_HandleTypeDef;

extern USBH_ClassTypeDef  AOA_Class;
#define USBH_AOA_CLASS    &AOA_Class

#endif  // AUDIO360_USBH_AOA_H
