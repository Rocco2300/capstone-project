#include "Camera.hpp"

#include <glm/gtx/transform.hpp>

#include <iostream>

glm::mat4 Camera::getView() { return m_view; }

glm::mat4 Camera::getPerspective() { return m_projection; }

void Camera::setView(glm::vec3 position, glm::vec3 center) {
    auto direction = glm::normalize(center - position);
    m_view = glm::lookAt(position, direction, glm::vec3(0.f, 1.f, 0.f));
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
    if (m_setProjection) {
        std::cerr << "Warning: projection already set for camera.\n";
    }

    m_projection    = glm::perspective(fov, aspect, near, far);
    m_setProjection = true;
}

void Camera::setOrthographic(float left,
                             float right,
                             float bottom,
                             float top,
                             float near,
                             float far) {
    if (m_setProjection) {
        std::cerr << "Warning: projection already set for camera.\n";
    }

    m_projection = glm::ortho(left, right, bottom, top, near, far);
}