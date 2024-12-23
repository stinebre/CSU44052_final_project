#version 330 core

in vec2 UV; // Texture coordinates from the vertex shader

out vec4 FragColor; // Output color

uniform sampler2D heightMap; // The height map (the result of the FFT pass)
uniform float time; // Time uniform to animate the ocean

void main()
{
    // Sample the height map at the current UV coordinates
    float height = texture(heightMap, UV).r;

    // Use the height value to create a simple color (for visualization, e.g., blue for the ocean)
    FragColor = vec4(0.0, 0.0, 1.0, 1.0) * height; // Modify this to create more interesting visuals
}
