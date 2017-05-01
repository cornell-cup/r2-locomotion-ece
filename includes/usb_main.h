

#ifndef _USB_MAIN
#define _USB_MAIN

#include <stdint.h>
#include "R2Protocol.h"

#define BUFFER_LENGTH   255

uint16_t readIndex, writeIndex;
uint8_t readBuffer[BUFFER_LENGTH];

int ProcessIO(struct R2ProtocolPacket *packet);

#endif