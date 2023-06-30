#version 460

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 textCoord;
layout(location = 2) in uint materialIndex;
layout(location = 3) in uint transformIndex;

void main() {
    gl_Position = vec4(position, 1.0f);
}