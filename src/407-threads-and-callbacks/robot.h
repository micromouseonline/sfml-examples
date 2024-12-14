//
// Created by peter on 30/11/24.
//

#ifndef IMGUI_SFML_STARTER_ROBOT_H
#define IMGUI_SFML_STARTER_ROBOT_H


#include <thread>
#include <functional>
#include <mutex>
#include <iostream>

class Robot {
public:
    void setSensorDataCallback(std::function<void()> callback) {
        sensorDataCallback = callback;
    }

    void run() {
        running = true;
        robotThread = std::thread(&Robot::simulate, this);
    }

    void stop() {
        running = false;
        if (robotThread.joinable()) {
            robotThread.join();
        }
    }

    void someFunction() {
        std::lock_guard<std::mutex> lock(robotMutex);
        std::cout << "Robot function called." << std::endl;
    }

    uint32_t getTicks() {
        return ticks;//

    }

private:
    void simulate() {
        while (running) {
            ticks++;
            if (sensorDataCallback) {
                sensorDataCallback();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::function<void()> sensorDataCallback;
    std::thread robotThread;
    std::atomic<bool> running{false};
    std::mutex robotMutex;

    uint32_t ticks;
};

#endif //IMGUI_SFML_STARTER_ROBOT_H
