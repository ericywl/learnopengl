#include <common.h>
#include <core/input.h>
#include <core/time.h>
#include <core/window.h>
#include <renderer/camera.h>
#include <renderer/fbo.h>
#include <renderer/ibo.h>
#include <renderer/light.h>
#include <renderer/rbo.h>
#include <renderer/renderer.h>
#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/ubo.h>
#include <renderer/vao.h>
#include <renderer/vbo.h>
#include <scene/model.h>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>

#define DEBUG

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

struct VertexData {
    std::shared_ptr<VertexArray> va;
    std::shared_ptr<VertexBuffer> vb;
    unsigned int count;
};

VertexData initQuad() {
    // positions
    glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3(1.0f, -1.0f, 0.0f);
    glm::vec3 pos4(1.0f, 1.0f, 0.0f);
    // texture coordinates
    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);
    // normal vector
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    // calculate tangent / bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;

    // triangle 1
    // ----------
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    // triangle 2
    // ----------
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    // clang-format off
    float quadVertices[] = {
		// positions            // normal         // texcoords  // tangent                          // bitangent
		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
	};
    // clang-format on

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);
    layout.Push<float>(3);
    layout.Push<float>(3);

    std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(quadVertices, (unsigned int)sizeof(quadVertices));
    std::shared_ptr<VertexArray> va = std::make_shared<VertexArray>();
    va->AddBuffer(*vb, layout);

    return VertexData{va, vb, 6};
}

VertexData initCube() {
    // clang-format off
    float cubeVertices[] = {
        // back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};
    // clang-format on

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);

    std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(cubeVertices, (unsigned int)sizeof(cubeVertices));
    std::shared_ptr<VertexArray> va = std::make_shared<VertexArray>();
    va->AddBuffer(*vb, layout);

    return VertexData{va, vb, 36};
}

int testLightsAndCubes(Window& window) {
    float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();

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

int testBackpackModel(Window& window) {
    float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();

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

    Model backpack("data/models/backpack/backpack.obj");
    // Shader bpShader("data/shaders/basic.vert", "data/shaders/basic.frag");
    Shader bpShader("data/shaders/explode.vert", "data/shaders/basic.frag", "data/shaders/explode.geom");
    Shader normShader("data/shaders/normal_viz.vert", "data/shaders/normal_viz.frag", "data/shaders/normal_viz.geom");
    bpShader.Bind();

    // Rendering loop
    while (!window.ShouldClose()) {
        // Clear screen
        renderer.Clear();
        processWindowInputs(window);
        double currentTime = Time::GetTime();
        bpShader.SetUniform1f("u_Time", (float)currentTime);

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

        {
            // Draw backpack model
            bpShader.Bind();
            bpShader.SetUniformMatrix4f("u_Projection", projection);
            bpShader.SetUniformMatrix4f("u_View", view);
            bpShader.SetUniformMatrix4f("u_Model", model);
            renderer.Draw(backpack, bpShader);
        }

        /*
        {
            // Draw normals
            normShader.Bind();
            normShader.SetUniformMatrix4f("u_Projection", projection);
            normShader.SetUniformMatrix4f("u_View", view);
            normShader.SetUniformMatrix4f("u_Model", model);
            normShader.SetUniformMatrix4f("u_InvTViewModel", glm::inverseTranspose(view * model));
            renderer.Draw(backpack, normShader);
        }
        */

        // Swap buffers and check events
        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int testDepthTestScene(Window& window) {
    float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();

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
    Texture screenTex(window.GetWidth(), window.GetHeight(), 4, TextureType::TextureAttachment,
                      TextureOptions(TextureMinFilter::Linear, TextureMagFilter::Linear, false));
    fbo.AddColorAttachment(screenTex, 0);
    RenderBuffer rbo(RenderBufferType::Depth24Stencil8, window.GetWidth(), window.GetHeight());
    fbo.AddRenderBufferAttachment(AttachmentType::DepthStencil, rbo);
    if (!fbo.IsComplete()) {
        spdlog::warn("[FrameBuffer Warn] FrameBuffer incomplete");
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

int testSkyBox(Window& window) {
    float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();

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

int testUniformBuffer(Window& window) {
    float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();

    // clang-format off
    float cubeVertices[] = {
        // positions         
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f,
    };
    // clang-format on

    Shader shader("data/shaders/ubo_test.vert", "data/shaders/ubo_test.frag");
    shader.Bind();

    VertexBuffer cubeVBO(cubeVertices, (unsigned int)sizeof(cubeVertices));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    VertexArray cubeVAO;
    cubeVAO.AddBuffer(cubeVBO, layout);

    // Uniform buffers are shared across all shaders!
    UniformBuffer matricesUBO(3 * sizeof(glm::mat4));
    // This binding corresponds to the binding that is written in the shader (binding = 2)
    matricesUBO.BindRange(0, 3 * sizeof(glm::mat4), 2);

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    Renderer renderer;
    renderer.SetDepthTest(true);

    glm::vec3 positionsAndColors[] = {
        glm::vec3(-0.75f, 0.75f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),     glm::vec3(0.75f, 0.75f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),    glm::vec3(-0.75f, -0.75f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(0.75f, -0.75f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
    };

    while (!window.ShouldClose()) {
        // Frame-time related calculations
        double currentTime = Time::GetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clear screen
        renderer.Clear();
        processWindowInputs(window);
        processCameraInputs(camera, (float)deltaTime);

        {
            // Projection matrix
            glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), aspectRatio, 0.1f, 100.0f);
            matricesUBO.InsertData(0, glm::value_ptr(projection), sizeof(glm::mat4));
            // View matrix
            glm::mat4 view = camera.ViewMatrix();
            matricesUBO.InsertData(sizeof(glm::mat4), glm::value_ptr(view), sizeof(glm::mat4));
        }

        {
            // Draw cubes
            for (unsigned int i = 0; i < 8; i += 2) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), positionsAndColors[i]);
                matricesUBO.InsertData(2 * sizeof(glm::mat4), glm::value_ptr(model), sizeof(glm::mat4));
                shader.SetUniform4f("u_Color", glm::vec4(positionsAndColors[i + 1], 1.0f));
                renderer.Draw(cubeVAO, 36);
            }
        }

        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int testInstancedQuads(Window& window) {
    float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();

    // clang-format off
    float quadVertices[] = {
		// positions     // colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};
    // clang-format on

    glm::vec2 translations[100]{};
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2) {
        for (int x = -10; x < 10; x += 2) {
            glm::vec2 translation{};
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    VertexArray vao;
    VertexBuffer quadVBO(quadVertices, (unsigned int)sizeof(quadVertices));
    VertexBufferLayout quadLayout;
    quadLayout.Push<float>(2);
    quadLayout.Push<float>(3);
    vao.AddBuffer(quadVBO, quadLayout);

    VertexBuffer instanceVBO(translations, (unsigned int)sizeof(translations));
    VertexBufferLayout instanceLayout;
    instanceLayout.Push<float>(2);
    vao.AddBuffer(instanceVBO, instanceLayout, true);

    Renderer renderer;
    Shader shader("data/shaders/instanced.vert", "data/shaders/instanced.frag");
    shader.Bind();

    while (!window.ShouldClose()) {
        renderer.Clear();
        processWindowInputs(window);

        renderer.DrawInstanced(vao, 6, 100);

        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int testInstancedAsteroids(Window& window) {
    float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();

    unsigned int amount = 100000;
    glm::mat4* modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime());  // initialize random seed
    float radius = 150.0;
    float offset = 25.0f;
    for (unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);

        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;  // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // Model
    Model planet("data/models/planet/planet.obj");
    Model asteroid("data/models/asteroid/rock.obj");

    VertexBuffer instanceVBO(modelMatrices, amount * sizeof(glm::mat4));
    VertexBufferLayout instanceLayout;
    for (unsigned int i = 0; i < 4; i++) {
        instanceLayout.Push<float>(4);
    }
    asteroid.AddInstancedBuffer(instanceVBO, instanceLayout);

    // Camera
    Camera camera(glm::vec3(0.0f, 10.0f, 155.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    // Shader
    Shader planetShader("data/shaders/basic.vert", "data/shaders/basic.frag");
    Shader asteroidShader("data/shaders/asteroid.vert", "data/shaders/basic.frag");

    Renderer renderer;
    renderer.SetDepthTest(true);

    // Framerate related
    double lastTimeF = Time::GetTime();
    int nbFrames = 0;

    while (!window.ShouldClose()) {
        double currentTime = Time::GetTime();
        // View matrix (reverse direction of where camera moves)
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        renderer.Clear();
        processWindowInputs(window);
        processCameraInputs(camera, (float)deltaTime);

        {
            // Framerate calculation
            nbFrames++;
            if (currentTime - lastTimeF >= 1.0) {
                spdlog::debug("{} ms/frame, {} fps", 1000.0 / double(nbFrames), nbFrames);
                nbFrames = 0;
                lastTimeF += 1.0;
            }
        }

        // Projection and view matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), aspectRatio, 0.1f, 1000.0f);
        glm::mat4 view = camera.ViewMatrix();

        {
            // Draw planet
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
            model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));

            planetShader.Bind();
            planetShader.SetUniformMatrix4f("u_Projection", projection);
            planetShader.SetUniformMatrix4f("u_View", view);
            planetShader.SetUniformMatrix4f("u_Model", model);
            renderer.Draw(planet, planetShader);
        }

        {
            // Draw asteroids
            asteroidShader.Bind();
            asteroidShader.SetUniformMatrix4f("u_Projection", projection);
            asteroidShader.SetUniformMatrix4f("u_View", view);
            renderer.DrawInstanced(asteroid, asteroidShader, amount);
        }

        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int testBlinnPhongLight(Window& window) {
    float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();

    // clang-format off
    float planeVertices[] = {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // clang-format on

    VertexBuffer planeVBO(planeVertices, (unsigned int)sizeof(planeVertices));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);
    VertexArray planeVAO;
    planeVAO.AddBuffer(planeVBO, layout);

    TextureOptions texOpts = {
        TextureMinFilter::Nearest,
        TextureMagFilter::Nearest,
        TextureWrap::Repeat,
        TextureWrap::Repeat,
    };
    Texture floorTex("data/textures/wood.png");
    Texture simpleSpecTex("data/textures/white_specular.png");
    floorTex.Bind(0);
    simpleSpecTex.Bind(1);

    Shader floorShader("data/shaders/phong.vert", "data/shaders/phong.frag");
    floorShader.Bind();
    floorShader.SetUniform1i("u_Material.diffuse", 0);
    floorShader.SetUniform1i("u_Material.specular", 1);
    floorShader.SetUniform1f("u_Material.shininess", 1.0f);

    PointLight pointLights[] = {
        {
            BasicLight{glm::vec3(1.0f, 1.0f, 1.0f), 0.05f, 1.0f, 0.4f},
            glm::vec3(0.0f, 0.0f, 0.0f),
            Attenuation(1.0f, 0.0f, 0.0f),
        },
    };

    Lighting::SetPointLights(floorShader, "u_NumPtLights", 1, "u_PtLights", pointLights);

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    Renderer renderer;
    renderer.SetBlending(true);
    renderer.SetDepthTest(true);

    bool blinn = false;

    while (!window.ShouldClose()) {
        double currentTime = Time::GetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        renderer.Clear();
        processWindowInputs(window);
        processCameraInputs(camera, (float)deltaTime);
        if (Input::IsKeyJustPressed(Key::B)) {
            blinn = !blinn;
            spdlog::debug("Blinn-Phong {}", (blinn ? "enabled" : "disabled"));
        }

        // Projection and view matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), aspectRatio, 0.1f, 100.0f);
        glm::mat4 view = camera.ViewMatrix();
        glm::mat4 model = glm::mat4(1.0);

        floorShader.SetUniform1i("u_EnableBlinn", (int)blinn);
        floorShader.SetUniform3f("u_ViewPos", camera.GetPosition());
        floorShader.SetUniformMatrix4f("u_Projection", projection);
        floorShader.SetUniformMatrix4f("u_View", view);
        floorShader.SetUniformMatrix4f("u_Model", model);
        floorShader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));

        renderer.Draw(planeVAO, 6);

        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

void renderShadowMappingScene(Renderer& renderer, Shader& shader, VertexArray& planeVAO, VertexArray& cubeVAO) {
    // Draw floor
    shader.SetUniformMatrix4f("u_Model", glm::mat4(1.0f));
    renderer.Draw(planeVAO, 6);

    // Draw cubes
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.SetUniformMatrix4f("u_Model", model);
    shader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
    renderer.Draw(cubeVAO, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.SetUniformMatrix4f("u_Model", model);
    shader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
    renderer.Draw(cubeVAO, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.SetUniformMatrix4f("u_Model", model);
    shader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
    renderer.Draw(cubeVAO, 36);
}

int testShadowMapping(Window& window) {
    float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

    // Initialize object vertices
    // clang-format off
    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // clang-format on

    VertexBuffer planeVBO(planeVertices, (unsigned int)sizeof(planeVertices));
    VertexBufferLayout planeLayout;
    planeLayout.Push<float>(3);
    planeLayout.Push<float>(3);
    planeLayout.Push<float>(2);
    VertexArray planeVAO;
    planeVAO.AddBuffer(planeVBO, planeLayout);

    // clang-format off
    float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	};
    // clang-format on

    VertexBuffer quadVBO(quadVertices, (unsigned int)sizeof(quadVertices));
    VertexBufferLayout quadLayout;
    quadLayout.Push<float>(3);
    quadLayout.Push<float>(2);
    VertexArray quadVAO;
    quadVAO.AddBuffer(quadVBO, quadLayout);

    // clang-format off
    float cubeVertices[] = {
        // back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};
    // clang-format on

    VertexBuffer cubeVBO(cubeVertices, (unsigned int)sizeof(cubeVertices));
    VertexBufferLayout cubeLayout;
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(2);
    VertexArray cubeVAO;
    cubeVAO.AddBuffer(cubeVBO, cubeLayout);

    float nearPlane = 1.0f, farPlane = 7.5f;
    glm::vec3 lightPosition(-2.0f, 4.0f, -1.0f);

    Texture depthMap(SHADOW_WIDTH, SHADOW_HEIGHT, 0, TextureType::DepthAttachment,
                     TextureOptions(TextureMinFilter::Nearest, TextureMagFilter::Nearest, TextureWrap::ClampToBorder,
                                    TextureWrap::ClampToBorder, false, false, glm::vec4(1.0f)));
    FrameBuffer depthMapFBO;
    depthMapFBO.AddDepthAttachment(depthMap);
    depthMapFBO.SetReadAndDrawBuffer(BufferValue::None);
    depthMapFBO.Unbind();

    Texture woodTex("data/textures/wood.png");
    Shader depthShader("data/shaders/simple_depth.vert", "data/shaders/simple_depth.frag");
    Shader shadowShader("data/shaders/shadow.vert", "data/shaders/shadow.frag");
    shadowShader.Bind();
    shadowShader.SetUniform1i("u_TextureDiffuse", 0);
    shadowShader.SetUniform1i("u_ShadowMap", 1);
    Shader debugShader("data/shaders/debug_depth.vert", "data/shaders/debug_depth.frag");
    debugShader.Bind();
    debugShader.SetUniform1i("u_DepthMap", 0);

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    Renderer renderer;
    renderer.SetDepthTest(true);

    while (!window.ShouldClose()) {
        double currentTime = Time::GetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        renderer.Clear();
        processWindowInputs(window);
        processCameraInputs(camera, (float)deltaTime);

        // We are doing directional light shadow mapping, so there is no need for perspective deform
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
        glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        {
            depthShader.Bind();
            depthShader.SetUniformMatrix4f("u_LightSpaceMatrix", lightSpaceMatrix);

            // Render depth map from light's perspective
            window.SetViewport(SHADOW_WIDTH, SHADOW_HEIGHT);
            depthMap.Bind(0);
            depthMapFBO.Bind();
            renderer.Clear(ClearBit::Depth);
            renderShadowMappingScene(renderer, depthShader, planeVAO, cubeVAO);
            depthMapFBO.Unbind();

            // Reset viewport
            window.SetViewport(window.GetWidth(), window.GetHeight());
            renderer.Clear();
        }

        {
            glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), aspectRatio, 0.1f, 100.0f);
            glm::mat4 view = camera.ViewMatrix();

            woodTex.Bind(0);
            depthMap.Bind(1);
            shadowShader.Bind();
            shadowShader.SetUniformMatrix4f("u_Projection", projection);
            shadowShader.SetUniformMatrix4f("u_View", view);
            shadowShader.SetUniform3f("u_ViewPos", camera.GetPosition());
            shadowShader.SetUniform3f("u_LightPos", lightPosition);
            shadowShader.SetUniformMatrix4f("u_LightSpaceMatrix", lightSpaceMatrix);
            renderShadowMappingScene(renderer, shadowShader, planeVAO, cubeVAO);
        }

        /*
        {
            debugShader.Bind();
            debugShader.SetUniform1f("u_Near", nearPlane);
            debugShader.SetUniform1f("u_Far", farPlane);
            depthMap.Bind(0);
            renderer.Draw(quadVAO, 6);
        }
        */

        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

void renderOmniShadowMappingScene(Renderer& renderer, Shader& shader, VertexData& cubeData, bool invTModel = true) {
    // Render room
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));
    shader.Bind();
    shader.SetUniformMatrix4f("u_Model", model);
    if (invTModel) {
        shader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
    }
    // Disable face cull to render a cube room (looking at inside of cube)
    renderer.SetFaceCulling(false);
    shader.SetUniform1i("u_ReverseNormals", 1);
    renderer.Draw(*cubeData.va, cubeData.count);
    shader.SetUniform1i("u_ReverseNormals", 0);
    renderer.SetFaceCulling(true);

    // Render cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.SetUniformMatrix4f("u_Model", model);
    if (invTModel) {
        shader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
    }
    renderer.Draw(*cubeData.va, cubeData.count);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.SetUniformMatrix4f("u_Model", model);
    if (invTModel) {
        shader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
    }
    renderer.Draw(*cubeData.va, cubeData.count);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.SetUniformMatrix4f("u_Model", model);
    if (invTModel) {
        shader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
    }
    renderer.Draw(*cubeData.va, cubeData.count);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.SetUniformMatrix4f("u_Model", model);
    if (invTModel) {
        shader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
    }
    renderer.Draw(*cubeData.va, cubeData.count);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.SetUniformMatrix4f("u_Model", model);
    if (invTModel) {
        shader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
    }
    renderer.Draw(*cubeData.va, cubeData.count);
}

int testOmniShadowMapping(Window& window) {
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

    VertexData cubeData = initCube();

    CubeMap depthCubeMap(SHADOW_WIDTH, SHADOW_HEIGHT, TextureType::DepthAttachment,
                         TextureOptions(TextureMinFilter::Nearest, TextureMagFilter::Nearest, TextureWrap::ClampToEdge,
                                        TextureWrap::ClampToEdge, TextureWrap::ClampToEdge));
    FrameBuffer depthMapFBO;
    depthMapFBO.AddCubeDepthAttachment(depthCubeMap);
    depthMapFBO.SetReadAndDrawBuffer(BufferValue::None);
    if (!depthMapFBO.IsComplete()) {
        spdlog::warn("[FrameBuffer Warn] FrameBuffer incomplete");
    }
    depthMapFBO.Unbind();

    Texture woodTex("data/textures/wood.png");
    Shader shadowShader("data/shaders/omni_shadow.vert", "data/shaders/omni_shadow.frag");
    Shader depthShader("data/shaders/omni_simple_depth.vert", "data/shaders/omni_simple_depth.frag",
                       "data/shaders/omni_simple_depth.geom");
    Shader lightShader("data/shaders/basic.vert", "data/shaders/light.frag");

    lightShader.Bind();
    lightShader.SetUniform3f("u_LightColor", 1.0f, 1.0f, 1.0f);

    shadowShader.Bind();
    shadowShader.SetUniform1i("u_TextureDiffuse", 0);
    shadowShader.SetUniform1i("u_DepthMap", 1);

    glm::vec3 lightPosition(0.0f);
    float nearPlane = 1.0f;
    float farPlane = 25.0f;

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    Renderer renderer;
    renderer.SetDepthTest(true);
    renderer.SetFaceCulling(true);

    while (!window.ShouldClose()) {
        double currentTime = Time::GetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        renderer.Clear();
        processWindowInputs(window);
        processCameraInputs(camera, (float)deltaTime);
        lightPosition.z = static_cast<float>(sin(Time::GetTime() * 0.5) * 3.0);

        {
            // Render to shadow map
            glm::mat4 shadowProjection =
                glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlane, farPlane);
            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosition,
                                                                      lightPosition + glm::vec3(1.0f, 0.0f, 0.0f),
                                                                      glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosition,
                                                                      lightPosition + glm::vec3(-1.0f, 0.0f, 0.0f),
                                                                      glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosition,
                                                                      lightPosition + glm::vec3(0.0f, 1.0f, 0.0f),
                                                                      glm::vec3(0.0f, 0.0f, 1.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosition,
                                                                      lightPosition + glm::vec3(0.0f, -1.0f, 0.0f),
                                                                      glm::vec3(0.0f, 0.0f, -1.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosition,
                                                                      lightPosition + glm::vec3(0.0f, 0.0f, 1.0f),
                                                                      glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosition,
                                                                      lightPosition + glm::vec3(0.0f, 0.0f, -1.0f),
                                                                      glm::vec3(0.0f, -1.0f, 0.0f)));

            depthMapFBO.Bind();
            depthShader.Bind();
            depthShader.SetUniform1f("u_FarPlane", farPlane);
            depthShader.SetUniform3f("u_LightPos", lightPosition);
            for (unsigned int i = 0; i < 6; i++) {
                depthShader.SetUniformMatrix4f("u_ShadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
            }

            window.SetViewport(SHADOW_WIDTH, SHADOW_HEIGHT);
            renderer.Clear(ClearBit::Depth);

            renderOmniShadowMappingScene(renderer, depthShader, cubeData, false);
            depthMapFBO.Unbind();

            // Reset viewport
            window.SetViewport(window.GetWidth(), window.GetHeight());
            renderer.Clear();
        }

        {
            // Render normal scene
            glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()),
                                                    (float)window.GetWidth() / (float)window.GetHeight(), 0.1f, 100.0f);
            glm::mat4 view = camera.ViewMatrix();

            {
                // Render light cube for visual purpose
                glm::mat4 model = glm::translate(glm::mat4(1.0f), lightPosition);
                model = glm::scale(model, glm::vec3(0.05f));
                lightShader.Bind();
                lightShader.SetUniformMatrix4f("u_Projection", projection);
                lightShader.SetUniformMatrix4f("u_View", view);
                lightShader.SetUniformMatrix4f("u_Model", model);
                renderer.Draw(*cubeData.va, cubeData.count);
            }

            shadowShader.Bind();
            shadowShader.SetUniformMatrix4f("u_Projection", projection);
            shadowShader.SetUniformMatrix4f("u_View", view);
            shadowShader.SetUniform3f("u_ViewPos", camera.GetPosition());
            shadowShader.SetUniform3f("u_LightPos", lightPosition);
            shadowShader.SetUniform1f("u_FarPlane", farPlane);

            woodTex.Bind(0);
            depthCubeMap.Bind(1);
            renderOmniShadowMappingScene(renderer, shadowShader, cubeData);
        }

        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int testNormalMapping(Window& window) {
    VertexData quadData = initQuad();
    VertexData cubeData = initCube();

    Texture brickDiffuse("data/textures/brickwall.jpg",
                         TextureOptions{TextureWrap::ClampToEdge, TextureWrap::ClampToEdge});
    Texture brickNormal("data/textures/brickwall_normal.jpg",
                        TextureOptions{TextureWrap::ClampToEdge, TextureWrap::ClampToEdge});

    Shader objShader("data/shaders/normal_mapping.vert", "data/shaders/normal_mapping.frag");
    Shader lightShader("data/shaders/basic.vert", "data/shaders/light.frag");

    // Activate textures (not changing per frame)
    brickDiffuse.Bind(0);
    brickNormal.Bind(1);

    // Set shader uniforms
    lightShader.Bind();
    lightShader.SetUniform3f("u_LightColor", 1.0f, 1.0f, 1.0f);
    objShader.Bind();
    objShader.SetUniform1i("u_DiffuseMap", 0);
    objShader.SetUniform1i("u_NormalMap", 1);

    // Camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0;  // Time between current frame and last frame
    double lastTime = 0.0;   // Time of last frame

    glm::vec3 lightPos(0.5f, 1.5f, 0.3f);

    Renderer renderer;
    renderer.SetDepthTest(true);

    while (!window.ShouldClose()) {
        double currentTime = Time::GetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        renderer.Clear();
        processWindowInputs(window);
        processCameraInputs(camera, (float)deltaTime);

        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), window.GetAspectRatio(), 0.1f, 100.0f);
        glm::mat4 view = camera.ViewMatrix();

        {
            // Render the light cube
            glm::mat4 model = glm::translate(glm::mat4(1.0f), lightPos);
            model = glm::scale(model, glm::vec3(0.03f));

            lightShader.Bind();
            lightShader.SetUniformMatrix4f("u_Projection", projection);
            lightShader.SetUniformMatrix4f("u_View", view);
            lightShader.SetUniformMatrix4f("u_Model", model);

            renderer.Draw(*cubeData.va, cubeData.count);
        }

        {
            // Rotate the quad to show normal mapping from multiple directions
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians((float)Time::GetTime() * -10.0f),
                                          glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));

            objShader.Bind();
            objShader.SetUniformMatrix4f("u_Model", model);
            objShader.SetUniformMatrix4f("u_View", view);
            objShader.SetUniformMatrix4f("u_Projection", projection);
            objShader.SetUniformMatrix4f("u_InvTModel", glm::inverseTranspose(model));
            objShader.SetUniform3f("u_LightPos", lightPos);
            objShader.SetUniform3f("u_ViewPos", camera.GetPosition());

            renderer.Draw(*quadData.va, quadData.count);
        }

        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}

int main() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    const unsigned int SCREEN_WIDTH = 800;
    const unsigned int SCREEN_HEIGHT = 600;

    Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "Learn OpenGL");
    window.SetVSync(true);
    window.SetInputSystem(true);

    return testNormalMapping(window);
}