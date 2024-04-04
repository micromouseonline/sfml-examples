#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

/***
 *  - Load a large  (3000x3000) map as an image into a texture
 *  - apply that texture to a rectangle of the same size
 *  - create a view of that map that is only 600x600 pixels
 *  - if we do not define a viewport, we see the view stretched out to fill the window
 *  - create a viewport that is the size of the view and positioned in the top left
 *  - draw the map using that view - it will just fill the viewport, scaling as needed
 *
 */

float element_space = 10.0;
float map_view_size = 600.0f;
float map_viewport_size = 300.0f;

float window_width = 1200;
float window_height = 800;

/***
 * Calculate the viewport coordinates from the window coordinates
 * The viewport is a rectanglular region in pixel coordinates
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

int main() {
  // Create the SFML window
  sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML Map and Mini Map View");

  // Load the map image and put it on a RectangleShape
  // we could use a sprite instead
  sf::Texture map_texture;
  if (!map_texture.loadFromFile("assets/images/map-3000x3000.png")) {
    return -1;
  }

  sf::RectangleShape map(sf::Vector2f(map_texture.getSize().x, map_texture.getSize().y));
  map.setTexture(&map_texture);

  /// There apears no single call that fetches the window as a rectangle
  sf::FloatRect windowRect(0, 0, (float)window.getSize().x, (float)window.getSize().y);

  /// create the view. It is a region of the world space we can see
  /// the view can be defined by a rectangle or a centre point and a size
  sf::View map_view(sf::FloatRect(0, 0, map_view_size, map_view_size));

  /// This rectangle is the location of the map in the window
  /// The view will be the scaled to fit the viewport automatically
  sf::FloatRect map_viewport_rect(element_space,                        //
                                  element_space,                        //
                                  map_viewport_size, map_viewport_size  //
  );
  /// but we do need to convert it into proportions of the window
  map_view.setViewport(calculate_viewport(map_viewport_rect, windowRect));

  /// The view  used when drawing most stuff to the window - 1:1 pixels:coordinates
  /// Note that the view is not updated automatically when the window is resized
  sf::View main_view = window.getDefaultView();

  // Main loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      else if (event.type == sf::Event::Resized) {  // seems to be generated when window is created as well
        /// get the new size of the window
        windowRect = sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height);
        /// ensure the viewports scale correctly - comment these to scale everything with the window
        // the default viewport is the whole window but we need to adjust it if stuff is to avoid being stretched
        main_view = sf::View(windowRect);
        /// the map viewport must be recalculated to keep it the same size on the screen
        map_view.setViewport(calculate_viewport(map_viewport_rect, windowRect));
      }
      if (event.type == sf::Event::MouseWheelMoved) {
        if (map_viewport_size >= 200 && map_viewport_size <= 600) {
          if (event.mouseWheel.delta > 0) {
            map_viewport_size *= 1.1;
          } else if (event.mouseWheel.delta < 0) {
            map_viewport_size *= 0.9;
          }
          map_viewport_size = std::clamp(map_viewport_size, 200.0f, 600.0f);  // C++17
          map_viewport_rect = {element_space, element_space, map_viewport_size, map_viewport_size};
          map_view.setViewport(calculate_viewport(map_viewport_rect, windowRect));
        }
      }
    }

    /// now we get to draw everything - first Clear the window
    window.clear();

    /// drawing to the whole window
    window.setView(main_view);
    sf::RectangleShape frame(sf::Vector2f(map_viewport_size + 10, map_viewport_size + 10));
    frame.setFillColor(sf::Color::Green);
    frame.setPosition(map_viewport_rect.left - 5.0f, map_viewport_rect.top - 5.0f);
    window.draw(frame);

    /// each view must be drawn separately? Not sure why.
    window.setView(map_view);
    window.draw(map);

    window.display();
  }

  return 0;
}
