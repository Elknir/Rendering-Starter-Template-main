#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include <cmath>

int main() {
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

    auto const triangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{
                0 /*Index de l'attribut dans le shader, on en reparle juste après*/}},
                .data   = {-1.f,-1.f, // Position2D du 1er sommet
                           +1.f,-1.f, // Position2D du 2ème sommet
                           0.f, +1.f  // Position2D du 3ème sommet
                           },}},}};

    auto const rectangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0}},
            .data   = {
                    -0.5f, -0.5f, // Position2D du 1er sommet
                    +0.5f, -0.5f, // Position2D du 2ème sommet
                    +0.5f, +0.5f, // Position2D du 3ème sommet
                    -0.5f, +0.5f  // Position2D du 4ème sommet
                    },
                    }},
                    .index_buffer   = {
                0, 1, 2, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
                0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
                },
                }};

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
        }};

   
    
    while (gl::window_is_open()) {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
        // Rendu à chaque frame
        glClearColor(0.f, 0.f, 1.f,1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
//        glClear(GL_COLOR_BUFFER_BIT);
        
        shader.bind();; // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
        shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        shader.set_uniform("displacement", glm::vec2{cos(gl::time_in_seconds()), sin(gl::time_in_seconds())});
        rectangle_mesh.draw();
    }
}