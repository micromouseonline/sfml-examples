#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "SFML/Graphics.hpp"

#include "robot.h"
#include "application.h"

Robot robot;
Application application(robot);

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
    sf::RenderWindow window{sf::VideoMode(800, 600), "406 multi threading for real time simulation", sf::Style::Default,
                            settings};

    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("./assets/fonts/national-park.otf")) {
        exit(1);
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color(140, 91, 54));
    text.setPosition(50, 5);
    application.start();
    robot.run();

    float x = 0;
    /// now we can do the main loop
    while (window.isOpen()) {
        ////  EVENTS    //////////////////////////////////////////////////////////////////////
        sf::Event event{};
        bool should_close = false;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                should_close = true;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.scancode == sf::Keyboard::Scancode::Escape) {
                    should_close = true;
                }
            }
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, (float) event.size.width, (float) event.size.height);
                window.setView(sf::View(visibleArea));  // or everything distorts
            }
        }
        if (should_close) {
//      running = false;
            window.close();
        }

        ////  DISPLAY   //////////////////////////////////////////////////////////////////////
        window.clear();
        window.draw(text);
        window.display();
        //////////////////////////////////////////////////////////////////////////////////////
    }
    robot.stop();
    application.stop();

    /// wait for the simple threads to terminate before we continue
    std::cout << "All the threads are joined" << std::endl;

    return 0;
}
