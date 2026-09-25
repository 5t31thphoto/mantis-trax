#include <Arduino.h>
#include <M5Cardputer.h>
#include "config.h"
#include "gnss.h"
#include "navigation.h"
#include "radio.h"
#include "ui.h"

static AppConfig cfg; static LocalFix local; static TrackerFix target; static HeadingEstimator heading; static uint32_t lastImu=0,lastDraw=0; static bool setupDone=false;
void setup(){auto c=M5.config();M5Cardputer.begin(c,true);uiBegin();uiSplash();configLoad(cfg);setupDone=uiSetup(cfg);if(!setupDone)configSave(cfg);gnssBegin();radioBegin(cfg);heading.reset();lastImu=millis();}
void loop(){M5Cardputer.update();gnssPoll();local=getLocalFix();uint32_t now=millis();auto iu=M5.Imu.update();if(iu){auto d=M5.Imu.getImuData();float dt=(now-lastImu)/1000.0f;lastImu=now;heading.update(d.gyro.z,dt,local);}radioPoll(cfg,target);TargetVector v=makeTargetVector(local,target,heading.heading());if(M5Cardputer.Keyboard.isChange()&&M5Cardputer.Keyboard.isPressed()){auto k=M5Cardputer.Keyboard.keysState();if(k.fn&&k.del){if(uiSetup(cfg))radioBegin(cfg);}}
if(now-lastDraw>100){uiRadar(cfg,local,target,v,heading.heading(),getRadioStatus());lastDraw=now;}delay(1);}
