#include "Input.hpp"

#include <GLFW/glfw3.h>

std::unordered_set<int> Keyboard::m_pressedKeys;

void Keyboard::callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        m_pressedKeys.insert(key);
    } else if (action == GLFW_RELEASE) {
        m_pressedKeys.erase(key);
    }
}

bool Keyboard::isKeyPressed(int key) { return m_pressedKeys.count(key); }

glm::vec2 Mouse::m_position{};
std::unordered_set<int> Mouse::m_pressedButtons;

void Mouse::posCallback(GLFWwindow* window, double x, double y) { m_position = {x, y}; }

void Mouse::buttonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        m_pressedButtons.insert(button);
    } else if (action == GLFW_RELEASE) {
        m_pressedButtons.erase(button);
    }
}

glm::vec2 Mouse::getPosition() { return m_position; }

bool Mouse::isButtonPressed(int button) { return m_pressedButtons.count(button); }

void Mouse::setCursorMode(int value) { glfwSetInputMode(m_window, GLFW_CURSOR, value); }

void Mouse::setRawMotion(bool value) {
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, value);
    }
}

void Mouse::setWindow(GLFWwindow* window) { m_window = window; }

void Mouse::setPosition(glm::vec2 position) {
    m_position = position;
    glfwSetCursorPos(m_window, m_position.x, m_position.y);
}