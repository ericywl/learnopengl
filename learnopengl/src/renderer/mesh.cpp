#include <renderer/mesh.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures)
    : m_Vertices(vertices), m_Indices(indices), m_Textures(textures) {
    m_VBO = new VertexBuffer(&vertices[0], (unsigned int)(vertices.size() * sizeof(Vertex)));
    m_IBO = new IndexBuffer(&indices[0], (unsigned int)indices.size());

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);

    m_VAO = new VertexArray;
    m_VAO->AddBuffer(*m_VBO, layout);
}

Mesh::~Mesh() {
    delete m_VBO;
    delete m_IBO;
    delete m_VAO;
}

void Mesh::SetupDraw(Shader& shader) const {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    unsigned int otherNr = 1;

    shader.Bind();
    for (unsigned int i = 0; i < m_Textures.size(); i++) {
        std::string name;
        switch (m_Textures[i]->GetType()) {
            case TextureType::Diffuse:
                name = "u_TextureDiffuse" + std::to_string(diffuseNr++);
                break;
            case TextureType::Specular:
                name = "u_TextureSpecular" + std::to_string(specularNr++);
                break;
            case TextureType::Normal:
                name = "u_TextureNormal" + std::to_string(normalNr++);
                break;
            case TextureType::Height:
                name = "u_TextureHeight" + std::to_string(heightNr++);
                break;
            default:
                name = "u_Texture" + std::to_string(otherNr++);
                break;
        }

        m_Textures[i]->Bind(i);
        shader.SetUniform1i(name, i);
    }
}
