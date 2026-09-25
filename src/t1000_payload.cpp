#include "t1000_payload.h"
static uint32_t u32be(const uint8_t*p){return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];}
static int32_t i32be(const uint8_t*p){return (int32_t)u32be(p);}
bool decodeT1000(const uint8_t*p,size_t len,TrackerFix&out){
  if(len<18) return false; uint8_t id=p[0]; if(id!=0x06 && id!=0x09) return false;
  size_t need=id==0x06?23:19; if(len<need) return false;
  out={}; out.frame=id; out.event=((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3]; out.motion=p[4]; out.utc=u32be(p+5); out.lon=i32be(p+9)/1000000.0; out.lat=i32be(p+13)/1000000.0; out.battery=p[id==0x06?22:18]; out.valid=(out.lat>=-90&&out.lat<=90&&out.lon>=-180&&out.lon<=180);
  return out.valid;
}
