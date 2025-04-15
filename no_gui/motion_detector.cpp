#include <pigpio.h>
#include "motion_detector.h"

bool isMotionDetected(int pin) {
    return gpioRead(pin) == 1;
}
