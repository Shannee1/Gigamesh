//
// Created by timo.homburg on 16.05.2024.
//

#include "ThreeJSONWriter.h"
#include <fstream>
#include <iostream>
bool
ThreeJSONWriter::writeFile(const std::filesystem::path &rFilename, const std::vector<sVertexProperties> &rVertexProps,
                           const std::vector<sFaceProperties> &rFaceProps, MeshSeedExt &rMeshSeed) {
    std::fstream filestr;
    filestr.imbue(std::locale("C"));
    filestr.open( rFilename, std::fstream::out );
    if( !filestr.is_open() ) {
        std::cerr << "[ThreeJSONWriter] Could not open file: '" << rFilename << "'.\n";
        return false;
    }
    filestr << "{\n";
    filestr << "\t\"metadata\":{\n";
    filestr << "\t\t\"version\":4.3,\n";
    filestr << "\t\t\"type\":\"Object\",\n";
    filestr << "\t\t\"generator\":\"ObjectExplorer\"\n";
    filestr << "\t},\n";
    filestr << "\t\"geometries\":[\n";
    filestr << "\t\t\t{\n";
    filestr << "\t\t\t\t\"uuid\":\"\"\n",
    filestr << "\t\t\t\t\"type\":\"BufferGeometry\",\n";
    filestr << "\t\t\t\t\"data\":{\n";
    filestr << "\t\t\t\t\t\"attributes\":{\n";
    filestr << "\t\t\t\t\t\t\"position\":{\n";
    filestr << "\t\t\t\t\t\t\t\"itemSize\":3,\n";
    filestr << "\t\t\t\t\t\t\t\"type\":\"Float32Array\",\n";
    filestr << "\t\t\t\t\t\t\t\"array\":[";
    bool first=true;
    int counter=0;
    for( const auto& vertexProp : rVertexProps) {
        if(!first){
            filestr << ", ";
        }
        filestr << vertexProp.mCoordX << ", " << vertexProp.mCoordY << ", " << vertexProp.mCoordZ;
        counter+=1;
        first=false;
    }
    filestr << "]\n";
    filestr << "\t\t\t\t\t\t\"},\n";
    filestr << "\t\t\t\t\t\t\"normal\":{\n";
    filestr << "\t\t\t\t\t\t\t\"itemSize\":3,\n";
    filestr << "\t\t\t\t\t\t\t\"type\":\"Float32Array\",\n";
    filestr << "\t\t\t\t\t\t\t\"array\":[";
    first=true;
    counter=0;
    for( const auto& vertexProp : rVertexProps) {
        if(!first){
            filestr << ", ";
        }
        filestr << vertexProp.mNormalX << ", " << vertexProp.mNormalY << ", " << vertexProp.mNormalZ;
        counter+=1;
        first=false;
    }
    filestr << "]\n";
    filestr << "\t\t\t\t\t\t\"},\n";
    filestr << "\t\t\t\t\t\t\"uv\":{\n";
    filestr << "\t\t\t\t\t\t\t\"itemSize\":2,\n";
    filestr << "\t\t\t\t\t\t\t\"type\":\"Float32Array\",\n";
    filestr << "\t\t\t\t\t\t\t\"array\":[]\n";
    filestr << "\t\t\t\t\t\t\"}\n";
    filestr << "\t\t\t\t\t\"},\n";
    filestr << "\t\t\t\t\t\"boundingSphere\":{\n";
    filestr << "\t\t\t\t\t\t\"center\":[0,0,0],\n";
    filestr << "\t\t\t\t\t\t\"radius\":0.0\n";
    filestr << "\t\t\t\t\t}\n";
    filestr << "\t\t\t\t}\n";
    filestr << "\t\t\t}\n";
    filestr << "\t\t],\n";
    filestr << "\t\t\"materials\":[],\n";
    filestr << "\t\t\"object\":{\n";
    filestr << "\t\t\t\"uuid\":\"\",\n";
    filestr << "\t\t\t\"type\":\"Scene\",\n";
    filestr << "\t\t\t\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1],\n";
    filestr << "\t\t\"children\":[\n";
    filestr << "\t\t{\n";
    filestr << "\t\t\t\"uuid\":\"\",";
    filestr << "\t\t\t\"name\":\"TheMesh\",";
    filestr << "\t\t\t\"type\":\"Mesh\",";
    filestr << "\t\t\t\"geometry\":\"\",";
    filestr << "\t\t\t\"matrix\":[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]\n";
    filestr << "\t\t}]}\n";
    filestr << "\t}\n";
    filestr.close();

    std::cout << "[OFFWriter] OFF written to: " << rFilename << "\n";
    return true;
}
