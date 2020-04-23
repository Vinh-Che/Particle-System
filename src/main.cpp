#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>

// Interface
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_stdlib.h"

#include <iostream>
#include <fstream>
#include <string>

#include <glm/glm.hpp>
#include <stb_image.h>

#include "shader.h"
#include "camera.h"
#include "particle-system.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// Window current width
unsigned int windowWidth = 800;
// Window current height
unsigned int windowHeight = 600;
// Window title
const char *windowTitle = "Particle System";
// Window pointer
GLFWwindow *window;

// Shader object
Shader *shader;
// Index (GPU) of the geometry buffer
unsigned int VBO;
// Index (GPU) vertex array object
unsigned int VAO;
// Index (GPU) of the texture
unsigned int textureID;

// Camera objects
Camera *camera;
// Particle system object
ParticleSystem *particleSystem;

// Toogles the camera's controls
bool cameraEnabled = false;
// Time since the last update
float lastUpdate;

struct MenuProperties
{
    int maxParticles;                  // Max number of particles supported by the particle system
    float ttl;                         // Particle's time to live
    float spawnInterval;               // Particle's spawn interval
    int particlesPerSpawn;             // Number of particles spawned per spawn
    glm::vec3 position;                // Base position of the spawned particles
    glm::vec3 positionVariance;        // Variance of the spawn position
    glm::vec3 direction;               // Base direction of the particles spawned
    float directionScale;              // Direction scale or speed of the spawned particles
    glm::vec3 directionVariance;       // Variance of the spawn direction
    float initialScale;                // Particle's scale at the its life begin
    float finalScale;                  // Particle's scale at the its life end
    float scaleVariance;               // Particle's scale variance
    glm::vec3 minInitialColor;         // Particle's minimun color at the its life begin
    glm::vec3 maxInitialColor;         // Particle's maximun color at the its life begin
    glm::vec3 minFinalColor;           // Particle's minimun color at the its life end
    glm::vec3 maxFinalColor;           // Particle's maximun color at the its life end
    float initialAplha;                // Particle's alpha at the its life begin
    float finalAlpha;                  // Particle's alpha at the its life end
    float alphaVariance;               // Particle's alpha variance
    glm::vec3 externalForce;           // External force direction that globaly influence the particles' direction (ie gravity)
    float externalForceVelocity;       // External force velocity
    std::string fileTextureName;       // Current texture path to be loaded
    std::string lastTextureLoaded;     // Last path of the loaded texture
    std::string configurationFilePath; // Path to the configuration file to be lodaded or saved
} menuOptions;

// Mouse CallBack
void processMousePos(GLFWwindow *, double, double);
// Keyboard Callback
void processKey(GLFWwindow *, int, int, int, int);
// Loads a texture into GPU
int loadTexture(const char *path);

/**
 * Changes the current loaded texture
*/
void changeTexture()
{
    // Loads the new texture
    const int newTexture = loadTexture(menuOptions.fileTextureName.c_str());

    // If the texture isn't loaded correctly, resets the texture path in the interface to the current texture loaded
    if (newTexture == -1)
        menuOptions.fileTextureName = menuOptions.lastTextureLoaded;
    else
    {
        // Deletes the current loaded texture
        glDeleteTextures(1, &textureID);
        // Replace the current texture
        textureID = newTexture;
        menuOptions.lastTextureLoaded = menuOptions.fileTextureName;
    }
}

/**
 * Initialice the Gui system
*/
void initGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

/**
 * Handles the window resize
 * @param window window pointer
 * @param width new width of the window
 * @param height new height of the window
 * */
void resize(GLFWwindow *window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    // Sets the OpenGL viewport size and position
    glViewport(0, 0, windowWidth, windowHeight);
}

/**
 * Initialize the glfw library
 * @returns true if everything goes ok
 * */
bool initWindow()
{
    // Initialize glfw
    glfwInit();
    // Sets the Opegl context to Opengl 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creates the window
    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

    // The window couldn't be created
    if (!window)
    {
        std::cout << "Failed to create the glfw window" << std::endl;
        glfwTerminate(); // Stops the glfw program
        return false;
    }

    // Creates the glfwContext, this has to be made before calling initGlad()
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Window resize callback
    glfwSetFramebufferSizeCallback(window, resize);
    // Cursor position and keyboard callback
    glfwSetCursorPosCallback(window, processMousePos);
    glfwSetKeyCallback(window, processKey);
    return true;
}
/**
 * Initialize the glad library
 * @returns true if everything goes ok
 * */
bool initGlad()
{
    // Initialize glad
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // If something went wrong during the glad initialization
    if (!status)
    {
        std::cout << status << std::endl;
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}
/**
 * Initialize the opengl context
 * */
void initGL()
{
    // Enables the z-buffer test
    glEnable(GL_DEPTH_TEST);
    // Sets the ViewPort
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

/**
 * Builds all the geometry buffers and
 * loads them up into the GPU
 * (Builds a simple quad)
*/
void buildGeometry()
{
    float quadVertices[] = {
        // Bottom left vertex
        -0.5f, -0.5f, 0.0f, // Position
        1.0f, 0.0f, 0.0f,   // Color
        // Bottom right vertex
        0.5f, -0.5f, 0.0f, // Position
        0.0f, 1.0f, 0.0f,  // Color
        // Top Right vertex
        0.5f, 0.5f, 0.0f, // Position
        0.0f, 0.0f, 1.0f, // Color

        // Top left vertex
        -0.5f, 0.5f, 0.0f, // Position
        1.0f, 0.0f, 1.0f   // Color

    };
    // Creates on GPU the vertex array
    glGenVertexArrays(1, &VAO);
    // Creates on GPU the vertex buffer object
    glGenBuffers(1, &VBO);
    // Binds the vertex array to set all the its properties
    glBindVertexArray(VAO);
    // Sets the buffer geometry data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    // Sets the vertex attributes
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);
}
/**
 * Loads a texture into the GPU
 * @param path path of the texture file
 * @returns GPU texture index
*/
int loadTexture(const char *path)
{
    unsigned int id;
    // Creates the texture on GPU
    glGenTextures(1, &id);
    // Loads the texture
    int textureWidth, textureHeight, numberOfChannels;
    // Flips the texture when loads it because in opengl the texture coordinates are flipped
    stbi_set_flip_vertically_on_load(true);
    // Loads the texture file data
    unsigned char *data = stbi_load(path, &textureWidth, &textureHeight, &numberOfChannels, 0);
    if (data)
    {
        // Gets the texture channel format
        GLenum format;
        switch (numberOfChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }

        // Binds the texture
        glBindTexture(GL_TEXTURE_2D, id);
        // Creates the texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
        // Creates the texture mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set the filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "ERROR:: Unable to load texture " << path << std::endl;
        glDeleteTextures(1, &id);
        id = -1;
    }
    // We dont need the data texture anymore because is loaded on the GPU
    stbi_image_free(data);

    return id;
}

/**
 * Sets the particle system properties based on the 
 * gui values
*/
void setParticlesParameters()
{
    particleSystem->setTTL(menuOptions.ttl);
    particleSystem->setParticleSpawns(menuOptions.particlesPerSpawn, menuOptions.spawnInterval);
    particleSystem->setPosition(menuOptions.position, menuOptions.positionVariance);
    particleSystem->setDirection(menuOptions.direction * menuOptions.directionScale, menuOptions.directionVariance * menuOptions.directionScale);
    particleSystem->setScale(menuOptions.initialScale, menuOptions.finalScale, menuOptions.scaleVariance);
    particleSystem->setColor(menuOptions.minInitialColor, menuOptions.maxInitialColor, menuOptions.minFinalColor, menuOptions.maxFinalColor);
    particleSystem->setAplha(menuOptions.initialAplha, menuOptions.finalAlpha, menuOptions.alphaVariance);
    particleSystem->setGlobalExternalForce(menuOptions.externalForce * menuOptions.externalForceVelocity);
}

/**
 * Initialize everything
 * @returns true if everything goes ok
*/
bool init()
{
    // Initialize the window, and the glad components
    if (!initWindow() || !initGlad())
        return false;

    // Initialize the opengl context
    initGL();

    // Init interface
    initGui();

    // Loads the shader
    shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    // Loads all the geometry into the GPU
    buildGeometry();
    // Loads the texture into the GPU
    textureID = loadTexture("assets/textures/spark.png");

    // Creates the camera
    camera = new Camera(glm::vec3(0, 0, 5), 45.0f, 0.01f, 100.0f, 5, 0.1f);

    // Sets an initial group of particles properties
    menuOptions.maxParticles = 1000;
    menuOptions.ttl = 200.f;
    menuOptions.particlesPerSpawn = 15;
    menuOptions.spawnInterval = 0.01f;

    menuOptions.position = glm::vec3(0);
    menuOptions.positionVariance = glm::vec3(0.02f, 0.0f, 0.0f);

    menuOptions.direction = glm::vec3(0, 1, 0);
    menuOptions.directionScale = 1.0f;
    menuOptions.directionVariance = glm::vec3(0.5f, 0.5f, 0.3f);

    menuOptions.initialScale = 0.2f;
    menuOptions.finalScale = 0.2f;
    menuOptions.scaleVariance = 0.0f;

    menuOptions.minInitialColor = glm::vec3(1);
    menuOptions.maxInitialColor = glm::vec3(1);
    menuOptions.minFinalColor = glm::vec3(1);
    menuOptions.maxFinalColor = glm::vec3(1);

    menuOptions.initialAplha = 1.0f;
    menuOptions.finalAlpha = 0.0f;
    menuOptions.alphaVariance = 0.0f;

    menuOptions.fileTextureName = "assets/textures/spark.png";
    menuOptions.lastTextureLoaded = "assets/textures/spark.png";

    menuOptions.externalForce = glm::vec3(0.0f);
    menuOptions.externalForceVelocity = 1.0f;

    menuOptions.configurationFilePath = "assets/configurations/config.ini";

    // Builds the particle system
    particleSystem = new ParticleSystem(menuOptions.maxParticles, camera);
    // Sets the particle system properties
    setParticlesParameters();

    return true;
}
/**
 * Process the keyboard input
 * There are ways of implementing this function through callbacks provide by
 * the GLFW API, check the GLFW documentation to find more
 * @param window window pointer
 * @param deltaTime Time since last update
*/
void processKeyboardInput(GLFWwindow *window, float deltaTime)
{
    // Reads the camera inputs
    if (!cameraEnabled)
        return;

    // Moves the camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->moveCamera(FRONT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->moveCamera(BACK, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->moveCamera(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->moveCamera(RIGHT, deltaTime);
}

/**
 * Process the mouse position
 * @param window Window pointer
 * @param xpos Mouse x position
 * @param ypos Mouse y position
*/
void processMousePos(GLFWwindow *window, double xpos, double ypos)
{
    if (!cameraEnabled)
        return;

    // Computes the mouse distances from the center of the screen
    const glm::vec2 mouseDelta(xpos - windowWidth / 2.0f, windowHeight / 2.0f - ypos);
    // Move the camera view
    camera->lookAround(mouseDelta);

    // Resets the pointer to the center of the screen
    glfwSetCursorPos(window, windowWidth / 2.0f, windowHeight / 2.0f);
}

/**
 * Process the keyboard keys
*/
void processKey(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    // Checks if the escape key is pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        // Tells glfw to close the window as soon as possible
        glfwSetWindowShouldClose(window, true);

    // Checks if the r key is pressed
    if (key == GLFW_KEY_R && action == GLFW_RELEASE)
    {
        // Reloads the shader
        delete shader;
        shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    }

    // Toogles the camera interaction
    if (key == GLFW_KEY_TAB && action == GLFW_RELEASE)
    {
        cameraEnabled = !cameraEnabled;

        if (cameraEnabled)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disables the mouse pointer
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Enables the mouse pointer

        // Sets the mouse position to the center
        glfwSetCursorPos(window, windowWidth / 2.0f, windowHeight / 2.0f);
    }
}

/**
 * Reloads the particle system based on the current menu configuration
*/
void reloadParticleSystem()
{
    delete particleSystem;
    particleSystem = new ParticleSystem(menuOptions.maxParticles, camera);
    setParticlesParameters();
}

/**
 * Transform a string into a int
 * @param value Value to transform
 * @param out Where the transformed value will be stored
 * @return Transformation succesful
*/
bool readProperty(std::string value, int &out)
{
    try
    {
        out = atoi(value.c_str());
    }
    catch (std::exception const &e)
    {
        std::cout << "error : " << e.what() << std::endl;
        return false;
    }
    return true;
}

/**
 * Transform a string into a float
 * @param value Value to transform
 * @param out Where the transformed value will be stored
 * @return Transformation succesful
*/
bool readProperty(std::string value, float &out)
{
    try
    {
        out = atof(value.c_str());
    }
    catch (std::exception const &e)
    {
        std::cout << "error : " << e.what() << std::endl;
        return false;
    }
    return true;
}

/**
 * Transform a string into a vec3
 * @param value Value to transform
 * @param out Where the transformed value will be stored
 * @return Transformation succesful
*/
bool readProperty(std::string value, glm::vec3 &out)
{
    char delimiter = ' ';
    size_t previous = 0, splitIndex;

    // Split the vector by spaces (x y z)
    for (int i = 0; i < 2; i++)
    {
        splitIndex = value.find(delimiter, previous);
        if (splitIndex == std::string::npos)
            return false;

        if (!readProperty(value.substr(previous, splitIndex - previous), out[i]))
            return false;
        previous = splitIndex + 1;
    }

    if (!readProperty(value.substr(previous, splitIndex - previous), out[2]))
        return false;

    return true;
}

/**
 * Stores a menu property
 * @param key Property to store
 * @param value String Value of the property 
 * @param properties Where the property will be stored
 * @return Stored succesfully
*/
bool storeProperty(std::string key, std::string value, MenuProperties &properties)
{
    /**
     * Looks for the possible property and stored it into
     * the corresponding menu properties field
    */
    if (key.compare("maxParticles") == 0)
    {
        if (!readProperty(value, properties.maxParticles))
            return false;
        return true;
    }
    if (key.compare("ttl") == 0)
    {
        if (!readProperty(value, properties.ttl))
            return false;
        return true;
    }
    if (key.compare("spawnInterval") == 0)
    {
        if (!readProperty(value, properties.spawnInterval))
            return false;
        return true;
    }
    if (key.compare("particlesPerSpawn") == 0)
    {
        if (!readProperty(value, properties.particlesPerSpawn))
            return false;
        return true;
    }
    if (key.compare("position") == 0)
    {
        if (!readProperty(value, properties.position))
            return false;
        return true;
    }
    if (key.compare("positionVariance") == 0)
    {
        if (!readProperty(value, properties.positionVariance))
            return false;
        return true;
    }
    if (key.compare("direction") == 0)
    {
        if (!readProperty(value, properties.direction))
            return false;
        return true;
    }
    if (key.compare("directionScale") == 0)
    {
        if (!readProperty(value, properties.directionScale))
            return false;
        return true;
    }
    if (key.compare("directionVariance") == 0)
    {
        if (!readProperty(value, properties.directionVariance))
            return false;
        return true;
    }
    if (key.compare("initialScale") == 0)
    {
        if (!readProperty(value, properties.initialScale))
            return false;
        return true;
    }
    if (key.compare("finalScale") == 0)
    {
        if (!readProperty(value, properties.finalScale))
            return false;
        return true;
    }
    if (key.compare("scaleVariance") == 0)
    {
        if (!readProperty(value, properties.scaleVariance))
            return false;
        return true;
    }
    if (key.compare("minInitialColor") == 0)
    {
        if (!readProperty(value, properties.minInitialColor))
            return false;
        return true;
    }
    if (key.compare("maxInitialColor") == 0)
    {
        if (!readProperty(value, properties.maxInitialColor))
            return false;
        return true;
    }
    if (key.compare("minFinalColor") == 0)
    {
        if (!readProperty(value, properties.minFinalColor))
            return false;
        return true;
    }
    if (key.compare("maxFinalColor") == 0)
    {
        if (!readProperty(value, properties.maxFinalColor))
            return false;
        return true;
    }
    if (key.compare("initialAplha") == 0)
    {
        if (!readProperty(value, properties.initialAplha))
            return false;
        return true;
    }
    if (key.compare("finalAlpha") == 0)
    {
        if (!readProperty(value, properties.finalAlpha))
            return false;
        return true;
    }
    if (key.compare("alphaVariance") == 0)
    {
        if (!readProperty(value, properties.alphaVariance))
            return false;
        return true;
    }
    if (key.compare("externalForce") == 0)
    {
        if (!readProperty(value, properties.externalForce))
            return false;
        return true;
    }
    if (key.compare("externalForceVelocity") == 0)
    {
        if (!readProperty(value, properties.externalForceVelocity))
            return false;
        return true;
    }
    if (key.compare("fileTextureName") == 0)
    {
        properties.fileTextureName = value;
        return true;
    }
    return false;
}

/**
 * Loads a particle system configuration from a file
*/
void loadConfiguration()
{
    std::ifstream file;

    // Open the file
    file.open(menuOptions.configurationFilePath);

    // Error
    if (!file)
    {
        std::cout << "Unable to open the configuration file " << menuOptions.configurationFilePath << std::endl;
        return;
    }

    std::string line;

    // Make a copie of the menu propeties
    MenuProperties newProperties = menuOptions;

    // Reads each line and stores the property
    while (std::getline(file, line))
    {
        std::cout << line << std::endl;
        // Splits the string
        std::size_t splitIndex = line.find(' ');

        // We got an error
        if (splitIndex == std::string::npos)
        {
            std::cout << "File " << menuOptions.configurationFilePath << " corrupted" << std::endl;
            return;
        }

        std::string key = line.substr(0, splitIndex);
        std::string value = line.substr(splitIndex + 1, line.size());

        std::cout << key << " " << value << std::endl;

        // We got an error
        if (!storeProperty(key, value, newProperties))
        {
            std::cout << "File " << menuOptions.configurationFilePath << " corrupted" << std::endl;
            return;
        }
    }

    // File read complete, copies the new properties
    menuOptions = newProperties;
    // Reloads the particle system
    reloadParticleSystem();
    changeTexture();

    // Close the file
    file.close();
}

/**
 * Saves the current particle system configuration to a file
*/
void saveConfiguration()
{
    std::ofstream file;
    file.open(menuOptions.configurationFilePath);

    if (!file)
        std::cout << "Couldn't open the file " << menuOptions.configurationFilePath << " for save" << std::endl;

    file << "maxParticles"
         << " " << menuOptions.maxParticles << std::endl;

    file << "ttl"
         << " " << menuOptions.ttl << std::endl;

    file << "spawnInterval"
         << " " << menuOptions.spawnInterval << std::endl;

    file << "particlesPerSpawn"
         << " " << menuOptions.particlesPerSpawn << std::endl;

    file << "position"
         << " " << menuOptions.position.x
         << " " << menuOptions.position.y
         << " " << menuOptions.position.z << std::endl;

    file << "positionVariance"
         << " " << menuOptions.positionVariance.x
         << " " << menuOptions.positionVariance.y
         << " " << menuOptions.positionVariance.z << std::endl;

    file << "direction"
         << " " << menuOptions.direction.x
         << " " << menuOptions.direction.y
         << " " << menuOptions.direction.z << std::endl;

    file << "directionScale"
         << " " << menuOptions.directionScale << std::endl;

    file << "directionVariance"
         << " " << menuOptions.directionVariance.x
         << " " << menuOptions.directionVariance.y
         << " " << menuOptions.directionVariance.z << std::endl;

    file << "initialScale"
         << " " << menuOptions.initialScale << std::endl;

    file << "finalScale"
         << " " << menuOptions.finalScale << std::endl;

    file << "scaleVariance"
         << " " << menuOptions.scaleVariance << std::endl;

    file << "minInitialColor"
         << " " << menuOptions.minInitialColor.x
         << " " << menuOptions.minInitialColor.y
         << " " << menuOptions.minInitialColor.z << std::endl;

    file << "maxInitialColor"
         << " " << menuOptions.maxInitialColor.x
         << " " << menuOptions.maxInitialColor.y
         << " " << menuOptions.maxInitialColor.z << std::endl;

    file << "minFinalColor"
         << " " << menuOptions.minFinalColor.x
         << " " << menuOptions.minFinalColor.y
         << " " << menuOptions.minFinalColor.z << std::endl;

    file << "maxFinalColor"
         << " " << menuOptions.maxFinalColor.x
         << " " << menuOptions.maxFinalColor.y
         << " " << menuOptions.maxFinalColor.z << std::endl;

    file << "initialAplha"
         << " " << menuOptions.initialAplha << std::endl;

    file << "finalAlpha"
         << " " << menuOptions.finalAlpha << std::endl;

    file << "alphaVariance"
         << " " << menuOptions.alphaVariance << std::endl;

    file << "externalForce"
         << " " << menuOptions.externalForce.x
         << " " << menuOptions.externalForce.y
         << " " << menuOptions.externalForce.z << std::endl;

    file << "externalForceVelocity"
         << " " << menuOptions.externalForceVelocity << std::endl;

    file << "fileTextureName"
         << " " << menuOptions.fileTextureName << std::endl;
}
/**
 * Creates/ Upates all the interface controls
*/
void updateInterface()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Properties"); // Creates the window interface

    if (ImGui::Button("Reset Camera"))
    {
        camera->resetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    }
    // Sets each interface control
    ImGui::TextWrapped("Changing the maximun number of particles will reset the particle system");
    if (ImGui::InputInt("Max Particles", &menuOptions.maxParticles))
    {
        menuOptions.maxParticles = glm::max(menuOptions.maxParticles, 0);
        menuOptions.particlesPerSpawn = glm::min(menuOptions.maxParticles, menuOptions.particlesPerSpawn);
        reloadParticleSystem();
    }
    if (ImGui::CollapsingHeader("Load/Save Coniguration"))
    {
        ImGui::InputTextWithHint("Path_Config", "path", &menuOptions.configurationFilePath);

        if (ImGui::Button("Load_Config"))
            loadConfiguration();
        if (ImGui::Button("Save_Config"))
            saveConfiguration();
    }
    if (ImGui::CollapsingHeader("Texture"))
    {
        ImGui::InputText("Path_Texture", &menuOptions.fileTextureName);
        if (ImGui::Button("Load_Texture"))
            changeTexture();
    }
    if (ImGui::CollapsingHeader("Spawn"))
    {
        ImGui::SliderInt("Particles per Spawn", &menuOptions.particlesPerSpawn, 1, menuOptions.maxParticles);

        if (ImGui::InputFloat("Time to live", &menuOptions.ttl, 0.001f, 0.01, 4))
            menuOptions.ttl = glm::max(menuOptions.ttl, 0.001f);

        if (ImGui::InputFloat("Spawn time interval", &menuOptions.spawnInterval, 0.001f, 0.01, 4))
            menuOptions.spawnInterval = glm::max(menuOptions.spawnInterval, 0.001f);
    }
    if (ImGui::CollapsingHeader("Position"))
    {
        ImGui::DragFloat3("P_Initial", &menuOptions.position[0], 0.001);

        ImGui::DragFloat3("P_Variance", &menuOptions.positionVariance[0], 0.001);
    }
    if (ImGui::CollapsingHeader("Direction"))
    {
        if (ImGui::DragFloat3("D_Initial", &menuOptions.direction[0], 0.001, -1.0f, 1.0f))
            menuOptions.direction = glm::clamp(menuOptions.direction, glm::vec3(-1), glm::vec3(1));

        if (ImGui::DragFloat3("D_Variance", &menuOptions.directionVariance[0], 0.001, -1.0f, 1.0f))
            menuOptions.directionVariance = glm::clamp(menuOptions.directionVariance, glm::vec3(-1), glm::vec3(1));

        ImGui::InputFloat("D_Speed", &menuOptions.directionScale, 0.001, 0.01, 4);
    }
    if (ImGui::CollapsingHeader("External Global Force"))
    {
        if (ImGui::DragFloat3("F_Direction", &menuOptions.externalForce[0], 0.001, -1.0f, 1.0f))
            menuOptions.externalForce = glm::clamp(menuOptions.externalForce, glm::vec3(-1), glm::vec3(1));

        ImGui::InputFloat("F_Speed", &menuOptions.externalForceVelocity, 0.001, 0.01, 4);
    }
    if (ImGui::CollapsingHeader("Scale"))
    {
        if (ImGui::InputFloat("S_Initial", &menuOptions.initialScale, 0.001, 0.01, 4))
            menuOptions.initialScale = glm::max(menuOptions.initialScale, 0.0f);

        if (ImGui::InputFloat("S_Final", &menuOptions.finalScale, 0.001, 0.01, 4))
            menuOptions.finalScale = glm::max(menuOptions.finalScale, 0.0f);

        if (ImGui::InputFloat("S_Variance", &menuOptions.scaleVariance, 0.001, 0.01, 4))
            menuOptions.scaleVariance = glm::max(menuOptions.scaleVariance, 0.0f);
    }
    if (ImGui::CollapsingHeader("Color"))
    {
        ImGui::Text("Initial");
        ImGui::ColorEdit3("I_Minimun", &menuOptions.minInitialColor[0]);
        ImGui::ColorEdit3("I_Maximum", &menuOptions.maxInitialColor[0]);

        ImGui::Text("Final");
        ImGui::ColorEdit3("F_Minimun", &menuOptions.minFinalColor[0]);
        ImGui::ColorEdit3("F_Maximum", &menuOptions.maxFinalColor[0]);
    }
    if (ImGui::CollapsingHeader("Alpha"))
    {
        if (ImGui::InputFloat("A_Initial", &menuOptions.initialAplha, 0.01, 0.001, 4))
            menuOptions.initialAplha = glm::max(menuOptions.initialAplha, 0.0f);

        if (ImGui::InputFloat("A_Final", &menuOptions.finalAlpha, 0.01, 0.001, 4))
            menuOptions.finalAlpha = glm::max(menuOptions.finalAlpha, 0.0f);

        if (ImGui::InputFloat("A_Variance", &menuOptions.alphaVariance, 0.01, 0.001, 4))
            menuOptions.alphaVariance = glm::max(menuOptions.alphaVariance, 0.0f);
    }
    ImGui::End();
}

/**
 * Render Function
*/
void render()
{
    ImGui::Render();
    // Clears the color and depth buffers from the frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use the shader
    shader->use();
    // Sets the projection and view matrices
    shader->setMat4("projection", camera->getProjectionMatrix(windowWidth, windowHeight));
    shader->setMat4("view", camera->getViewMatrix());

    // Sets the current texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->setInt("text1", 0);

    // Renders the particle system
    particleSystem->draw(shader, VAO);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    // Draw interface
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap the buffer
    glfwSwapBuffers(window);
}

/**
 * App main loop
*/
void update()
{
    lastUpdate = glfwGetTime();
    // Loop until something tells the window, that it has to be closed
    while (!glfwWindowShouldClose(window))
    {
        // Computes the delta time
        const float currentTime = glfwGetTime();
        const float deltaTime = currentTime - lastUpdate;
        lastUpdate = currentTime;

        // Checks for keyboard inputs
        processKeyboardInput(window, deltaTime);

        // Sets the particle system properties
        setParticlesParameters();

        // Updates the particle system
        particleSystem->update(deltaTime);

        // Upadtes the interface
        updateInterface();

        // Renders everything
        render();

        // Check and call events
        glfwPollEvents();
    }
}
/**
 * App starting point
 * @param argc Number of arguments
 * @param argv Running arguments
 * @returns App exit code
 */
int main(int argc, char const *argv[])
{
    // Initialize all the app components
    if (!init())
    {
        // Something went wrong
        std::cin.ignore();
        return -1;
    }

    std::cout << "=====================================================" << std::endl
              << "        Press Escape to close the program            " << std::endl
              << "=====================================================" << std::endl;

    // Starts the app main loop
    update();

    // Deletes the texture from the gpu
    glDeleteTextures(1, &textureID);
    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &VAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &VBO);
    // Destroy the shader
    delete shader;
    // Deletes the camera
    delete camera;
    // Deletes the particle system
    delete particleSystem;

    // Clear the interface
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Stops the glfw program
    glfwTerminate();

    return 0;
}
