#version 330 core

in vec2 UV; // Texture coordinates from the vertex shader

out vec4 FragColor; // Output color

uniform sampler2D heightMap; // The height map (output from the FFT pass)
uniform sampler2D oceanTexture; // The ocean texture (e.g., normal map or color map)
uniform vec2 textureScale; // Texture scaling factor

void main()
{
    // Sample the height map at the current UV coordinates
    float height = texture(heightMap, UV).r;

    // Fetch the color from the ocean texture using the scaled UV coordinates
    vec3 oceanColor = texture(oceanTexture, UV * textureScale).rgb;

    // Combine the height with the texture color for the final ocean appearance
    vec3 finalColor = oceanColor * (1.0 - height); // The height value could be used to modify shading or texture

    // Output the final color
    FragColor = vec4(finalColor, 1.0);
}
