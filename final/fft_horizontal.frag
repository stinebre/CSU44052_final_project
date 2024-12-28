#version 330 core

uniform sampler2D inputTexture; 
uniform int passNumber;          
uniform float time;

out vec4 FragColor;

void main()
{
    ivec2 texelCoords = ivec2(gl_FragCoord.xy);  
    float stepSize = float(1 << passNumber);

    // Calculate offsets for texel
    vec2 texelSize = vec2(1.0) / textureSize(inputTexture, 0); 
    vec2 offsetA = texelCoords * texelSize;  
    vec2 offsetB = (texelCoords - ivec2(stepSize, 0)) * texelSize;  

    // Sample texture
    vec2 valueA = texture(inputTexture, offsetA).xy;
    vec2 valueB = texture(inputTexture, offsetB).xy;

    // Twiddle factor
    float angle = -2.0 * 3.14159265359 * float(texelCoords.x % int(stepSize)) / float(stepSize);
    float phaseShift = time * 0.1;  // Time-based phase shift
    angle += phaseShift;
    vec2 twiddle = vec2(cos(angle), sin(angle));

    // Butterfly operation
    vec2 combined = valueA + twiddle * valueB;
    combined += sin(time * 0.1 + float(texelCoords.x) * 0.01) * 0.5;

    FragColor = vec4(combined, 0.0, 1.0);
}
