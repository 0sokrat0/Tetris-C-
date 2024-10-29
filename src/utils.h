#pragma once
#include <raylib.h>

inline bool EventTriggered(double interval, double &lastTime) {
    double currentTime = GetTime();
    if (currentTime - lastTime >= interval) {
        lastTime = currentTime;
        return true;
    }
    return false;
}
