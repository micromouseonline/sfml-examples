//
// Created by peter on 05/04/24.
//
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

class Animation {
 private:
  float m_frame;
  float m_speed;
  sf::Sprite m_sprite;
  std::vector<sf::IntRect> m_frames;

 public:
  Animation(const sf::Texture& t, int x, int y, int width, int height, int count, float speed) : m_frame(0), m_speed(speed), m_sprite(), m_frames() {
    /**
     * We assume any animations have frames that are in columns in the same row
     * ·---·---·---·---·---·---·
     * | 0 | 1 | 2 | 3 | 4 | 5 |
     * ·---·---·---·---·---·---·
     *
     */
    for (int i = 0; i < count; i++) {
      m_frames.push_back(sf::IntRect(x + i * width, y, width, height));
    }
    m_sprite.setTexture(t);
    m_sprite.setOrigin(static_cast<float>(width) / 2.f, static_cast<float>(height) / 2.f);
  }

  const sf::Sprite sprite() const { return m_sprite; }

  void update(float dt) {
    m_frame += m_speed * dt;
    auto n = static_cast<float>(m_frames.size());
    if (m_frame >= n) {
      m_frame -= n;
    }
    if (n > 0) {
      m_sprite.setTextureRect(m_frames[static_cast<int>(m_frame)]);
    }
  }

  bool isEnd() const { return m_frame + m_speed >= static_cast<float>(m_frames.size()); };
};