#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;

uniform mat4 MVP;
uniform sampler2D heightMap;

out vec2 fragUV;
out vec3 worldPosition;

void main() {
    fragUV = vertexUV;
    worldPosition = vertexPosition;

    // Sample the height from the heightmap
    float height = texture(heightMap, fragUV).r;
    height = sign(height) * (1.0 - exp(-abs(height)));

    // Displace the vertex position in the y-direction
    vec3 displacedPosition = vertexPosition;
    displacedPosition.y += height * 0.4;  // Scale height for better visibility

    gl_Position = MVP * vec4(displacedPosition, 1.0);
    //fragUV = vec2(height);  // Pass height as UV for debugging
}
