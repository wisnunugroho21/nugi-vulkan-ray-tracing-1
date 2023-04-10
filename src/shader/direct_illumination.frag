#version 460

// ------------- layout -------------

#define KEPSILON 0.00001

#include "helper/struct.glsl"

layout(origin_upper_left) in vec4 gl_FragCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform readonly RasterUbo {
	mat4 projection;
	mat4 view;
	vec3 realCameraPos;
} ubo;

layout(set = 0, binding = 1) buffer readonly GlobalLight {
	PointLight pointLights[100];
	uint numLight;
} globalLight;

layout(set = 0, binding = 2) buffer readonly GlobalModel {
  Model models[100];
  uint numModel;
} globalModel;

layout(set = 0, binding = 3) buffer readonly GlobalBvh {
  BvhNode bvhNodes[100];
  uint numNode;
} globalBvh;

layout(set = 1, binding = 0, rgba8) uniform readonly image2D positionResource;
layout(set = 1, binding = 1, rgba8) uniform readonly image2D albedoResource;
layout(set = 1, binding = 2, rgba8) uniform readonly image2D normalResource;
layout(set = 1, binding = 3, rgba8) uniform readonly image2D materialResource;

layout(push_constant) uniform Push {
  uint randomSeed;
} push;

// ------------- function ------------- 

uvec2 imgSize = uvec2(imageSize(positionResource));

uint rngStateXY = (imgSize.x * uint(gl_FragCoord.y) + uint(gl_FragCoord.x)) * (push.randomSeed + 1);
uint rngStateYX = (imgSize.y * uint(gl_FragCoord.x) + uint(gl_FragCoord.y)) * (push.randomSeed + 1);

#include "helper/random.glsl"
#include "helper/trace.glsl"
#include "helper/brdf.glsl"

// ------------- Material -------------

float shade(float NoV, float NoL, float NoH, float VoH, vec3 material) {
  float f0 = 0.16 * (material.z * material.z); // F0 for dielectics in range [0.0, 0.16].  default FO is (0.16 * 0.5^2) = 0.04
  f0 = mix(f0, 1.0, material.x);

  // specular microfacet (cook-torrance) BRDF
  float F = fresnelSchlick(VoH, f0);
  float D = D_GGX(NoH, material.y);
  float G = G_Smith(NoV, NoL, material.y);
  float spec = (F * D * G) / (4.0 * max(NoV, 0.001) * max(NoL, 0.001));

  // diffuse
  float rhoD = 1.0;
  rhoD *= rhoD - F; // if not specular, use as diffuse (optional)
  rhoD *= (1.0 - material.x); // no diffuse for metals
  float diff = rhoD / pi;

  return diff + spec;
}

// ------------- Main -------------

void main() {
  vec3 fragPosWorld = imageLoad(positionResource, ivec2(gl_FragCoord.xy)).xyz;
  vec3 fragColor = imageLoad(albedoResource, ivec2(gl_FragCoord.xy)).xyz;
  vec3 fragNormalWorld = imageLoad(normalResource, ivec2(gl_FragCoord.xy)).xyz;
  vec3 fragMaterial = imageLoad(materialResource, ivec2(gl_FragCoord.xy)).xyz;

  vec3 totalColor = vec3(0.0);

  for (int i = 0; i < globalLight.numLight; i++) {
    vec3 viewDirection = normalize(ubo.realCameraPos - fragPosWorld);
    vec3 lightDirection = normalize(globalLight.pointLights[i].sphere.center - fragPosWorld);
    vec3 halAngle = normalize(lightDirection + viewDirection); // half vector

    float NoV = clamp(dot(fragNormalWorld, viewDirection), 0.001, 1.0);
    float NoL = clamp(dot(fragNormalWorld, lightDirection), 0.001, 1.0);
    float NoH = clamp(dot(fragNormalWorld, halAngle), 0.001, 1.0);
    float VoH = clamp(dot(viewDirection, halAngle), 0.001, 1.0);

    Ray r;
    r.origin = fragPosWorld;
    r.direction = lightDirection;

    HitRecord hitLight = hitSphere(globalLight.pointLights[i].sphere, r, 0.001, 1000000.0); 
    HitRecord hitObject = hitBvh(r, 0.001, (hitLight.isHit) ? hitLight.t : 1000000.0);

    if (hitObject.isHit) {
      continue;
    }

    vec3 distance = (hitLight.point - r.origin) / 10.0;
    float brdf = shade(NoV, NoL, NoH, VoH, fragMaterial);
    float NloL = dot(hitLight.faceNormal.normal, -1.0 * normalize(r.direction));

    totalColor += areaSphere(globalLight.pointLights[i].sphere) * globalLight.pointLights[i].color * brdf * NoL * NloL / dot(distance, distance);
  }

  outColor = vec4(totalColor * fragColor, 1.0);
}