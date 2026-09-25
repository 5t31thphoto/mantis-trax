#pragma once
#include <Arduino.h>
#include "config.h"
#include "gnss.h"
#include "navigation.h"
#include "radio.h"
void uiBegin(); void uiSplash(); bool uiSetup(AppConfig&cfg); void uiRadar(const AppConfig&cfg,const LocalFix&local,const TrackerFix&target,const TargetVector&vec,float heading,const RadioStatus&rs);
