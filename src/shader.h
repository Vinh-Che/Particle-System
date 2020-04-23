#pragma once
#include <string>
#include <glm/glm.hpp>

// Types of shader supported by the shader class
enum shaderType
{
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	GEOMETRY_SHADER,
	PROGRAM
};

/**
 * Loads a program shader from files and provides functions to use it
*/
class Shader
{
public:
	/**
	* Loads and compiles a shader
	* @param vertexPath Path to the vertex shader
	* @param fragmentPath Path to the fragment shader
	*/
	Shader(const char *vertexPath, const char *fragmentPath);

	/**
	* Loads and compiles a shader
	* @param vertexPath Path to the vertex shader
	* @param fragmentPath Path to the fragment shader
	* @param gemotryPath Path to the geometry shader
	*/
	Shader(const char *vertexPath, const char *fragmentPath, const char *gemotryPath);

	/**
	* Shader destructor
	*/
	~Shader();

	/**
	* Enables the shader to be use
	*/
	void use();

	/**
	* Sets a bool uniform
	* @param name uniform name
	* @param value value to be set
	*/
	void setBool(const std::string &name, bool value) const;

	/**
	* Sets an int uniform
	* @param name uniform name
	* @param value value to be set
	*/
	void setInt(const std::string &name, int value) const;

	/**
	* Sets an int uniform
	* @param name uniform name
	* @param value value to be set
	*/
	void setFloat(const std::string &name, float value) const;

	/**
	* Sets an vec2 uniform
	* @param name uniform name
	* @param value value to be set
	*/
	void setVec2(const std::string &name, const glm::vec2 &value) const;

	/**
	* Sets an vec2 uniform
	* @param name uniform name
	* @param x vec2 x value
	* @param y vec2 y value
	*/
	void setVec2(const std::string &name, float x, float y) const;

	/**
	* Sets an vec3 uniform
	* @param name uniform name
	* @param value vector value
	*/
	void setVec3(const std::string &name, const glm::vec3 &value) const;

	/**
	* Sets an vec3 uniform
	* @param name uniform name
	* @param x vec3 x value
	* @param y vec3 y value
	* @param z vec3 z value
	*/
	void setVec3(const std::string &name, float x, float y, float z) const;

	/**
	* Sets an vec4 uniform
	* @param name uniform name
	* @param value vec4 value
	*/
	void setVec4(const std::string &name, const glm::vec4 &value) const;

	/**
	* Sets an vec4 uniform
	* @param name uniform name
	* @param x vec4 x value
	* @param y vec4 y value
	* @param z vec4 z value
	* @param w vec4 w value
	*/
	void setVec4(const std::string &name, float x, float y, float z, float w);

	/**
	* Sets an mat2 uniform
	* @param name uniform name
	* @param mat mat2 value
	*/
	void setMat2(const std::string &name, const glm::mat2 &mat) const;

	/**
	* Sets an mat3 uniform
	* @param name uniform name
	* @param mat mat3 value
	*/
	void setMat3(const std::string &name, const glm::mat3 &mat) const;

	/**
	* Sets an mat4 uniform
	* @param name uniform name
	* @param mat mat4 value
	*/
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

	unsigned int ID; // Program shader ID in GPU

private:
	/**
	* Loads a shader code and compiles it
	* @param path Path to the shader code
	* @param type Type of shader to be compiled
	* @param shaderID Shader code ID assigned by the GPU, if the code compiles
	* @returns Compilation status
	*/
	bool compileShaderCode(const char *path, shaderType type, unsigned int &shaderID);

	/**
	* Links individual shader codes into a shader program
	* @param vertexShaderID GPU id of the vertex shader
	* @param fragmentShaderID GPU id of the fragment shader
	* @returns Linking status
	*/
	bool linkProgram(unsigned int vertexShaderID, unsigned int fragmentShaderID);

	/**
	* Links individual shader codes into a shader program
	* @param vertexShaderID GPU id of the vertex shader
	* @param fragmentShaderID GPU id of the fragment shader
	* @param geometryShaderID GPU id of the geometry shader
	* @returns Linking status
	*/
	bool linkProgram(unsigned int vertexShaderID, unsigned int fragmentShaderID, unsigned int geometryShaderID);
};
