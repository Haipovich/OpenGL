#include "Camera.h"
#include <MyMath/MyMath.h>
#include <algorithm>

Camera::Camera(MyMath::vec3 position, MyMath::vec3 up, float yaw, float pitch)
    : Front(MyMath::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

MyMath::mat4 Camera::GetViewMatrix() {
    return MyMath::mat4::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 75.0f)
        Zoom = 75.0f;
}

void Camera::updateCameraVectors() {
    MyMath::vec3 front;
    front.x = cos(MyMath::radians(Yaw)) * cos(MyMath::radians(Pitch));
    front.y = sin(MyMath::radians(Pitch));
    front.z = sin(MyMath::radians(Yaw)) * cos(MyMath::radians(Pitch));
    Front = MyMath::normalize(front);
    Right = MyMath::normalize(MyMath::cross(Front, WorldUp));
    Up = MyMath::normalize(MyMath::cross(Right, Front));
} 
