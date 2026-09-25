#pragma once
#include <Arduino.h>
#include "gnss.h"
#include "t1000_payload.h"
struct TargetVector { bool valid=false; double distance_m=0; double bearing_deg=0; double north_m=0; double east_m=0; double relative_deg=0; };
TargetVector makeTargetVector(const LocalFix& local,const TrackerFix& target,double heading_deg);
class HeadingEstimator { public: void reset(); void update(float gz_dps,float dt,const LocalFix& local); float heading() const{return heading_deg;} bool hasCourse() const{return course_valid;} private: float heading_deg=0; bool initialized=false,course_valid=false; double lastLat=0,lastLon=0; uint32_t lastMoveMs=0; };
