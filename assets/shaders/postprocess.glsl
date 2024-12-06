#type vertex
#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 fTexCoords;

void main() {
    fTexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}


#type fragment
#version 430 core

out vec4 color;
in vec2 fTexCoords;

uniform sampler2D screenTexture;

void main() {
    color = texture(screenTexture, fTexCoords);
}

