#version 330 core
in vec3 vNormal;
in vec4 vColor;
in vec4 vFragPosLightSpace;

out vec4 FragColor;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform sampler2D uShadowMap;

float calculateShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0) return 0.0;
    
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = 0.005;
    return currentDepth - bias > closestDepth ? 0.6 : 0.0;
}

void main() {
    // Диффузное освещение
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos);
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Тень
    float shadow = calculateShadow(vFragPosLightSpace);
    
    // Итоговый цвет
    vec3 result = (diff * (1.0 - shadow)) * uLightColor;
    FragColor = vec4(vColor.rgb * result, vColor.a);
}