#version 330 core

layout(location = 0) in vec3 vertexPosition; // Vertex position
layout(location = 1) in vec2 vertexUV;       // Texture coordinates

out vec2 UV; // Pass texture coordinates to fragment shader

uniform mat4 MVP; // Model-View-Projection matrix

uniform float waveScale; // Wave scale (affects wave height)
uniform vec2 textureScale; // Scale for texture coordinates

uniform sampler2D heightMap; // The height map (output from the FFT pass)

void main()
{
    // Fetch the height from the height map
    float height = texture(heightMap, vertexUV).r;

    // Scale the height to simulate the wave's vertical displacement
    vec3 adjustedPosition = vertexPosition;
    adjustedPosition.y += height * waveScale;

    // Apply the MVP transformation
    gl_Position = MVP * vec4(adjustedPosition, 1.0);

    // Pass the UV coordinates to the fragment shader, with scaling
    UV = vertexUV * textureScale;
}
