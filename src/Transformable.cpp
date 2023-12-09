#include "Transformable.hpp"

#include <glm/gtx/transform.hpp>

Transformable::Transformable() {
    m_position = glm::vec3(0.f, 0.f, 0.f);
    m_rotation = glm::vec3(0.f, 0.f, 0.f);
    m_scale = glm::vec3(1.f, 1.f, 1.f);

    m_transform = glm::mat4(1.f);
}

glm::vec3 Transformable::getScale() { return m_scale; }

glm::vec3 Transformable::getPosition() { return m_position; }

glm::vec3 Transformable::getRotation() { return m_rotation; }

void Transformable::setScale(glm::vec3 scale) {
    m_scale = scale;
    m_transformNeedUpdate = true;
}

void Transformable::setPosition(glm::vec3 position) {
    m_position = position;
    m_transformNeedUpdate = true;
}

void Transformable::setRotation(glm::vec3 rotation) {
    m_rotation = rotation;
    m_transformNeedUpdate = true;
}

void Transformable::move(glm::vec3 offset) {
    setPosition(m_position + offset);
}

void Transformable::scale(glm::vec3 factor) {
    setScale(m_scale * factor);
}

void Transformable::rotate(glm::vec3 angle) {
    setRotation(m_rotation + angle);
}

glm::mat4 Transformable::getTransform() {
    if (m_transformNeedUpdate) {
        updateTransform();
    }

    return m_transform;
}

void Transformable::updateTransform() {
    m_transform = glm::mat4(1.f);

    auto scale = glm::mat4(1.f);
    auto rotation = glm::mat4(1.f);
    auto translate = glm::mat4(1.f);

    scale = glm::scale(scale, m_scale);
    rotation = glm::rotate(rotation, glm::radians(m_rotation.x),
                           glm::vec3(1.f, 0.f, 0.f));
    rotation = glm::rotate(rotation, glm::radians(m_rotation.y),
                           glm::vec3(0.f, 1.f, 0.f));
    rotation = glm::rotate(rotation, glm::radians(m_rotation.z),
                           glm::vec3(0.f, 0.f, 1.f));
    translate = glm::translate(translate, m_position);

    m_transform = m_transform * translate * scale * rotation;
    m_transformNeedUpdate = false;
}