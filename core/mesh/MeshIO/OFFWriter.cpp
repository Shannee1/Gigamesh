//
// Created by timo.homburg on 16.05.2024.
//


#include "OFFWriter.h"
#include <fstream>
#include <iostream>

bool OFFWriter::writeFile(const std::filesystem::path &rFilename, const std::vector<sVertexProperties> &rVertexProps,
                          const std::vector<sFaceProperties> &rFaceProps, MeshSeedExt &rMeshSeed) {

    std::fstream filestr;
    filestr.imbue(std::locale("C"));
    filestr.open( rFilename, std::fstream::out );
    if( !filestr.is_open() ) {
        std::cerr << "[OFFWriter] Could not open file: '" << rFilename << "'.\n";
        return false;
    }
    filestr << "OFF\n";
    filestr << std::to_string(rVertexProps.size()) << " " << std::to_string(rFaceProps.size()) << " 0\n";
    for( const auto& vertexProp : rVertexProps) {
        filestr << "3 " << vertexProp.mCoordX << " "
                << vertexProp.mCoordY << " " << vertexProp.mCoordZ << " " << vertexProp.mColorRed << " " << vertexProp.mColorGrn << " " << vertexProp.mColorBle << "\n";
    }
    for( const auto& faceProp : rFaceProps) {
        // VRML index for vertices start with ZERO!!!:
        filestr << "\t\t\t" << faceProp.vertexIndices[0] << " "
                << faceProp.vertexIndices[1] << " " << faceProp.vertexIndices[2] << " -1\n";
    }
    filestr.close();

    std::cout << "[OFFWriter] OFF written to: " << rFilename << "\n";
    return true;
    return true;
}
