#version 420 core

in vec3 vPosition;
in vec3 vNormal;

out vec3 fNormal;
out vec3 fViewVertex;

uniform mat4 modelViewProjMat;
uniform mat4 modelViewMat;
uniform mat4 normalMat;

void main() {
    gl_Position = modelViewProjMat * vec4(vPosition, 1.0);
    vec4 normal = normalMat * vec4(vNormal, 1.0);
    vec4 viewVertex = modelViewMat * vec4(vPosition, 1.0);

    fNormal = normal.xyz;
    fViewVertex = viewVertex.xyz;
}
