#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include <thread>
#include "imgui-SFML.h"
#include "imgui.h"
#include "implot.h"
#include "profile.h"
#include "robot.h"

///
const int NUM_POINTS = 3000;
float x_data[NUM_POINTS] = {0};
float y_data[NUM_POINTS];

int simBot() {
  Robot robot;
  Behavior behavior(robot);

  // Simulate a 1 kHz systick
  const float deltaTime = 0.001f;  // 1 ms

  // Request a move
  robot.setSpeeds(800, 0);
  behavior.requestMove(1000.0f, 1000.0f, 4.0f, 2900.0f);
  float t = 0;
  int i = 0;
  while (behavior.m_profile.getPhase() != Profile::Phase::Complete || i < NUM_POINTS) {
    robot.update(deltaTime);
    behavior.update(deltaTime);
    x_data[i] = t;
    y_data[i] = robot.velocity;
    std::cout << t << " - " << robot.distance << " - " << robot.velocity << std::endl;
    // Simulate a delay for the systick
    t += deltaTime;
    i += 1;
  }
  return i;
}

float fa = 3.0;
float fb = 4.0;
int main() {
  int steps = simBot();
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 16;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(800, 600), "302-implot-with-imgui", sf::Style::Default, settings};

  window.setVerticalSyncEnabled(true);
  if (!ImGui::SFML::Init(window)) {
    return -1;
  }

  ImPlot::CreateContext();

  sf::Clock deltaClock{};

  /// this is the 'game loop'
  while (window.isOpen()) {
    /// process all the inputs
    sf::Event event{};
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(window, event);
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }
    //
    //    /// update the objects
    //    /// ImGui MUST get updated every cycle
    sf::Time time = deltaClock.restart();
    ImGui::SFML::Update(window, time);

    ImGui::Begin("Hello, ImPLot");
    static bool show_demo = false;
    bool cb_changed = ImGui::Checkbox("Show ImPlot Demo", &show_demo);
    ImGui::End();

    ImGui::Begin("A Simple Plot");
    if (ImPlot::BeginPlot("Line Plots")) {
      ImPlot::SetupAxes("x", "y");
      ImPlot::SetupAxisLimits(ImAxis_X1, 0, 2.50);
      ImPlot::SetupAxisLimits(ImAxis_Y1, -10, 1200);
      ImPlot::PlotLine("f(x)", x_data, y_data, steps);
      // ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      // ImPlot::PlotLine("g(x)", xs2, ys2, 20,ImPlotLineFlags_Segments);
      ImPlot::EndPlot();
    }
    ImGui::End();
    //
    //    ImGui::ShowDemoWindow();
    if (show_demo) {
      ImPlot::ShowDemoWindow();
    }
    if (cb_changed) {
      std::cout << "Yeh!" << std::endl;
    }

    /// and redraw the window
    window.clear();
    ImGui::SFML::Render(window);
    window.display();
  }
  ImPlot::DestroyContext();
  //  ImGui::DestroyContext();

  return 0;
}
