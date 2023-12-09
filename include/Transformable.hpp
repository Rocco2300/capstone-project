#pragma once

#include <glm/glm.hpp>

class Transformable {
private:
    glm::vec3 m_position{};
    glm::vec3 m_rotation{};
    glm::vec3 m_scale{};

    glm::mat4 m_transform{};
    bool m_transformNeedUpdate{};

public:
    Transformable();

    glm::vec3 getScale();
    glm::vec3 getPosition();
    glm::vec3 getRotation();

    void setScale(glm::vec3 scale);
    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);

    void move(glm::vec3 offset);
    void scale(glm::vec3 factor);
    void rotate(glm::vec3 angle);

    glm::mat4 getTransform();

private:
    void updateTransform();
};