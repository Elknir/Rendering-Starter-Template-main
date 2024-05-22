#version 410

layout(location = 0) in vec3 in_position ;
uniform vec3 displacement = vec3(0,0,0);
uniform float aspect_ratio = 1;
uniform mat4 view_projection_matrix;
out vec3 vertex_position;

layout(location = 1) in vec2 in_uv;
out vec2 uv;

void main()
{
    vertex_position = in_position;
    uv = in_uv;
    
    vec3 position = in_position + displacement;
    gl_Position = view_projection_matrix * vec4((position.x ), position.y, position.z, 1.);
}