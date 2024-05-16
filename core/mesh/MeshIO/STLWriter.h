//
// Created by timo.homburg on 16.05.2024.
//

#ifndef GIGAMESH_STLWRITER_H
#define GIGAMESH_STLWRITER_H


#include "MeshWriter.h"

class STLWriter : public MeshWriter {
public:
    STLWriter() = default;

    // MeshWriter interface
public:
    bool writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed) override;

};

#endif //GIGAMESH_STLWRITER_H
