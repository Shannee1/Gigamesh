//
// Created by timo.homburg on 14.05.2024.
//

#include "ColladaWriter.h"
#include <fstream>
#include <iostream>


bool
ColladaWriter::writeFile(const std::filesystem::path &rFilename, const std::vector<sVertexProperties> &rVertexProps,
                         const std::vector<sFaceProperties> &rFaceProps, MeshSeedExt &rMeshSeed) {
    std::fstream filestr;
    filestr.imbue(std::locale("C"));
    filestr.open( rFilename, std::fstream::out );
    if( !filestr.is_open() ) {
        std::cerr << "[ColladaWriter] Could not open file: '" << rFilename << "'.\n";
        return false;
    }
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    std::cout << "[MeshIO] File open for writing: '" << rFilename << "'.\n";

    filestr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    filestr << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n";
    filestr << "<library_geometries>\n";
    filestr << "<geometry id=\"shape0-lib\" name=\"shape0\">\n";
    filestr << "<mesh>\n";
    filestr << "<source id=\"shape0-lib-positions\" name=\"position\">\n";
    filestr << "<float_array id=\"shap0-lib-positions-array\" count=\"" << std::to_string(rVertexProps.size()) << "\">";
    for( const auto& vertexProp : rVertexProps) {
        filestr << vertexProp.mCoordX << " " << vertexProp.mCoordY << " " << vertexProp.mCoordZ << " ";
    }
    filestr << "</float_array>\n";
    filestr << "<technique_common>\n";
    filestr << "<accessor count=\"" << std::to_string(rVertexProps.size()/3) <<  "\" source=\"#shape0-lib-positions-array\" stride=\"3\">\n";
    filestr << "<param name=\"X\" type=\"float\"/>\n";
    filestr << "<param name=\"Y\" type=\"float\"/>\n";
    filestr << "<param name=\"Z\" type=\"float\"/>\n";
    filestr << "</accessor>\n";
    filestr << "</technique_common>\n";
    filestr << "</source>\n";
    filestr << "<source id=\"shape0-lib-normals\" name=\"normal\">\n";
    filestr << "<float_array id=\"shap0-lib-normals-array\" count=\"" << std::to_string(rVertexProps.size()) << "\">";
    for( const auto& vertexProp : rVertexProps) {
        filestr << vertexProp.mNormalX << " " << vertexProp.mNormalY << " " << vertexProp.mNormalZ << " ";
    }
    filestr << "</float_array>\n";
    filestr << "<technique_common>\n";
    filestr << "<accessor count=\"" << std::to_string(rVertexProps.size()/3) <<  "\" source=\"#shape0-lib-normals-array\" stride=\"3\">\n";
    filestr << "<param name=\"X\" type=\"float\"/>\n";
    filestr << "<param name=\"Y\" type=\"float\"/>\n";
    filestr << "<param name=\"Z\" type=\"float\"/>\n";
    filestr << "</accessor>\n";
    filestr << "</technique_common>\n";
    filestr << "</source>\n";
    filestr << "<source id=\"shape0-lib-vcolor\" name=\"vcolor\">\n";
    filestr << "<float_array id=\"shap0-lib-vcolor-array\" count=\"" << std::to_string(rVertexProps.size()*4) << "\">";
    for( const auto& vertexProp : rVertexProps) {
        filestr << " " << std::to_string(vertexProp.mColorRed) << " " << std::to_string(vertexProp.mColorGrn) << " " << std::to_string(vertexProp.mColorBle)  << " " << std::to_string(vertexProp.mColorAlp) << " ";
    }
    filestr << "</float_array>\n";
    filestr << "<technique_common>\n";
    filestr << "<accessor count=\"" << std::to_string(rVertexProps.size()/4) <<  "\" source=\"#shape0-lib-vcolor-array\" stride=\"4\">\n";
    filestr << "<param name=\"R\" type=\"float\"/>\n";
    filestr << "<param name=\"G\" type=\"float\"/>\n";
    filestr << "<param name=\"B\" type=\"float\"/>\n";
    filestr << "<param name=\"A\" type=\"float\"/>\n";
    filestr << "</accessor>\n";
    filestr << "</technique_common>\n";
    filestr << "</source>\n";
    filestr << "<vertices id=\"shape0-lib-vertices\">\n";
    filestr << "<input semantic=\"POSITION\" source=\"#shape0-lib-positions\"/>\n";
    filestr << "</vertices>\n";
    filestr << "<triangles count=\"" << std::to_string(rFaceProps.size()) << "\">\n";
    filestr << "<input offset=\"0\" semantic=\"VERTEX\" source=\"#shape0-lib-vertices\"/>\n";
    filestr << "<input offset=\"1\" semantic=\"COLOR\" source=\"#shape0-lib-vcolor\"/>\n";
    filestr << "</triangles>\n";
    filestr << "</mesh>\n";
    filestr << "</geometry>\n";
    filestr << "</library_geometries>\n";
    filestr << "<library_visual_scences>\n";
    filestr << "<visual_scene id=\"VisualSceneNode\" name=\"VisualScene\">\n";
    filestr << "<node id=\"node\" name=\"node\">\n";
    filestr << "<instance_geometry url=\"#shape0-lib\"/>\n";
    filestr << "</node>\n";
    filestr << "</visual_scene>\n";
    filestr << "</library_visual_scences>\n";
    filestr << "<scene>\n";
    filestr << "<instance_visual_scene url=\"#VisualSceneNode\">\n";
    filestr << "</scene>\n";
    filestr << "</COLLADA>\n";
    filestr.close();
    std::cout << "[ColladaWriter] COLLADA written to: " << rFilename << "\n";
    return true;
}
