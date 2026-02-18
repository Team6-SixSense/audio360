/**
  ******************************************************************************
  * @file    usbh_aoa.c
  * @brief   This file contains the Android Open Accessory (AOA) class
  *          implementation for the USB Host Library.
  * @see https://source.android.com/docs/core/interaction/accessories/aoa
  ******************************************************************************
  */

#include "usbh_aoa.h"

#include "usbh_def.h"
#include "usbh_pipes.h"

/** @defgroup AOA_Private_Defines
  * @{
  */
#define AOA_HANDSHAKE       0x33  /*!< Get Protocol Request */
#define AOA_SEND_DESCRIPTOR 0x34  /*!< Send String Descriptor Request */
#define AOA_REBOOT          0x35  /*!< Start Accessory Mode Request */
/**
  * @}
  */

/** @defgroup AOA_Private_FunctionPrototypes
  * @{
  */
static USBH_StatusTypeDef USBH_AOA_InterfaceInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_AOA_InterfaceDeInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_AOA_ClassRequest(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_AOA_Process(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_AOA_SOF(USBH_HandleTypeDef *phost);
/**
  * @}
  */

/** @defgroup AOA_Private_Variables
  * @{
  */
static char* Manufacturer = "audio360\0";
static char* Model        = "audio360_1\0";
static char* Description  = "STM32 Controller\0";
static char* Version      = "1.0\0";
static char* Uri          = "http://omaralam.ca\0";
static char* Serial       = "00000001\0";

AOA_HandleTypeDef aoa_handle;

extern USBH_HandleTypeDef hUsbHostFS;

static char* aoa_descriptors[6];
/**
  * @}
  */

/* Define the Class Logic */
/**
  * @brief  AOA Class Information
  */
USBH_ClassTypeDef AOA_Class = {
  "AOA",
  0xFF, // 255, this is not needed for us.
  USBH_AOA_InterfaceInit,
  USBH_AOA_InterfaceDeInit,
  USBH_AOA_ClassRequest,
  USBH_AOA_Process,
  USBH_AOA_SOF,
  NULL
};

/** @defgroup AOA_Private_Functions
  * @{
  */

/**
  * @brief  Start of Frame (SOF) Event Process. We do not need this.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_AOA_SOF(USBH_HandleTypeDef *phost)
{
  return USBH_OK;
}

uint8_t Is_AOA_Connected() {
  if (aoa_handle.state == AOA_STATE_CONNECTED) {
    return 1;
  }
  else {
    return 0;
  }
}

/* 1. Initialization: Finds Pipes */
/**
  * @brief  Initialize the AOA class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_AOA_InterfaceInit(USBH_HandleTypeDef *phost) {

  aoa_descriptors[0] = Manufacturer;
  aoa_descriptors[1] = Model;
  aoa_descriptors[2] = Description;
  aoa_descriptors[3] = Version;
  aoa_descriptors[4] = Uri;
  aoa_descriptors[5] = Serial;

  if (phost->device.DevDesc.idVendor == 0x18D1 &&
     (phost->device.DevDesc.idProduct == 0x2D00 || phost->device.DevDesc.idProduct == 0x2D01)) {

    /* Find Bulk Endpoints (Simplified for Interface 0) */
    /* Note: need to iterate interfaces if 0 doesn't work */
    aoa_handle.OutEp = phost->device.CfgDesc.Itf_Desc[0].Ep_Desc[0].bEndpointAddress;
    aoa_handle.InEp  = phost->device.CfgDesc.Itf_Desc[0].Ep_Desc[1].bEndpointAddress;

    /* Ensure OutEp has bit 7 cleared (Host->Device) */
    if(aoa_handle.OutEp & 0x80) {
      uint8_t temp = aoa_handle.OutEp;
      aoa_handle.OutEp = aoa_handle.InEp;
      aoa_handle.InEp = temp;
    }

    aoa_handle.OutPipe = USBH_AllocPipe(phost, aoa_handle.OutEp);
    aoa_handle.InPipe  = USBH_AllocPipe(phost, aoa_handle.InEp);

    USBH_OpenPipe(phost, aoa_handle.OutPipe, aoa_handle.OutEp, phost->device.address, phost->device.speed, USB_EP_TYPE_BULK, 64);
    USBH_OpenPipe(phost, aoa_handle.InPipe, aoa_handle.InEp, phost->device.address, phost->device.speed, USB_EP_TYPE_BULK, 64);

    USBH_LL_SetToggle(phost, aoa_handle.InPipe, 0);
    USBH_LL_SetToggle(phost, aoa_handle.OutPipe, 0);

    aoa_handle.state = AOA_STATE_CONNECTED; /* Ready to talk! */
     } else {
       /* Not in AOA mode yet. Prepare the handshake. */
       aoa_handle.state = AOA_STATE_SEND_AOA_PROTOCOL_REQ;
     }
  return USBH_OK;
}

/**
  * @brief  De-Initialize the AOA class. Close out any opened pipes.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_AOA_InterfaceDeInit(USBH_HandleTypeDef *phost) {
  if(aoa_handle.OutPipe) {
    USBH_ClosePipe(phost, aoa_handle.OutPipe);
    USBH_FreePipe(phost, aoa_handle.OutPipe);
    aoa_handle.OutPipe = 0;
    aoa_handle.state = AOA_STATE_IDLE;
  }

  return USBH_OK;
}

/**
  * @brief  Handle the Class Requests.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_AOA_ClassRequest(USBH_HandleTypeDef *phost) {
  // not needed for aoa.
  return USBH_OK;
}


/**
  * @brief  Manage the AOA State Machine.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_AOA_Process(USBH_HandleTypeDef *phost) {
  USBH_StatusTypeDef status = USBH_BUSY;
  static uint8_t string_index = 0;
  static uint16_t protocol = 0;

  switch (aoa_handle.state) {
    case AOA_STATE_SEND_AOA_PROTOCOL_REQ: {
      // We do request 51 here
      if (phost->RequestState == CMD_SEND) {
        phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
        phost->Control.setup.b.bRequest = AOA_HANDSHAKE;
        phost->Control.setup.b.wValue.w = 0;
        phost->Control.setup.b.wIndex.w = 0;
        phost->Control.setup.b.wLength.w = 2;
      }
      status = USBH_CtlReq(phost, (uint8_t*) &protocol, 2);

      if (status == USBH_OK) {
        if (protocol >= 1) {
          aoa_handle.state = AOA_STATE_SEND_APP_DESCRIPTOR;
          string_index = 0;
        }
      } else if (status != USBH_BUSY) {
        aoa_handle.state = AOA_STATE_SEND_AOA_PROTOCOL_REQ;
      }
      break;
    }
    case AOA_STATE_SEND_APP_DESCRIPTOR: {
      size_t len_string = strlen(aoa_descriptors[string_index]);
      if (phost->RequestState == CMD_SEND) {
        phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
        phost->Control.setup.b.bRequest = AOA_SEND_DESCRIPTOR;
        phost->Control.setup.b.wValue.w = 0;
        phost->Control.setup.b.wIndex.w = string_index;
        phost->Control.setup.b.wLength.w = len_string;
      }

      status = USBH_CtlReq(phost,(uint8_t*) aoa_descriptors[string_index], len_string);
      if (status == USBH_OK) {
        string_index++;
        if (string_index >= 6) {
          aoa_handle.timer = HAL_GetTick();
          aoa_handle.state = AOA_STATE_WAIT_REBOOT_REQ;
        }
      }
      break;
    }
    case AOA_STATE_WAIT_REBOOT_REQ: {
      if ((HAL_GetTick() - aoa_handle.timer) > 100) { // Wait 100ms
        aoa_handle.state = AOA_SEND_REBOOT_REQ;
      }
      status = USBH_OK; // Keep returning OK so the loop runs
      break;
    }
    case AOA_SEND_REBOOT_REQ: {

      /* Start Accessory Mode (Request 53) */
      if (phost->RequestState == CMD_SEND) {
        phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
        phost->Control.setup.b.bRequest = AOA_REBOOT;
        phost->Control.setup.b.wValue.w = 0;
        phost->Control.setup.b.wIndex.w = 0;
        phost->Control.setup.b.wLength.w = 0;
      }

      status = USBH_CtlReq(phost, NULL, 0);

      if (status == USBH_OK) {
        /* Device will now Re-Enumerate. We sit here and wait for Disconnect event. */
        aoa_handle.timer = HAL_GetTick();
        aoa_handle.state = AOA_STATE_WAIT_FOR_NEXT_ENUM;
      }
      break;
    }

    case AOA_STATE_WAIT_FOR_NEXT_ENUM: {
      // Phone should be resetting now.
      if ((HAL_GetTick() - aoa_handle.timer) > 500) {
        USBH_UsrLog("AOA Watchdog: Missed Disconnect Event. Forcing Reset.");

        /* FORCE the Host Core to realize the device is gone/changed */
        /* This forces a re-enumeration of whatever is currently on the bus */
        USBH_ReEnumerate(phost);

        /* Reset our state to prevent loops */
        aoa_handle.state = AOA_STATE_IDLE;
      }
      status = USBH_OK;
      break;
    }

    case AOA_STATE_CONNECTED: {
      status = USBH_OK;
      break;
    }
    default:
      break;
  }
  return status;
}

/**
  * @}
  */

/**
  * @brief  Transmits data over AOA interface
  * @param  pbuff: Pointer to data buffer
  * @param  length: Length of data to be sent
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_AOA_Transmit(uint8_t *pbuff, uint16_t length) {
  if (aoa_handle.state != AOA_STATE_CONNECTED) {
    return USBH_FAIL;
  }

  USBH_URBStateTypeDef URB_Status = USBH_LL_GetURBState(&hUsbHostFS, aoa_handle.OutPipe);

  if (URB_Status != USBH_URB_DONE && URB_Status != USBH_URB_IDLE) {
    return USBH_BUSY;
  }

  return USBH_BulkSendData(&hUsbHostFS, pbuff, length, aoa_handle.OutPipe, 0);
}