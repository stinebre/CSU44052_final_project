#version 330 core
in vec2 fragUV;
in vec3 worldPosition;

uniform sampler2D heightMap;
uniform vec3 lightDir;      // Directional light (e.g., sun)
uniform vec3 ambientColor;  // Ambient light color (dark blue tone)
uniform vec3 cameraPos;       // View direction (camera position)

out vec4 FragColor;

void main() {
    // Sample the height map to get the height
    float height = texture(heightMap, fragUV).r;

    // Compute the gradient of the height map for the normal
    vec2 texelSize = 1.0 / textureSize(heightMap, 0).xy;
    float heightLeft = texture(heightMap, fragUV - vec2(texelSize.x, 0)).r;
    float heightRight = texture(heightMap, fragUV + vec2(texelSize.x, 0)).r;
    float heightDown = texture(heightMap, fragUV - vec2(0, texelSize.y)).r;
    float heightUp = texture(heightMap, fragUV + vec2(0, texelSize.y)).r;

    vec3 normal = normalize(vec3(
        (heightLeft - heightRight) * 2.0,  // X component of the normal
        1.0,                               // Y component (up direction)
        (heightDown - heightUp) * 2.0      // Z component of the normal
    ));

    // Compute the lighting (diffuse + ambient)
    float diffuse = max(dot(normal, -lightDir), 0.0);
    vec3 baseBlue = vec3(0.1, 0.2, 0.6); // Darker blue base color
    vec3 lighting = ambientColor * baseBlue + diffuse * baseBlue;

    // Add specular highlights (constrained to a lighter blue tone)
    vec3 viewDir = normalize(cameraPos - worldPosition);
    vec3 halfVector = normalize(-lightDir + viewDir);  // Halfway vector between light and view
    float specular = pow(max(dot(normal, halfVector), 0.0), 8.0); // Shininess factor
    vec3 specularColor = vec3(0.1, 0.2, 0.6) * specular * 0.3; // Soft specular blue

    // Add a Fresnel effect for edge highlights (constrained to light blue)
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 3.0);
    vec3 fresnelColor = mix(lighting, vec3(0.1, 0.2, 0.6), fresnel); // Blend towards light blue edge

    // Final color, keeping within the blue range
    vec3 finalColor = clamp(fresnelColor + specularColor, 0.0, 1.0);

    FragColor = vec4(finalColor, 1.0);
}
