#pragma once
#include <Arduino.h>
struct TrackerFix { bool valid=false; uint8_t frame=0; double lat=0; double lon=0; uint32_t utc=0; uint8_t battery=0; uint8_t motion=0; uint32_t event=0; };
bool decodeT1000(const uint8_t *p, size_t len, TrackerFix &out);
