//
// Created by timo.homburg on 16.05.2024.
//

#ifndef GIGAMESH_OFFWRITER_H
#define GIGAMESH_OFFWRITER_H

#include "MeshWriter.h"

class OFFWriter : public MeshWriter {
public:
    OFFWriter() = default;

    // MeshWriter interface
public:
    bool writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed) override;

};


#endif //GIGAMESH_OFFWRITER_H
