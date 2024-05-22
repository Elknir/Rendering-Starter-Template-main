#version 410

layout(location = 0) in vec2 in_position ;
uniform vec2 displacement = vec2(0,0);
uniform float aspect_ratio = 1;

void main()
{
    vec2 position = in_position + displacement;
    gl_Position = vec4((position.x / aspect_ratio), position.y, 0., 1.);
}