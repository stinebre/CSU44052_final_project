#version 330 core

uniform sampler2D horizontalPassTexture;  // Input texture containing the data
uniform int passNumber;          // Current FFT pass number (0 to log2(height) - 1)
uniform float time;              // Time value from the fftVerticalPass function

out vec4 FragColor;  // Output of the vertical FFT pass

void main()
{
    ivec2 texelCoords = ivec2(gl_FragCoord.xy);  // Current fragment coordinates
    float stepSize = float(1 << passNumber);     // Step size for this pass

    // Calculate the texel offsets for butterfly combinations
    vec2 texelSize = vec2(1.0) / textureSize(horizontalPassTexture, 0);
    vec2 offsetA = vec2(texelCoords.x, texelCoords.y) * texelSize;
    vec2 offsetB = vec2(texelCoords.x, texelCoords.y - stepSize) * texelSize;

    // Sample the data
    vec2 valueA = texture(horizontalPassTexture, offsetA).xy;
    vec2 valueB = texture(horizontalPassTexture, offsetB).xy;

    // Compute the twiddle factor (using the pass number and time to animate the phase shift)
    float angle = -2.0 * 3.14159265359 * float(texelCoords.y % int(stepSize)) / (2.0 * stepSize);
    float phaseShift = time * 0.1;  // Time-based phase shift
    angle += phaseShift;
    vec2 twiddle = vec2(cos(angle), sin(angle));

    // Combine the results (Butterfly operation)
    vec2 combined = valueA + twiddle * valueB;

    // Write the result
    FragColor = vec4(combined, 0.0, 1.0);
}

/* simple but working 
#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D horizontalPassTexture;

void main() {
    float wave = texture(horizontalPassTexture, UV).r * cos(UV.y * 10.0);
    FragColor = vec4(wave, 0.0, 0.0, 1.0);
}
*/

