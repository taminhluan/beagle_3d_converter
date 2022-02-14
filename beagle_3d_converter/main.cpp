#pragma warning(disable  : 4996)

#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <fstream>
// for cout << setPrecision(3);
#include <iomanip>
#include <format>

#include <chrono>
#include <ctime> 

bool isVerbose = false;
std::string inputPath;
std::string outputPath;

void processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // extract vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // vertices.push_back(vertex);
        if (isVerbose) {
            std::cout << "Found 1 vertice:" << mesh->mVertices[i].x << " - " << mesh->mVertices[i].y << " - " << mesh->mVertices[i].y << "\n";
        }

        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
    }

    // extract indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        if (face.mNumIndices == 3) {

            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                if (isVerbose) {
                    std::cout << "Found 1 indice:" << face.mIndices[j] << "\n";
                }
                indices.push_back(face.mIndices[j]);
            }
        }
        else {
            std::cout << "Face isnot a triangle: mNumberIndices: " << face.mNumIndices << "\n";
        }
    }

    { // make triangle & write CSV
        std::ofstream myfile;
        myfile.open(outputPath, std::ios_base::app);

        myfile.setf(std::ios::fixed);
        for (unsigned int i = 0; i < indices.size(); i += 3) {
            // indices: 0,1,2 , 3,4,5, ...
            // triangles: 0,      1, ....

            unsigned int iA = indices[i + 0];
            unsigned int iB = indices[i + 1];
            unsigned int iC = indices[i + 2];

            
            myfile << std::fixed << std::setprecision(6) << vertices[iA * 3 + 0];
            myfile << ",";
            myfile << std::fixed << std::setprecision(6) << vertices[iA * 3 + 1];
            myfile << ",";
            myfile << std::fixed << std::setprecision(6) << vertices[iA * 3 + 2];
            myfile << ",";

            myfile << std::fixed << std::setprecision(6) << vertices[iB * 3 + 0];
            myfile << ",";
            myfile << std::fixed << std::setprecision(6) << vertices[iB * 3 + 1];
            myfile << ",";
            myfile << std::fixed << std::setprecision(6) << vertices[iB * 3 + 2];
            myfile << ",";

            myfile << std::fixed << std::setprecision(6) << vertices[iC * 3 + 0];
            myfile << ",";
            myfile << std::fixed << std::setprecision(6) << vertices[iC * 3 + 1];
            myfile << ",";
            myfile << std::fixed << std::setprecision(6) << vertices[iC * 3 + 2];
            myfile << "\n";
        }
        myfile.close();
    }
    
}

void processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void print_help() {
    std::cout << "beagle_3d_converter.exe <<input_path>> [ <<output_path>>, <<verbose>> ]\n\n";

    std::cout << "output_path: default value is output.csv\n";
    std::cout << "verbose: default value is false\n";

}

int main(int argc, char* argv[]) {
    { // INPUT
        std::cout << "BEAGLE 3D Conveter\n";
        for (int i = 0; i < argc; i++) {
            std::cout << argv[i] << "\n";
        }

        if (argc <= 1) {
            print_help();
            return -1;
        }

        inputPath = argv[1];
        outputPath = "output.csv";
        if (argc >= 3) {
            outputPath = argv[2];
        }

        if (argc >= 4) {
            isVerbose = true;
        }
    }

    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cout << "start computation at " << std::ctime(&start_time) << "\n";
    {

        // Processing file
        Assimp::Importer importer;
        //"C:/Users/luantm/Workshop/beagle_3d_converter/external/assimp/test/models/FBX/spider.fbx"
        const aiScene* scene = importer.ReadFile(inputPath, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return 0;
        }


        processNode(scene->mRootNode, scene);
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "finished computation at " << std::ctime(&end_time)
        << "elapsed time: " << elapsed_seconds.count() << "s\n";
    return 0;
}