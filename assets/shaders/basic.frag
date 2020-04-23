#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
in vec2 textCoord;

uniform vec4 color;
uniform sampler2D text1;

// Fragment Color
out vec4 fragColor;

void main()
{
    vec4 textureColor = texture(text1, textCoord);
    fragColor = textureColor * color;
}