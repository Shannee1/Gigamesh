//
// Created by timo.homburg on 16.05.2024.
//

#include "STLWriter.h"
#include <fstream>
#include <iostream>

bool STLWriter::writeFile(const std::filesystem::path &rFilename, const std::vector<sVertexProperties> &rVertexProps,
                          const std::vector<sFaceProperties> &rFaceProps, MeshSeedExt &rMeshSeed) {

    std::fstream filestr;
    filestr.imbue(std::locale("C"));
    filestr.open( rFilename, std::fstream::out );
    if( !filestr.is_open() ) {
        std::cerr << "[STLWriter] Could not open file: '" << rFilename << "'.\n";
        return false;
    }
    filestr << "solid "<< rFilename << "\n";
    for( const auto& faceProp : rFaceProps) {
        filestr << "facet normal " << "\n";
        filestr << "\touter loop\n";
        //filestr << "\t\tvertex " << faceProp.vertexIndices[0].
        // VRML index for vertices start with ZERO!!!:
        //filestr << "\t\t\t" << faceProp.vertexIndices[0] << " "
        //        << faceProp.vertexIndices[1] << " " << faceProp.vertexIndices[2] << " -1\n";
        filestr << "\tendloop\n";
        filestr << "endfacet\n";
    }
    filestr << "endsolid "<< rFilename << "\n";
    filestr.close();
    std::cout << "[STLWriter] STL written to: " << rFilename << "\n";
    return true;
}
