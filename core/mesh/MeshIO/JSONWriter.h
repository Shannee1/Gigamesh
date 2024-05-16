//
// Created by timo.homburg on 16.05.2024.
//

#ifndef GIGAMESH_JSONWRITER_H
#define GIGAMESH_JSONWRITER_H


#include "MeshWriter.h"

class JSONWriter : public MeshWriter {
public:
    JSONWriter() = default;

    // MeshWriter interface
public:
    bool writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed) override;

};


#endif //GIGAMESH_JSONWRITER_H
