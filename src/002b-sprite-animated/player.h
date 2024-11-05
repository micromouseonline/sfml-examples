//
// Created by peter on 05/04/24.
//

#pragma once

#include <cmath>
#include "entity.h"

/***
 * Game objects are all derived from the Entity base class
 *
 */
class Player final : public Entity {
 private:
  bool mThrust = false;
  const float mAcceleration = 0.17f;
  const float mMaxSpeed = 12.f;
  const float mDrag = 0.99f;
  const float mDegtorad = 0.017453f;

 public:
  Player(float width, float height, const Animation& a, float x, float y, float angle = 0.f, float radius = 1.f) : Entity(a, x, y, angle, radius) {
    mName = PLAYER;
    mWindowWidth = width;
    mWindowHeight = height;
    mDx = 0;
    mDy = 0;
  };

  bool thrust() const { return mThrust; }
  void setThrust(const bool thrust) { mThrust = thrust; }

  void reset(const Animation& a, float x, float y, float angle) {
    mAnimation = a;
    mX = x;
    mY = y;
    mAngle = angle;
    mDx = 0;
    mDy = 0;
  }

  void update(float dt) override {
    Entity::update(dt);

    if (mThrust) {
      float theta = (-90 + mAngle) * mDegtorad;
      mDx += std::cos(theta) * mAcceleration;
      mDy += std::sin(theta) * mAcceleration;
      mDx = std::cos(theta) * 1.2;
      mDy = std::sin(theta) * 1.2;

    } else {
      //      mDx *= mDrag;
      //      mDy *= mDrag;
    }

    float speed = std::sqrt(mDx * mDx + mDy * mDy);
    if (speed > mMaxSpeed) {
      mDx *= mMaxSpeed / speed;
      mDy *= mMaxSpeed / speed;
    }

    mX += mDx;
    mY += mDy;

    if (mX > mWindowWidth) {
      mX = 0;
    }
    if (mX < 0) {
      mX = mWindowWidth;
    }
    if (mY > mWindowHeight) {
      mY = 0;
    }
    if (mY < 0) {
      mY = mWindowHeight;
    }
  };
};