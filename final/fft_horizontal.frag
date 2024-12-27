#version 330 core

uniform sampler2D inputTexture;  // Input texture containing the data
uniform int passNumber;          // Current FFT pass number (0 to log2(width) - 1)
uniform float time;

out vec4 FragColor;  // Output of the horizontal FFT pass

void main()
{
    ivec2 texelCoords = ivec2(gl_FragCoord.xy);  // Current fragment coordinates
    float stepSize = float(1 << passNumber);     // Step size for this pass, 2^passNumber

    // Calculate the texel offsets for butterfly combinations
    vec2 texelSize = vec2(1.0) / textureSize(inputTexture, 0); // The size of a single texel in texture space
    vec2 offsetA = texelCoords * texelSize;  // Normalized coordinates for A
    vec2 offsetB = (texelCoords - ivec2(stepSize, 0)) * texelSize;  // Horizontal offset for B

    // Sample the data from the texture
    vec2 valueA = texture(inputTexture, offsetA).xy;
    vec2 valueB = texture(inputTexture, offsetB).xy;

    // Compute the twiddle factor based on the pass and the current x-coordinate
    float angle = -2.0 * 3.14159265359 * float(texelCoords.x % int(stepSize)) / float(stepSize);
    float phaseShift = time * 0.1;  // Time-based phase shift
    angle += phaseShift;
    vec2 twiddle = vec2(cos(angle), sin(angle));

    // Butterfly operation: combine the two values with the twiddle factor
    vec2 combined = valueA + twiddle * valueB;
    combined += sin(time * 0.1 + float(texelCoords.x) * 0.01) * 0.5;

    // Output the combined result
    FragColor = vec4(combined, 0.0, 1.0);
}

/* simple but working
#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform float time;

void main() {
    float wave = sin(UV.x * 10.0 + time);
    FragColor = vec4(wave, 0.0, 0.0, 1.0);
}
*/
