#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

uniform int n = 1;
uniform int frameIndex = 0;
uniform mat4 model;
uniform mat4 projection; 
uniform int flipVertical = 0;
uniform int flipHorizontal = 0;

out vec3 ourColor;
out vec2 TexCoord;

void main() 
{
    gl_Position = projection * model * vec4(position, 1.0f);
    ourColor = color;
    float x = ( (1.0f/n) * (texCoord.x + frameIndex));
    TexCoord = vec2(flipHorizontal == 1? 1.0f - x : x, flipVertical == 1 ? texCoord.y : 1.0f - texCoord.y);
}



