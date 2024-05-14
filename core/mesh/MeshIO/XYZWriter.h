//
// Created by timo.homburg on 14.05.2024.
//

#ifndef GIGAMESH_XYZWRITER_H
#define GIGAMESH_XYZWRITER_H

#include "MeshWriter.h"

class XYZWriter : public MeshWriter {
public:
    XYZWriter() = default;

    // MeshWriter interface
public:
    bool writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed) override;

};


#endif //GIGAMESH_XYZWRITER_H
