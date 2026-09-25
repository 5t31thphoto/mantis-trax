#pragma once
#include <Arduino.h>
#include <TinyGPSPlus.h>
struct LocalFix { bool valid=false; double lat=0; double lon=0; double course=0; double speed_mps=0; uint8_t sats=0; uint32_t age_ms=0; };
void gnssBegin(); void gnssPoll(); LocalFix getLocalFix();
