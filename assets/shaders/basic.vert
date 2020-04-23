#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
// Atributte 1 of the vertex
layout (location = 1) in vec3 vertexColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform float scale = 1;

// Vertex data out data
out vec3 vColor;
out vec2 textCoord;

void main()
{
    vColor = vertexColor;
    textCoord = vec2(vertexPosition.xy + 0.5);
    gl_Position =  projection * view  * model * vec4( scale * vertexPosition, 1.0f);
}