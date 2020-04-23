#include "particle-system.h"
#include <glad/glad.h>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <algorithm>

/**
 * Computes a random number from a base a variance
 * @param baseValue Median number of the random
 * @param variance Variance of the random centered of the baseValue
 * @return Random number in the range [baseValue - variance, baseValue + variance]
*/
float randomValue(float baseValue, float variance)
{
    float random = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
    return baseValue + variance * random;
}

/**
 * Computes a random vector from a base a variance
 * @param baseValue Median vector of the random
 * @param variance Variance of the random centered of the baseValue
 * @return Random vector in the range [baseValue - variance, baseValue + variance]
*/
glm::vec3 randomValue(glm::vec3 baseValue, glm::vec3 variance)
{
    return glm::vec3(randomValue(baseValue.x, variance.x),
                     randomValue(baseValue.y, variance.y),
                     randomValue(baseValue.z, variance.z));
}

/**
 * Builds a random number between the range [min, max]
 * @param min Minimun posible random value
 * @param max Maximun posible random value
 * @return Random number in the range [min, max]
*/
float randomValueInterpolated(float min, float max)
{
    float random = (float)rand() / (float)RAND_MAX;

    return glm::mix(min, max, random);
}

/**
 * Builds a random vector between the range [min, max]
 * @param min Minimun posible random value
 * @param max Maximun posible random value
 * @return Random vector in the range [min, max]
*/
glm::vec3 randomValueInterpolated(glm::vec3 min, glm::vec3 max)
{
    return glm::vec3(randomValueInterpolated(min.x, max.x),
                     randomValueInterpolated(min.y, max.y),
                     randomValueInterpolated(min.z, max.z));
}

ParticleSystem::ParticleSystem(unsigned int maxAmountOfParticles, Camera *camera)
{
    // Sets the maximun number of particles in supported by the particles system
    this->maxAmountofParticles = maxAmountOfParticles;
    this->timeSinceLastSpawn = 0;
    this->lastParticleSpawned = 0;

    this->camera = camera;

    // Sets the size of the particle system
    this->particles.resize(this->maxAmountofParticles);

    // Sets the random number generator seed
    srand(time(NULL));
}

ParticleSystem::~ParticleSystem()
{
    // Clear all the particles from the particle system
    this->particles.clear();
}

void ParticleSystem::setParticleSpawns(unsigned int numberOfParticles, float spawnInterval)
{
    this->particlesPerSpawn = numberOfParticles;
    this->spawnInterval = spawnInterval;
}

void ParticleSystem::setTTL(float ttl)
{
    this->ttl = ttl;
}

void ParticleSystem::setPosition(glm::vec3 position, glm::vec3 variance)
{
    this->position = position;
    this->positionVariance = variance;
}

void ParticleSystem::setScale(float initialScale, float finalScale, float variance)
{
    this->initialScale = initialScale;
    this->finalScale = finalScale;
    this->scaleVariance = variance;
}

void ParticleSystem::setDirection(glm::vec3 direction, glm::vec3 variance)
{
    this->direction = direction;
    this->directionVariance = variance;
}

void ParticleSystem::setColor(glm::vec3 minBaseColor, glm::vec3 maxBaseColor, glm::vec3 minFinalColor, glm::vec3 maxFinalColor)
{
    this->minBaseColor = minBaseColor;
    this->maxBaseColor = maxBaseColor;
    this->minFinalColor = minFinalColor;
    this->maxFinalColor = maxFinalColor;
}

void ParticleSystem::setAplha(float initialAlpha, float finalAlpha, float variance)
{
    // Clamps the alpha values between the minimun and maximun possible alpha values
    this->initialAlpha = glm::clamp(initialAlpha, 0.0f, 1.0f);
    this->finalAlpha = glm::clamp(finalAlpha, 0.0f, 1.0f);
    this->alphaVariance = glm::clamp(variance, 0.0f, 1.0f);
}

void ParticleSystem::setGlobalExternalForce(glm::vec3 globalExternalForce)
{
    this->globalExternalForce = globalExternalForce;
}

void ParticleSystem::update(float deltaTime)
{
    // Increase the time since the last particles spawn
    this->timeSinceLastSpawn += deltaTime;

    // Spawns a new set of particles
    if (this->timeSinceLastSpawn >= this->spawnInterval)
    {
        this->spawnParticles();
        this->timeSinceLastSpawn = 0.0f;
    }

    // Updates each particles
    for (unsigned int i = 0; i < this->maxAmountofParticles; i++)
        this->particles[i].update(deltaTime, this->globalExternalForce);
}

void ParticleSystem::draw(Shader *shader, unsigned int quadVAO)
{
    // Binds the particles geometry
    glBindVertexArray(quadVAO);

    for (unsigned int i = 0; i < this->maxAmountofParticles; i++)
    {
        // Sets the particles uniform properties
        this->particles[i].draw(shader, this->camera);
        // Binds the vertex array to be drawn
        // Renders the quad geometry
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArray(0);
}

void ParticleSystem::spawnParticles()
{
    /**
     * Spawns each new particle one by one
     * The particles aren't created or deleted from the array,
     * when a new particle has to be spawned a particle from the array is reseted or respwaned
     * no matter if the particle is alive or dead. 
     * All the particles are recycled
    */
    for (unsigned int i = 0; i < this->particlesPerSpawn; i++)
    {
        // Spawns the next particle in the array
        this->spawnParticle(this->lastParticleSpawned);
        // Sets the index of the next particle to be spawned
        this->lastParticleSpawned = (this->lastParticleSpawned + 1) % this->maxAmountofParticles;
    }
}

void ParticleSystem::spawnParticle(unsigned int index)
{
    // Creates a new random position
    const glm::vec3 newPosition(randomValue(this->position, this->positionVariance));
    //Creates a new random direction
    const glm::vec3 newDirection(randomValue(this->direction, this->directionVariance));
    // Creates a new random initial and final scale
    const float newInitialScale = randomValue(this->initialScale, this->scaleVariance);
    const float newFinalScale = randomValue(this->finalScale, this->scaleVariance);
    // Creates a new random initial and final color
    const glm::vec3 newInitialColor(randomValueInterpolated(this->minBaseColor, this->maxBaseColor));
    const glm::vec3 newFinalColor(randomValueInterpolated(this->minFinalColor, this->maxFinalColor));
    // Creates a new random initial and final alpha
    const float newInitialAlpha = glm::clamp(randomValue(this->initialAlpha, this->alphaVariance),
                                             0.0f, 1.0f);
    const float newFinalAlpha = glm::clamp(randomValue(this->finalAlpha, this->alphaVariance),
                                           0.0f, 1.0f);

    // Resets the given particle from the particles array
    this->particles[index].reset(this->ttl, newPosition, newDirection, newInitialScale, newFinalScale,
                                 newInitialColor, newFinalColor, newInitialAlpha, newFinalAlpha);
}
