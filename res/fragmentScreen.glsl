#version 410
uniform sampler2D tex;
in vec2 uv;
out vec4 out_color;
void main()
{
    vec4 tex_color =  texture(tex, uv);
    float nb_color = (tex_color.x + tex_color.y + tex_color.z) / 3.f;
    out_color = vec4(vec3(nb_color), 1.f);
    out_color = tex_color;
}
