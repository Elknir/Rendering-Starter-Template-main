#version 410
layout(location = 0) in vec3 in_position ;
out vec2 uv;
layout(location = 1) in vec2 in_uv;

void main() {
    uv = in_uv;
    gl_Position = vec4((in_position.x ), in_position.y, in_position.z, 1.);
}
