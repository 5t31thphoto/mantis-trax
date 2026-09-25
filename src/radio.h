#pragma once
#include <Arduino.h>
#include <RadioLib.h>
#include "config.h"
#include "t1000_payload.h"
struct RadioStatus { bool ready=false; bool locked=false; float freq=0; uint8_t dr=1; int rssi=0; float snr=0; uint32_t packets=0; uint32_t bad=0; uint32_t fcnt=0; };
void radioBegin(const AppConfig&cfg); void radioPoll(const AppConfig&cfg, TrackerFix&target); RadioStatus getRadioStatus();
