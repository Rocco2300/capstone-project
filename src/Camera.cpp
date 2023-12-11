#include "Camera.hpp"

#include <glm/gtx/transform.hpp>

Camera::Camera(glm::vec3 position,
               glm::vec3 direction,
               float fov,
               float aspect,
               float near,
               float far)
    : m_position{position}, m_direction{direction} {
    setPerspective(fov, aspect, near, far);
}

glm::mat4 Camera::getView() { return createView(); }

glm::mat4 Camera::getPerspective() { return m_perspective; }

void Camera::setView(glm::vec3 position, glm::vec3 direction) {
    m_position = position;
    m_direction = direction;
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
    m_perspective = glm::perspective(fov, aspect, near, far);
}

glm::mat4 Camera::createView() {
    return glm::lookAt(m_position, m_position + m_direction, glm::vec3(0.f, 1.f, 0.f));
}