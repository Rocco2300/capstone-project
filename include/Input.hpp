#pragma once

#include <glm/glm.hpp>

#include <unordered_set>

class GLFWwindow;

class Keyboard {
private:
    static std::unordered_set<int> m_pressedKeys;

public:
    static void callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static bool isKeyPressed(int key);
};

class Mouse {
private:
    static GLFWwindow* m_window;

    static glm::vec2 m_position;
    static std::unordered_set<int> m_pressedButtons;

public:
    static void posCallback(GLFWwindow* window, double x, double y);
    static void buttonCallback(GLFWwindow* window, int button, int action, int mods);

    static glm::vec2 getPosition();
    static bool isButtonPressed(int button);

    static void setCursorMode(int value);
    static void setRawMotion(bool value);
    static void setWindow(GLFWwindow* window);
    static void setPosition(glm::vec2 position);
};