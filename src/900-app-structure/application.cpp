//
// Created by peter on 23/11/24.
//
#include "application.h"

Application::Application() : m_window("Application", sf::Vector2u(800, 600)), m_elapsed(sf::Time::Zero), mStatisticsUpdateTime(sf::Time::Zero), m_robot() {
  RestartClock();
  m_elapsed = sf::Time::Zero;
  mStatisticsUpdateTime = sf::Time::Zero;

  m_textbox.Setup(5, 12, 300, sf::Vector2f(450, 10));
  m_textbox.Add("Hello World!");
  m_window.AddObserver(this);
  m_robot.Start(this);
}

Application::~Application() {
  m_robot.Stop();  // Ensure the robot thread stops
}

void Application::OnEvent(const Event& event) {
  switch (event.type) {
    case EventType::SFML_EVENT:
      if (event.event.type == sf::Event::MouseWheelScrolled) {
        std::string msg = "SFML Event - Mouse Wheel Scrolled ";
        event.event.mouseWheelScroll.delta > 0 ? msg += "Up" : msg += "Down";
        m_textbox.Add(msg);
      }
      break;
    case EventType::USER_EVENT:
      m_textbox.Add("USER Event");
      break;
    default:
      m_textbox.Add("UNHANDLED Event");
      break;
  }
}

void Application::Update(sf::Time deltaTime) {
  m_window.Update();  // call this first to process window events
  m_elapsed += deltaTime;
  // Update sensor data for the robot
  CalculateSensorValues();
  // now read back what the robot sees
  SensorValues sensors = m_robot.GetSensorValues();
  char str[100];
  sprintf(str, "%4d %4d %4d %4d ", (int)sensors.lfs_value, (int)sensors.lds_value, (int)sensors.rds_value, (int)sensors.rfs_value);
  std::string msg = std::to_string(sensors.lfs_value) + " " + std::to_string(sensors.rfs_value);
  m_textbox.Add(str);
}

void Application::Render() {
  m_window.BeginDraw();
  sf::RenderWindow& window = *m_window.GetRenderWindow();
  draw_line(window, {40, 90}, mp, sf::Color::Red);
  m_textbox.Render(window);

  // Draw the robot using RobotDisplay
  sf::Vector2f pose = m_robot.GetPose();
  float orientation = m_robot.GetOrientation();
  RobotDisplay::Draw(*m_window.GetRenderWindow(), pose, orientation);

  m_window.EndDraw();
}

void Application::HandleInput() {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    mp = sf::Vector2f(sf::Mouse::getPosition(*m_window.GetRenderWindow()));
    std::string msg = "Left Mouse button pressed";
    m_textbox.Add(msg);
  }
}

void Application::Run() {
  while (!GetWindow()->IsDone()) {
    HandleInput();
    Update();
    Render();
    RestartClock();
  }
}

void Application::UpdateStatistics(sf::Time elapsedTime) {
  (void)(elapsedTime);
  //
}

Window* Application::GetWindow() {
  return &m_window;  //
}

sf::Time Application::GetElapsed() {
  return m_elapsed;  //
}

void Application::RestartClock() {
  m_clock.restart();  //
}

void Application::CalculateSensorValues() {
  uint32_t ticks = m_robot.millis();
  /// just modify the sensor values a bit to see that they change
  float v = 50.0f + (ticks % 1000) / 10.0f;
  {
    std::lock_guard<std::mutex> lock(m_sensorDataMutex);
    m_sensorValues.lfs_value = v;
    m_sensorValues.rfs_value = v;
    m_sensorValues.lds_value = v;
    m_sensorValues.rds_value = v;
    m_sensorValues.lfs_dist = sqrtf(v);
    m_sensorValues.rfs_dist = sqrtf(v);
    m_sensorValues.lds_dist = sqrtf(v);
    m_sensorValues.rds_dist = sqrtf(v);
  }
}

SensorValues& Application::GetSensorValues() {
  return m_sensorValues;
}
