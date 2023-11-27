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
    filestr << "\"POSITION\" : 1" << '\n';
    filestr << "}," << '\n';
    filestr << "\"indices\" : 0" << '\n';
    filestr << "} ]" << '\n';
    filestr << "}" << '\n';
    filestr << "]," << '\n';
    //Buffer contains the actual data

    unsigned int index = 0;
    //string indexBuffer = "";
    std::vector<BYTE> indexBuffer;
    std::vector<BYTE> vertexBuffer;
    std::vector<BYTE> normalsBuffer;

    //save min max of the vectors for the accessors
    float minX = (float) rVertexProps[0].mCoordX;
    float maxX = (float) rVertexProps[0].mCoordX;
    float minY = (float) rVertexProps[0].mCoordY;
    float maxY = (float) rVertexProps[0].mCoordY;
    float minZ = (float) rVertexProps[0].mCoordZ;
    float maxZ = (float) rVertexProps[0].mCoordZ;
    float minNormalX = (float) rVertexProps[0].mNormalX;
    float maxNormalX = (float) rVertexProps[0].mNormalX;
    float minNormalY = (float) rVertexProps[0].mNormalY;
    float maxNormalY = (float) rVertexProps[0].mNormalY;
    float minNormalZ = (float) rVertexProps[0].mNormalZ;
    float maxNormalZ = (float) rVertexProps[0].mNormalZ;

    //add all faces to file buffer
    for( const auto& faceProp : rFaceProps) {
        //add for each face the assigned vertices
        for( const auto& faceVertexIndex: faceProp.vertexIndices){

            // get the bytes
            int a = (index >>  24) & 0xff;  // next byte, bits 24-32
            int b = (index >>  16) & 0xff;  // next byte, bits 16-23
            int c = (index >>  8) & 0xff;  // next byte, bits 8-15
            int d = index         & 0xff;  // low-order byte: bits 0-7

            //change the parts to achieve little endian stringt
            //indexBuffer += indexHexPart2;
            //indexBuffer += indexHexPart1;
            unsigned char part1Byte = a;
            unsigned char part2Byte = b;
            unsigned char part3Byte = c;
            unsigned char part4Byte = d;
            indexBuffer.push_back(part4Byte);
            indexBuffer.push_back(part3Byte);
            indexBuffer.push_back(part2Byte);
            indexBuffer.push_back(part1Byte);

            //---------------------
            //Vertices
            //---------------------

            //save vertex coords as float
            float coordX = (float) rVertexProps[faceVertexIndex].mCoordX;
            float coordY = (float) rVertexProps[faceVertexIndex].mCoordY;
            float coordZ = (float) rVertexProps[faceVertexIndex].mCoordZ;

            //update min max
            if (coordX < minX){
                minX = coordX;
            }
            if (coordY < minY){
                minY = coordY;
            }
            if (coordZ < minZ){
                minZ = coordZ;
            }
            if (coordX > maxX){
                maxX = coordX;
            }
            if (coordY > maxY){
                maxY = coordY;
            }
            if (coordZ > maxZ){
                maxZ = coordZ;
            }

            char const * coordXStream = (char const *)&coordX;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)coordXStream[i];
                unsigned char byteAsChar = byteValue;
                vertexBuffer.push_back(byteAsChar);
            }

            //first z for gltf
            char const * coordZStream = (char const *)&coordZ;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)coordZStream[i];
                unsigned char byteAsChar = byteValue;
                vertexBuffer.push_back(byteAsChar);
            }

            char const * coordYStream = (char const *)&coordY;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)coordYStream[i];
                unsigned char byteAsChar = byteValue;
                vertexBuffer.push_back(byteAsChar);
            }

            //--------------------------
            //Normals
            //--------------------------

            //save vertex coords as float
            float normalX = (float) rVertexProps[faceVertexIndex].mNormalX;
            float normalY = (float) rVertexProps[faceVertexIndex].mNormalY;
            float normalZ = (float) rVertexProps[faceVertexIndex].mNormalZ;

            //update min max
            if (normalX < minNormalX){
                minNormalX = normalX;
            }
            if (normalY < minNormalY){
                minNormalY = normalY;
            }
            if (normalZ < minNormalZ){
                minNormalZ = normalZ;
            }
            if (normalX > maxNormalX){
                maxNormalX = normalX;
            }
            if (normalY > maxNormalY){
                maxNormalY = normalY;
            }
            if (normalZ > maxNormalZ){
                maxNormalZ = normalZ;
            }

            char const * normalXStream = (char const *)&normalX;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)normalXStream[i];
                unsigned char byteAsChar = byteValue;
                normalsBuffer.push_back(byteAsChar);
            }

            //first z for gltf
            char const * normalZStream = (char const *)&normalZ;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)normalZStream[i];
                unsigned char byteAsChar = byteValue;
                normalsBuffer.push_back(byteAsChar);
            }

            char const * normalYStream = (char const *)&normalY;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)normalYStream[i];
                unsigned char byteAsChar = byteValue;
                normalsBuffer.push_back(byteAsChar);
            }

            index++;
        }
    }

    int nrOfBytesIndexBuffer = indexBuffer.size();
    //add two zero bytes
    unsigned char zeroBytes = int(0);
    indexBuffer.push_back(zeroBytes);
    indexBuffer.push_back(zeroBytes);
    int nrOfBytesIndexBufferPlusOffset = indexBuffer.size();

    //Every buffer are concatenated to the final buffer
    //and base64 encoded
    std::vector<BYTE> entireBuffer = indexBuffer;
    entireBuffer.insert(entireBuffer.end(), vertexBuffer.begin(), vertexBuffer.end());
    entireBuffer.insert(entireBuffer.end(), normalsBuffer.begin(), normalsBuffer.end());
    std::string entireBufferEncoded = base64_encode(&entireBuffer[0], entireBuffer.size());
    int nrOfBytesVertexBuffer = vertexBuffer.size();
    int nrOfBytesNormalsBuffer = normalsBuffer.size();
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

    filestr << "{" << '\n';
    filestr << "\"buffer\" : 0," << '\n';
    filestr << "\"byteOffset\" : " << std::to_string(nrOfBytesIndexBufferPlusOffset) << "," << '\n';
    filestr << "\"byteLength\" : " << std::to_string(nrOfBytesVertexBuffer + nrOfBytesNormalsBuffer) << "," << '\n';
    filestr << "\"target\" : 34962" << '\n';
    filestr << "}" << '\n';
    filestr << "]," << '\n';

    //accessors
    //index accessor
    filestr << "\"accessors\" : [" << '\n';
    filestr << "{" << '\n';
    filestr << "\"bufferView\" : 0," << '\n';
    filestr << "\"byteOffset\" : 0," << '\n';
    filestr << "\"componentType\" : 5125," << '\n';
    filestr << "\"count\" : " << std::to_string(index) << "," << '\n';
    filestr << "\"type\" : \"SCALAR\"," << '\n';
    filestr << "\"max\" : [" << std::to_string(index) << "]," << '\n';
    filestr << "\"min\" : [ 0 ]" << '\n';
    filestr << "}," << '\n';
    //vertex accessor
    filestr << "{" << '\n';
    filestr << "\"bufferView\" : 1," << '\n';
    filestr << "\"byteOffset\" : 0," << '\n';
    filestr << "\"componentType\" : 5126," << '\n';
    filestr << "\"count\" : " << std::to_string(index) << "," << '\n';
    filestr << "\"type\" : \"VEC3\"," << '\n';
    filestr << "\"max\" : [ " << std::to_string(maxX) << "," << std::to_string(maxZ) << "," << std::to_string(maxY) << "]," << '\n';
    filestr << "\"min\" : [ " << std::to_string(minX) << "," << std::to_string(minZ) << "," << std::to_string(minY) << "]" << '\n';
    filestr << "}," << '\n';
    filestr << "{" << '\n';
    //normal accesor
    filestr << "\"bufferView\" : 1," << '\n';
    //byte offset inside the second buffer view
    filestr << "\"byteOffset\" : " << std::to_string(nrOfBytesVertexBuffer) << "," << '\n';
    filestr << "\"componentType\" : 5126," << '\n';
    filestr << "\"count\" : " << std::to_string(index) << "," << '\n';
    filestr << "\"type\" : \"VEC3\"," << '\n';
    filestr << "\"max\" : [ " << std::to_string(maxNormalX) << "," << std::to_string(maxNormalZ) << "," << std::to_string(maxNormalY) << "]," << '\n';
    filestr << "\"min\" : [ " << std::to_string(minNormalX) << "," << std::to_string(minNormalZ) << "," << std::to_string(minNormalY) << "]" << '\n';
    filestr << "}" << '\n';
    filestr << "]," << '\n';

    //GLTF version
    filestr << "\"asset\" : {" << '\n';
    filestr << "\"version\" : \"2.0\"" << '\n';
    filestr << "}" << '\n';
    filestr << "}" << '\n';





    filestr.close();

    LOG::debug() << "[ObjWriter::" << __FUNCTION__ << "] write ASCII OBJ:      " << static_cast<float>( clock() -timeStart ) / CLOCKS_PER_SEC << " seconds. " << '\n';

    return true;
}
