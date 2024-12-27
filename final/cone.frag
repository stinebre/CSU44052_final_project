#version 330 core

in vec3 color;
in vec3 worldPosition;
in vec3 worldNormal; 
in vec4 fragPosLightSpace;

out vec3 finalColor;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;

uniform sampler2D shadowMap;

void main()
{
	vec3 lightColor = vec3(1.0);
	vec3 ambient = 0.10 * lightColor;

	// lambertian lighting equation
	vec3 lightDir = normalize(lightPosition - worldPosition);  
	float cosine = dot(worldNormal, lightDir);
	float lambert = max(cosine, 0.0);
	finalColor = color * lightColor * lambert + ambient;

	// reinhard tone mapping
	float exposure = 5.0;
    vec3 mapped = vec3(1.0) - exp(-finalColor * exposure);
	
    // gamma correction 
	const float gamma = 2.2;
    mapped = pow(mapped, vec3(1.0 / gamma));
  
	// shadows
	vec3 uv = fragPosLightSpace.xyz / fragPosLightSpace.w;
	uv = uv * 0.5 + 0.5;
	float existingDepth = texture(shadowMap, uv.xy).r; 
	float depth = uv.z;
	float shadow = (depth >= existingDepth + 1e-3) ? 0.2 : 1.0; 

	if (fragPosLightSpace.x < -fragPosLightSpace.w || fragPosLightSpace.x > fragPosLightSpace.w ||
        fragPosLightSpace.y < -fragPosLightSpace.w || fragPosLightSpace.y > fragPosLightSpace.w ||
        fragPosLightSpace.z < -fragPosLightSpace.w || fragPosLightSpace.z > fragPosLightSpace.w)
    {
        shadow = 1.0;; 
    }

	finalColor = finalColor * mapped * shadow;
}



//in vec3 reflectionVector;

//uniform samplerCube skybox;
    //vec3 grey = vec3(1.0);
   // vec3 envColor = texture(skybox, reflectionVector).rgb;
   // vec3 finalColor = mix(finalColor, envColor, 0.65);

