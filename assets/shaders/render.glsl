#type vertex
#version 430 core
layout (location=0) in vec3 aPos;// Position of the vertex
layout (location=1) in vec4 aColor;// Color of the vertex
layout (location=2) in vec2 aTexCoords;// Texture coordinates
layout (location=3) in float aTexId;// Texture ID
layout (location=4) in float aShapeType;// Shape type (0 for quad, 1 for circle)

uniform mat4 uWorldProjection;
uniform mat4 uView;

out vec4 fColor;
out vec2 fTexCoords;
out float fTexId;
out float fShapeType;

void main()
{
    fColor = aColor;
    fTexCoords = aTexCoords;
    fTexId = aTexId;
    fShapeType = aShapeType;

    gl_Position = uWorldProjection * uView * vec4(aPos, 1.0);
}


#type fragment
#version 430 core

in vec4 fColor;// Color passed from the vertex shader
in vec2 fTexCoords;// Texture coordinates passed from the vertex shader
in float fTexId;// Texture ID passed from the vertex shader
in float fShapeType;// Shape type passed from the vertex shader

uniform sampler2D uTextures[16];// Array of texture samplers

out vec4 color;

void main() {

    if (fShapeType == 2.0) { // text

        int id = int(fTexId);
        color = fColor * texture(uTextures[id], fTexCoords).r;
    }
    else if (fShapeType == 1.0) { // circles
        color = vec4(1.0f, 1.0f, 1.0f, 1.0f); // implement circles
    } else {
        if (fTexId > 0.0) { // quads
            int id = int(fTexId);
            color = fColor * texture(uTextures[id], fTexCoords);
        } else {
            color = fColor;
        }
    }

    if(color.a != 0.0f) {
        color.a = 1.0f;
    }
}

