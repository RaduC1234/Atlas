#type vertex
#version 430 core
layout (location=0) in vec3 aPos;// Position of the vertex
layout (location=1) in vec4 aColor;// Color of the vertex
layout (location=2) in vec2 aTexCoords;// Texture coordinates
layout (location=3) in float aTexId;// Texture ID
layout (location=4) in float aShapeType;// Shape type (0 for quad, 1 for circle)
layout (location=5) in float aCircleRadius;// Circle radius

uniform mat4 uWorldProjection;
uniform mat4 uView;

out vec4 fColor;
out vec2 fTexCoords;
out float fTexId;
out float fShapeType;
out float fCircleRadius;

void main()
{
    fColor = aColor;
    fTexCoords = aTexCoords;
    fTexId = aTexId;
    fShapeType = aShapeType;
    fCircleRadius = aCircleRadius;

    gl_Position = uWorldProjection * uView * vec4(aPos, 1.0);
}


#type fragment
#version 430 core

in vec4 fColor;// Color passed from the vertex shader
in vec2 fTexCoords;// Texture coordinates passed from the vertex shader
in float fTexId;// Texture ID passed from the vertex shader
in float fShapeType;// Shape type passed from the vertex shader
in float fCircleRadius;// Circle radius passed from the vertex shader

uniform sampler2D uTextures[16];// Array of texture samplers

out vec4 color;

void main() {

    if (fShapeType == 2.0) { // if rendering text

        int id = int(fTexId);
        color = fColor * texture(uTextures[id], fTexCoords).r;
    }
    else if (fShapeType == 1.0) { // If we are rendering a circle
        // Calculate the distance from the center of the circle
        float dist = length(fTexCoords - vec2(0.5, 0.5));// Assuming the circle is centered in the quad

        // Check if the distance exceeds the circle radius
        if (dist > fCircleRadius) {
            discard;// Discard the fragment if it's outside the circle
        }

        // If we're inside the circle, we still apply color (can also apply texture here)
        color = fColor;// Apply color or texture as desired

    } else {
        // Normal texture handling for other shapes (like quads)
        if (fTexId > 0.0) {
            int id = int(fTexId);
            color = fColor * texture(uTextures[id], fTexCoords);
        } else {
            color = fColor;// Just use the color if no texture is used
        }
    }
}

