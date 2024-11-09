### Pure Pursuit: An Overview

**Pure Pursuit** is a simple, geometric path-tracking algorithm widely used in robotics and autonomous vehicles. It
calculates steering commands to follow a given path by always targeting a "lookahead point" on the path. This approach
is particularly effective for two-wheeled, non-holonomic robots, like differential-drive robots.

---

### How Pure Pursuit Works

1. **Path Representation**:
    - The path to be followed is represented as a series of waypoints or a continuous curve.

2. **Lookahead Point**:
    - A **lookahead point** is a point on the path located a fixed distance \( L \) ahead of the robot’s current
      position.
    - The lookahead distance \( L \) is a crucial parameter:
        - **Shorter \( L \)** results in tighter, more responsive turns but can lead to instability or overshooting.
        - **Larger \( L \)** smooths the trajectory but may struggle with sharp turns.

3. **Steering Calculation**:
    - The robot calculates the curvature of the path needed to reach the lookahead point.
    - From this curvature, the robot derives a steering angle or angular velocity.

4. **Control Command**:
    - The robot generates linear (\( v \)) and angular (\( \omega \)) velocities based on the computed steering.

---

### Mathematical Foundation

#### Key Variables:

- \( P_r = (x_r, y_r) \): Robot's current position.
- \( \theta_r \): Robot's heading angle (orientation).
- \( P_t = (x_t, y_t) \): Lookahead point on the path.
- \( L \): Lookahead distance.
- \( \Delta x = x_t - x_r \): Horizontal distance to the lookahead point.
- \( \Delta y = y_t - y_r \): Vertical distance to the lookahead point.

#### Steering Angle Calculation:

1. Transform the lookahead point into the robot’s local coordinate frame:
   \[
   x_l = \cos(\theta_r) \cdot \Delta x + \sin(\theta_r) \cdot \Delta y
   \]
   \[
   y_l = -\sin(\theta_r) \cdot \Delta x + \cos(\theta_r) \cdot \Delta y
   \]

2. Compute the curvature \( \kappa \), which is inversely proportional to the radius of the turning circle:
   \[
   \kappa = \frac{2 \cdot y_l}{L^2}
   \]

3. From the curvature, derive the angular velocity (\( \omega \)) or steering angle (\( \delta \)):
    - Angular velocity:
      \[
      \omega = v \cdot \kappa
      \]
    - Steering angle (for cars or bicycles):
      \[
      \delta = \arctan(L \cdot \kappa)
      \]

#### Differential Drive Implementation:

For a differential drive robot, the angular velocity is used to control the turning rate:
\[
v_{\text{left}} = v - \frac{L \cdot \omega}{2}
\]
\[
v_{\text{right}} = v + \frac{L \cdot \omega}{2}
\]

---

### Advantages of Pure Pursuit

1. **Simplicity**:
    - Easy to implement and computationally efficient.
2. **Smooth Trajectories**:
    - Naturally generates smooth curves, especially with a larger lookahead distance.
3. **Robustness**:
    - Works well for a variety of path shapes and does not require complex tuning.

---

### Challenges and Limitations

1. **Parameter Sensitivity**:
    - Choosing an appropriate lookahead distance \( L \) is critical.
    - Dynamic adjustment of \( L \) (e.g., based on speed) may improve performance.

2. **Sharp Turns**:
    - Can struggle with sharp corners if the lookahead point jumps far ahead on the path.
    - May need additional logic to slow down or switch to a different algorithm for tight turns.

3. **Path Overshoot**:
    - The robot may overshoot the path, especially at high speeds or with a large \( L \).

4. **Non-Ideal Paths**:
    - Works best with smooth, continuous paths. Disjoint waypoints or noisy paths require preprocessing.

---

### Practical Enhancements

1. **Dynamic Lookahead**:
    - Adjust \( L \) based on the robot's speed:
      \[
      L = k_v \cdot v + L_{\text{min}}
      \]
    - Ensures stability at high speeds and responsiveness at low speeds.

2. **Path Preprocessing**:
    - Smooth out jagged or discontinuous paths using splines or filters.

3. **Error Handling**:
    - Implement error-recovery mechanisms if the robot strays far from the path (e.g., switching to a `seek` behavior
      temporarily).

4. **Integration with Obstacle Avoidance**:
    - Modify the target point or curvature calculation based on real-time obstacle detection.

---

### Example Implementation

```cpp
void purePursuit(PVector& lookaheadPoint, float lookaheadDistance, float robotSpeed) {
    // Compute the local coordinates of the lookahead point
    float dx = lookaheadPoint.x - m_position.x;
    float dy = lookaheadPoint.y - m_position.y;

    // Transform to the robot's frame
    float local_x = cos(m_orientation) * dx + sin(m_orientation) * dy;
    float local_y = -sin(m_orientation) * dx + cos(m_orientation) * dy;

    // Calculate curvature
    float curvature = 2 * local_y / (lookaheadDistance * lookaheadDistance);

    // Compute angular velocity
    float omega = robotSpeed * curvature;

    // Apply control
    float leftSpeed = robotSpeed - (wheelBase * omega / 2);
    float rightSpeed = robotSpeed + (wheelBase * omega / 2);
    setWheelSpeeds(leftSpeed, rightSpeed);
}
```

---

Would you like a more detailed example or help adapting it to a specific robot?