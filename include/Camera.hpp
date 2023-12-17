#pragma once

#include <glm/glm.hpp>

class Camera {
private:
    glm::mat4 m_view;
    glm::mat4 m_projection;

    bool m_setProjection{};

public:
    Camera() = default;

    glm::mat4 getView();
    glm::mat4 getPerspective();

    void setView(glm::vec3 position, glm::vec3 center);
    void setPerspective(float fov, float aspect, float near = 0.1f, float far = 1000.0f);
    void setOrthographic(float left,
                         float right,
                         float bottom,
                         float top,
                         float near = 0.1f,
                         float far  = 1000.0f);
};