#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <opengl-framework/opengl-framework.hpp>

struct PointLight {

    glm::vec3 Position;
    float Intensity;

    PointLight() : Position(0, 0, 0), Intensity(100.0f) {}
    explicit PointLight(const glm::vec3& in_Position): Position(in_Position), Intensity(100.0f) {}
    explicit PointLight(const glm::vec3& in_Position, const float &in_Intensity) : Position(in_Position), Intensity(in_Intensity) {}
};

int main() {
    gl::init("OpenGL OBJ");
    gl::maximize_window();
    std::string obj_path = gl::make_absolute_path("res/obj/boat/fourareen.obj").string();

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(obj_path)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    std::vector<float> data{};

    //Shapes
    for (unsigned int s = 0; s < shapes.size(); s++) {
        //Faces
        unsigned int index_offset = 0;
        for (unsigned int f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            unsigned int fv = shapes[s].mesh.num_face_vertices[f];

            //Vertices in faces
            for (unsigned int v = 0; v < fv; v++) {
                // Vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                data.push_back(attrib.vertices[3*int(idx.vertex_index)+0]);
                data.push_back(attrib.vertices[3*int(idx.vertex_index)+2]);
                data.push_back(attrib.vertices[3*int(idx.vertex_index)+1]);

                // Normal
                if (idx.normal_index >= 0) {
                    glm::vec3 normals_normalized = glm::normalize(glm::vec3{
                        attrib.normals[3*int(idx.normal_index)+0],
                        attrib.normals[3*int(idx.normal_index)+2],
                        attrib.normals[3*int(idx.normal_index)+2]
                    });
                    data.push_back(normals_normalized.x);
                    data.push_back(normals_normalized.y);
                    data.push_back(normals_normalized.z);
                }

                // Texcoord 
                if (idx.texcoord_index >= 0) {
                    data.push_back(attrib.texcoords[2*int(idx.texcoord_index)+0]);
                    data.push_back(attrib.texcoords[2*int(idx.texcoord_index)+1]);
                }

            }
            index_offset += fv;

        }
    }

    auto const boat_mesh = gl::Mesh{
            {
                .vertex_buffers = {
                    {
                        .layout = {
                            gl::VertexAttribute::Position3D{0},
                            gl::VertexAttribute::Normal3D{1},
                            gl::VertexAttribute::UV{2}},
                            // gl::VertexAttribute::ColorRGB{3}},
                        .data = data,
                    }}
            }};

    //Render Target
    auto render_target = gl::RenderTarget{gl::RenderTarget_Descriptor{
        .width          = gl::framebuffer_width_in_pixels(),
        .height         = gl::framebuffer_height_in_pixels(),
        .color_textures = {
            gl::ColorAttachment_Descriptor{
                .format  = gl::InternalFormat_Color::RGBA8,
                .options = {
                    .minification_filter  = gl::Filter::NearestNeighbour, 
                    .magnification_filter = gl::Filter::NearestNeighbour,
                    .wrap_x               = gl::Wrap::ClampToEdge,
                    .wrap_y               = gl::Wrap::ClampToEdge,
                },
            },
        },
        .depth_stencil_texture = gl::DepthStencilAttachment_Descriptor{
            .format  = gl::InternalFormat_DepthStencil::Depth32F,
            .options = {
                .minification_filter  = gl::Filter::NearestNeighbour,
                .magnification_filter = gl::Filter::NearestNeighbour,
                .wrap_x               = gl::Wrap::ClampToEdge,
                .wrap_y               = gl::Wrap::ClampToEdge,
            },
        },
    }};

    auto const texture = gl::Texture{
        gl::TextureSource::File{ 
            .path           = "res/obj/boat/fourareen2K_albedo.jpg",
            .flip_y         = true, 
            .texture_format = gl::InternalFormat::RGBA8,
        },
        gl::TextureOptions{
            .minification_filter  = gl::Filter::Linear, 
            .magnification_filter = gl::Filter::Linear, 
            .wrap_x               = gl::Wrap::Repeat, 
            .wrap_y               = gl::Wrap::Repeat,
        }
    };

    //Camera
    auto camera = gl::Camera{};
    gl::set_events_callbacks({
        camera.events_callbacks(),
        {
            .on_mouse_pressed = [&](gl::MousePressedEvent const& e) {
                std::cout << "Mouse pressed at " << e.position.x << " " << e.position.y << '\n';
            },
            .on_framebuffer_resized = [&](gl::FramebufferResizedEvent const& e) {
                render_target.resize(e.width_in_pixels, e.height_in_pixels);
            },
        },
    });
    

    auto const screen_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {
                gl::VertexAttribute::Position2D{0 },
                gl::VertexAttribute::UV{1 }
            },
            .data   = {
                -1.f, 1.f, 0.f, 1.f, // Top left
                1.f, 1.f, 1.f, 1.f, // Top right
                1.f, -1.f, 1.f, 0.f, // Bottom right
                -1.f, -1.f, 0.f, 0.f, // Bottom left
            },
        }},
        .index_buffer = {
            0, 1, 2,
            0, 2, 3
        }
    }};


    auto const obj_shader = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/vertexOBJ.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragmentOBJ.glsl"},
    }};

    auto const screen_shader = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/vertexScreen.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragmentScreen.glsl"},
    }};
    
    glEnable(GL_DEPTH_TEST);

    PointLight point_light;
    point_light.Intensity = 5000.0f;

    // Update function
    while (gl::window_is_open()) {
        glm::mat4 const view_matrix = camera.view_matrix();

        glm::mat4 const projection_matrix = glm::infinitePerspective(
            glm::radians(90.f),
            gl::framebuffer_aspect_ratio(),
            0.0001f 
        );
        glm::mat4 const ortho_matrix = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);

        glm::mat4 const translation_matrix = glm::translate(
            glm::mat4{1.f},
            glm::vec3{-2.f, 0.f, -1.f}
        );
        glm::mat4 const rotation_matrix = glm::rotate(
            glm::mat4{1.f},
            0.f,
            glm::vec3{0.f, 0.f, 1.f}
        );
        glm::mat4 const model_matrix = rotation_matrix * translation_matrix;


        glm::mat4 const view_projection_matrix =  projection_matrix * view_matrix * model_matrix;

        render_target.render([&] {
            point_light.Position = glm::vec3{cos(gl::time_in_seconds()), sin(gl::time_in_seconds()), 0.};
            point_light.Position *= 100.0f;


            obj_shader.bind();
            obj_shader.set_uniform("view_projection_matrix", view_projection_matrix);
            obj_shader.set_uniform("tex", texture);
            obj_shader.set_uniform("global_light", glm::normalize(glm::vec3{0.5, -1., -1.}));
            obj_shader.set_uniform("global_light_intensity", 0.5f);
            obj_shader.set_uniform("point_light", point_light.Position);
            obj_shader.set_uniform("point_light_intensity", point_light.Intensity);
            obj_shader.set_uniform("global_illumination", 0.15f);

            glClearColor(0.f, 0.f, 1.f, 1.f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            boat_mesh.draw();
        });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        screen_shader.bind();
        screen_shader.set_uniform("tex", render_target.color_texture(0));
        screen_mesh.draw();
    }
}
