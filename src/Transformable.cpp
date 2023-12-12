#include "Transformable.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

Transformable::Transformable() {
    m_scale = glm::vec3(1.f, 1.f, 1.f);
    m_origin = glm::vec3(0.f, 0.f, 0.f);
    m_position = glm::vec3(0.f, 0.f, 0.f);
    m_rotation = glm::vec3(0.f, 0.f, 0.f);

    updateTransform();
}

glm::vec3 Transformable::getScale() { return m_scale; }

glm::vec3 Transformable::getOrigin() { return m_origin; }

glm::vec3 Transformable::getPosition() { return m_position; }

glm::vec3 Transformable::getRotation() { return m_rotation; }

void Transformable::setScale(glm::vec3 scale) {
    m_scale = scale;
    m_transformNeedUpdate = true;
}

void Transformable::setOrigin(glm::vec3 origin) {
    m_origin = origin;
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

void Transformable::move(glm::vec3 offset) { setPosition(m_position + offset); }

void Transformable::scale(glm::vec3 factor) { setScale(m_scale * factor); }

void Transformable::rotate(glm::vec3 angle) { setRotation(m_rotation + angle); }

glm::mat4 Transformable::getTransform() {
    if (m_transformNeedUpdate) {
        updateTransform();
    }

    return m_transform;
}

void Transformable::updateTransform() {
    const auto identity = glm::mat4(1.f);

    auto yaw = glm::eulerAngleY(glm::radians(m_rotation.y));
    auto roll = glm::eulerAngleZ(glm::radians(m_rotation.z));
    auto pitch = glm::eulerAngleX(glm::radians(m_rotation.x));
    auto rotation = roll * pitch * yaw;

    auto scale = glm::scale(identity, m_scale);
    auto translate = glm::translate(identity, m_position);
    auto originTranslate = glm::translate(identity, -m_origin);

    m_transform = translate * scale * rotation * originTranslate;
    m_transformNeedUpdate = false;
}