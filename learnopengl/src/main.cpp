#include <core/input.h>
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
    Window display(800, 600, "LearnOpenGL");
    display.SetVSync(true);
    display.AddInputSystem(input, "kb");

    float vertices[] = {
        // positions        // colors         // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f,  // top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f   // top left
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

    Shader shader("data/shaders/basic.vert", "data/shaders/basic.frag");
    shader.Bind();
    // shader.SetUniform4f("u_Color", 1.0f, 0.5f, 0.2f, 1.0f);

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

    // Rendering loop
    while (!display.ShouldClose()) {
        if (input.IsKeyPressed(Key::Esc)) {
            display.Close();
        }

        renderer.Clear();
        renderer.Draw(va, ib);

        // Swap buffers and check events
        display.SwapBuffers();
        display.PollEvents();
    }

    return 0;
}