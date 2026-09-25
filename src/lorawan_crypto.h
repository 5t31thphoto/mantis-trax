#pragma once
#include <Arduino.h>

bool lorawanVerifyMic(const uint8_t *phy, size_t len, uint32_t devAddr, uint32_t fCnt, const uint8_t nwkSKey[16]);
void lorawanDecryptPayload(uint8_t *payload, size_t len, uint32_t devAddr, uint32_t fCnt, uint8_t direction, const uint8_t key[16]);
