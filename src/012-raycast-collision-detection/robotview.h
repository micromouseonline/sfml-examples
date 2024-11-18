//
// Created by peter on 02/11/2024.
//

#ifndef ROBOTVIEW_H
#define ROBOTVIEW_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "robot.h"

class RobotView {
 public:
  RobotView(Robot& robot) : m_robot(robot) {
    m_shape.setSize(sf::Vector2f(robot.m_width, robot.m_height));
    m_shape.setOrigin(robot.m_width / 2, robot.m_height / 2);
    m_shape.setFillColor(sf::Color::Green);
    m_shape.setPosition(m_robot.m_x, m_robot.m_y);
    sf::Image image;
    image.create(robot.m_width, robot.m_height, sf::Color::Blue);
    m_texture.loadFromImage(image);
    m_sprite.setTexture(m_texture);
    m_sprite.setOrigin(robot.m_origin_x, robot.m_origin_y);
    m_sprite.setPosition(m_robot.m_x, m_robot.m_y);
  }

  bool load_texture(const std::string& filename) {
    if (!m_texture.loadFromFile(filename)) {
      std::cerr << "Unable to load texture\n";
      return false;
    }
    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(sf::IntRect(0, 0, m_robot.m_width, m_robot.m_height));
    m_sprite.setOrigin(m_robot.m_origin_x, m_robot.m_origin_y);
    update();
    return true;
  }

  void update() {
    m_shape.rotate(m_robot.m_angle);
    m_shape.setPosition(m_robot.m_x, m_robot.m_y);
    m_sprite.setRotation(m_robot.m_angle);
    m_sprite.setPosition(m_robot.m_x, m_robot.m_y);
    // setTextureRect(sf::IntRect(m_robot.m_state * m_robot.m_width, 0, m_robot.m_width, m_robot.m_height));
    //    setRotation(m_robot.m_angle);
    //    setPosition(m_robot.m_x, m_robot.m_y);
  }
  void draw(sf::RenderTarget& target) {
    if (m_sprite.getTexture()) {
      target.draw(m_sprite);
    } else {
      target.draw(m_shape);
    }
  }

 private:
  Robot& m_robot;
  sf::RectangleShape m_shape;
  sf::Sprite m_sprite;
  sf::Texture m_texture;
};

#endif  // ROBOTVIEW_H
