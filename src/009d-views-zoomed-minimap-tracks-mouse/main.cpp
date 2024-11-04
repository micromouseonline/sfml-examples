#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

/***
 * Load a large  (3000x3000) map as an image into a
 */

float element_space = 10.0;
float map_view_size = 1000.0f;
float sidebar_width = 300.0f;
float sidebar_height = 600.0f;
float mini_map_view_size = 300.0f;
float ui_title_font_height = 24.0f;
float ui_title_height = ui_title_font_height + 5.0f;
float window_width = map_view_size + sidebar_width + 3 * element_space;
float window_height = map_view_size + 2 * element_space;

/***
 * Calculate the viewport coordinates from the window coordinates
 * viewport is a rectanglular region in pixel coordinates
 * window is the window in which we want to see the viewport (in pixel coordinates)
 *
 * returns a normalised viewport using fractions of the window
 *
 */
sf::FloatRect calculate_viewport(const sf::FloatRect& viewport, const sf::FloatRect& window) {
  const float x = viewport.left / window.width;
  const float y = viewport.top / window.height;
  const float w = viewport.width / window.width;
  const float h = viewport.height / window.height;
  return {x, y, w, h};
}

/// map the mouse position withinn the viewport to a position in the view
sf::Vector2f constrain_mouse(float mx, float my, sf::FloatRect& view_port_rect, sf::View& view) {
  float mapScale = view.getSize().x / view_port_rect.width;

  // Calculate the scaled mouse position relative to the viewport
  float scaledMouseX = (mx - view_port_rect.left) * mapScale;
  float scaledMouseY = (my - view_port_rect.top) * mapScale;
  // Constrain the scaled mouse position to the map boundaries
  float minX = mini_map_view_size / 2.0f;
  float maxX = view.getSize().x - mini_map_view_size / 2.0f;
  float minY = mini_map_view_size / 2.0f;
  float maxY = view.getSize().y - mini_map_view_size / 2.0f;
  // Clamp the scaled mouse position within the map boundaries
  float clampedX = std::clamp(scaledMouseX, minX, maxX);
  float clampedY = std::clamp(scaledMouseY, minY, maxY);
  return {clampedX, clampedY};
}

int main() {
  sf::Color bg_colour(128, 128, 128, 255);
  sf::Color ui_colour(0, 63, 0, 128);

  // Create the SFML window
  sf::RenderWindow window(sf::VideoMode(window_width, window_height), WINDOW_TITLE);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/national-park.otf")) {
    exit(1);
  }

  // Load the map image and put it on a RectangleShape
  sf::Texture map_texture;
  if (!map_texture.loadFromFile("assets/images/map-3000x3000.png")) {
    return -1;
  }

  sf::RectangleShape map(sf::Vector2f(map_texture.getSize().x, map_texture.getSize().y));
  map.setTexture(&map_texture);

  /// Define the main regions for elements in the window
  /// This rectangle is the location of the main map in the window
  sf::FloatRect main_map_view_port_rect(element_space,                //
                                        element_space,                //
                                        map_view_size, map_view_size  //
  );

  /// This rectangle is the location of the mini map in the window
  sf::FloatRect mini_map_view_port_rect(map_view_size + 2 * element_space,                   //
                                        map_view_size + element_space - mini_map_view_size,  //
                                        mini_map_view_size, mini_map_view_size               //
  );

  /// This rectangle is the location of the other UI elements
  sf::FloatRect ui_rect(map_view_size + 2 * element_space,  // left
                        element_space,                      // top
                        sidebar_width, sidebar_height       //
  );

  sf::FloatRect visibleArea(0, 0, (float)window.getSize().x, (float)window.getSize().y);
  /// view the entire map
  sf::View main_map_view(sf::FloatRect(0, 0, map_texture.getSize().x, map_texture.getSize().y));
  main_map_view.setViewport(calculate_viewport(main_map_view_port_rect, visibleArea));

  /// make the mini map view the same size as its viewport area to get 1:1 pixels to coordinates
  sf::View mini_map_view(sf::FloatRect(0, 0, mini_map_view_size, mini_map_view_size));
  mini_map_view.setViewport(calculate_viewport(mini_map_view_port_rect, visibleArea));

  /// The view  used when drawing most stuff to the window - 1:1 pixels:coordinates
  sf::View main_view = window.getDefaultView();

  /// A filled rectangle represents the UI area
  sf::RectangleShape rect(ui_rect.getSize());
  rect.setPosition(ui_rect.getPosition());
  rect.setFillColor(ui_colour);

  /// A simple label for the minimap
  sf::Text text("Minimap", font, ui_title_font_height);
  text.setPosition(mini_map_view_port_rect.getPosition().x,                   //
                   mini_map_view_port_rect.getPosition().y - ui_title_height  //
  );

  // Main loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      else if (event.type == sf::Event::Resized) {  // seems to be generated when window is created as well
        /// get the new size of the window
        visibleArea = sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height);
        /// ensure the viewports scale correctly - comment these to scale everything with the window
        /// TODO: work out how to scale with same aspect ratio.
        main_map_view.setViewport(calculate_viewport(main_map_view_port_rect, visibleArea));
        mini_map_view.setViewport(calculate_viewport(mini_map_view_port_rect, visibleArea));
        main_view = sf::View(visibleArea);  // the viewport is the whole window by default
      }
    }

    /// constrain the mouse coordinates to the map
    float mx = sf::Mouse::getPosition(window).x;
    float my = sf::Mouse::getPosition(window).y;
    if (main_map_view_port_rect.contains(mx, my)) {
      mini_map_view.setCenter(constrain_mouse(mx, my, main_map_view_port_rect, main_map_view));
    }

    /// now we get to draw everything - first Clear the window
    window.clear(bg_colour);

    /// each view must be drawn separately? Not sure why.
    window.setView(main_view);
    window.draw(rect);
    window.draw(text);

    window.setView(main_map_view);
    window.draw(map);

    window.setView(mini_map_view);
    window.draw(map);

    window.display();
  }

  return 0;
}
