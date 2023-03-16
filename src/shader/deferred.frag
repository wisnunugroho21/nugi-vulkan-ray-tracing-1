#version 460

// ------------- layout -------------

layout(origin_upper_left) in vec4 gl_FragCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0, rgba32) uniform writeonly image2D positionResource;
layout(set = 0, binding = 1, rgba32) uniform writeonly image2D albedoResource;
layout(set = 0, binding = 2, rgba32) uniform writeonly image2D normalResource;

layout(set = 0, binding = 0) uniform readonly GlobalUbo {
	mat4 projection;
	mat4 view;
	mat4 inverseView;
} ubo;

layout(set = 0, binding = 1) uniform readonly GlobalLight {
	PointLight pointLights[10];
	uint numLight;
} globalLight;

void main() {
  vec3 fragPosWorld = imageLoad(positionResource, ivec2(gl_FragCoord.xy)).xyz;
  vec3 fragColor = imageLoad(albedoResource, ivec2(gl_FragCoord.xy)).xyz;
  vec3 fragNormalWorld = imageLoad(normalResource, ivec2(gl_FragCoord.xy)).xyz;

  vec3 diffuseLight = vec4(0.0);
  vec3 specularLight = vec3(0.0);

  vec3 surfaceNormal = normalize(fragNormalWorld);
  vec3 cameraPosWorld = ubo.inverseView[3].xyz;
  vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

  for (int i = 0; i < globalLight.numLights; i++) {
    PointLight light = globalLight.pointLights[i];

    vec3 directionToLight = light.position.xyz - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);
    directionToLight = normalize(directionToLight);
    
    float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;
    diffuseLight += intensity * cosAngIncidence;

    // specular light
    vec3 halAngle = normalize(directionToLight + viewDirection);
    float blinnTerm = dot(surfaceNormal, halAngle);
    blinnTerm = clamp(blinnTerm, 0, 1);
    blinnTerm = pow(blinnTerm, 256.0); // higher value -> sharper light
    specularLight += intensity * blinnTerm;
  }

  outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
}