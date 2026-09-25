#include "gnss.h"
static HardwareSerial GpsSerial(1); static TinyGPSPlus gps; static LocalFix fix;
void gnssBegin(){ GpsSerial.begin(115200,SERIAL_8N1,15,13); }
void gnssPoll(){ while(GpsSerial.available()) gps.encode(GpsSerial.read()); fix.valid=gps.location.isValid()&&gps.location.age()<3000; if(gps.location.isValid()){fix.lat=gps.location.lat();fix.lon=gps.location.lng();} if(gps.course.isValid())fix.course=gps.course.deg(); if(gps.speed.isValid())fix.speed_mps=gps.speed.mps(); if(gps.satellites.isValid())fix.sats=(uint8_t)min<uint32_t>(255,gps.satellites.value()); fix.age_ms=gps.location.age(); }
LocalFix getLocalFix(){return fix;}
