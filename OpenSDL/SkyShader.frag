ф#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform float uTime;
uniform vec2 uResolution;

// Функция шума для облаков
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 st, int octaves) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 4.0;
    
    for(int i = 0; i < octaves; i++) {
        value += amplitude * noise(st * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return value;
}

void main() {
    vec2 uv = vUV;
    
    // Небо: градиент от синего к светлому
    vec3 skyColorTop = vec3(0.2, 0.5, 0.9);
    vec3 skyColorBottom = vec3(0.7, 0.8, 1.0);
    vec3 skyColor = mix(skyColorBottom, skyColorTop, uv.y);
    
    // СОЛНЦЕ (жёлтый круг)
    vec2 sunPos = vec2(0.7, 0.85);  // позиция солнца (x, y)
    float sunRadius = 0.12;
    float sunGlow = 0.18;
    
    float distToSun = distance(uv, sunPos);
    
    // Солнце
    float sunIntensity = 1.0 - smoothstep(sunRadius - 0.02, sunRadius, distToSun);
    vec3 sunColor = vec3(1.0, 0.9, 0.5);
    
    // Свечение вокруг солнца
    float glowIntensity = 1.0 - smoothstep(sunRadius, sunGlow, distToSun);
    vec3 glowColor = vec3(1.0, 0.7, 0.3);
    
    // ОБЛАКА (с помощью шума)
    vec2 cloudUV = uv * 3.0 + vec2(uTime * 0.05, 0.0);
    float clouds1 = fbm(cloudUV, 4);
    float clouds2 = fbm(uv * 5.0 - vec2(uTime * 0.03, 0.0), 3);
    
    // Форма облаков (только в верхней части неба)
    float cloudMask = smoothstep(0.0, 0.5, uv.y);
    float clouds = (clouds1 * 0.8 + clouds2 * 0.5) * cloudMask;
    
    // Обрезаем облака, чтобы не перекрывали солнце
    clouds *= 1.0 - smoothstep(sunRadius - 0.05, sunRadius + 0.1, distToSun);
    
    vec3 cloudColor = vec3(1.0, 1.0, 1.0);
    
    // Смешиваем цвета
    vec3 finalColor = skyColor;
    finalColor += glowColor * glowIntensity * 0.8;
    finalColor += sunColor * sunIntensity;
    finalColor = mix(finalColor, cloudColor, clouds * 0.7);
    
    FragColor = vec4(finalColor, 1.0);
}