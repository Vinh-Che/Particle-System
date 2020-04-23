#pragma once

#include <glm/glm.hpp>
#include "shader.h"
#include "camera.h"

class Particle
{
public:
    /**
     * Creates a new particle
    */
    Particle();
    /**
     * Updates the particle properties
     * @param deltaTime Time since last update
     * @param externalForce Force to be applied to the particle direction over time (i.e gravity)
    */
    void update(float deltaTime, glm::vec3 externalForce);
    /**
     * Sets all the uniforms of the particles
     * @param shader Shader used to render the particle
     * @param camera Camera used to render the particle
    */
    void draw(Shader *shader, Camera *camera);
    /**
     * Resets all the particles properties, sets it alive
     * @param liveTime Particle's live time in seconds
     * @param position Particle's initial position
     * @param direction Particle's direction
     * @param initialScale Particle's scale at its life begin
     * @param finalScale Particle's scale at its life end
     * @param initialColor Particle's color at its life begin
     * @param finalColor Particle's color at its life end
     * @param initialAlpha Particle's alpha at its life begin
     * @param finalAlpha Particle's alpha at its life end
    */
    void reset(float liveTime, glm::vec3 position, glm::vec3 direction, float initialScale, float finalScale,
               glm::vec3 initialColor, glm::vec3 finalColor, float initialAlpha, float finalAlpha);
    /**
     * Gets the particle's position
     * @return Particle's position
    */
    glm::vec3 getPosition();

private:
    /**
     * Computes the model matrix used orient the particle to face the camera
     * @param camera Camera to which the camera will face
     * @return Model matrix to orient the particle towards the camera
    */
    glm::mat4 computeBillBoardMatrix(Camera *camera);

    bool alive;             // Particle's status
    glm::vec3 position;     // Particle's position
    glm::vec3 direction;    // Particle's direction
    float initialScale;     // Particle's initial scale
    float finalScale;       // Particle's final scale
    glm::vec3 initialColor; // Particle's initial color
    glm::vec3 finalColor;   // Particle's final color
    float initialAlpha;     // Particle's initial alpha
    float finalAlpha;       // Particle's final alpha
    float ttl;              // Particle's time to live (seconds)
    float liveTime;         // Particle's initial full live time (seconds)
};