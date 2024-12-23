#version 330 core

in vec3 fragPosition;
in vec3 fragColor;
in vec3 fragNormal;

out vec4 color;

uniform vec3 lightWorldPosition; // Light position in world space
uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;

uniform samplerCube cubemap;

float calculateShadow(vec4 fragLightSpace) {
    vec3 projCoords = fragLightSpace.xyz / fragLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 0.5 : 1.0;

    return projCoords.z > 1.0 ? 1.0 : shadow;
}

void main() {
    vec3 ambient = 0.2 * fragColor;

    vec3 lightDir = normalize(lightWorldPosition - fragPosition);
    float diff = max(dot(fragNormal, lightDir), 0.0);
    vec3 diffuse = diff * fragColor;

    vec3 viewDir = normalize(-fragPosition);
    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = vec3(0.5) * spec;

    vec4 fragLightSpace = lightSpaceMatrix * vec4(fragPosition, 1.0);
    float shadow = calculateShadow(fragLightSpace);

    vec3 finalColor = (ambient + shadow * (diffuse + specular));
    color = vec4(finalColor, 1.0);
}
