#version 330 core
in vec2 fragUV;

uniform sampler2D heightMap;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;

out vec4 FragColor;

void main() {
    float height = texture(heightMap, fragUV).r;
    vec3 normal = normalize(vec3(0.0, 1.0, 0.0) + height * vec3(0.1, 1.0, 0.1));

    float diffuse = max(dot(normal, -lightDir), 0.0);
    vec3 color = ambientColor + diffuse * lightColor;

    FragColor = vec4(color, 1.0);
}
