//
// Created by timo.homburg on 14.05.2024.
//

#ifndef GIGAMESH_COLLADAWRITER_H
#define GIGAMESH_COLLADAWRITER_H

#include "MeshWriter.h"

class ColladaWriter : public MeshWriter {
public:
    ColladaWriter() = default;

    // MeshWriter interface
public:
    bool writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed) override;


};


#endif //GIGAMESH_COLLADAWRITER_H
