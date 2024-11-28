#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include "imgui-SFML.h"
#include "imgui.h"
#include "implot.h"

///
const int NUM_POINTS = 600;
float x_data[NUM_POINTS] = {0};
float y_data[NUM_POINTS];

void generate_data(float fx, float fy) {
  for (int x = 0; x < NUM_POINTS; x++) {
    x_data[x] = sinf((float)x / fx);
    y_data[x] = sinf((float)x / fy);
  }
}

float fa = 40.0;
float fb = 30.0;
int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(1100, 800), WINDOW_TITLE, sf::Style::Default, settings};

  window.setVerticalSyncEnabled(true);
  if (!ImGui::SFML::Init(window)) {
    return -1;
  }
  ImGui::GetStyle().AntiAliasedFill = true;
  ImGui::GetStyle().AntiAliasedLines = true;

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

    ImGui::SetNextWindowSize(ImVec2(400, 650));
    ImGui::Begin("A Simple Plot", nullptr, ImGuiWindowFlags_NoResize);
    if (ImPlot::BeginPlot("First Plot")) {
      ImPlot::SetNextLineStyle(ImPlot::GetColormapColor(0));
      ImPlot::PlotLine("Time Series", x_data, NUM_POINTS);
      ImPlot::SetNextLineStyle(ImPlot::GetColormapColor(1));
      ImPlot::PlotLine("Time Series", y_data, NUM_POINTS);
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("Another Plot")) {
      ImPlot::PlotLine("Simple XY", x_data, y_data, NUM_POINTS);
      ImPlot::EndPlot();
    }
    ImGui::End();
    ImPlot::ShowDemoWindow();

    /// and redraw the window
    window.clear();
    ImGui::SFML::Render(window);
    window.display();
  }
  ImPlot::DestroyContext();
  ImGui::SFML::Shutdown();

  return 0;
}
