#version 330 core

in vec3 reflectionVector;
in vec3 normal;
in vec3 color; 

// Output color
out vec4 finalColor;

// Cube map sampler
uniform samplerCube skybox;

void main() {
    vec3 envColor = texture(skybox, reflectionVector).rgb;
    vec3 grey = vec3(0.5);
    vec3 tintedColor = mix(envColor, grey, 0.65);
    finalColor = vec4(tintedColor, 1.0);
}
