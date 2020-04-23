#include "particle.h"
#include <iostream>

Particle::Particle()
{
    // Sets the particle as a dead one
    this->ttl = 0;
    this->liveTime = 0;
    this->alive = false;
}

void Particle::update(float deltaTime, glm::vec3 externalForce)
{
	// Reduce its live time
	this->ttl -= deltaTime;
	// Checks if the particle still alive
	this->alive = this->ttl > 0.0f;

    if (!this->alive)
        return;

    // Updates its position
    this->position += (this->direction) * deltaTime;
    // Updates its direction by the influence of a external force
    this->direction += externalForce * deltaTime;
}

void Particle::draw(Shader *shader, Camera *camera)
{
    if (!this->alive)
        return;

    // Computes its remaining live fraction
	const float t = glm::clamp(1.0f - this->ttl / this->liveTime, 0.0f, 1.0f);

    // Computes the particle current scale given its live fraction
    const float currentScale = glm::mix(this->initialScale, this->finalScale, t);
    // Computes the particle current alpha given its live fraction
    const float alpha = glm::mix(this->initialAlpha, this->finalAlpha, t);
	
   // Computes the particle current color given its live fraction
    const glm::vec3 currentColor = glm::mix(this->initialColor, this->finalColor, t);

    // Computes the orientation of the particle and sets its model matrix in the shader
    shader->setMat4("model", this->computeBillBoardMatrix(camera));
    // Sets the color and scale in the shader
    shader->setFloat("scale", currentScale);
    shader->setVec4("color", glm::vec4(currentColor.r, currentColor.g, currentColor.b, alpha));
}

void Particle::reset(float liveTime, glm::vec3 position, glm::vec3 direction, float initialScale, float finalScale, glm::vec3 initialColor, glm::vec3 finalColor, float initialAlpha, float finalAlpha)
{
    // Reset all the particle's properties
    this->liveTime = liveTime;
    this->ttl = liveTime;
    this->position = position;
    this->direction = direction;
    this->initialScale = initialScale;
    this->finalScale = finalScale;
    this->initialColor = initialColor;
    this->finalColor = finalColor;
    this->initialAlpha = initialAlpha;
    this->finalAlpha = finalAlpha;
    // Sets the particle as alive
    this->alive = true;
}

glm::mat4 Particle::computeBillBoardMatrix(Camera *camera)
{
    /**
     *  See https://nehe.gamedev.net/article/billboarding_how_to/18011/  
     *      4.2. Individual Billboarding
     *      7. Using Those Billboard Vectors
     *      8. Rendering a Billboard
    */

    // Computes the vector that goes towards the camera from the particle position
    const glm::vec3 billBoardFrontVector = glm::normalize(camera->getPosition() - this->position);
    // Computes the particle's right vector, using as input the camera up vector
    const glm::vec3 billBoardRightVector = glm::normalize(glm::cross(camera->getUpVector(), billBoardFrontVector));
    // Recomputes the up vector of the billboard (this will ensure that the right, front and up vector are perpendicular to each other)
    const glm::vec3 billBoardUpVector = glm::normalize(glm::cross(billBoardFrontVector, billBoardRightVector));

    // Builds the particle lookat matrix
    glm::mat4 billboardModelMatrix(0);

    billboardModelMatrix[0].x = billBoardRightVector.x;
    billboardModelMatrix[0].y = billBoardRightVector.y;
    billboardModelMatrix[0].z = billBoardRightVector.z;
    billboardModelMatrix[0].w = 0;

    billboardModelMatrix[1].x = billBoardUpVector.x;
    billboardModelMatrix[1].y = billBoardUpVector.y;
    billboardModelMatrix[1].z = billBoardUpVector.z;
    billboardModelMatrix[1].w = 0;

    billboardModelMatrix[2].x = billBoardFrontVector.x;
    billboardModelMatrix[2].y = billBoardFrontVector.y;
    billboardModelMatrix[2].z = billBoardFrontVector.z;
    billboardModelMatrix[2].w = 0;

    billboardModelMatrix[3].x = this->position.x;
    billboardModelMatrix[3].y = this->position.y;
    billboardModelMatrix[3].z = this->position.z;
    billboardModelMatrix[3].w = 1;

    return billboardModelMatrix;
}

glm::vec3 Particle::getPosition()
{
    return this->position;
}