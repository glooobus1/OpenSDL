#version 330 core
in vec3 vColor;
in vec2 vUV;

out vec4 FragColor;

uniform bool uUseTexture;
uniform sampler2D uTexture;
uniform bool uUseNoise;
uniform float uNoiseStrength;
uniform float uTime;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec4 finalColor;
    
    if (uUseTexture) {
        finalColor = texture(uTexture, vUV);
    } else {
        finalColor = vec4(vColor, 1.0);
    }
    
    if (uUseNoise) {
        float n = random(vUV + uTime);
        finalColor.rgb += (n - 0.5) * uNoiseStrength;
    }
    
    FragColor = finalColor;
}