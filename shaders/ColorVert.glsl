#version 420 core

in vec3 vPosition;

uniform mat4 modelViewProjMat;

void main() {
    gl_Position = modelViewProjMat * vec4(vPosition, 1.0);
}
