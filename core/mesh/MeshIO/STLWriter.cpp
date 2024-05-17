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
    int counter=0;
    for( const auto& faceProp : rFaceProps) {
        if(counter<rVertexProps.size()) {
            filestr << "facet normal " << "\n";
            filestr << "\touter loop\n";
            filestr << "\t\tvertex " << rVertexProps.at(counter).mCoordX << " " << rVertexProps.at(counter).mCoordY
                    << " " << rVertexProps.at(counter).mCoordZ << "\n";
            counter++;
            filestr << "\t\tvertex " << rVertexProps.at(counter).mCoordX << " " << rVertexProps.at(counter).mCoordY
                    << " " << rVertexProps.at(counter).mCoordZ << "\n";
            counter++;
            filestr << "\t\tvertex " << rVertexProps.at(counter).mCoordX << " " << rVertexProps.at(counter).mCoordY
                    << " " << rVertexProps.at(counter).mCoordZ << "\n";
            counter++;
            filestr << "\tendloop\n";
            filestr << "endfacet\n";
        }
    }
    filestr << "endsolid "<< rFilename << "\n";
    filestr.close();
    std::cout << "[STLWriter] STL written to: " << rFilename << "\n";
    return true;
}
