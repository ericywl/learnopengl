#include <core/application.h>
#include <core/input.h>
#include <core/time.h>
#include <renderer/camera.h>
#include <renderer/ibo.h>
#include <renderer/renderer.h>
#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/vao.h>
#include <renderer/vbo.h>

#include <iterator>

#define DEBUG

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;
const double TARGET_FPS = 60.0;

void processCameraInputs(Camera& camera, float deltaTime) {
    // WASD to control camera movement
    if (Input::IsKeyPressed(Key::W)) {
        camera.ProcessMovement(CameraMovement::Forward, deltaTime);
    }
    if (Input::IsKeyPressed(Key::S)) {
        camera.ProcessMovement(CameraMovement::Backward, deltaTime);
    }
    if (Input::IsKeyPressed(Key::A)) {
        camera.ProcessMovement(CameraMovement::Left, deltaTime);
    }
    if (Input::IsKeyPressed(Key::D)) {
        camera.ProcessMovement(CameraMovement::Right, deltaTime);
    }

    // Y-axis scroll to control camera zoom
    camera.ProcessZoom(Input::GetMouseScrollDelta().second);

    // Mouse movement to control camera rotation
    if (Input::IsMouseButtonPressed(MouseButton::Left)) {
        std::pair<float, float> mouseOffset = Input::GetMousePositionDelta();
        camera.ProcessRotation(-mouseOffset.first, -mouseOffset.second);
    }
}

void processWindowInputs(Window& window) {
    // Check for window close triggers
    if (Input::IsKeyPressed(Key::Esc)) {
        window.Close();
    }

    // Fullscreen toggle
    if (Input::IsKeyJustPressed(Key::Enter, ModifierBits::Alt)) {
        window.ToggleFullScreen();
    }
}

int main() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    Application app(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL");
    Window window = app.GetWindow();
    window.SetVSync(true);
    window.SetInputSystem(true);

    // Cube vertices with 3 position and 2 texture coordinates
    // clang-format off
    float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	};
    // clang-format on

    unsigned int indices[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
                              18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

    VertexBuffer vb(vertices, (unsigned int)sizeof(vertices));
    IndexBuffer ib(indices, (unsigned int)std::size(indices));

    VertexBufferLayout layout;
    layout.Push<float>(3);  // positions
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
    renderer.SetDepthTest(true);
    renderer.SetLineMode(false);
    renderer.SetBlending(false);
    renderer.SetClearColor(Color{0.2f, 0.3f, 0.6f, 1.0f});

    glm::vec3 cubeTranslations[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),     glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),   glm::vec3(1.5f, 2.0f, -2.5f),  glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame
    double lastTimeF = Time::GetTime();
    int nbFrames = 0;

    // Rendering loop
    while (!window.ShouldClose()) {
        // Clear screen
        renderer.Clear();
        processWindowInputs(window);

        // Framerate calculation
        double currentTime = Time::GetTime();
        nbFrames++;
        if (currentTime - lastTimeF >= 1.0) {
            spdlog::debug("{} ms/frame, {} fps", 1000.0 / double(nbFrames), nbFrames);
            nbFrames = 0;
            lastTimeF += 1.0;
        }

        // Projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), aspectRatio, 0.1f, 100.0f);
        shader.SetUniformMatrix4fv("u_Projection", glm::value_ptr(projection));

        // View matrix (reverse direction of where camera moves)
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        processCameraInputs(camera, (float)deltaTime);
        glm::mat4 view = camera.ViewMatrix();
        shader.SetUniformMatrix4fv("u_View", glm::value_ptr(view));

        for (unsigned int i = 0; i < 10; i++) {
            // Model matrix
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubeTranslations[i]);
            float angle = 20.0f * (i + 1);
            model = glm::rotate(model, (float)Time::GetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.SetUniformMatrix4fv("u_Model", glm::value_ptr(model));

            // Draw elements
            renderer.Draw(va, ib);
        }

        // Swap buffers and check events
        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}
