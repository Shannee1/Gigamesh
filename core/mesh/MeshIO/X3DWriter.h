//
// Created by timo.homburg on 13.05.2024.
//

#ifndef GIGAMESH_X3DWRITER_H
#define GIGAMESH_X3DWRITER_H


#include "MeshWriter.h"

class X3DWriter : public MeshWriter {
public:
    X3DWriter() = default;

    // MeshWriter interface
public:
    bool writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed) override;

};


#endif //GIGAMESH_X3DWRITER_H
