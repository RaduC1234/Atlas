#type vertex
#version 430 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}


#type fragment
#version 430 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec2 resolution; // Screen resolution (width, height)

void main() {
    vec2 texelSize = 1.0 / resolution; // Size of one texel
    vec3 result = vec3(0.0);

    // Gaussian weights for 9 samples (1 center + 4 on each side)
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    // Sample the texture in a 9-tap kernel (both horizontal and vertical)
    for (int i = -4; i <= 4; ++i) {
        vec2 offset = vec2(float(i)) * texelSize;

        // Accumulate horizontal and vertical samples
        result += texture(screenTexture, TexCoords + vec2(offset.x, 0.0)).rgb * weights[abs(i)];
        result += texture(screenTexture, TexCoords + vec2(0.0, offset.y)).rgb * weights[abs(i)];
    }

    // Normalize the result to avoid oversaturation
    FragColor = vec4(result / 2.0, 1.0);
}




