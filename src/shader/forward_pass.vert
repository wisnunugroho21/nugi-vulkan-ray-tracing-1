#version 460

#include "core/struct.glsl"

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 textCoord;
layout(location = 2) in vec4 normal;
layout(location = 3) in uint materialIndex;
layout(location = 4) in uint transformIndex;

layout(location = 0) out vec3 positionFrag;
layout(location = 1) out vec3 textCoordFrag;
layout(location = 2) flat out vec3 normalFrag;
layout(location = 3) flat out vec3 albedoColorFrag;
layout(location = 4) flat out vec3 materialFrag;

layout(set = 0, binding = 0) uniform readonly RasterUbo {
	mat4 projection;
	mat4 view;
} ubo;

layout(set = 0, binding = 1) buffer readonly MaterialSsbo {
  Material materials[];
};

layout(set = 0, binding = 2) buffer readonly TransformationSsbo {
  Transformation transformations[];
};

void main() {
	gl_Position = ubo.projection * ubo.view * transformations[transformIndex].pointMatrix * position;

	positionFrag = position.xyz;
	textCoordFrag = textCoord.xyz;
	normalFrag = normalize(mat3(transformations[transformIndex].normalMatrix) * normal.xyz);
	albedoColorFrag = materials[materialIndex].baseColor;
	materialFrag = vec3(materials[materialIndex].metallicness, materials[materialIndex].roughness, materials[materialIndex].fresnelReflect);
}