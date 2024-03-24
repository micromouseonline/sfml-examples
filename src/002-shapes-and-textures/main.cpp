#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <math.h>

/***
 * Create a simple SFML window with custom title text and a couple of shapes drawn in it
 */

int main()
{
    /// Any antialiasing has to be set globally when creating the window:
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8; // the number of multisamplings to use. 4 is probably fine
    sf::RenderWindow window{sf::VideoMode(600, 400), "002-shapes-and-textures", sf::Style::Default, settings};
    window.setFramerateLimit(60);
    // window.setPosition(sf::Vector2i(20, 20));

    /// First we need to load a texture:
    sf::Texture texture;
    if (!texture.loadFromFile("./assets/images/spaceship-mono.png")) {
        std::cerr << "Unable to load texture\n";
        exit(1);
    }

    /// Rectangles are simple shapes with a width and height
    sf::Vector2f size(40, 36);
    size.x *= 8;
    size.y *= 8;
    sf::RectangleShape rect(size);
    rect.setOrigin(size.x / 2, size.y / 2);
    rect.setPosition(300, 200);
    // rect.setFillColor(sf::Color(255, 255, 0, 192));
    /// and we can map a texture to it
    /// the texture will be scaled to fit the rectangle
    rect.setTexture(&texture); // texture is a sf::Texture
    rect.setTextureRect(sf::IntRect(39, 0, 40, 36));

    /// Or we can have an actual sprite:
    /// It is not a good idea to load a lot of textures but for a demo...
    sf::Texture chicken;
    if (!chicken.loadFromFile("./assets/images/chicken.png")) {
        std::cerr << "Unable to load texture\n";
        exit(1);
    }

    const int seq[] = {1, 2, 1, 0};
    sf::Sprite chick;
    chick.setTexture(chicken);
    chick.setTextureRect(sf::IntRect(0, 0, 32, 32));
    chick.setOrigin(16, 16);
    chick.setPosition(200, 200);
    chick.scale(2, 2);
    // setting a colour overlays the texture
    chick.setColor(sf::Color(255, 255, 0, 255));

    sf::Clock deltaClock{};
    int state = 0;
    int f = 0;

    /// this is the 'game loop'
    while (window.isOpen()) {
        /// process all the inputs
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            /// if you want items to stay the same size, you must look after resize events
            /// it can still look scrappy during the resize
            /// This works because the scene is effectively infinite and the window is just
            /// a view of it starting at (0,0)
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, (float) event.size.width, (float) event.size.height);
                /// Note that constructing a view with a sf::Rect defines the corners
                /// Using two points defines the _centre_ and the width,height
                window.setView(sf::View(visibleArea)); // or everything distorts
            }
        }

        /// update the objects
        sf::Time time = deltaClock.restart();
        chick.rotate(90 * time.asSeconds());
        float angle = chick.getRotation();
        float dx = std::cos((angle - 90) * 3.14 / 180) * 200;
        float dy = std::sin((angle - 90) * 3.14 / 180) * 200;
        chick.move(sf::Vector2f(dx, dy) * time.asSeconds());
        chick.setTextureRect(sf::IntRect(32 * seq[state / 5], 0, 32, 32));
        state = (++state) % 20;

        /// rotate the rect and its texture rotates with it.
        /// the graphics card handles all that.
        rect.rotate(45 * time.asSeconds());

        /// and redraw the window
        window.clear();
        window.draw(rect);
        window.draw(chick);
        window.display();
    }

    return 0;
}
