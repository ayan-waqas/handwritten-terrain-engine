#ifndef CAMERA_H
#define CAMERA_H

#include "../math/Vec3.h"
#include "../math/Mat4.h"
#include <cmath>
#include <string>

class Camera {
public:
    Vec3 position;
    Vec3 front;
    Vec3 up;
    Vec3 right;
    Vec3 worldUp;

    float yaw;
    float pitch;
    float speed;
    float sensitivity;

    Camera(Vec3 startPos = Vec3(0.0f, 30.0f, 60.0f), Vec3 startUp = Vec3(0.0f, 1.0f, 0.0f), float startYaw = -90.0f, float startPitch = -20.0f)
        : position(startPos), worldUp(startUp), yaw(startYaw), pitch(startPitch), speed(20.0f), sensitivity(0.1f) {
        updateCameraVectors();
    }

    Mat4 getViewMatrix() const {
        return Mat4::lookAt(position, position + front, up);
    }

    // view matrix for rendering water planar reflection pass
    Mat4 getReflectionViewMatrix(float waterY) const {
        Vec3 invPos = Vec3(position.x, 2.0f * waterY - position.y, position.z);
        Vec3 invFront = Vec3(front.x, -front.y, front.z).normalize();
        Vec3 invUp = Vec3(0.0f, 1.0f, 0.0f);
        return Mat4::lookAt(invPos, invPos + invFront, invUp);
    }

    void processKeyboard(const std::string& direction, float deltaTime) {
        float velocity = speed * deltaTime;
        if (direction == "FORWARD")
            position = position + front * velocity;
        if (direction == "BACKWARD")
            position = position - front * velocity;
        if (direction == "LEFT")
            position = position - right * velocity;
        if (direction == "RIGHT")
            position = position + right * velocity;
    }

    void processMouseMovement(float xOffset, float yOffset) {
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        updateCameraVectors();
    }

private:
    void updateCameraVectors() {
        float radYaw = yaw * (3.1415926535f / 180.0f);
        float radPitch = pitch * (3.1415926535f / 180.0f);

        Vec3 newFront;
        newFront.x = std::cos(radYaw) * std::cos(radPitch);
        newFront.y = std::sin(radPitch);
        newFront.z = std::sin(radYaw) * std::cos(radPitch);
        front = newFront.normalize();

        right = Vec3::cross(front, worldUp).normalize();
        up = Vec3::cross(right, front).normalize();
    }
};

#endif
