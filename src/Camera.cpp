#include "Camera.hpp"
#include "Input.hpp"

#include <GLFW/glfw3.h>
#include <glm/gtx/euler_angles.hpp>

#include <fmt/core.h>

glm::mat4 Camera::getView() {
    if (m_viewNeedUpdate) {
        updateView();
    }

    return m_view;
}

glm::vec3 Camera::getPosition() {
    return m_position;
}

glm::mat4 Camera::getProjection() { return m_projection; }

void Camera::setSpeed(float speed) { m_speed = speed; }

void Camera::setSensitivity(float sensitivity) { m_sensitivity = sensitivity; }

void Camera::update(float deltaTime) {
    float speed = m_speed;
    if (Keyboard::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        speed *= 10.f;
    }
    auto deltaRight   = m_right * speed * deltaTime;
    auto deltaForward = m_direction * speed * deltaTime;
    auto deltaUp      = glm::vec3(0.f, 1.f, 0.f) * speed * deltaTime;

    if (Keyboard::isKeyPressed(GLFW_KEY_W)) {
        m_position += deltaForward;
        m_viewNeedUpdate = true;
    }
    if (Keyboard::isKeyPressed(GLFW_KEY_S)) {
        m_position -= deltaForward;
        m_viewNeedUpdate = true;
    }
    if (Keyboard::isKeyPressed(GLFW_KEY_A)) {
        m_position -= deltaRight;
        m_viewNeedUpdate = true;
    }
    if (Keyboard::isKeyPressed(GLFW_KEY_D)) {
        m_position += deltaRight;
        m_viewNeedUpdate = true;
    }
    if (Keyboard::isKeyPressed(GLFW_KEY_SPACE)) {
        m_position += deltaUp;
        m_viewNeedUpdate = true;
    }
    if (Keyboard::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
        m_position -= deltaUp;
        m_viewNeedUpdate = true;
    }

    float rotSpeed = 100.0f;
    if (Keyboard::isKeyPressed(GLFW_KEY_UP)) {
        m_pitch -= rotSpeed * deltaTime;
        m_viewNeedUpdate = true;
    }
    if (Keyboard::isKeyPressed(GLFW_KEY_DOWN)) {
        m_pitch += rotSpeed * deltaTime;
        m_viewNeedUpdate = true;
    }
    if (Keyboard::isKeyPressed(GLFW_KEY_RIGHT)) {
        m_yaw += rotSpeed * deltaTime;
        m_viewNeedUpdate = true;
    }
    if (Keyboard::isKeyPressed(GLFW_KEY_LEFT)) {
        m_yaw -= rotSpeed * deltaTime;
        m_viewNeedUpdate = true;
    }

    if (Mouse::isButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        Mouse::setCursorMode(GLFW_CURSOR_DISABLED);

        // this is so that we don't getTexture absurd delta when first pressing right click
        auto mouseDelta = (!m_rotating) ? glm::vec2(0.f, 0.f) : Mouse::getPosition();
        deltaTime = 0.16f;
        m_yaw += mouseDelta.x * m_sensitivity * deltaTime;
        m_pitch += mouseDelta.y * m_sensitivity * deltaTime;
        Mouse::setPosition({0.f, 0.f});


        m_rotating       = true;
        m_viewNeedUpdate = true;
    } else {
        m_rotating = false;
        Mouse::setCursorMode(GLFW_CURSOR_NORMAL);
    }

    m_pitch = glm::clamp(m_pitch, -80.f, 80.f);
    if (m_yaw <= -360.f || m_yaw >= 360.f) {
        m_yaw = 0.f;
    }

    auto yawRot = glm::rotate(glm::mat4(1.f), glm::radians(m_yaw), glm::vec3(0.f, 1.f, 0.f));
    m_right     = glm::vec4(m_initialRight, 1.f) * yawRot;
    m_direction = glm::vec4(m_initialDirection, 1.f) * yawRot;

    auto pitchRot = glm::rotate(glm::mat4(1.f), glm::radians(m_pitch), m_right);
    m_direction   = glm::vec4(m_direction, 1.f) * pitchRot;
}

void Camera::setView(glm::vec3 position, glm::vec3 center) {
    auto direction = glm::normalize(center - position);

    m_position         = position;
    m_direction        = direction;
    m_right            = glm::cross(m_direction, glm::vec3(0.f, 1.f, 0.f));
    m_initialRight     = m_right;
    m_initialPosition  = m_position;
    m_initialDirection = m_direction;

    updateView();
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
    m_projection = glm::perspective(fov, aspect, near, far);
}

void Camera::setOrthographic(float left,
                             float right,
                             float bottom,
                             float top,
                             float near,
                             float far) {
    m_projection = glm::ortho(left, right, bottom, top, near, far);
}

void Camera::updateView() {
    m_view           = glm::lookAt(m_position, m_position + m_direction, glm::vec3(0.f, 1.f, 0.f));
    m_viewNeedUpdate = false;
}