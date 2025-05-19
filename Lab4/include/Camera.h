#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>

#include <MyMath/mat4.h>
#include <MyMath/vec3.h>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class Camera {
public:
    MyMath::vec3 Position;
    MyMath::vec3 Front;
    MyMath::vec3 Up;
    MyMath::vec3 Right;
    MyMath::vec3 WorldUp;
    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(MyMath::vec3 position = MyMath::vec3(0.0f, 0.0f, 3.0f), MyMath::vec3 up = MyMath::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    MyMath::mat4 GetViewMatrix();
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};
#endif 
