//
// Created by timo.homburg on 16.05.2024.
//

#ifndef GIGAMESH_THREEJSONWRITER_H
#define GIGAMESH_THREEJSONWRITER_H

#include "MeshWriter.h"

class ThreeJSONWriter: public MeshWriter {
    public:
        ThreeJSONWriter() = default;

    // MeshWriter interface
    public:
        bool writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed) override;

};


#endif //GIGAMESH_THREEJSONWRITER_H
