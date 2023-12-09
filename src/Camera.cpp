#include "Camera.hpp"

#include <glm/gtx/transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 direction) {
    setPosition(position);

    m_view = glm::lookAt(glm::vec3(0.f, 0.f, 0.f), direction,
                         glm::vec3(0.f, 1.f, 0.f));
}

glm::mat4 Camera::getView() { return m_view * getTransform(); }