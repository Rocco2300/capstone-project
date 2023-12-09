#pragma once

#include "Transformable.hpp"

class Camera : public Transformable {
private:
    glm::mat4 m_view;

public:
    Camera(glm::vec3 position, glm::vec3 direction);

    glm::mat4 getView();
};