#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "particle.h"
#include "shader.h"
#include "camera.h"

/**
 * Creates a configurable particle system
*/
class ParticleSystem
{
public:
    /**
     * Builds a particle system
     * @param maxAmountOfParticles Maximun number of particles supported by the particle system
     * @param camera Camera's pointer
    */
    ParticleSystem(unsigned int maxAmountOfParticles, Camera *camera);
    /**
     * Destroys the particle system
    */
    ~ParticleSystem();
    /**
     * Sets the parameters of the particle spawn
     * @param numberOfParticles Number of particles spawned on each spawn interval
     * @param spawnInterval Time between particles spawn
    */
    void setParticleSpawns(unsigned int numberOfParticles, float spawnInterval);
    /**
     * Sets the time to live of the spawned particles
     * @param ttl Life time of the new particles in seconds
    */
    void setTTL(float ttl);
    /**
     * Sets the position and position variance of the particles emitted by the particle system
     * @param position Base position of the particles emitted
     * @param variance Variance of the particles initial position
    */
    void setPosition(glm::vec3 position, glm::vec3 variance = glm::vec3(0.0f));
    /**
     * Sets the initial direction and variance of the emitted particles
     * @param direction Base direction of the particles emitted
     * @param variance Variance of the initial particles' direction
    */
    void setDirection(glm::vec3 direction, glm::vec3 variance = glm::vec3(0.0f));
    /**
     * Sets the initial direction and variance of the emitted particles
     * @param initialScale Base scale of the particles emitted
     * @param finalScale Final scale of the particles
     * @param scalevariation Variance of the particles' scale
    */
    void setScale(float initialScale, float finalScale, float variance = 0);
    /**
     * Sets the initial color variance and the final color variance of the emitted particles
     * @param minBaseColor Minimun color range of the initial particles' color
     * @param maxBaseColor Maximun color range of the initial particles' color
     * @param minFinalColor Minimun color range of the final particles' color
     * @param maxFinalColor Maximun color range of the final particles' color
    */
    void setColor(glm::vec3 minBaseColor, glm::vec3 maxBaseColor, glm::vec3 minFinalColor, glm::vec3 maxFinalColor);
    /**
     * Sets the initial and final alpha of the particles
     * @param initialAlpha Initial particles' alpha
     * @param finalAlpha Final particles' alpha
     * @param variance Alpha variance
    */
    void setAplha(float initialAlpha, float finalAlpha, float variance = 0);
    /**
     * Sets a global force to all particles on update (i.e gravity)
     * @param globalExternalForce External force vector
    */
    void setGlobalExternalForce(glm::vec3 globalExternalForce);
    /**
     * Updates the particle system
     * @param deltaTime Time since the last update
    */
    void update(float deltaTime);
    /**
     * Draws the particles of the particle system
    */
    void draw(Shader *shader, unsigned int quadVAO);

private:
    /**
     * Spawns a new group of particles, the number of particles
     * spawned is configured through the particlesPerSpawn property
    */
    void spawnParticles();
    /**
     * Spawns a new particle
     * Sets all the base properties of a give particle
     * @param index Particle's index to be spawned
    */
    void spawnParticle(unsigned int index);

    Camera *camera; // Camera's pointers used to draw the particles

    float ttl; // Base time to live of the spawned particles

    glm::vec3 position;         // Position of the particle system
    glm::vec3 positionVariance; // Position variance of the particles's intial position

    float initialScale;  // Particles initial scale
    float finalScale;    // Particles final scale
    float scaleVariance; // Particles scale variance

    glm::vec3 direction;         // Base direction of the particles spawned
    glm::vec3 directionVariance; // Direction variance of the particles emitted

    glm::vec3 minBaseColor;  // Min range of the base color for the spawned particles
    glm::vec3 maxBaseColor;  // Max range of the base color for the spawned particles
    glm::vec3 minFinalColor; // Min range of the particles final color
    glm::vec3 maxFinalColor; // Max range of the particles final color

    float initialAlpha;  // Particles initial alpha
    float finalAlpha;    // Particles final alpha
    float alphaVariance; // Particles alpha variance

    unsigned int maxAmountofParticles; // Maximun amount of particles supported by the particle system
    unsigned int particlesPerSpawn;    // Number of particles spawned per spwan interval
    float spawnInterval;               // Time between particles spawn

    float timeSinceLastSpawn;         // Time since the last particle spawn
    unsigned int lastParticleSpawned; // Index of the last particle spawned

    glm::vec3 globalExternalForce; // Sets a global director force to all particles (i.e gravity)

    std::vector<Particle> particles; // All the particles in the system dead or alive
};