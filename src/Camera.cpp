#include "Camera.hpp"

#include <glm/gtx/transform.hpp>

Camera::Camera(glm::vec3 position,
               glm::vec3 center,
               float fov,
               float aspect,
               float near,
               float far) {
    setPosition(position);
    setView(position, center);
    setPerspective(fov, aspect, near, far);
}

glm::mat4 Camera::getView() { return glm::inverse(getTransform()) * m_view; }

glm::mat4 Camera::getPerspective() { return m_perspective; }

void Camera::setView(glm::vec3 position, glm::vec3 center) {
    setPosition(position);
    auto direction = glm::normalize(center - position);
    m_view         = glm::lookAt(glm::vec3(0.f), direction, glm::vec3(0.f, 1.f, 0.f));
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
    m_perspective = glm::perspective(fov, aspect, near, far);
}