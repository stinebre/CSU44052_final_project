#version 330 core

in vec3 color;
in vec2 uv;

out vec3 finalColor;

uniform sampler2D textureSampler;

void main()
{
	finalColor = color * texture(textureSampler, uv).rgb;
}
