#version 330 core

uniform sampler2D horizontalPassTexture;
uniform int passNumber; 
uniform float time;

out vec4 FragColor;

void main()
{
    ivec2 texelCoords = ivec2(gl_FragCoord.xy);
    float stepSize = float(1 << passNumber);

    // Calculate offsets for texel
    vec2 texelSize = vec2(1.0) / textureSize(horizontalPassTexture, 0);
    vec2 offsetA = vec2(texelCoords.x, texelCoords.y) * texelSize;
    vec2 offsetB = vec2(texelCoords.x, texelCoords.y - stepSize) * texelSize;

    // Sample the data
    vec2 valueA = texture(horizontalPassTexture, offsetA).xy;
    vec2 valueB = texture(horizontalPassTexture, offsetB).xy;

    // Twiddle factor
    float angle = -2.0 * 3.14159265359 * float(texelCoords.y % int(stepSize)) / (2.0 * stepSize);
    float phaseShift = time * 0.1;  // Time-based phase shift
    angle += phaseShift;
    vec2 twiddle = vec2(cos(angle), sin(angle));

    // Combine the results (Butterfly operation)
    vec2 combined = valueA + twiddle * valueB;

    // Write the result
    FragColor = vec4(combined, 0.0, 1.0);
}