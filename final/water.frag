#version 330 core

in vec2 fragUV;
in vec3 worldPosition;
in vec4 fragPosLightSpace;

out vec4 FragColor;

uniform sampler2D heightMap;
uniform sampler2D shadowMap;
uniform vec3 lightDir;      
uniform vec3 ambientColor;  
uniform vec3 cameraPos;      
uniform mat4 lightSpaceMatrix;

void main() {
    float height = texture(heightMap, fragUV).r;

    // Calculate normal using the height map
    vec2 texelSize = 1.0 / textureSize(heightMap, 0).xy;
    float heightLeft = texture(heightMap, fragUV - vec2(texelSize.x, 0)).r;
    float heightRight = texture(heightMap, fragUV + vec2(texelSize.x, 0)).r;
    float heightDown = texture(heightMap, fragUV - vec2(0, texelSize.y)).r;
    float heightUp = texture(heightMap, fragUV + vec2(0, texelSize.y)).r;
    vec3 normal = normalize(vec3((heightLeft - heightRight) * 2.0, 1.0, (heightDown - heightUp) * 2.0));

    // Diffuse and ambient
    float diffuse = max(dot(normal, -lightDir), 0.0);
    vec3 baseBlue = vec3(0.2, 0.3, 0.7); // Base ocean color
    vec3 lighting = ambientColor * baseBlue + diffuse * baseBlue;

    // Specular highlights
    vec3 viewDir = normalize(cameraPos - worldPosition);
    vec3 halfVector = normalize(-lightDir + viewDir);  
    float specular = pow(max(dot(normal, halfVector), 0.0), 16.0); 
    vec3 specularColor = vec3(0.3, 0.4, 0.8) * specular * 0.3; 

    // Fresnel 
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 3.0);
    vec3 fresnelColor = mix(lighting, vec3(0.1, 0.2, 0.8), fresnel); 
    vec3 finalColor = clamp(fresnelColor + specularColor, 0.0, 1.0);

    // Shadows from other objects in the scene
    vec3 uv = fragPosLightSpace.xyz / fragPosLightSpace.w; 
    uv = uv * 0.5 + 0.5; 
    float shadow = 1.0;
    if (uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0) {
        float depthValue = texture(shadowMap, uv.xy).r;
        float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
        shadow = uv.z > depthValue + bias ? 0.2 : 1.0;
    }

    FragColor = vec4(finalColor * shadow, 1.0);
}
