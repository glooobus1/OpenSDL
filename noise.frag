// noise.frag
#version 120
varying vec2 vUV;
uniform float uTime;
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
void main() {
    float noise = random(vUV + uTime);
    gl_FragColor = vec4(noise, noise * 0.5, 1.0 - noise, 1.0);
}