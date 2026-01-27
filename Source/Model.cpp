#include "../Header/Model.h"
#include "../Header/stb_image.h"
#include <iostream>

Model::Model() : diffuseTex(0), emissionTex(0), roughnessTex(0), aoTex(0), opacityTex(0) {}

Model::~Model() {
    for (auto& mesh : meshes) {
        if (mesh.VAO) glDeleteVertexArrays(1, &mesh.VAO);
        if (mesh.VBO) glDeleteBuffers(1, &mesh.VBO);
        if (mesh.EBO) glDeleteBuffers(1, &mesh.EBO);
    }
}

bool Model::loadModel(const std::string& filepath) {
    Assimp::Importer importer;

    // Read file with flags:
    // aiProcess_Triangulate: Convert all faces (quads) to triangles
    // aiProcess_FlipUVs: Flip texture coordinates for OpenGL
    // aiProcess_GenNormals: Create normals if they are missing
    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    // Check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }

    // Start processing from the root node
    processNode(scene->mRootNode, scene);

    std::cout << "Model Loaded Successfully: " << filepath << std::endl;
    return true;
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // Process all meshes in the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // Recursively process children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Process Vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // Position (X, Y, Z)
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        // Texture Coordinates (U, V)
        if (mesh->mTextureCoords[0]) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }

        // Normals (NX, NY, NZ)
        if (mesh->HasNormals()) {
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        }
        else {
            vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        }

        // Tangents (FOR NORMAL TEXTURE, to make the light bounce off realistically)
        if (mesh->HasTangentsAndBitangents()) {
            vertices.push_back(mesh->mTangents[i].x);
            vertices.push_back(mesh->mTangents[i].y);
            vertices.push_back(mesh->mTangents[i].z);
        }
        else {
            vertices.push_back(1.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
        }

    }

    // Process Indices (Faces)
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Create Buffers
    Mesh newMesh;
    newMesh.indexCount = indices.size();

    glGenVertexArrays(1, &newMesh.VAO);
    glGenBuffers(1, &newMesh.VBO);
    glGenBuffers(1, &newMesh.EBO);

    glBindVertexArray(newMesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, newMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Layout 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Layout 1: UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Layout 2: Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Layout 3: Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    return newMesh;
}

void Model::render() {

    // Slot 0: Diffuse
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTex);

    // Slot 1: Emission
    glActiveTexture(GL_TEXTURE1);
    if (emissionTex) glBindTexture(GL_TEXTURE_2D, emissionTex);
    else glBindTexture(GL_TEXTURE_2D, 0);

    // Slot 2: Roughness
    glActiveTexture(GL_TEXTURE2);
    if (roughnessTex) glBindTexture(GL_TEXTURE_2D, roughnessTex);
    else glBindTexture(GL_TEXTURE_2D, 0);

    // Slot 3: Ambient Occlusion
    glActiveTexture(GL_TEXTURE3);
    if (aoTex) glBindTexture(GL_TEXTURE_2D, aoTex);
    else glBindTexture(GL_TEXTURE_2D, 0);

    // Slot 4: Opacity
    glActiveTexture(GL_TEXTURE4);
    if (opacityTex) glBindTexture(GL_TEXTURE_2D, opacityTex);
    else glBindTexture(GL_TEXTURE_2D, 0);

    // Draw Mesh
    for (auto& mesh : meshes) {
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Reset/Unbind
    glActiveTexture(GL_TEXTURE0);
}

unsigned int Model::loadTexture(const std::string& filepath) {
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    // Wrapping/Filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image data
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip vertically (because OpenGL!!!)
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4) 
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Texture Loaded: " << filepath << std::endl;
        stbi_image_free(data);
        
        return id;
    }
    else {
        std::cout << "Failed to load texture: " << filepath << std::endl;
        stbi_image_free(data);
        return 0;
    }
}