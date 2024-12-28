#version 330 core

in vec3 worldPosition;
in vec3 worldNormal; 
in vec4 fragPosLightSpace;

out vec3 finalColor;

uniform sampler2D shadowMap;
uniform samplerCube skybox; 
uniform vec3 lightDir;       
uniform vec3 cameraPos;       

void main()
{
    vec3 lightColor = vec3(1.0);
    vec3 ambientColor = 0.10 * lightColor;
    vec3 surfaceColor = vec3(0.5); 

    vec3 normal = normalize(worldNormal);
    vec3 lightDirNorm = normalize(lightDir);

    // Diffuse and ambient
    float diffuse = max(dot(normal, -lightDir), 0.0);
    vec3 lighting = ambientColor * surfaceColor + diffuse * surfaceColor;

    // Specular highlights
    vec3 viewDir = normalize(cameraPos - worldPosition);
    vec3 halfVector = normalize(-lightDirNorm + viewDir);  // Halfway vector
    float specular = pow(max(dot(normal, halfVector), 0.0), 16.0); // Shininess factor
    vec3 specularColor = vec3(1.0) * specular * 0.3; // Specular highlights

    // Environment mapping 
    vec3 reflectDir = reflect(-viewDir, normal); // Reflection direction
	reflectDir.y = -reflectDir.y;
    vec3 envColor = texture(skybox, reflectDir).rgb; // Sample the cube map
    envColor *= 0.6; // Adjust reflection intensity

    // Shadows
    vec3 uv = fragPosLightSpace.xyz / fragPosLightSpace.w;
    uv = uv * 0.5 + 0.5; // Transform from NDC to texture coordinates
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || uv.z > 1.0) {
        // Outside shadow map bounds; no shadow applied
        uv.z = 1.0;
    }
    float existingDepth = texture(shadowMap, uv.xy).r;
    float bias = max(0.005 * (1.0 - dot(normal, lightDirNorm)), 0.0005); // Shadow bias
    float shadow = (uv.z > existingDepth + bias) ? 0.2 : 1.0;

    lighting = (lighting + specularColor) * shadow;

    // Blend with environment mapping
    vec3 finalLighting = mix(lighting, envColor, 0.3); 

    // Reinhard tone mapping
    float exposure = 1.0; // Adjust for scene brightness
    vec3 mapped = vec3(1.0) - exp(-finalLighting * exposure);

    // Gamma correction
    const float gamma = 2.2;
    finalColor = pow(mapped, vec3(1.0 / gamma));
}
