//
// Created by timo.homburg on 13.05.2024.
//

#include "X3DWriter.h"

#include <ctime>
#include <fstream>
#include <iostream>

bool X3DWriter::writeFile(const std::filesystem::path &rFilename, const std::vector<sVertexProperties> &rVertexProps,
                          const std::vector<sFaceProperties> &rFaceProps, MeshSeedExt &rMeshSeed) {

    std::fstream filestr;
    filestr.imbue(std::locale("C"));
    filestr.open( rFilename, std::fstream::out );
    if( !filestr.is_open() ) {
        std::cerr << "[X3DWriter] Could not open file: '" << rFilename << "'.\n";
        return false;
    }
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    std::cout << "[MeshIO] File open for writing: '" << rFilename << "'.\n";

    filestr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    filestr << "<!DOCTYPE X3D PUBLIC \"ISO//Web3D//DTD X3D 4.0//EN\" \"http://www.web3d.org/specifications/x3d-4.0.dtd\">\n";

    filestr << "<X3D profile=\"Interchange\" version=\"4.0\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema-instance\" xsd:noNamespaceSchemaLocation=\"http://www.web3d.org/specifications/x3d-4.0.xsd\">\n";
    filestr << "<head>\n";
    filestr << "<meta content=\"Generated with GigaMesh\" name=\"description\"/>\n";
    filestr << "<meta content=\"" << now->tm_mday << "/" << (now->tm_mon + 1) << "/" <<  (now->tm_year + 1900)<< "\" name=\"created\"/>\n";
    filestr << "<meta content=\"Exported with GigaMesh, http://www.gigamesh.eu\" name=\"generator\"/>\n";
    filestr << "</head>\n";
    filestr << "<Scene>\n";
    filestr << "<Shape DEF=" << rFilename << ">\n";
    filestr << "<IndexedFaceSet coordIndex=\"";
    for( const auto& faceProp : rFaceProps) {
        filestr << " " << faceProp.vertexIndices[0] << " "
                << faceProp.vertexIndices[1] << " " << faceProp.vertexIndices[2];
    }
    filestr << "\">\n";
    filestr << "<Coordinate point=\"";
    for( const auto& vertexProp : rVertexProps) {
        filestr << vertexProp.mCoordX << " " << vertexProp.mCoordY << " " << vertexProp.mCoordZ << " ";
    }
    filestr << "\"/>\n";
    filestr << "<ColorRGBA color=\"";
    for( const auto& vertexProp : rVertexProps) {
        filestr << std::to_string(vertexProp.mColorRed) << " " << std::to_string(vertexProp.mColorGrn) << " " << std::to_string(vertexProp.mColorBle)  << " " << std::to_string(vertexProp.mColorAlp) << " ";
    }
    filestr << "\"/>\n";
    filestr << "<Normal vector=\"";
    for( const auto& vertexProp : rVertexProps) {
        filestr << vertexProp.mNormalX << " " << vertexProp.mNormalY << " " << vertexProp.mNormalZ << " ";
    }
    filestr << "\"/>\n";
    filestr << "</IndexedFaceSet>\n";
    filestr << "</Shape>\n";
    filestr << "</Scene>\n";
    filestr << "</X3D>\n";
    filestr.close();

    std::cout << "[X3DWriter] X3D written to: " << rFilename << "\n";
    return true;
}
