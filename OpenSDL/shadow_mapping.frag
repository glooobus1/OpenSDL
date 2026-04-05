#version 120

uniform sampler2D uShadowMap;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
varying vec4 vFragPosLightSpace;

float calculateShadow(vec4 fragPosLightSpace) {
    // Перспективное деление
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Преобразование в [0,1] диапазон
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0) return 0.0;
    
    float closestDepth = texture2D(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 0.4 : 0.0;
    
    return shadow;
}

void main() {
    float shadow = calculateShadow(vFragPosLightSpace);
    gl_FragColor = vec4(vec3(1.0 - shadow), 1.0);
}