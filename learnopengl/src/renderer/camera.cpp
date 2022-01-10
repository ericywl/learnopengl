#include <renderer/camera.h>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, CameraOptions options)
    : m_Position(position), m_WorldUp(worldUp), m_Options(options) {
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float worldUpX, float worldUpY, float worldUpZ,
               CameraOptions options)
    : m_Options(options) {
    m_Position = glm::vec3(posX, posY, posZ);
    m_WorldUp = glm::vec3(worldUpX, worldUpY, worldUpZ);
    updateCameraVectors();
}

glm::mat4 Camera::ViewMatrix() const {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::ProcessMovement(CameraMovementBit direction, float deltaTime) {
    float velocity = m_Options.MovementSpeed * deltaTime;
    glm::vec3 finalDir(0.0f);

    if (static_cast<int>(direction & CameraMovementBit::Forward)) {
        finalDir += m_Front;
    }
    if (static_cast<int>(direction & CameraMovementBit::Backward)) {
        finalDir -= m_Front;
    }
    if (static_cast<int>(direction & CameraMovementBit::Left)) {
        finalDir -= m_Right;
    }
    if (static_cast<int>(direction & CameraMovementBit::Right)) {
        finalDir += m_Right;
    }
    if (static_cast<int>(direction & CameraMovementBit::Up)) {
        finalDir += m_Up;
    }
    if (static_cast<int>(direction & CameraMovementBit::Down)) {
        finalDir -= m_Up;
    }

    if (glm::length(finalDir) != 0) {
        m_Position += glm::normalize(finalDir) * velocity;
    }
}

void Camera::ProcessRotation(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= m_Options.Sensitivity;
    yOffset *= m_Options.Sensitivity;

    m_Options.Yaw += xOffset;
    m_Options.Pitch += yOffset;

    // Make sure to clamp pitch so that screen doesn't flip
    if (constrainPitch) {
        if (m_Options.Pitch > 89.0f) {
            m_Options.Pitch = 89.0f;
        }
        if (m_Options.Pitch < -89.0f) {
            m_Options.Pitch = -89.0f;
        }
    }

    updateCameraVectors();
}

void Camera::ProcessZoom(float offset) {
    m_Options.Zoom -= offset;
    if (m_Options.Zoom < 1.0f) {
        m_Options.Zoom = 1.0f;
    }

    if (m_Options.Zoom > MAX_ZOOM_LEVEL) {
        m_Options.Zoom = MAX_ZOOM_LEVEL;
    }
}

void Camera::updateCameraVectors() {
    // Calculate camera's front vector
    glm::vec3 front{
        cos(glm::radians(m_Options.Yaw)) * cos(glm::radians(m_Options.Pitch)),
        sin(glm::radians(m_Options.Pitch)),
        sin(glm::radians(m_Options.Yaw)) * cos(glm::radians(m_Options.Pitch)),
    };
    m_Front = glm::normalize(front);

    // Recalculate up and right vector
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
