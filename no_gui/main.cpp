#include <pigpio.h>
#include <iostream>
#include "motion_detector.h"
#include "camera_stream.h"

#define PIR_PIN 17

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed!" << std::endl;
        return 1;
    }

    gpioSetMode(PIR_PIN, PI_INPUT);

    initCamera();

    std::cout << "Monitoring motion..." << std::endl;
    while (true) {
        if (isMotionDetected(PIR_PIN)) {
            startStreaming(PIR_PIN);
        }
        gpioDelay(100000); // 100ms
    }

    gpioTerminate();
    return 0;
}
