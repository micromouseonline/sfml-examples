#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include "imgui-SFML.h"
#include "imgui.h"
#include "implot.h"

///
const int NUM_POINTS = 314;
float x_data[NUM_POINTS] = {0};
float y_data[NUM_POINTS];

void generate_data(float fx, float fy) {
  for (int x = 0; x < NUM_POINTS; x++) {
    x_data[x] = sinf((float)x / fx);
    y_data[x] = sinf((float)x / fy);
  }
}

float fa = 3.0;
float fb = 4.0;
int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 16;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(800, 600), "302-implot-with-imgui", sf::Style::Default, settings};

  window.setVerticalSyncEnabled(true);
  if (!ImGui::SFML::Init(window)) {
    return -1;
  }

  generate_data(fa, fb);
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
    if (ImPlot::BeginPlot("My Plot")) {
      ImPlot::PlotLine("Simple XY", x_data, y_data, NUM_POINTS);
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
