#include "Camera.h"
#include <MyMath/MyMath.h> // Для MyMath::functions (например, radians, normalize, cross)
#include <algorithm> // для std::max/min, если нужно для ProcessMouseScroll

// Конструктор с векторами
Camera::Camera(MyMath::vec3 position, MyMath::vec3 up, float yaw, float pitch)
    : Front(MyMath::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Возвращает матрицу вида, вычисленную с использованием углов Эйлера и LookAt матрицы
MyMath::mat4 Camera::GetViewMatrix() {
    return MyMath::mat4::lookAt(Position, Position + Front, Up);
}

// Обрабатывает ввод, полученный от любой клавиатуроподобной системы ввода.
// Принимает входной параметр в виде определенного камерой ENUM (для абстракции от системы ввода)
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
    // Если нужно движение вверх/вниз, можно добавить:
    // if (direction == UP_MOVE) Position += WorldUp * velocity; // или просто Up, если не нужен "полет"
    // if (direction == DOWN_MOVE) Position -= WorldUp * velocity;
}

// Обрабатывает ввод, полученный от системы ввода мыши.
// Ожидает смещение как в x, так и в y направлении.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Убеждаемся, что когда тангаж выходит за пределы обзора, экран не переворачивается
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Обновляем векторы Front, Right и Up, используя обновленные углы Эйлера
    updateCameraVectors();
}

// Обрабатывает ввод, полученный от колеса прокрутки мыши.
// Требуется только ввод по оси Y
void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset; // yoffset обычно 1 или -1
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 75.0f) // Ограничиваем максимальный зум
        Zoom = 75.0f;
}

// Вычисляет векторы front, right и up камеры из (обновленных) углов Эйлера камеры
void Camera::updateCameraVectors() {
    // Вычисляем новый вектор Front
    MyMath::vec3 front;
    front.x = cos(MyMath::radians(Yaw)) * cos(MyMath::radians(Pitch));
    front.y = sin(MyMath::radians(Pitch));
    front.z = sin(MyMath::radians(Yaw)) * cos(MyMath::radians(Pitch));
    Front = MyMath::normalize(front);
    // Также пересчитываем вектор Right и Up
    Right = MyMath::normalize(MyMath::cross(Front, WorldUp));  // Нормализуем векторы, потому что их длина становится ближе к 0, чем больше вы смотрите вверх или вниз, что приводит к более медленному движению.
    Up = MyMath::normalize(MyMath::cross(Right, Front));
} 
