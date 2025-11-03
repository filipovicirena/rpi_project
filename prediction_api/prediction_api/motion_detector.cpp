#include <pigpio.h>
#include <iostream>
#include "motion_detector.h"

bool isMotionDetected(int pin) {
    return gpioRead(pin);
}
