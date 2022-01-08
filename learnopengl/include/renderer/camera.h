#pragma once

#include <common.h>
#include <math.h>

const float MAX_ZOOM_LEVEL = 45.0f;

enum class CameraMovementBit : int {
    None = 0x0,
    Forward = 0x1,
    Backward = 0x2,
    Left = 0x4,
    Right = 0x8,
};

inline CameraMovementBit operator|(CameraMovementBit a, CameraMovementBit b) {
    return static_cast<CameraMovementBit>(static_cast<int>(a) | static_cast<int>(b));
}

inline CameraMovementBit operator&(CameraMovementBit a, CameraMovementBit b) {
    return static_cast<CameraMovementBit>(static_cast<int>(a) & static_cast<int>(b));
}

struct CameraOptions {
    float Yaw, Pitch, MovementSpeed, Sensitivity, Zoom;
};

const CameraOptions defaultCameraOptions = {-90.0f, 0.0f, 2.5f, 0.1f, 45.0f};

class Camera {
   private:
    glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;
    CameraOptions m_Options;

   public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
           CameraOptions options = defaultCameraOptions);

    Camera(float posX, float posY, float posZ, float worldUpX, float worldUpY, float worldUpZ,
           CameraOptions options = defaultCameraOptions);

    glm::mat4 ViewMatrix() const;
    void ProcessMovement(CameraMovementBit direction, float deltaTime);
    void ProcessRotation(float xOffset, float yOffset, bool constrainPitch = true);
    void ProcessZoom(float offset);

    inline glm::vec3 GetPosition() const {
        return m_Position;
    }

    inline glm::vec3 GetFront() const {
        return m_Front;
    }

    inline float GetZoom() const {
        return m_Options.Zoom;
    }

   private:
    void updateCameraVectors();
};
