#include "config.h"
#include <LittleFS.h>
#include <Preferences.h>
#include <ArduinoJson.h>
static Preferences nvs;
static bool validHex(const String&s,size_t n){if(s.length()!=n)return false;for(char c:s){if(!isxdigit((unsigned char)c))return false;}return true;}
uint32_t parseHex32(const String&s){return strtoul(s.c_str(),nullptr,16);}
bool hexKey(const String&s,uint8_t o[16]){if(!validHex(s,32))return false;for(int i=0;i<16;i++)o[i]=(uint8_t)strtoul(s.substring(i*2,i*2+2).c_str(),nullptr,16);return true;}
bool configLoad(AppConfig&c){
  if(!LittleFS.begin(true)) return false; File f=LittleFS.open("/config.json","r"); if(f){JsonDocument d; if(deserializeJson(d,f)==DeserializationError::Ok){c.devAddr=d["device"]["dev_addr"]|c.devAddr;c.devEui=d["device"]["dev_eui"]|c.devEui;c.initialFcnt=d["device"]["initial_fcnt"]|c.initialFcnt;c.subBand=d["lorawan"]["sub_band"]|c.subBand;c.fPort=d["lorawan"]["f_port"]|c.fPort;c.adr=d["lorawan"]["adr"]|c.adr;c.nwkSKey=d["lorawan"]["nwk_s_key"]|c.nwkSKey;c.appSKey=d["lorawan"]["app_s_key"]|c.appSKey;c.preferredDr=d["radio"]["preferred_dr"]|c.preferredDr;c.scanAllDr=d["radio"]["scan_all_data_rates"]|c.scanAllDr;c.scanHoldMs=d["radio"]["scan_hold_ms"]|c.scanHoldMs;c.lockAfterPacket=d["radio"]["lock_after_packet"]|c.lockAfterPacket;c.maxRangeM=d["navigation"]["max_range_m"]|c.maxRangeM;c.units=d["navigation"]["units"]|c.units;} f.close();}
  nvs.begin("mantistrax",false); if(nvs.isKey("devaddr")) c.devAddr=nvs.getString("devaddr",c.devAddr); if(nvs.isKey("nwk"))c.nwkSKey=nvs.getString("nwk",c.nwkSKey);if(nvs.isKey("app"))c.appSKey=nvs.getString("app",c.appSKey);if(nvs.isKey("fcnt"))c.initialFcnt=nvs.getUInt("fcnt",c.initialFcnt);return true;
}
bool configSave(const AppConfig&c){nvs.begin("mantistrax",false);nvs.putString("devaddr",c.devAddr);nvs.putString("nwk",c.nwkSKey);nvs.putString("app",c.appSKey);nvs.putUInt("fcnt",c.initialFcnt);return true;}
