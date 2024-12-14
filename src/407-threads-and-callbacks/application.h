//
// Created by peter on 30/11/24.
//

#ifndef IMGUI_SFML_STARTER_APPLICATION_H
#define IMGUI_SFML_STARTER_APPLICATION_H

#include <functional>
#include <thread>
#include <mutex>
#include "robot.h"

class Application {
public:
    Application(Robot &robot) : robot(robot) {
        robot.setSensorDataCallback(std::bind(&Application::handleSensorData, this));
    }

    void start() {
        appThread = std::thread(&Application::run, this);
    }

    void stop() {
        running = false;
        if (appThread.joinable()) {
            appThread.join();
        }
    }

private:
    void run() {
        running = true;
        while (running) {
            // Application logic here
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void handleSensorData() {
        std::lock_guard<std::mutex> lock(appMutex);
        std::cout << "Application handling sensor data." << robot.getTicks() << std::endl;
        // Call function in Robot
        robot.someFunction();
    }

    Robot &robot;
    std::thread appThread;
    std::atomic<bool> running{false};
    std::mutex appMutex;
};

#endif //IMGUI_SFML_STARTER_APPLICATION_H
