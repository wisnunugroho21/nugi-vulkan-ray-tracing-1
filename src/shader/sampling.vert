#version 460

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 textCoord;
layout(location = 2) in vec4 normal;
layout(location = 3) in uint materialIndex;
layout(location = 4) in uint transformIndex;

void main() {
    gl_Position = position;
}