#include "../Public/Mesh.h"
#include "../Public/VertexArray.h"
#include "../Public/VertexBuffer.h"
#include "../Public/VertexBufferLayout.h"
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>


void Mesh::ProcessNode(aiNode* node, const aiScene* scene)
{
    // Process each mesh located at the current node
    for (uint i = 0; i < node->mNumMeshes; i++)
    {
        // The node object only contains indices to index the actual objects in the scene.
        // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[i];

        ProcessMesh(mesh);
    }

    // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (uint i = 0; i < node->mNumChildren; i++)
    {
        this->ProcessNode(node->mChildren[i], scene);
    }
}

void Mesh::ProcessMesh(aiMesh* mesh)
{
    std::vector<float> position_texture_normal;

    // Walk through each of the mesh's vertices
    for (uint i = 0; i < mesh->mNumVertices; i++)
    {
        position_texture_normal.push_back(mesh->mVertices[i].x);
        position_texture_normal.push_back(mesh->mVertices[i].y);
        position_texture_normal.push_back(mesh->mVertices[i].z);

        // Texture Coordinates
        if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
        {
            position_texture_normal.push_back(mesh->mTextureCoords[0][i].x);
            position_texture_normal.push_back(mesh->mTextureCoords[0][i].y);
        }
        else
        {
            position_texture_normal.push_back(0.0f);
            position_texture_normal.push_back(0.0f);
        }

        position_texture_normal.push_back(mesh->mNormals[i].x);
        position_texture_normal.push_back(mesh->mNormals[i].y);
        position_texture_normal.push_back(mesh->mNormals[i].z);
    }

    std::vector<uint> indicies;

    // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (uint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices < 3) {
            continue;
        }
        // Retrieve all indices of the face and store them in the indices vector
        for (uint j = 0; j < face.mNumIndices; j++)
        {
            indicies.push_back(face.mIndices[j]);
        }
    }

    m_VA = new VertexArray();
    m_VBL = new VertexBufferLayout();
    m_IB = new IndexBuffer(indicies.data(), indicies.size());
    m_VB = new VertexBuffer(position_texture_normal.data(), position_texture_normal.size() * sizeof(float));

    // positions
    m_VBL->Push<float>(3);
    // texture
    m_VBL->Push<float>(2);
    // normals
    m_VBL->Push<float>(3);

    m_VA->AddBuffer(*m_VB, *m_VBL);
}

void Mesh::LoadMesh(const std::string& path)
{
    // Read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    // Check for errors
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene);
}


Mesh::Mesh(const std::string& path)
{
	LoadMesh(path);
}

Mesh::~Mesh()
{
    UnBind();
    delete m_VB;
    delete m_VBL;
    delete m_IB;
    delete m_VA;
}

void Mesh::Bind() const
{
    m_VA->Bind();
    m_IB->Bind();
}

void Mesh::UnBind() const
{
    m_VA->UnBind();
    m_IB->UnBind();
}
