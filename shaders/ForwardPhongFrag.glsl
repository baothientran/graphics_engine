#version 420 core

in vec3 fViewVertex;
in vec3 fNormal;

out vec4 color;


uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 lightAmbient;
uniform float lightRadius;


uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float diffuseReflection;
uniform float specularReflection;
uniform float shininess;

void main() {
    vec3 ambient = lightAmbient * diffuseColor;

    vec3 vertToLight = normalize(lightPosition - fViewVertex);
    vec3 normal = normalize(fNormal);
    vec3 diffuse = lightColor * diffuseReflection * diffuseColor * max(0.0, dot(normal, vertToLight));

    vec3 viewDir = normalize(-fViewVertex);
    vec3 H = normalize(vertToLight + viewDir);
    vec3 specular = lightColor * specularReflection * specularColor * pow(max(0.0, dot(normal, H)), shininess);

    float dist = length(lightPosition - fViewVertex);
    float attenuation = clamp(1.0 - dist * dist / (lightRadius * lightRadius), 0.0, 1.0);
    attenuation *= attenuation;

    color = vec4(ambient + attenuation * (diffuse + specular), 1.0);
}
