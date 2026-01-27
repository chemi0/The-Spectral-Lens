#pragma once
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

// Assimp Includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Mesh {
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};

class Model {
public:
    Model();
    ~Model();

    // Main function to load the file
    bool loadModel(const std::string& filepath);

    bool loadTexture(const std::string& filepath);
    unsigned int textureID;

    // Texture Slots
    unsigned int diffuseTex; // Slot 0 
    unsigned int emissionTex; // Slot 1
    unsigned int roughnessTex; // ..
    unsigned int aoTex;
    unsigned int opacityTex;

    // Draws the model
    void render();

private:
    std::vector<Mesh> meshes; // A model is made of multiple meshes

    // Helper functions for Assimp's recursive structure
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};