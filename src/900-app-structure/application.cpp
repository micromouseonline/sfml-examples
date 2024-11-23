//
// Created by peter on 23/11/24.
//
#include "application.h"

Application::Application()
    : m_window("Application", sf::Vector2u(800, 600)), m_elapsed(sf::Time::Zero), mStatisticsNumFrames(0), mStatisticsUpdateTime(sf::Time::Zero), m_robot() {
  RestartClock();
  m_elapsed = sf::Time::Zero;
  mStatisticsNumFrames = 0;
  mStatisticsUpdateTime = sf::Time::Zero;

  m_textbox.Setup(5, 12, 300, sf::Vector2f(450, 10));
  m_textbox.Add("Hello World!");
  m_window.AddObserver(this);
  m_robot.Start();
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
  std::string msg = "Millis: " + std::to_string(m_robot.millis());
  m_textbox.Add(msg);
  m_elapsed += deltaTime;
  // Update sensor data for the robot
  {
    std::lock_guard<std::mutex> lock(m_sensorDataMutex);
    //    m_sensorData = {0,0,0};// Generate new sensor data based on the environment
  }
  m_robot.UpdateSensors(m_sensorData);
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
