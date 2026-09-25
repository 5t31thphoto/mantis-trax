#pragma once
#include <Arduino.h>
struct AppConfig { String devAddr="00000000",devEui=""; uint32_t initialFcnt=0; uint8_t subBand=2,fPort=5,preferredDr=1; bool adr=false,scanAllDr=true,lockAfterPacket=true; uint32_t scanHoldMs=1200; String nwkSKey="00000000000000000000000000000000",appSKey="00000000000000000000000000000000"; float maxRangeM=500; String units="imperial"; };
bool configLoad(AppConfig&c); bool configSave(const AppConfig&c); bool hexKey(const String&s,uint8_t out[16]); uint32_t parseHex32(const String&s);
