#version 460

// ------------- layout ------------- 

layout(origin_upper_left) in vec4 gl_FragCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0, rgba8) uniform image2D accumulateImage;
layout(set = 0, binding = 1, rgba8) uniform readonly image2D rayTraceImage;

layout(push_constant) uniform Push {
  uint randomSeed;
} push;

void main() {
  vec4 accColor = imageLoad(accumulateImage, ivec2(gl_FragCoord.xy));
  vec4 totalColor = imageLoad(rayTraceImage, ivec2(gl_FragCoord.xy));

  totalColor = (totalColor + accColor * push.randomSeed) / (push.randomSeed + 1.0);

  imageStore(accumulateImage, ivec2(gl_FragCoord.xy), totalColor);
  outColor = totalColor;
}