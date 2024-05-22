#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

typedef struct AnimeCameraState {
    glm::vec3 position;
    glm::vec3 front;
    float fov;
};

bool isRunningTransition = false;
AnimeCameraState startCamera = { glm::vec3(0.0f, 0.0f, 45.0f), glm::vec3(0.0f, 0.0f, -1.0f), 45.0f };
AnimeCameraState endCamera = { glm::vec3(0.0f, 0.0f, 45.0f), glm::vec3(0.0f, 0.0f, -1.0f), 45.0f };
float startTransitionTime;
float transitionTimeLength = 2.5f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    bool focusOrigin = false;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 45.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 worldUp;
    glm::vec3 right;
    float fov = 45.0f;
    double cameraSpeed = 0.05f;
    double lastX = 0.0;
    double lastY = 0.0;
    double pitch = 0.0;
    double yaw = 0.0;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = 0.0, float pitch = 0.0) : front(glm::vec3(0.0f, 0.0f, -1.0f))
    {
        position = position;
        worldUp = up;
        yaw = yaw;
        pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    {
        pos = glm::vec3(posX, posY, posZ);
        worldUp = glm::vec3(upX, upY, upZ);
        yaw = yaw;
        pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(pos, pos + front, up);
    }


private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));
    }
};

#endif