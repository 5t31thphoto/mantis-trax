#include "radio.h"
#include "lorawan_crypto.h"
#include <Wire.h>

static constexpr int PIN_NSS=5,PIN_DIO1=4,PIN_RST=3,PIN_BUSY=6,PIN_SCK=40,PIN_MOSI=14,PIN_MISO=39;
static constexpr uint8_t IOE_ADDR=0x43;
static constexpr uint8_t IOE_DIR=0x03, IOE_OUT=0x05;
SX1262 radio=new Module(PIN_NSS,PIN_DIO1,PIN_RST,PIN_BUSY);
static RadioStatus st; static uint32_t wantedAddr=0; static uint8_t nwk[16],app[16]; static uint8_t chan=0,drIndex=0; static uint32_t lastTune=0;
static const float freqs[8]={903.9,904.1,904.3,904.5,904.7,904.9,905.1,905.3};
static const uint8_t drs[4]={1,0,2,3};
static uint8_t sfForDr(uint8_t dr){return dr==0?10:dr==1?9:dr==2?8:7;}
static void antennaOn(){Wire1.begin(8,9,400000);Wire1.beginTransmission(IOE_ADDR);Wire1.write(IOE_DIR);Wire1.write(0xFE);Wire1.endTransmission();Wire1.beginTransmission(IOE_ADDR);Wire1.write(IOE_OUT);Wire1.write(0x01);Wire1.endTransmission();}
static bool tune(float f,uint8_t dr){int16_t rc=radio.begin(f,125.0,sfForDr(dr),5,0x12,10,8,1.6,false); if(rc!=RADIOLIB_ERR_NONE)return false; radio.setDio2AsRfSwitch(true); st.freq=f;st.dr=dr;return true;}
static bool parseFrame(const uint8_t*p,size_t len,const AppConfig&cfg,TrackerFix&target,uint32_t&acceptedFcnt){
 if(len<7)return false; uint8_t mtype=p[0]>>5;if(mtype!=2&&mtype!=4)return false; uint32_t addr=(uint32_t)p[1]|((uint32_t)p[2]<<8)|((uint32_t)p[3]<<16)|((uint32_t)p[4]<<24);if(addr!=wantedAddr)return false;uint8_t fctrl=p[5];uint16_t f16=(uint16_t)p[6]|((uint16_t)p[7]<<8);uint8_t fopts=fctrl&0x0F;size_t macStart=8,portPos=macStart+fopts;if(portPos>=len-4)return false;uint8_t port=p[portPos];size_t payPos=portPos+1,payLen=len-4-payPos;if(payLen==0)return false;
 uint32_t base=st.fcnt; uint32_t candidates[5]={base,base+1,base>0x10000?base-0x10000:0,(base&0xFFFF0000)|f16,(base+0x10000&0xFFFF0000)|f16};
 uint32_t best=0;bool ok=false;for(uint8_t i=0;i<5&&!ok;i++){uint32_t fc=(candidates[i]&0xFFFF0000)|f16;if(fc<base&&base-fc>0x8000)fc+=0x10000;if(fc>base&&fc-base>0x8000)fc-=0x10000;if(lorawanVerifyMic(p,len,addr,fc,nwk)){best=fc;ok=true;}}
 if(!ok)return false;uint8_t plain[255];if(payLen>sizeof(plain))return false;memcpy(plain,p+payPos,payLen);const uint8_t*key=(port==0?nwk:app);lorawanDecryptPayload(plain,payLen,addr,best,0,key);TrackerFix t;if(port!=cfg.f_port)return false;if(!decodeT1000(plain,payLen,t))return false;target=t;acceptedFcnt=best;return true;
}
void radioBegin(const AppConfig&cfg){antennaOn();wantedAddr=parseHex32(cfg.devAddr);hexKey(cfg.nwkSKey,nwk);hexKey(cfg.appSKey,app);st={};st.fcnt=cfg.initialFcnt;uint8_t first=cfg.scanAllDr?cfg.preferredDr:cfg.preferredDr;if(tune(freqs[0],first))st.ready=true;lastTune=millis();}
void radioPoll(const AppConfig&cfg,TrackerFix&target){if(!st.ready)return; if(!st.locked && millis()-lastTune>cfg.scanHoldMs){chan=(chan+1)%8;if(chan==0)drIndex=(drIndex+1)%4;uint8_t dr=cfg.scanAllDr?drs[drIndex]:cfg.preferredDr;tune(freqs[chan],dr);lastTune=millis();}
 uint8_t buf[255];int16_t rc=radio.receive(buf,sizeof(buf),50);if(rc==RADIOLIB_ERR_NONE){size_t n=radio.getPacketLength();uint32_t fc=0;if(parseFrame(buf,n,cfg,target,fc)){st.fcnt=fc;st.locked=cfg.lockAfterPacket;st.packets++;st.rssi=radio.getRSSI();st.snr=radio.getSNR();}else st.bad++;}}
RadioStatus getRadioStatus(){return st;}
