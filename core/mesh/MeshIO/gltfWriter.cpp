//
// GigaMesh - The GigaMesh Software Framework is a modular software for display,
// editing and visualization of 3D-data typically acquired with structured light or
// structure from motion.
// Copyright (C) 2009-2020 Hubert Mara
//
// This file is part of GigaMesh.
//
// GigaMesh is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GigaMesh is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GigaMesh.  If not, see <http://www.gnu.org/licenses/>.
//
#include "gltfWriter.h"
#include <fstream>
#include <iostream>
#include <locale>
#include <filesystem>
#include <map>
#include <list>
#include<iostream>
#include<string>
#include <algorithm>

#include "gltfWriter.h"
#include <GigaMesh/logging/Logging.h>

using namespace std;

//source: https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

std::string base64_encode(BYTE const* buf, unsigned int bufLen) {
  std::string ret;
  int i = 0;
  int j = 0;
  BYTE char_array_3[3];
  BYTE char_array_4[4];

  while (bufLen--) {
    char_array_3[i++] = *(buf++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';
  }

  return ret;
}

void GltfWriter::addFloatHexStringToBuffer(std::vector<BYTE> *buffer, float value)
{
    char const * valueStream = (char const *)&value;
    for (size_t i = 0; i != sizeof(float); ++i)
    {
        int byteValue = (int)valueStream[i];
        unsigned char byteAsChar = byteValue;
        buffer->push_back(byteAsChar);
    }
}

void GltfWriter::createBuffersWithoutTextureCoords( std::vector<BYTE> *indexBuffer, std::vector<BYTE> *vertexBuffer, std::vector<BYTE> *normalsBuffer,
                                                    float minPosition[3], float maxPosition[3], float minNormals[3], float maxNormals[3],
                                                    const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps)
{

    //add all faces to file buffer
    for( const auto& faceProp : rFaceProps) {
        //add for each face the assigned vertices

        //the texture coordinates are saved in a face property
        //therefore, the textureIndex serves to access them
        unsigned int textureIndex = 0;
        for( const auto& faceVertexIndex: faceProp.vertexIndices){

            // get the bytes
            int a = (faceVertexIndex >>  24) & 0xff;  // next byte, bits 24-32
            int b = (faceVertexIndex >>  16) & 0xff;  // next byte, bits 16-23
            int c = (faceVertexIndex >>  8) & 0xff;  // next byte, bits 8-15
            int d = faceVertexIndex         & 0xff;  // low-order byte: bits 0-7

            //change the parts to achieve little endian stringt
            unsigned char part1Byte = a;
            unsigned char part2Byte = b;
            unsigned char part3Byte = c;
            unsigned char part4Byte = d;
            indexBuffer->push_back(part4Byte);
            indexBuffer->push_back(part3Byte);
            indexBuffer->push_back(part2Byte);
            indexBuffer->push_back(part1Byte);
        }
    }

    //The Vertices and Normals are stored once
    //the mesh is defined by the faces through the indices
    //the index refers to a position/normal


    for( const auto& vertProp : rVertexProps) {

        //-----------------------------------
        //Position
        //-----------------------------------

        //save vertex coords as float
        float coordX = (float) vertProp.mCoordX;
        float coordY = (float) vertProp.mCoordY;
        float coordZ = (float) vertProp.mCoordZ;

        //update min max
        if (coordX < minPosition[0]){
            minPosition[0] = coordX;
        }
        if (coordY < minPosition[1]){
            minPosition[1] = coordY;
        }
        if (coordZ < minPosition[2]){
            minPosition[2] = coordZ;
        }
        if (coordX > maxPosition[0]){
            maxPosition[0] = coordX;
        }
        if (coordY > maxPosition[1]){
            maxPosition[1] = coordY;
        }
        if (coordZ > maxPosition[2]){
            maxPosition[2] = coordZ;
        }

        addFloatHexStringToBuffer(vertexBuffer,coordX);
        addFloatHexStringToBuffer(vertexBuffer,coordY);
        addFloatHexStringToBuffer(vertexBuffer,coordZ);

        //--------------------------
        //Normals
        //--------------------------
        if(mExportVertNormal){
            //save vertex coords as float
            float normalX = (float) vertProp.mNormalX;
            float normalY = (float) vertProp.mNormalY;
            float normalZ = (float) vertProp.mNormalZ;


            //update min max
            if (normalX < minNormals[0]){
                minNormals[0] = normalX;
            }
            if (normalY < minNormals[1]){
                minNormals[1] = normalY;
            }
            if (normalZ < minNormals[2]){
                minNormals[2] = normalZ;
            }
            if (normalX > maxNormals[0]){
                maxNormals[0] = normalX;
            }
            if (normalY > maxNormals[1]){
                maxNormals[1] = normalY;
            }
            if (normalZ > maxNormals[2]){
                maxNormals[2] = normalZ;
            }

            addFloatHexStringToBuffer(normalsBuffer,normalX);
            addFloatHexStringToBuffer(normalsBuffer,normalY);
            addFloatHexStringToBuffer(normalsBuffer,normalZ);

        }
    }
}

void GltfWriter::createBuffersIncludingTextureCoords(std::vector<BYTE> *indexBuffer, std::vector<BYTE> *vertexBuffer, std::vector<BYTE> *normalsBuffer, std::vector<BYTE> *uvCoordsBuffer,
                                                     float minPosition[3], float maxPosition[3], float minNormals[3], float maxNormals[3], float minTextureCoords[2], float maxTextureCoords[2],
                                                      const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps)
{
    //initialize min and max
    minTextureCoords[0] = (float) rFaceProps[0].textureCoordinates[0];
    minTextureCoords[1] = (float) rFaceProps[0].textureCoordinates[1];
    maxTextureCoords[0] = (float) rFaceProps[0].textureCoordinates[0];
    maxTextureCoords[1] = (float) rFaceProps[0].textureCoordinates[1];
    unsigned int index = 0;
    //add all faces to file buffer
    for( const auto& faceProp : rFaceProps) {
        //add for each face the assigned vertices

        //the texture coordinates are saved in a face property
        //therefore, the textureIndex serves to access them
        unsigned int textureIndex = 0;
        for( const auto& faceVertexIndex: faceProp.vertexIndices){

            // get the bytes
            int a = (index >>  24) & 0xff;  // next byte, bits 24-32
            int b = (index >>  16) & 0xff;  // next byte, bits 16-23
            int c = (index >>  8) & 0xff;  // next byte, bits 8-15
            int d = index         & 0xff;  // low-order byte: bits 0-7

            //change the parts to achieve little endian stringt
            unsigned char part1Byte = a;
            unsigned char part2Byte = b;
            unsigned char part3Byte = c;
            unsigned char part4Byte = d;
            indexBuffer->push_back(part4Byte);
            indexBuffer->push_back(part3Byte);
            indexBuffer->push_back(part2Byte);
            indexBuffer->push_back(part1Byte);

            //-----------------------------------
            //Position
            //-----------------------------------

            //save vertex coords as float
            float coordX = (float) rVertexProps[faceVertexIndex].mCoordX;
            float coordY = (float) rVertexProps[faceVertexIndex].mCoordY;
            float coordZ = (float) rVertexProps[faceVertexIndex].mCoordZ;

            //update min max
            if (coordX < minPosition[0]){
                minPosition[0] = coordX;
            }
            if (coordY < minPosition[1]){
                minPosition[1] = coordY;
            }
            if (coordZ < minPosition[2]){
                minPosition[2] = coordZ;
            }
            if (coordX > maxPosition[0]){
                maxPosition[0] = coordX;
            }
            if (coordY > maxPosition[1]){
                maxPosition[1] = coordY;
            }
            if (coordZ > maxPosition[2]){
                maxPosition[2] = coordZ;
            }

            addFloatHexStringToBuffer(vertexBuffer,coordX);
            addFloatHexStringToBuffer(vertexBuffer,coordY);
            addFloatHexStringToBuffer(vertexBuffer,coordZ);


            //--------------------------
            //Normals
            //--------------------------
            if(mExportVertNormal){
                //save vertex coords as float
                float normalX = (float) rVertexProps[faceVertexIndex].mNormalX;
                float normalY = (float) rVertexProps[faceVertexIndex].mNormalY;
                float normalZ = (float) rVertexProps[faceVertexIndex].mNormalZ;


                //update min max
                if (normalX < minNormals[0]){
                    minNormals[0] = normalX;
                }
                if (normalY < minNormals[1]){
                    minNormals[1] = normalY;
                }
                if (normalZ < minNormals[2]){
                    minNormals[2] = normalZ;
                }
                if (normalX > maxNormals[0]){
                    maxNormals[0] = normalX;
                }
                if (normalY > maxNormals[1]){
                    maxNormals[1] = normalY;
                }
                if (normalZ > maxNormals[2]){
                    maxNormals[2] = normalZ;
                }

                addFloatHexStringToBuffer(normalsBuffer,normalX);
                addFloatHexStringToBuffer(normalsBuffer,normalY);
                addFloatHexStringToBuffer(normalsBuffer,normalZ);

            }
            //--------------------------
            //Texture Coords (UV)
            //--------------------------

            //save texture coords as float
            //mirrow the values
            float coordU = (float) faceProp.textureCoordinates[2*textureIndex];
            float coordV = 1.0- (float) faceProp.textureCoordinates[2*textureIndex + 1];

            //update min max
            if (coordU < minTextureCoords[0]){
                minTextureCoords[0] = coordU;
            }
            if (coordV < minTextureCoords[1]){
                minTextureCoords[1] = coordV;
            }
            if (coordU > maxTextureCoords[0]){
                maxTextureCoords[0] = coordU;
            }
            if (coordV > maxTextureCoords[1]){
                maxTextureCoords[1] = coordV;
            }


            addFloatHexStringToBuffer(uvCoordsBuffer,coordU);
            addFloatHexStringToBuffer(uvCoordsBuffer,coordV);

            textureIndex++;
            index++;

        }
    }
}



bool GltfWriter::writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed)
{
    fstream filestr;
    filestr.imbue(std::locale("C"));
    int timeStart; // for performance mesurement

    timeStart = clock();
    filestr.open( rFilename, fstream::out );
    if( !filestr.is_open() ) {
        LOG::error() << "[ObjWriter::" << __FUNCTION__ << "] ERROR: Could not open file: '" << rFilename << "'!" << '\n';
        return false;
    } else {
        LOG::debug() << "[ObjWriter::" << __FUNCTION__ << "] File open for writing: '" << rFilename << "'." << '\n';
    }

    //following https://github.khronos.org/glTF-Tutorials/gltfTutorial/gltfTutorial_003_MinimalGltfFile.html
    //basic structure as JSON
    //Mesh and scene description
    filestr << "{" << '\n';
    filestr << "\"scene\": 0," << '\n';
    filestr << "\"scenes\" : [" << '\n';
    filestr << "{" << '\n';
    filestr << "\"nodes\" : [ 0 ]" << '\n';
    filestr << "}" << '\n';
    filestr << "]," << '\n';
    filestr << "\"nodes\" : [" << '\n';
    filestr << "{" << '\n';
    filestr << "\"mesh\" : 0" << '\n';
    filestr << "}" << '\n';
    filestr << "]," << '\n';
    //content definition
    filestr << "\"meshes\" : [" << '\n';
    filestr << "{" << '\n';
    filestr << "\"primitives\" : [ {" << '\n';
    filestr << "\"attributes\" : {" << '\n';

    if(mExportVertNormal){
        filestr << "\"POSITION\" : 1," << '\n';
        if (mExportTextureCoordinates){
                filestr << "\"NORMAL\" : 2," << '\n';
                filestr << "\"TEXCOORD_0\" : 3" << '\n';
        }
        else
        {
           filestr << "\"NORMAL\" : 2" << '\n';
        }
    }
    else{
        if (mExportTextureCoordinates){
                filestr << "\"POSITION\" : 1," << '\n';
                filestr << "\"TEXCOORD_0\" : 2" << '\n';
        }
        else
        {
            filestr << "\"POSITION\" : 1" << '\n';
        }
    }

    filestr << "}," << '\n';
    if (!mExportTextureCoordinates){
        filestr << "\"indices\" : 0" << '\n';
    }
    else{
         filestr << "\"indices\" : 0," << '\n';
         filestr << "\"material\" : 0" << '\n';
    }
    filestr << "} ]" << '\n';
    filestr << "}" << '\n';
    filestr << "]," << '\n';


    //add texture configuration
    //uv coordinates are in the buffer
    if (mExportTextureCoordinates){
        //TODO Add PBR parameter
        filestr << "\"materials\" : [ {" << '\n';
        filestr << "\"pbrMetallicRoughness\" : {" << '\n';
        filestr << "\"baseColorTexture\" : {" << '\n';
        filestr << "\"index\" : 0" << '\n';
        filestr << " }," << '\n';
        filestr << "\"metallicFactor\" : 0.0," << '\n';
        filestr << "\"roughnessFactor\" : 1.0" << '\n';
        filestr << "}" << '\n';
        filestr << "} ]," << '\n';
        //set file system to the mesh file, otherwise the relative dir based on the GigaMesh build dir
        auto prevPath = std::filesystem::current_path();
        std::filesystem::current_path(std::filesystem::absolute(rFilename).parent_path());
        //TODO multiple texture files
        std::vector<std::filesystem::path>& textureFiles = MeshWriter::getModelMetaDataRef().getTexturefilesRef();

        filestr << "\"textures\" : [ {" << '\n';
        filestr << "\"sampler\" : 0," << '\n';
        filestr << "\"source\" : 0" << '\n';
        filestr << "} ]," << '\n';
        filestr << "\"images\" : [ {" << '\n';
        filestr << "\"uri\" : \"" << std::filesystem::relative(textureFiles[0]).string() << "\"" << '\n';
        filestr << "} ]," << '\n';
        filestr << "\"samplers\" : [ {" << '\n';
        filestr << "\"magFilter\" : 9729," << '\n';
        filestr << "\"minFilter\" : 9987," << '\n';
        filestr << "\"wrapS\" : 33648," << '\n';
        filestr << "\"wrapT\" : 33648" << '\n';
        filestr << "} ]," << '\n';
        //reset filesystem
        std::filesystem::current_path(prevPath);
    }


    //Buffer contains the actual data

    unsigned int index = 0;
    std::vector<BYTE> indexBuffer;
    std::vector<BYTE> vertexBuffer;
    std::vector<BYTE> normalsBuffer;
    std::vector<BYTE> uvCoordsBuffer;


    //save min max of the vectors for the accessors
    //structure: x, y ,z
    float minPosition[3] = {(float) rVertexProps[0].mCoordX, (float) rVertexProps[0].mCoordY,(float) rVertexProps[0].mCoordZ};
    float maxPosition[3] = {(float) rVertexProps[0].mCoordX, (float) rVertexProps[0].mCoordY,(float) rVertexProps[0].mCoordZ};
    float minNormals[3] = {(float) rVertexProps[0].mNormalX, (float) rVertexProps[0].mNormalY,(float) rVertexProps[0].mNormalZ};
    float maxNormals[3] = {(float) rVertexProps[0].mNormalX, (float) rVertexProps[0].mNormalY,(float) rVertexProps[0].mNormalZ};
    //will be initialized within the method
    //structure: u, v
    float minTextureCoords[2];
    float maxTextureCoords[2];

    //is required inside the buffer accessor definition
    //primitives = vertex, face and texture coordinates
    unsigned int nrOfPrimitives = 0;
    //similar behavior as above: the maximum value of the indices depends on the export method
    //if each vertex of each face is written separatly, then the max values is 3 times the number of faces because the index of the primitives are just incremented
    //otherwise the indices include the actual indices of the vertices
    unsigned int maxIndexValue = 0;

    if(!mExportTextureCoordinates){
        createBuffersWithoutTextureCoords(&indexBuffer,&vertexBuffer,&normalsBuffer, minPosition, maxPosition, minNormals, maxNormals, rVertexProps,rFaceProps);
        //the face are described by indices --> each vertex is written once
        nrOfPrimitives = rVertexProps.size();
        maxIndexValue = rVertexProps.size()-1;
    }
    else{
        //if the texture coordinates have to be saved, then the faces must be stored by vertices
        //--> For each face, 3 positions, normals and texture coordinates are saved
        //This is due to the GLTF requirement that primitive buffers (positions, normals and textcoords) must consist of the number of entries.
        createBuffersIncludingTextureCoords(&indexBuffer,&vertexBuffer,&normalsBuffer,&uvCoordsBuffer, minPosition, maxPosition, minNormals, maxNormals, minTextureCoords, maxTextureCoords, rVertexProps,rFaceProps);
        //each face is defined by 3 vertices
        nrOfPrimitives = 3*rFaceProps.size();
        maxIndexValue = 3*rFaceProps.size() - 1;
    }


    int nrOfBytesIndexBuffer = indexBuffer.size();
    //add zero bytes if it isn't a multiple of 4
    int bytesToBeFilled = nrOfBytesIndexBuffer % 4;
    if (bytesToBeFilled != 0){
        unsigned char zeroBytes = int(0);
        for(int i = 0; i < bytesToBeFilled; i++){
            indexBuffer.push_back(zeroBytes);
        }
    }
    int nrOfBytesIndexBufferPlusOffset = indexBuffer.size();

    //Every buffer are concatenated to the final buffer
    //and base64 encoded
    std::vector<BYTE> entireBuffer = indexBuffer;
    entireBuffer.insert(entireBuffer.end(), vertexBuffer.begin(), vertexBuffer.end());
    entireBuffer.insert(entireBuffer.end(), normalsBuffer.begin(), normalsBuffer.end());
    if (mExportTextureCoordinates){
        entireBuffer.insert(entireBuffer.end(), uvCoordsBuffer.begin(), uvCoordsBuffer.end());
    }
    std::string entireBufferEncoded = base64_encode(&entireBuffer[0], entireBuffer.size());
    int nrOfBytesVertexBuffer = vertexBuffer.size();
    int nrOfBytesNormalsBuffer = normalsBuffer.size();
    int nrOfBytesUVBuffer = uvCoordsBuffer.size();
    int nrOfBytes = entireBuffer.size();


    filestr << "\"buffers\" : [" << '\n';
    filestr << "{" << '\n';
    filestr << "\"uri\" : \"data:application/octet-stream;base64,"<< entireBufferEncoded << "\"," << '\n';
    filestr << "\"byteLength\" : " << std::to_string(nrOfBytes) << '\n';
    filestr << "}" << '\n';
    filestr << "]," << '\n';
    // bufferviews together with accessors defines different windows inside the buffer
    //first the indices as unsigned int
    //second vertex data as flot vec3
    filestr << "\"bufferViews\" : [" << '\n';
    filestr << "{" << '\n';
    filestr << "\"buffer\" : 0," << '\n';
    filestr << "\"byteOffset\" : 0," << '\n';
    filestr << "\"byteLength\" : " << std::to_string(nrOfBytesIndexBuffer) << "," << '\n';
    filestr << "\"target\" : 34963" << '\n';
    filestr << "}," << '\n';

    //vertex buffer
    filestr << "{" << '\n';
    filestr << "\"buffer\" : 0," << '\n';
    filestr << "\"byteOffset\" : " << std::to_string(nrOfBytesIndexBufferPlusOffset) << "," << '\n';
    //filestr << "\"byteLength\" : " << std::to_string(nrOfBytesVertexBuffer + nrOfBytesNormalsBuffer + nrOfBytesUVBuffer) << "," << '\n';
    filestr << "\"byteLength\" : " << std::to_string(nrOfBytesVertexBuffer) << "," << '\n';
    filestr << "\"target\" : 34962" << '\n';
    if (!mExportVertNormal && !mExportTextureCoordinates){
        //no further value buffers --> last entry
        filestr << "}" << '\n';
    }
    else{
        filestr << "}," << '\n';

        if(mExportVertNormal){
            //normals buffer
            filestr << "{" << '\n';
            filestr << "\"buffer\" : 0," << '\n';
            filestr << "\"byteOffset\" : " << std::to_string(nrOfBytesIndexBufferPlusOffset+nrOfBytesVertexBuffer) << "," << '\n';
            filestr << "\"byteLength\" : " << std::to_string(nrOfBytesNormalsBuffer) << "," << '\n';
            filestr << "\"target\" : 34962" << '\n';
            if (!mExportTextureCoordinates){
                filestr << "}" << '\n';
            }
            else {
                filestr << "}," << '\n';
            }
        }
        if (mExportTextureCoordinates){
            //uv buffer
            filestr << "{" << '\n';
            filestr << "\"buffer\" : 0," << '\n';
            filestr << "\"byteOffset\" : " << std::to_string(nrOfBytesIndexBufferPlusOffset+nrOfBytesVertexBuffer+nrOfBytesNormalsBuffer) << "," << '\n';
            filestr << "\"byteLength\" : " << std::to_string(nrOfBytesUVBuffer) << "," << '\n';
            filestr << "\"target\" : 34962" << '\n';
            filestr << "}" << '\n';
        }
    }
    filestr << "]," << '\n';

    //accessors
    //index accessor
    filestr << "\"accessors\" : [" << '\n';
    filestr << "{" << '\n';
    filestr << "\"bufferView\" : 0," << '\n';
    filestr << "\"byteOffset\" : 0," << '\n';
    filestr << "\"componentType\" : 5125," << '\n';
    filestr << "\"count\" : " << std::to_string(3*rFaceProps.size()) << "," << '\n';
    filestr << "\"type\" : \"SCALAR\"," << '\n';
    filestr << "\"max\" : [" << std::to_string(maxIndexValue) << "]," << '\n';
    filestr << "\"min\" : [ 0 ]" << '\n';
    filestr << "}," << '\n';

    //-----------------------------------------------------
    //vertex accessor
    filestr << "{" << '\n';
    filestr << "\"bufferView\" : 1," << '\n';
    filestr << "\"byteOffset\" : 0," << '\n';
    filestr << "\"componentType\" : 5126," << '\n';
    //filestr << "\"count\" : " << std::to_string(index) << "," << '\n';
    filestr << "\"count\" : " << std::to_string(nrOfPrimitives) << "," << '\n';
    filestr << "\"type\" : \"VEC3\"," << '\n';
    //keep the dot save in the export. Location configuration of the system could lead to comma export of floats
    std::string maxXString = std::to_string(maxPosition[0]);
    std::replace( maxXString.begin(), maxXString.end(), ',', '.');
    std::string maxYString = std::to_string(maxPosition[1]);
    std::replace( maxYString.begin(), maxYString.end(), ',', '.');
    std::string maxZString = std::to_string(maxPosition[2]);
    std::replace( maxZString.begin(), maxZString.end(), ',', '.');
    std::string minXString = std::to_string(minPosition[0]);
    std::replace( minXString.begin(), minXString.end(), ',', '.');
    std::string minYString = std::to_string(minPosition[1]);
    std::replace( minYString.begin(), minYString.end(), ',', '.');
    std::string minZString = std::to_string(minPosition[2]);
    std::replace( minZString.begin(), minZString.end(), ',', '.');

    filestr << "\"max\" : [ " << maxXString << "," << maxYString << "," << maxZString << "]," << '\n';
    filestr << "\"min\" : [ " << minXString << "," << minYString << "," << minZString << "]" << '\n';
    if (!mExportVertNormal && !mExportTextureCoordinates){
        filestr << "}" << '\n';
    }
    else{
        filestr << "}," << '\n';
        if(mExportVertNormal){
            filestr << "{" << '\n';
            //------------------------------------------------------
            //normal accesor
            filestr << "\"bufferView\" : 2," << '\n';
            filestr << "\"byteOffset\" : 0," << '\n';
            filestr << "\"componentType\" : 5126," << '\n';
            filestr << "\"count\" : " << std::to_string(nrOfPrimitives) << "," << '\n';
            filestr << "\"type\" : \"VEC3\"," << '\n';
            std::setprecision(16);
            std::string maxNormalXString = std::to_string(maxNormals[0]);
            std::replace( maxNormalXString.begin(), maxNormalXString.end(), ',', '.');
            std::string maxNormalYString = std::to_string(maxNormals[1]);
            std::replace( maxNormalYString.begin(), maxNormalYString.end(), ',', '.');
            std::string maxNormalZString = std::to_string(maxNormals[2]);
            std::replace( maxNormalZString.begin(), maxNormalZString.end(), ',', '.');
            std::string minNormalXString = std::to_string(minNormals[0]);
            std::replace( minNormalXString.begin(), minNormalXString.end(), ',', '.');
            std::string minNormalYString = std::to_string(minNormals[1]);
            std::replace( minNormalYString.begin(), minNormalYString.end(), ',', '.');
            std::string minNormalZString = std::to_string(minNormals[2]);
            std::replace( minNormalZString.begin(), minNormalZString.end(), ',', '.');
            std::setprecision(6);

            filestr << "\"max\" : [ " << maxNormalXString << "," << maxNormalYString << "," << maxNormalZString  << "]," << '\n';
            filestr << "\"min\" : [ " << minNormalXString << "," << minNormalYString << "," << minNormalZString  << "]" << '\n';

            if (!mExportTextureCoordinates){
                filestr << "}" << '\n';
            }
            else{
                filestr << "}," << '\n';
            }
        }
        //-----------------------------------------------------------
        //uv buffer / texture coords accesor
        if(mExportTextureCoordinates){
            filestr << "{" << '\n';
            //bufferView ID depends on the normals export decision
            if (mExportVertNormal){
                filestr << "\"bufferView\" : 3," << '\n';
            }
            else{
                filestr << "\"bufferView\" : 2," << '\n';
            }
            filestr << "\"byteOffset\" : 0," << '\n';
            filestr << "\"componentType\" : 5126," << '\n';
            filestr << "\"count\" : " << std::to_string(nrOfPrimitives) << "," << '\n';
            filestr << "\"type\" : \"VEC2\"," << '\n';
            std::string maxUString = std::to_string(maxTextureCoords[0]);
            std::replace( maxUString.begin(), maxUString.end(), ',', '.');
            std::string maxVString = std::to_string(maxTextureCoords[1]);
            std::replace( maxVString.begin(), maxVString.end(), ',', '.');
            std::string minUString = std::to_string(minTextureCoords[0]);
            std::replace( minUString.begin(), minUString.end(), ',', '.');
            std::string minVString = std::to_string(minTextureCoords[1]);
            std::replace( minVString.begin(), minVString.end(), ',', '.');

            filestr << "\"max\" : [ " << maxUString << "," << maxVString << "]," << '\n';
            filestr << "\"min\" : [ " << minUString << "," << minVString << "]" << '\n';
            filestr << "}" << '\n';
        }
    }
    filestr << "]," << '\n';

    //GLTF version
    filestr << "\"asset\" : {" << '\n';
    filestr << "\"version\" : \"2.0\"" << '\n';
    filestr << "}" << '\n';
    filestr << "}" << '\n';

    filestr.close();

    LOG::debug() << "[GltfWriter::" << __FUNCTION__ << "] write ASCII GLTF:      " << static_cast<float>( clock() -timeStart ) / CLOCKS_PER_SEC << " seconds. " << '\n';

    return true;
}
