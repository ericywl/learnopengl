#include <core/input.h>
#include <core/time.h>
#include <core/window.h>
#include <renderer/ibo.h>
#include <renderer/renderer.h>
#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/vao.h>
#include <renderer/vbo.h>

#include <iterator>

int main() {
    InputSystem input(std::vector<Key>{Key::Esc});
    Window window(800, 600, "LearnOpenGL");
    window.SetVSync(true);
    window.AddInputSystem(input, "kb");

    float vertices[] = {
        // positions        // colors         // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };

    VertexBuffer vb(vertices, (unsigned int)sizeof(vertices));
    IndexBuffer ib(indices, (unsigned int)std::size(indices));

    VertexBufferLayout layout;
    layout.Push<float>(3);  // positions
    layout.Push<float>(3);  // colors
    layout.Push<float>(2);  // texture coords

    VertexArray va;
    va.AddBuffer(vb, layout);

    // Initialize shader
    Shader shader("data/shaders/basic.vert", "data/shaders/basic.frag");
    shader.Bind();

    // Initialize and set texture in shader
    TextureOptions texOpts = {
        TextureMinFilter::Nearest,
        TextureMaxFilter::Nearest,
        TextureWrap::Repeat,
        TextureWrap::Repeat,
    };
    Texture tex1("data/textures/awesomeface.png", texOpts);
    Texture tex2("data/textures/container.jpg", texOpts);
    tex1.Bind(0);
    tex2.Bind(1);
    shader.SetUniform1i("u_Texture1", 0);
    shader.SetUniform1i("u_Texture2", 1);

    Renderer renderer;
    renderer.SetLineMode(false);
    renderer.SetBlending(false);
    renderer.SetClearColor(Color{0.2f, 0.3f, 0.6f, 1.0f});

    // Rendering loop
    while (!window.ShouldClose()) {
        // Clear screen
        renderer.Clear();
        if (input.IsKeyPressed(Key::Esc)) {
            window.Close();
        }

        // Model matrix
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, (float)Time::GetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        shader.SetUniformMatrix4fv("u_Model", glm::value_ptr(trans));

        // Draw elements
        renderer.Draw(va, ib);

        // Set new model matrix
        trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(glm::sin(Time::GetTime())));
        shader.SetUniformMatrix4fv("u_Model", glm::value_ptr(trans));

        // Draw same elements with different model matrix
        renderer.Draw(va, ib);

        // Swap buffers and check events
        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}