#pragma once

#include "Transformable.hpp"

class Camera : public Transformable {
private:
    glm::mat4 m_view;
    glm::mat4 m_perspective;

public:
    Camera() = default;
    Camera(glm::vec3 position, glm::vec3 center, float fov, float aspect, float near, float far);

    glm::mat4 getView();
    glm::mat4 getPerspective();

    void setView(glm::vec3 position, glm::vec3 center);
    void setPerspective(float fov, float aspect, float near, float far);
};