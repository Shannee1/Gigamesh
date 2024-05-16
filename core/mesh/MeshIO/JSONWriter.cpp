//
// Created by timo.homburg on 16.05.2024.
//

#include "JSONWriter.h"
#include <fstream>
#include <iostream>
bool JSONWriter::writeFile(const std::filesystem::path &rFilename, const std::vector<sVertexProperties> &rVertexProps,
                           const std::vector<sFaceProperties> &rFaceProps, MeshSeedExt &rMeshSeed) {
    std::fstream filestr;
    filestr.imbue(std::locale("C"));
    filestr.open( rFilename, std::fstream::out );
    if( !filestr.is_open() ) {
        std::cerr << "[JSONWriter] Could not open file: '" << rFilename << "'.\n";
        return false;
    }
    filestr << "{\n";
    filestr << "\t\"vertices\":\n";
    filestr << "\t[\n";
    filestr << "\t\t{\n";
    filestr << "\t\t\t\"name\":\"position_buffer\",\n";
    filestr << "\t\t\t\"size\":3,\n";
    filestr << "\t\t\t\"type\":\"float32\",\n";
    filestr << "\t\t\t\"normalized\":false,\n";
    filestr << "\t\t\t\"values\":[\n\t\t\t\t";
    int counter=0;
    bool first=true;
    for( const auto& vertexProp : rVertexProps) {
        if(!first){
            filestr << ", ";
        }
        filestr << vertexProp.mCoordX << ", "<< vertexProp.mCoordY << ", " << vertexProp.mCoordZ;
        if(counter%10==0) {
            filestr << "\n\t\t\t\t";
        }
        counter+=1;
        first=false;
    }
    filestr << "\t\t\t]\n\t\t\t";
    filestr << "\t\t},\n";
    filestr << "\t\t{\n";
    filestr << "\t\t\t\"name\":\"normal_buffer\",\n";
    filestr << "\t\t\t\"size\":3,\n";
    filestr << "\t\t\t\"type\":\"float32\",\n";
    filestr << "\t\t\t\"normalized\":false,\n";
    filestr << "\t\t\t\"values\":[\n\t\t\t\t";
    counter=0;
    first=true;
    for( const auto& vertexProp : rVertexProps) {
        if(!first){
            filestr << ", ";
        }
        filestr << vertexProp.mNormalX << ", "<< vertexProp.mNormalY << ", " << vertexProp.mNormalZ;
        if(counter%10==0) {
            filestr << "\n\t\t\t\t";
        }
        counter+=1;
        first=false;
    }
    filestr << "\t\t\t]\n\t\t\t";
    filestr << "\t\t},\n";
    filestr << "\t\t{\n";
    filestr << "\t\t\t\"name\":\"color_buffer\",\n";
    filestr << "\t\t\t\"size\":4,\n";
    filestr << "\t\t\t\"type\":\"uint8\",\n";
    filestr << "\t\t\t\"normalized\":false,\n";
    filestr << "\t\t\t\"values\":[\n\t\t\t\t";
    counter=0;
    first=true;
    for( const auto& vertexProp : rVertexProps) {
        if(!first){
            filestr << ", ";
        }
        filestr << std::to_string(vertexProp.mColorRed) << ", "<< std::to_string(vertexProp.mColorGrn) << ", " << std::to_string(vertexProp.mColorBle) << ", " << std::to_string(vertexProp.mColorAlp);
        if(counter%10==0) {
            filestr << "\n\t\t\t";
        }
        counter+=1;
        first=false;
    }
    filestr << "\t\t\t]\n\t\t\t";
    filestr << "\t\t}\n";
    filestr << "\t],\n";
    filestr << "\t\"connectivity\":[\n";
    filestr << "\t\t{\n";
    filestr << "\t\t\t\"name\":\"triangles\",\n";
    filestr << "\t\t\t\"mode\":\"triangles_list\",\n";
    filestr << "\t\t\t\"size\":3,\n";
    filestr << "\t\t\t\"type\":\"uint32\",\n";
    filestr << "\t\t\t\"normalized\":false,\n";
    filestr << "\t\t\t\"values\":[\n\t\t\t\t";
    counter=0;
    first=true;
    for( const auto& faceProp : rFaceProps) {
        if(!first){
            filestr << ", ";
        }
        filestr << faceProp.vertexIndices[0] << ", " << faceProp.vertexIndices[1] << ", " << faceProp.vertexIndices[2];
        if(counter%10==0) {
            filestr << "\n\t\t\t\t";
        }
        counter+=1;
        first=false;
    }
    filestr << "\t\t\t]\n\t\t\t";
    filestr << "\t\t}\n";
    filestr << "\t],\n";
    filestr << "\t\"mapping\":[\n";
    filestr << "\t\t{\n";
    filestr << "\t\t\t\"name\":\"standard\",\n";
    filestr << "\t\t\t\"primitives\":\"triangles\",\n";
    filestr << "\t\t\t\"attributes\":[\n";
    filestr << "\t\t\t\t{\n";
    filestr << "\t\t\t\t\t\"source\":\"position_buffer\",\n";
    filestr << "\t\t\t\t\t\"semantic\":\"position\",\n";
    filestr << "\t\t\t\t\t\"set\":0\n";
    filestr << "\t\t\t\t},\n";
    filestr << "\t\t\t\t{\n";
    filestr << "\t\t\t\t\t\"source\":\"normal_buffer\",\n";
    filestr << "\t\t\t\t\t\"semantic\":\"normal\",\n";
    filestr << "\t\t\t\t\t\"set\":0\n";
    filestr << "\t\t\t\t},\n";
    filestr << "\t\t\t\t{\n";
    filestr << "\t\t\t\t\t\"source\":\"color_buffer\",\n";
    filestr << "\t\t\t\t\t\"semantic\":\"color\",\n";
    filestr << "\t\t\t\t\t\"set\":0\n";
    filestr << "\t\t\t\t}\n";
    filestr << "\t\t\t]\n";
    filestr << "\t\t}\n";
    filestr << "\t],\n";
    filestr << "\t\"custom\":null\n";
    filestr << "}\n";
    filestr.close();
    std::cout << "[JSONWriter] JSON written to: " << rFilename << "\n";
    return true;
}
