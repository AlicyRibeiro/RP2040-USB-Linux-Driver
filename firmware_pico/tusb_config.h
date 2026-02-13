#pragma once

#include "tusb_option.h"

// Pico (RP2040)
#define CFG_TUSB_MCU OPT_MCU_RP2040
#define CFG_TUSB_OS  OPT_OS_PICO

// Device mode
#define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE

// Endpoint 0
#define CFG_TUD_ENDPOINT0_SIZE 64

// Enable Vendor Class
#define CFG_TUD_VENDOR            1
#define CFG_TUD_VENDOR_RX_BUFSIZE 64
#define CFG_TUD_VENDOR_TX_BUFSIZE 64

// Disable other classes
#define CFG_TUD_CDC   0
#define CFG_TUD_MSC   0
#define CFG_TUD_HID   0
#define CFG_TUD_MIDI  0
