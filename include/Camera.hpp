#pragma once

#include "Transformable.hpp"

class Camera {
private:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_direction;

    glm::mat4 m_view;
    glm::mat4 m_perspective;

public:
    Camera() = default;
    Camera(glm::vec3 position,
           glm::vec3 direction,
           float fov,
           float aspect,
           float near,
           float far);

    glm::mat4 getView();
    glm::mat4 getPerspective();

    void setView(glm::vec3 position, glm::vec3 direction);
    void setPerspective(float fov, float aspect, float near, float far);

private:
    glm::mat4 createView();
};