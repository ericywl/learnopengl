#include <common.h>
#include <core/application.h>
#include <core/input.h>
#include <core/time.h>
#include <renderer/camera.h>
#include <renderer/fbo.h>
#include <renderer/ibo.h>
#include <renderer/light.h>
#include <renderer/rbo.h>
#include <renderer/renderer.h>
#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/vao.h>
#include <renderer/vbo.h>
#include <scene/model.h>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#define DEBUG

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;
const double TARGET_FPS = 60.0;

void processCameraInputs(Camera& camera, float deltaTime) {
    // WASD to control camera movement
    CameraMovementBit moveBit = CameraMovementBit::None;
    if (Input::IsKeyPressed(Key::W)) {
        moveBit |= CameraMovementBit::Forward;
    }
    if (Input::IsKeyPressed(Key::S)) {
        moveBit |= CameraMovementBit::Backward;
    }
    if (Input::IsKeyPressed(Key::A)) {
        moveBit |= CameraMovementBit::Left;
    }
    if (Input::IsKeyPressed(Key::D)) {
        moveBit |= CameraMovementBit::Right;
    }
    if (Input::IsKeyPressed(Key::Space)) {
        moveBit |= Input::IsKeyPressed(Key::LCtrl) ? CameraMovementBit::Down : CameraMovementBit::Up;
    }
    camera.ProcessMovement(moveBit, deltaTime);

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
    if (Input::IsKeyJustPressed(Key::Enter, ModifierBit::Alt)) {
        window.ToggleFullScreen();
    }
}

int renderLightsAndCubes() {
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    Application app(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL");
    Window window = app.GetWindow();
    window.SetVSync(true);
    window.SetInputSystem(true);

    // Cube vertices with 3 position, 3 normal and 2 texture coordinates
    // clang-format off
        float objVertices[] = {
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
                 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
                 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
                 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
                 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
                 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
                 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
                 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
                 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        };
    // clang-format on

    unsigned int indices[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
                              18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

    VertexBuffer objVB(objVertices, (unsigned int)sizeof(objVertices));
    IndexBuffer objIB(indices, (unsigned int)std::size(indices));

    VertexBufferLayout layout;
    layout.Push<float>(3);  // positions
    layout.Push<float>(3);  // normals
    layout.Push<float>(2);  // texture coords

    VertexArray objVA;
    objVA.AddBuffer(objVB, layout);
    VertexArray lightVA;
    lightVA.AddBuffer(objVB, layout);

    // Object starting translations
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),     glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),   glm::vec3(1.5f, 2.0f, -2.5f),  glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };
    glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);

    // Initialize shader
    Shader objShader("data/shaders/phong.vert", "data/shaders/phong.frag");
    Shader lightShader("data/shaders/basic.vert", "data/shaders/light.frag");

    // Initialize and set texture in shader
    TextureOptions texOpts = {
        TextureMinFilter::Nearest,
        TextureMagFilter::Nearest,
        TextureWrap::Repeat,
        TextureWrap::Repeat,
    };
    Texture tex1("data/textures/container2.png", TextureType::Texture, texOpts);
    Texture tex2("data/textures/container2_specular.png", TextureType::Specular, texOpts);
    tex1.Bind(0);
    tex2.Bind(1);
    // objShader.SetUniform1i("u_Texture1", 0);
    // objShader.SetUniform1i("u_Texture2", 1);

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    // Lights
    DirectionalLight dirLight = {
        BasicLight{glm::vec3(1.0f, 1.0f, 1.0f), 0.05f, 0.15f, 0.25f},
        glm::vec3(-0.2f, -1.0f, -0.3f),
    };
    PointLight pointLights[] = {
        {
            BasicLight{glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, 0.5f, 1.0f},
            glm::vec3(1.2f, 1.0f, 2.0f),
            Attenuation(1.0f, 0.09f, 0.032f),
        },
    };
    SpotLight spotLights[] = {
        BasicLight{glm::vec3(1.0f, 1.0f, 1.0f), 0.1f, 0.6f, 0.3f},
        camera.GetPosition(),
        camera.GetFront(),
        Attenuation(1.0f, 0.09f, 0.032f),
        cos(glm::radians(12.5f)),
        cos(glm::radians(17.5f)),
    };

    // Set light color for light cube shader
    lightShader.Bind();
    lightShader.SetUniform3f("u_LightColor", pointLights[0].Inner.Color);

    // Set light uniforms
    objShader.Bind();
    Lighting::SetDirectionalLight(objShader, "u_DirLight", dirLight);
    Lighting::SetPointLights(objShader, "u_NumPtLights", 1, "u_PtLights", pointLights);
    Lighting::SetSpotLights(objShader, "u_NumSpLights", 1, "u_SpLights", spotLights);
    // Set object material (diffuse and specular are texture indices)
    objShader.SetUniform1f("u_Material.shininess", 32.0f);
    objShader.SetUniform1i("u_Material.diffuse", 0);
    objShader.SetUniform1i("u_Material.specular", 1);

    Renderer renderer;
    renderer.SetDepthTest(true);
    renderer.SetLineMode(false);
    renderer.SetBlending(false);

    // Framerate related
    double lastTimeF = Time::GetTime();
    int nbFrames = 0;

    // Rendering loop
    while (!window.ShouldClose()) {
        // Clear screen
        renderer.Clear();
        processWindowInputs(window);
        double currentTime = Time::GetTime();

        {
            // Framerate calculation
            nbFrames++;
            if (currentTime - lastTimeF >= 1.0) {
                spdlog::debug("{} ms/frame, {} fps", 1000.0 / double(nbFrames), nbFrames);
                nbFrames = 0;
                lastTimeF += 1.0;
            }
        }

        // Projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), aspectRatio, 0.1f, 100.0f);

        // View matrix (reverse direction of where camera moves)
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        processCameraInputs(camera, (float)deltaTime);
        glm::mat4 view = camera.ViewMatrix();

        {
            // Drawing light cube
            lightShader.Bind();
            lightShader.SetUniformMatrix4f("u_Projection", projection);
            lightShader.SetUniformMatrix4f("u_View", view);

            lightPosition.x = 1.0f + sin((float)currentTime) * 3.0f;
            lightPosition.y = sin((float)currentTime / 3.0f);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, lightPosition);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.SetUniformMatrix4f("u_Model", model);
            renderer.Draw(lightVA, objIB);
        }

        {
            // Set light stuff
            objShader.Bind();
            objShader.SetUniform3f("u_PtLights[0].position", lightPosition);
            objShader.SetUniform3f("u_SpLights[0].position", camera.GetPosition());
            objShader.SetUniform3f("u_SpLights[0].direction", camera.GetFront());

            // Drawing objects
            objShader.SetUniformMatrix4f("u_Projection", projection);
            objShader.SetUniformMatrix4f("u_View", view);
            objShader.SetUniform3f("u_ViewPos", camera.GetPosition());

            for (unsigned int i = 0; i < 10; i++) {
                // Model matrix
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angle = 20.0f * (i + 1);
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                objShader.SetUniformMatrix4f("u_Model", model);
                objShader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));

                // Draw elements
                renderer.Draw(objVA, objIB);
            }
        }

        // Swap buffers and check events
        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int renderBackpackModel() {
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    Application app(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL");
    Window window = app.GetWindow();
    window.SetVSync(true);
    window.SetInputSystem(true);

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    Renderer renderer;
    renderer.SetDepthTest(true);
    renderer.SetLineMode(false);
    renderer.SetBlending(false);

    // Framerate related
    double lastTimeF = Time::GetTime();
    int nbFrames = 0;

    Shader bpShader("data/shaders/basic.vert", "data/shaders/basic.frag");
    Model backpack("data/models/backpack/backpack.obj");

    // Rendering loop
    while (!window.ShouldClose()) {
        // Clear screen
        renderer.Clear();
        processWindowInputs(window);
        double currentTime = Time::GetTime();

        {
            // Framerate calculation
            nbFrames++;
            if (currentTime - lastTimeF >= 1.0) {
                spdlog::debug("{} ms/frame, {} fps", 1000.0 / double(nbFrames), nbFrames);
                nbFrames = 0;
                lastTimeF += 1.0;
            }
        }

        // Projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), aspectRatio, 0.1f, 100.0f);

        // View matrix (reverse direction of where camera moves)
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        processCameraInputs(camera, (float)deltaTime);
        glm::mat4 view = camera.ViewMatrix();

        // Model matrix
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f));
        model = glm::scale(model, glm::vec3(1.0f));

        // Draw backpack model
        bpShader.Bind();
        bpShader.SetUniformMatrix4f("u_Projection", projection);
        bpShader.SetUniformMatrix4f("u_View", view);
        bpShader.SetUniformMatrix4f("u_Model", model);
        renderer.Draw(backpack, bpShader);

        // Swap buffers and check events
        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int renderDepthTestScene() {
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    Application app(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL");
    Window window = app.GetWindow();
    window.SetVSync(true);
    window.SetInputSystem(true);

    // clang-format off
    float cubeVertices[] = {
        // positions          // texture Coords
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
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float planeVertices[] = {
        // positions          // texture coords
        // (note texture coords are higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
    };

    float transparentVertices[] = {
        // positions         // texture coords
        0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  0.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  0.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  1.0f
    };

    // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    float quadVertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    float smallerQuadVertices[] = {
        // positions   // texture coords
         0.3f,  1.0f,  1.0f, 1.0f,
         0.3f,  0.3f,  1.0f, 0.0f,
         1.0f,  0.3f,  0.0f, 0.0f,

         0.3f,  1.0f,  1.0f, 1.0f,
         1.0f,  0.3f,  0.0f, 0.0f,
         1.0f,  1.0f,  0.0f, 1.0f
    };
    // clang-format on

    Shader normalShader("data/shaders/depth.vert", "data/shaders/depth.frag");
    Shader singleColorShader("data/shaders/depth.vert", "data/shaders/single_color.frag");
    Shader screenShader("data/shaders/screen.vert", "data/shaders/screen.frag");

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);

    VertexBuffer cubeVBO(cubeVertices, (unsigned int)sizeof(cubeVertices));
    VertexArray cubeVAO;
    cubeVAO.AddBuffer(cubeVBO, layout);

    VertexBuffer planeVBO(planeVertices, (unsigned int)sizeof(planeVertices));
    VertexArray planeVAO;
    planeVAO.AddBuffer(planeVBO, layout);

    VertexBuffer windowVBO(transparentVertices, (unsigned int)sizeof(transparentVertices));
    VertexArray windowVAO;
    windowVAO.AddBuffer(windowVBO, layout);

    VertexBufferLayout screenLayout;
    screenLayout.Push<float>(2);
    screenLayout.Push<float>(2);

    VertexBuffer quadVBO(quadVertices, (unsigned int)sizeof(quadVertices));
    VertexArray quadVAO;
    quadVAO.AddBuffer(quadVBO, screenLayout);

    VertexBuffer smallerQuadVBO(smallerQuadVertices, (unsigned int)sizeof(smallerQuadVertices));
    VertexArray smallerQuadVAO;
    smallerQuadVAO.AddBuffer(smallerQuadVBO, screenLayout);

    std::vector<glm::vec3> windowPositions{
        glm::vec3(-1.5f, 0.0f, -0.48f), glm::vec3(1.5f, 0.0f, 0.51f), glm::vec3(0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),  glm::vec3(0.5f, 0.0f, -0.6f),
    };

    // Framebuffer related
    FrameBuffer fbo;
    Texture screenTex(nullptr, SCREEN_WIDTH, SCREEN_HEIGHT, 4, TextureType::Attachment,
                      TextureOptions(TextureMinFilter::Linear, TextureMagFilter::Linear, false));
    fbo.AddTextureAttachment(screenTex, 0);
    RenderBuffer rbo(RenderBufferType::Depth24Stencil8, SCREEN_WIDTH, SCREEN_HEIGHT);
    fbo.AddRenderBufferAttachment(AttachmentType::DepthStencil, rbo);
    if (!fbo.IsComplete()) {
        spdlog::warn("[FrameBuffer Warn] FrameBuffer incompelete");
    }
    screenShader.Bind();
    screenShader.SetUniform1i("u_ScreenTexture", 0);

    // Load all the textures
    Texture cubeTex("data/textures/container.jpg");
    Texture floorTex("data/textures/metal.png");
    Texture grassTex("data/textures/grass.png", TextureOptions(TextureWrap::ClampToEdge, TextureWrap::ClampToEdge));
    Texture windowTex("data/textures/blending_transparent_window.png",
                      TextureOptions(TextureWrap::ClampToEdge, TextureWrap::ClampToEdge));
    normalShader.Bind();
    normalShader.SetUniform1i("u_Texture1", 0);

    Renderer renderer;
    renderer.SetLineMode(false);
    // Blending
    renderer.SetBlending(true);
    // Depth testing
    renderer.SetDepthTest(true);
    renderer.SetDepthFunc(TestFunc::Less);
    // Stencil testing
    renderer.SetStencilTest(false);
    // If any test fail, we do nothing aka. keep the value
    // If both test succeeds, replace the stencil buffer value
    renderer.SetStencilAction(TestAction::Keep, TestAction::Keep, TestAction::Replace);
    // Face culling
    renderer.SetFaceCulling(false);
    renderer.SetCulledFace(CulledFace::Back);

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    // Frustum clipping planes
    const float nearPlane = 0.1f;
    const float farPlane = 100.0f;
    // normalShader.SetUniform1f("u_Near", nearPlane);
    // normalShader.SetUniform1f("u_Far", farPlane);

    // Rendering loop
    while (!window.ShouldClose()) {
        // Frame time calculation
        double currentTime = Time::GetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clear screen
        renderer.SetClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        renderer.Clear(ClearBit::Color | ClearBit::Depth);
        processWindowInputs(window);
        processCameraInputs(camera, (float)deltaTime);

        // Projection and view matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), aspectRatio, nearPlane, farPlane);
        glm::mat4 view = camera.ViewMatrix();
        normalShader.Bind();
        normalShader.SetUniformMatrix4f("u_View", view);
        normalShader.SetUniformMatrix4f("u_Projection", projection);

        {
            // Make sure we don't update stencil buffer while drawing floor
            // renderer.SetStencilMask(0x00);

            // Draw floor
            floorTex.Bind(0);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
            normalShader.SetUniformMatrix4f("u_Model", model);
            renderer.Draw(planeVAO, 6);
        }

        {
            // All fragments should pass the stencil test
            renderer.SetStencilFunc(TestFunc::Always, 1, 0xFF);
            // Enable writing to stencil buffer
            renderer.SetStencilMask(0xFF);

            // Draw cubes
            cubeTex.Bind(0);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
            normalShader.SetUniformMatrix4f("u_Model", model);
            renderer.Draw(cubeVAO, 36);

            model = glm::mat4(1.0f);
            model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
            normalShader.SetUniformMatrix4f("u_Model", model);
            renderer.Draw(cubeVAO, 36);
        }

        /*
        {
            // The above draw has written values into stencil buffer on the cube fragments
            renderer.SetStencilFunc(TestFunc::NotEqual, 1, 0xFF);
            // Disable writing to stencil buffer
            renderer.SetStencilMask(0x00);
            // Disable depth test so outline doesn't get clipped by anything
            renderer.SetDepthTest(false);

            // Use single color shader for drawing outline
            singleColorShader.Bind();
            singleColorShader.SetUniformMatrix4f("u_View", view);
            singleColorShader.SetUniformMatrix4f("u_Projection", projection);

            // Draw scaled-up cubes
            // The result would be scaled-up cubes minus the normal cubes (from stencil buffer) but in single color
            cubeTex.Bind(0);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
            model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
            singleColorShader.SetUniformMatrix4f("u_Model", model);
            renderer.Draw(cubeVAO, 36);

            model = glm::mat4(1.0f);
            model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
            singleColorShader.SetUniformMatrix4f("u_Model", model);
            renderer.Draw(cubeVAO, 36);

            // Reset depth and stencil options since we change it to draw outline only
            renderer.SetStencilMask(0xFF);
            renderer.SetStencilFunc(TestFunc::Always, 1, 0xFF);
            renderer.SetDepthTest(true);
            // Reset shader to the normal one for next rendering
            normalShader.Bind();
        }
        */

        {
            // Note: Always draw transparent objects after opaque objects
            // Sort windows based on distance from camera
            std::map<float, glm::vec3> sortedWPs;
            for (unsigned int i = 0; i < windowPositions.size(); i++) {
                float distance = glm::length(camera.GetPosition() - windowPositions[i]);
                sortedWPs[distance] = windowPositions[i];
            }

            // Draw window after all opaque objects and in reverse-order (furthest from camera first)
            windowTex.Bind(0);
            for (std::map<float, glm::vec3>::reverse_iterator it = sortedWPs.rbegin(); it != sortedWPs.rend(); ++it) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), it->second);
                normalShader.SetUniformMatrix4f("u_Model", model);
                renderer.Draw(windowVAO, 6);
            }
        }

        {
            fbo.Unbind();
            // Disable depth testing so that screen quad isn't discarded due to depth test
            renderer.SetDepthTest(false);
            // Disable blending so that screen quad doesn't get blended
            renderer.SetBlending(false);
            renderer.SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            renderer.Clear(ClearBit::Color);

            screenTex.Bind(0);
            screenShader.Bind();
            renderer.Draw(quadVAO, 6);
            renderer.Draw(smallerQuadVAO, 6);

            // Reset depth testing and blending since we disabled it to draw screen quad
            renderer.SetDepthTest(true);
            renderer.SetBlending(true);
            // Reset framebuffer so that we would draw the next scene onto it
            fbo.Bind();
        }

        // Swap buffers and check events
        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int renderSkyBox() {
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    Application app(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL");
    Window window = app.GetWindow();
    window.SetVSync(true);
    window.SetInputSystem(true);

    // clang-format off
    float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
    // clang-format on

    VertexBuffer skyboxVBO(skyboxVertices, (unsigned int)sizeof(skyboxVertices));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    VertexArray skyboxVAO;
    skyboxVAO.AddBuffer(skyboxVBO, layout);

    const std::string faces[] = {
        "data/textures/skybox/right.jpg",  "data/textures/skybox/left.jpg",  "data/textures/skybox/top.jpg",
        "data/textures/skybox/bottom.jpg", "data/textures/skybox/front.jpg", "data/textures/skybox/back.jpg",
    };
    CubeMap skyboxMap(faces,
                      TextureOptions(TextureMinFilter::Linear, TextureMagFilter::Linear, TextureWrap::ClampToEdge,
                                     TextureWrap::ClampToEdge, TextureWrap::ClampToEdge));
    Model backpack("data/models/backpack/backpack.obj");

    Shader bpShader("data/shaders/env_mapping.vert", "data/shaders/env_mapping.frag");
    bpShader.Bind();
    bpShader.SetUniform1i("u_Skybox", 0);
    Shader skyboxShader("data/shaders/skybox.vert", "data/shaders/skybox.frag");
    skyboxShader.Bind();
    skyboxShader.SetUniform1i("u_Cubemap", 0);

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    Renderer renderer;
    renderer.SetDepthTest(true);

    // Rendering loop
    while (!window.ShouldClose()) {
        // Frame-time related calculations
        double currentTime = Time::GetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clear screen
        renderer.Clear();
        processWindowInputs(window);
        processCameraInputs(camera, (float)deltaTime);

        // Projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), aspectRatio, 0.1f, 100.0f);
        {
            // View matrix
            glm::mat4 view = camera.ViewMatrix();

            // Model matrix
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(0.0f));
            model = glm::scale(model, glm::vec3(0.5f));

            // Draw backpack model
            bpShader.Bind();
            bpShader.SetUniformMatrix4f("u_Projection", projection);
            bpShader.SetUniformMatrix4f("u_View", view);
            bpShader.SetUniformMatrix4f("u_Model", model);
            bpShader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
            bpShader.SetUniform3f("u_CameraPos", camera.GetPosition());

            skyboxMap.Bind();
            renderer.Draw(backpack, bpShader);
        }

        {
            renderer.SetDepthFunc(TestFunc::LessEqual);
            // Remove translation so that skybox doesn't move around with camera
            glm::mat4 view = glm::mat4(glm::mat3(camera.ViewMatrix()));

            skyboxShader.Bind();
            skyboxShader.SetUniformMatrix4f("u_Projection", projection);
            skyboxShader.SetUniformMatrix4f("u_View", view);

            skyboxMap.Bind();
            renderer.Draw(skyboxVAO, 36);
            renderer.SetDepthFunc(TestFunc::Less);
        }

        // Swap buffers and check events
        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int main() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    return renderSkyBox();
}
