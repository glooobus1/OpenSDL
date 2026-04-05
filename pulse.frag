// pulse.frag
#version 120
uniform float uTime;
uniform vec4 uColor;
void main() {
    float pulse = (sin(uTime * 3.0) * 0.5 + 0.5);
    gl_FragColor = uColor * pulse;
}