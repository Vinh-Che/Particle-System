#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(glm::vec3 position, float fov, float nearClippingPlane, float farClippingPlane, float movementSpeed, float sensitivity)
{
    this->position = position;
    this->fov = fov;
    this->nearClippingPlane = nearClippingPlane;
    this->farClippingPlane = farClippingPlane;
    this->movementSpeed = movementSpeed;
    this->sensitivity = sensitivity;

    this->upVector = glm::vec3(0, 1, 0);
    this->frontVector = glm::vec3(0, 0, -1);
    // The right vector is perpendicular to the up vector and the front vector
    this->rightVector = glm::normalize(glm::cross(this->frontVector, this->upVector));

    this->yawAngle = -90;
    this->pitchAngle = 0;
}

void Camera::moveCamera(CameraDirection direction, float deltaTime)
{
    // The movement speed will depend on the time, so the camera movement is CPU independent
    const float currentSpeed = deltaTime * this->movementSpeed;

    switch (direction)
    {
    case FRONT:
        this->position += this->frontVector * currentSpeed;
        break;
    case BACK:
        this->position -= this->frontVector * currentSpeed;
        break;
    case LEFT:
        this->position -= this->rightVector * currentSpeed;
        break;
    case RIGHT:
        this->position += this->rightVector * currentSpeed;
        break;
    }
}
void Camera::lookAround(glm::vec2 mouseDelta)
{
    this->yawAngle += mouseDelta.x * this->sensitivity;
    this->pitchAngle += mouseDelta.y * this->sensitivity;

    // Cap the pitch angle between [-89.0f, 89.0f] so the screen doesn't get flipped
    this->pitchAngle = glm::max<float>(-89.0f, glm::min<float>(89.0f, this->pitchAngle));

    const float yawRadians = glm::radians(this->yawAngle);
    const float pitchRadians = glm::radians(this->pitchAngle);

    // Calculate the front vector from the Camera's Euler angles
    this->frontVector.x = cos(yawRadians) * cos(pitchRadians);
    this->frontVector.y = sin(pitchRadians);
    this->frontVector.z = sin(yawRadians) * cos(pitchRadians);
    this->frontVector = glm::normalize(this->frontVector);

    const glm::vec3 upWorld(0, 1, 0); // Up world vector

    // Re-calculate the right and up vector
    this->rightVector = glm::normalize(glm::cross(this->frontVector, upWorld));
    this->upVector = glm::normalize(glm::cross(this->rightVector, this->frontVector));
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(this->position, this->position + this->frontVector, this->upVector);
}

glm::mat4 Camera::getProjectionMatrix(float windowWidth, float windowHeight)
{
    return glm::perspective(this->fov, windowWidth / windowHeight, this->nearClippingPlane, this->farClippingPlane);
}

const glm::vec3 &Camera::getPosition()
{
    return this->position;
}

const glm::vec3 &Camera::getFrontVector()
{
    return this->frontVector;
}

const glm::vec3 &Camera::getUpVector()
{
    return this->upVector;
}

const glm::vec3 &Camera::getRightVector()
{
    return this->rightVector;
}

void Camera::resetPosition(glm::vec3 position)
{
    this->position = position;

    this->upVector = glm::vec3(0, 1, 0);
    this->frontVector = glm::vec3(0, 0, -1);
    // The right vector is perpendicular to the up vector and the front vector
    this->rightVector = glm::normalize(glm::cross(this->frontVector, this->upVector));

    this->yawAngle = -90;
    this->pitchAngle = 0;
}