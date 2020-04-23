#pragma once

#include <glm/glm.hpp>

/**
 * Camera's posible movement directions
*/
enum CameraDirection
{
    FRONT,
    BACK,
    LEFT,
    RIGHT
};

/**
 * Implements a basic FPS camera
*/
class Camera
{
public:
    /**
     * Creates a camera
     * @param position Camera's position
     * @param fov Camera's aperture angle
     * @param nearClippingPlane Camera's near clipping plane
     * @param farClippingPlane Camera's far clipping plane
     * @param movementSpeed Camera's movement speed
     * @param sensitivity Camera's mouse sensitivity
    */
    Camera(glm::vec3 position, float fov, float nearClippingPlane, float farClippingPlane, float movementSpeed, float sensitivity);
    /**
     * Moves the camera in a given direction
     * @param direction Movement direction
     * @param deltaTime Time since the last update
    */
    void moveCamera(CameraDirection direction, float deltaTime);
    /**
     * Change view
     * @param mouseDelta Mouse movement
    */
    void lookAround(glm::vec2 mouseDelta);
    /**
     * Computes the current view matrix
     * @return View matrix
    */
    glm::mat4 getViewMatrix();
    /**
     * Computes the current projection matrix
     * @param windowWidth Current window width
     * @param windowHeight Current window height
     * @return Projection matrix
    */
    glm::mat4 getProjectionMatrix(float windowWidth, float windowHeight);
    /**
     * Gets the camera position
     * @return Constant references (can't be changed) to the camera's position
    */
    const glm::vec3 &getPosition();
    /**
     * Gets the camera front vector
     * @return Constant references (can't be changed) to the camera's front vector
    */
    const glm::vec3 &getFrontVector();
    /**
     * Gets the camera up vector
     * @return Constant references (can't be changed) to the camera's up vector
    */
    const glm::vec3 &getUpVector();
    /**
     * Gets the camera right vector
     * @return Constant references (can't be changed) to the camera's right vector
    */
    const glm::vec3 &getRightVector();

    /**
     * Resets the state of the camera to a given position and the base orientation
     * @param position Position to place the camera
    */
    void resetPosition(glm::vec3 position);

private:
    glm::vec3 position;    // Camera's position
    glm::vec3 upVector;    // Camera's up vector
    glm::vec3 frontVector; // Camera's front vector
    glm::vec3 rightVector; // Camera's right vector

    float fov;               // Camera's aperture angle
    float nearClippingPlane; // Camera's near clipping plane
    float farClippingPlane;  // Camera's far clipping plane
    float movementSpeed;     // Camera's movement speed
    float sensitivity;       // Camera's mouse sensitivity

    float yawAngle;   // Camera's yaw
    float pitchAngle; // Camera's pitch
};