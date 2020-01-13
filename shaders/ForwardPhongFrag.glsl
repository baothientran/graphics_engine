#version 420 core

#define MAX_LIGHTS 10

struct PointLight {
    vec3 position;
    vec3 color;
    float radius;
};


in vec3 fViewVertex;
in vec3 fNormal;

out vec4 color;

uniform vec3 lightAmbient;

uniform PointLight pointLights[MAX_LIGHTS];
//uniform vec3 lightPosition[MAX_LIGHTS];
//uniform vec3 lightColor[MAX_LIGHTS];
//uniform float lightRadius[MAX_LIGHTS];


uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

void main() {
    vec3 ambient = lightAmbient * ambientColor;

    vec3 diffuseSpecular = vec3(0.0f);
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        vec3 lightDirection = normalize(pointLights[i].position - fViewVertex);
        vec3 normal = normalize(fNormal);
        vec3 diffuse = pointLights[i].color * diffuseColor * max(0.0, dot(normal, lightDirection));

        vec3 viewDirection = normalize(-fViewVertex);
        vec3 H = normalize(lightDirection + viewDirection);
        vec3 specular = pointLights[i].color * specularColor * pow(max(0.0, dot(normal, H)), shininess);

        float dist = length(pointLights[i].position - fViewVertex);
        float attenuation = clamp(1.0 - dist * dist / (pointLights[i].radius  * pointLights[i].radius), 0.0, 1.0);
        attenuation *= attenuation;

        diffuseSpecular += attenuation * (diffuse + specular);
    }

    color = vec4(ambient + diffuseSpecular, 1.0);
}
