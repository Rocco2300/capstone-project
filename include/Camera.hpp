#pragma once

#include <glm/glm.hpp>

class Camera {
private:
    glm::mat4 m_view;
    glm::mat4 m_projection;

    float m_yaw{};
    float m_pitch{};
    glm::vec3 m_right;
    glm::vec3 m_position;
    glm::vec3 m_direction;

    glm::vec3 m_initialRight;
    glm::vec3 m_initialPosition;
    glm::vec3 m_initialDirection;

    float m_speed{};
    bool m_rotating{};
    float m_sensitivity{};
    bool m_viewNeedUpdate{};

public:
    Camera() = default;

    glm::mat4 getView();
    glm::mat4 getProjection();

    void setSpeed(float speed);
    void setSensitivity(float sensitivity);

    void update(float deltaTime);

    void setView(glm::vec3 position, glm::vec3 center);
    void setPerspective(float fov, float aspect, float near = 0.1f, float far = 1000.0f);
    void setOrthographic(float left,
                         float right,
                         float bottom,
                         float top,
                         float near = 0.1f,
                         float far  = 1000.0f);

protected:
    void updateView();
};