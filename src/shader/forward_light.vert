#version 460

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

struct Sphere {
  vec3 center;
  float radius;
};

struct PointLight {
	Sphere sphere;
  vec3 color;
};

struct Material {
  vec3 color;
	float metallicness;
  float roughness;
  float fresnelReflect;
};

struct Transform {
  mat4 modelMatrix;
	mat4 normalMatrix;
};

layout(location = 0) out vec2 fragOffset;
layout(location = 1) out vec3 positionFrag;
layout(location = 2) out vec3 albedoFrag;

layout(set = 0, binding = 0) uniform readonly RasterUbo {
	mat4 projection;
	mat4 view;
	vec3 realCameraPos;
} ubo;

layout(set = 0, binding = 1) buffer readonly GlobalLight {
	PointLight pointLights[100];
	uint numLight;
} globalLight;

layout(set = 0, binding = 4) buffer readonly GlobalMaterial {
  Material materials[100];
};

layout(set = 0, binding = 5) uniform readonly GlobalTransform {
  Transform transform[100];
};

void main() {
  fragOffset = OFFSETS[gl_VertexIndex % 6];

  vec3 cameraRightWorld = { ubo.view[0][0], ubo.view[1][0], ubo.view[2][0] };
  vec3 cameraUpWorld = { ubo.view[0][1], ubo.view[1][1], ubo.view[2][1] };

  vec3 positionWorld = globalLight.pointLights[int(gl_VertexIndex / 6)].sphere.center
    + globalLight.pointLights[int(gl_VertexIndex / 6)].sphere.radius * fragOffset.x * cameraRightWorld
    + globalLight.pointLights[int(gl_VertexIndex / 6)].sphere.radius * fragOffset.y * cameraUpWorld;

  gl_Position = ubo.projection * ubo.view * vec4(positionWorld, 1.0);
  
  positionFrag = positionWorld;
  albedoFrag = globalLight.pointLights[int(gl_VertexIndex / 6)].color;
}