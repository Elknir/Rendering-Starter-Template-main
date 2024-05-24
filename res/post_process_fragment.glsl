#version 410

out vec4 out_color;
uniform sampler2D image_rendered;
in vec2 uv;


void main() {
    vec4 texture_color = texture(image_rendered, uv);
    out_color = vec4(texture_color.x, 0.1, 0.1, texture_color.w);
}
