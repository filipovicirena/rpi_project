#include <pigpio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h> // for sleep()

#define PIR_PIN 17  // GPIO 17 (physical pin 11)

// Global variable to store PIR sensor state
int level = 0;  // Default to no motion

// Capture and show video for the duration of motion
void manage_stream(cv::VideoCapture& cap) {
    cv::Mat frame;

    while (true) {
        level = gpioRead(PIR_PIN);  // Read the PIR sensor value

        if (level == 1) {  // If motion is detected
            std::cout << "Motion detected! Starting camera stream..." << std::endl;
            while (level == 1) {  // Keep streaming while motion is detected
                cap >> frame;
                if (frame.empty()) continue;

                cv::imshow("Camera Stream", frame);

                // If the level goes back to 0 (motion stopped), break the loop
                level = gpioRead(PIR_PIN);  // Continuously check the sensor value
                if (level == 0) {
                    std::cout << "Motion ended. Stopping camera stream." << std::endl;
                    break;
                }

                // Allow user to press 'q' to quit the stream manually
                if (cv::waitKey(30) == 'q') {
                    break;
                }
            }
        } else {
            // If no motion detected, just wait a bit before checking again
            gpioDelay(100000);  // 100ms delay to reduce CPU usage
        }
    }

    cap.release();
    cv::destroyAllWindows();
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed!" << std::endl;
        return -1;
    }

    gpioSetMode(PIR_PIN, PI_INPUT);  // Set the PIR_PIN as input

    // Open the camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Camera failed to open!" << std::endl;
        gpioTerminate();
        return -1;
    }

    manage_stream(cap);  // Start the stream management loop

    gpioTerminate();  // Clean up when done
    return 0;
}
