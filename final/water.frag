#version 330 core
in vec2 fragUV;
in vec3 worldPosition;

out vec4 FragColor;

uniform sampler2D heightMap;
uniform vec3 lightDir;      
uniform vec3 ambientColor;  
uniform vec3 cameraPos;       

void main() {
    float height = texture(heightMap, fragUV).r;

    // Calculate normal
    vec2 texelSize = 1.0 / textureSize(heightMap, 0).xy;
    float heightLeft = texture(heightMap, fragUV - vec2(texelSize.x, 0)).r;
    float heightRight = texture(heightMap, fragUV + vec2(texelSize.x, 0)).r;
    float heightDown = texture(heightMap, fragUV - vec2(0, texelSize.y)).r;
    float heightUp = texture(heightMap, fragUV + vec2(0, texelSize.y)).r;
    vec3 normal = normalize(vec3((heightLeft - heightRight) * 2.0, 1.0, (heightDown - heightUp) * 2.0));

    // Diffuse and ambient
    float diffuse = max(dot(normal, -lightDir), 0.0);
    vec3 baseBlue = vec3(0.1, 0.2, 0.6); // Darker blue base color
    vec3 lighting = ambientColor * baseBlue + diffuse * baseBlue;

    // Specular highlights
    vec3 viewDir = normalize(cameraPos - worldPosition);
    vec3 halfVector = normalize(-lightDir + viewDir);  
    float specular = pow(max(dot(normal, halfVector), 0.0), 8.0); 
    vec3 specularColor = vec3(0.1, 0.2, 0.6) * specular * 0.3; 

    // Fresnel (edge highlights)
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 3.0);
    vec3 fresnelColor = mix(lighting, vec3(0.1, 0.2, 0.6), fresnel); // Blend towards light blue edge

    // Final color (clamped to blue tones)
    vec3 finalColor = clamp(fresnelColor + specularColor, 0.0, 1.0);

    FragColor = vec4(finalColor, 1.0);
}
