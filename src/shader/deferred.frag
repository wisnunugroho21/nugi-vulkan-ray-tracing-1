#version 460

// ------------- layout -------------

layout(origin_upper_left) in vec4 gl_FragCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0, rgba8) uniform image2D positionResource;

void main() {
  outColor = imageLoad(positionResource, ivec2(gl_FragCoord.xy));
}