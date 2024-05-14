//
// Created by timo.homburg on 14.05.2024.
//

#include "XYZWriter.h"
#include <fstream>
#include <iostream>

bool XYZWriter::writeFile(const std::filesystem::path &rFilename, const std::vector<sVertexProperties> &rVertexProps,
                          const std::vector<sFaceProperties> &rFaceProps, MeshSeedExt &rMeshSeed) {
    std::fstream filestr;
    filestr.imbue(std::locale("C"));
    filestr.open( rFilename, std::fstream::out );
    if( !filestr.is_open() ) {
        std::cerr << "[X3DWriter] Could not open file: '" << rFilename << "'.\n";
        return false;
    }
    std::cout << "[MeshIO] File open for writing: '" << rFilename << "'.\n";
    for( const auto& vertexProp : rVertexProps) {
        filestr << vertexProp.mCoordX << " " << vertexProp.mCoordY << " " << vertexProp.mCoordZ << "\n";
    }
    filestr.close();
    std::cout << "[X3DWriter] X3D written to: " << rFilename << "\n";
    return true;
}
