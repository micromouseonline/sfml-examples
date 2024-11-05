//
// Created by peter on 05/04/24.
//

#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include "animation.h"

/***
 * Entities are the game objects. They have an animation associated with them
 *
 */

enum eName { PLAYER, MISSILE };
class Entity {
 protected:
  float mWindowHeight;
  float mWindowWidth;
  float mAngle;
  float mR;
  float mDy;
  float mDx;
  float mX;
  float mY;
  eName mName;
  Animation mAnimation;
  std::default_random_engine mRand;

 public:
  Entity(const Animation& a, float x, float y, float angle = 0.f, float radius = 1.f)
      : mX(x), mY(y), mDx(), mDy(), mR(radius), mAngle(angle), mWindowWidth(), mWindowHeight(), mName(PLAYER), mAnimation(a), mRand() {}

  float x() const { return mX; }
  float y() const { return mY; }
  void setPosition(float x, float y) {
    mX = x;
    mY = y;
  };

  float radius() const { return mR; }
  float angle() const { return mAngle; }
  eName name() const { return mName; }
  const Animation animation() const { return mAnimation; }

  void setAngle(const float angle) { mAngle = angle; }
  void setName(const eName name) { mName = name; }
  void setAnimation(const Animation& a) { mAnimation = a; }

  virtual void update(float dt) {  //
    mAnimation.update(dt);
  };

  void draw(sf::RenderWindow& app) {
    sf::Sprite sprite = mAnimation.sprite();
    sprite.setPosition(mX, mY);
    //    sprite.setRotation(mAngle);
    sprite.setScale(2, 2);
    app.draw(sprite);
  };

  virtual ~Entity() = default;
};