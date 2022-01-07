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
        TextureMaxFilter::Nearest,
        TextureWrap::Repeat,
        TextureWrap::Repeat,
    };
    Texture tex1("data/textures/container2.png", texOpts);
    Texture tex2("data/textures/container2_specular.png", texOpts);
    tex1.Bind(0);
    tex2.Bind(1);
    // objShader.SetUniform1i("u_Texture1", 0);
    // objShader.SetUniform1i("u_Texture2", 1);

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    // Set light uniforms for light cube shader
    lightShader.Bind();
    lightShader.SetUniform3f("u_LightColor", 1.0f, 1.0f, 1.0f);

    // Set point light uniforms
    objShader.Bind();
    objShader.SetUniform3f("u_PtLight.ambient", 0.2f, 0.2f, 0.2f);
    objShader.SetUniform3f("u_PtLight.diffuse", 0.5f, 0.5f, 0.5f);
    objShader.SetUniform3f("u_PtLight.specular", 1.0f, 1.0f, 1.0f);
    objShader.SetUniform3f("u_PtLight.position", lightPosition);
    objShader.SetUniform1f("u_PtLight.constant", 1.0f);
    objShader.SetUniform1f("u_PtLight.linear", 0.09f);
    objShader.SetUniform1f("u_PtLight.quadratic", 0.032f);
    // Set directional light uniforms
    objShader.SetUniform3f("u_DirLight.ambient", 0.05f, 0.05f, 0.05f);
    objShader.SetUniform3f("u_DirLight.diffuse", 0.15f, 0.15f, 0.15f);
    objShader.SetUniform3f("u_DirLight.specular", 0.25f, 0.25f, 0.25f);
    objShader.SetUniform3f("u_DirLight.direction", -0.2f, -1.0f, -0.3f);
    // Set spot light uniforms
    objShader.SetUniform3f("u_SpLight.ambient", 0.1f, 0.1f, 0.1f);
    objShader.SetUniform3f("u_SpLight.diffuse", 0.5f, 0.5f, 0.5f);
    objShader.SetUniform3f("u_SpLight.specular", 1.0f, 1.0f, 1.0f);
    objShader.SetUniform3f("u_SpLight.position", camera.GetPosition());
    objShader.SetUniform1f("u_SpLight.constant", 1.0f);
    objShader.SetUniform1f("u_SpLight.linear", 0.09f);
    objShader.SetUniform1f("u_SpLight.quadratic", 0.032f);
    objShader.SetUniform1f("u_SpLight.dropOff", cos(glm::radians(12.5f)));
    objShader.SetUniform1f("u_SpLight.cutOff", cos(glm::radians(17.5f)));
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

        // Calculate light color
        // glm::vec3 lightColor{sin(currentTime * 2.0f), sin(currentTime * 0.7f), sin(currentTime * 1.4f)};
        // glm::vec3 lightAmbient = lightColor * glm::vec3(0.2f);
        // glm::vec3 lightDiffuse = lightColor * glm::vec3(0.7f);

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
            // Drawing objects
            objShader.Bind();
            objShader.SetUniformMatrix4f("u_Projection", projection);
            objShader.SetUniformMatrix4f("u_View", view);
            objShader.SetUniform3f("u_ViewPos", camera.GetPosition());
            // Set light stuff
            objShader.SetUniform3f("u_PtLight.position", lightPosition);
            objShader.SetUniform3f("u_SpLight.position", camera.GetPosition());
            objShader.SetUniform3f("u_SpLight.direction", camera.GetFront());
            // objShader.SetUniform3f("u_PtLight.ambient", lightAmbient);
            // objShader.SetUniform3f("u_PtLight.diffuse", lightDiffuse);

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
