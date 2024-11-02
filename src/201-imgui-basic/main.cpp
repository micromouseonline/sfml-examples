#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include "imgui-SFML.h"
#include "imgui.h"
// sadly the definition of M_PI is nor platform independent
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Properties {
  float radius = 20.0f;
  float pos_x = 600.0f;
  float pos_y = 100.0f;
  float angle = 0;
  int points = 5;

  sf::Color colour = sf::Color::Green;
};

void draw_divider() {
  ImVec2 p1 = ImGui::GetCursorScreenPos();
  float padding = ImGui::GetStyle().WindowPadding.x;
  ImVec2 p2 = ImVec2(p1.x + ImGui::GetWindowWidth() - 2 * padding, p1.y);  // Adjust the length as needed
  // Draw a horizontal line
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->AddLine(p1, p2, IM_COL32(255, 255, 255, 100), 2.0f);
  // Add a small space below the line
  ImGui::Dummy(ImVec2(0.0f, 10.0f));  // Adjust the height as needed
}

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 16;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(800, 600), "002-shapes-and-textures", sf::Style::Default, settings};

  window.setVerticalSyncEnabled(true);
  if (!ImGui::SFML::Init(window)) {
    return -1;
  }
  Properties properties;
  sf::CircleShape shape(properties.radius);
  shape.setFillColor(properties.colour);
  shape.setOrigin(properties.radius, properties.radius);
  shape.setPosition(properties.pos_x, properties.pos_y);
  shape.setPointCount(properties.points);

  sf::Clock deltaClock{};

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

    /// update the objects
    /// ImGui MUST get updated every cycle
    sf::Time time = deltaClock.restart();
    ImGui::SFML::Update(window, time);

    /// wrap each new dialog in a begin...end pair
    ImGui::Begin("Hello, world!");
    ImGui::Text("CTRL+click to edit widget with keyboard");
    draw_divider();

    static bool show_demo = false;
    //    bool cb_changed = ImGui::Checkbox("Show ImGui Demo", &show_demo);
    if (ImGui::SliderInt("Points", &properties.points, 3, 36)) {
      shape.setPointCount(properties.points);
    }
    if (ImGui::SliderFloat("Radius", &properties.radius, 20, 300, "%4.1f")) {
      shape.setRadius(properties.radius);
      shape.setOrigin(properties.radius, properties.radius);
    }
    if (ImGui::SliderAngle("Angle", &properties.angle)) {  // actually uses radians
      shape.setRotation(properties.angle * 360.0f / (float)M_PI);
    }
    ImGui::End();

    if (show_demo) {
      ImGui::ShowDemoWindow();
    }

    /// and redraw the window
    window.clear();
    window.draw(shape);
    ImGui::SFML::Render(window);

    window.display();
  }

  return 0;
}
