#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include <cmath>
#include <iostream>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

int main() {
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

//    auto const triangle_mesh = gl::Mesh{{
//        .vertex_buffers = {{
//            .layout = {gl::VertexAttribute::Position2D {
//                0 /*Index de l'attribut dans le shader, on en reparle juste après*/}},
//                .data   = {-1.f,-1.f, // Position2D du 1er sommet
//                           1.f,-1.f, // Position2D du 2ème sommet
//                           0.f, 1.f  // Position2D du 3ème sommet
//                           },}},}};

    auto const rectangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
            
            .layout = {gl::VertexAttribute::Position3D{0}, gl::VertexAttribute::UV{1}},
            .data   = {
                    -0.5f, -0.5f,-0.5f, 0, 0, // Position2D du sommet 0
                    0.5f, -0.5f,-0.5f, 1, 0, // Position2D du sommet 1
                    0.5f, 0.5f,-0.5f, 1, 1, // Position2D du sommet 2
                    -0.5f, 0.5f, -0.5f, 0, 1, // Position2D du sommet 3

                    -0.5f, -0.5f,0.5f,  0, 0,// Position2D du sommet 4
                    0.5f, -0.5f,0.5f,  1, 0,// Position2D du sommet 5
                    0.5f, 0.5f,0.5f, 1, 1, // Position2D du sommet 6
                    -0.5f, 0.5f,0.5f, 0, 1,// Position2D du sommet 7
                    },
                    }},
                    .index_buffer   = {
                0, 1, 2, 
                0, 2, 3, // 1ere face 
                0, 1, 5,
                0, 4, 5, // 2eme face
                1, 2, 5,
                2, 5, 6, // 3eme face
                2, 3, 6,
                3, 6, 7, // 4eme face
                3, 4, 7,
                0, 3, 4, // 5eme face
                4, 5, 6,
                4, 6, 7, // 6eme face
                },
                }
                
    };

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
        }};

    auto camera = gl::Camera{};
    gl::set_events_callbacks({
        camera.events_callbacks(),
        {
                .on_mouse_pressed = [&](gl::MousePressedEvent const& e) {
                 std::cout << "Mouse pressed at " << e.position.x << " " << e.position.y << '\n';
                },
        },
    });

    glm::mat4 const projection_matrix =
            glm::infinitePerspective(2.f /*field of view in radians*/,
                                     gl::framebuffer_aspect_ratio() /*aspect ratio*/,
                                     0.001f /*near plane*/);

    glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds() /*angle de la rotation*/, glm::vec3{1.f, 1.f, 1.f} /* axe autour duquel on tourne */);
    glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f} /* déplacement */);
    glm::mat4 const rotationTranslation =  translation * rotation ;

    auto const texture = gl::Texture{
            gl::TextureSource::File{ // Peut être un fichier, ou directement un tableau de pixels
                    .path           = "res/texture.png",
                    .flip_y         = true, // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
                    .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
            },
            gl::TextureOptions{
                    .minification_filter  = gl::Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
                    .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
                    .wrap_x               = gl::Wrap::Repeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
                    .wrap_y               = gl::Wrap::Repeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
            }
    };


    glEnable(GL_DEPTH_TEST);
    // Rendu à chaque frame
    while (gl::window_is_open()) {
        glClearColor(0.2, 0.2, 0.2,1.f); // Couleur de fond
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Vient remplacer glClear(GL_COLOR_BUFFER_BIT); 
        shader.bind();; // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw().
        //shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        //shader.set_uniform("displacement", glm::vec2{cos(gl::time_in_seconds()), sin(gl::time_in_seconds())});
        glm::mat4 const view_matrix = camera.view_matrix();
        shader.set_uniform("view_projection_matrix", projection_matrix * view_matrix ); // * rotationTranslation
        shader.set_uniform("my_texture", texture);
        rectangle_mesh.draw();
    }
}