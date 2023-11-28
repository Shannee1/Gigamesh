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
    filestr << "\"POSITION\" : 1," << '\n';

    if (mExportTextureCoordinates){
       filestr << "\"NORMAL\" : 2," << '\n';
       filestr << "\"TEXCOORD_0\" : 3" << '\n';
    }
    else
    {
       filestr << "\"NORMAL\" : 2" << '\n';
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
    //string indexBuffer = "";
    std::vector<BYTE> indexBuffer;
    std::vector<BYTE> vertexBuffer;
    std::vector<BYTE> normalsBuffer;
    std::vector<BYTE> uvCoordsBuffer;

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

    //the variables have to be declared outside the 'if'
    //otherwise it leads to a compiler error in the section below (texture coords buffer)
    float minU;
    float minV;
    float maxU;
    float maxV;
    if(mExportTextureCoordinates){
        float minU = 1.0 - (float) rFaceProps[0].textureCoordinates[0];
        float minV = (float) rFaceProps[0].textureCoordinates[1];
        float maxU = 1.0 - (float) rFaceProps[0].textureCoordinates[0];
        float maxV = (float) rFaceProps[0].textureCoordinates[1];
    }

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


            char const * coordYStream = (char const *)&coordY;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)coordYStream[i];
                unsigned char byteAsChar = byteValue;
                vertexBuffer.push_back(byteAsChar);
            }

            char const * coordZStream = (char const *)&coordZ;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)coordZStream[i];
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

            char const * normalYStream = (char const *)&normalY;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)normalYStream[i];
                unsigned char byteAsChar = byteValue;
                normalsBuffer.push_back(byteAsChar);
            }

            char const * normalZStream = (char const *)&normalZ;
            for (size_t i = 0; i != sizeof(float); ++i)
            {
                int byteValue = (int)normalZStream[i];
                unsigned char byteAsChar = byteValue;
                normalsBuffer.push_back(byteAsChar);
            }

            //--------------------------
            //Texture Coords (UV)
            //--------------------------
            if (mExportTextureCoordinates){
                //save texture coords as float
                //mirrow the values
                float coordU = (float) faceProp.textureCoordinates[2*textureIndex];
                float coordV = 1.0- (float) faceProp.textureCoordinates[2*textureIndex + 1];

                //update min max
                if (coordU < minU){
                    minU = coordU;
                }
                if (coordV < minV){
                    minV = coordV;
                }
                if (coordU > maxU){
                    maxU = coordU;
                }
                if (coordV > maxV){
                    maxV = coordV;
                }


                char const * coordUStream = (char const *)&coordU;
                for (size_t i = 0; i != sizeof(float); ++i)
                {
                    int byteValue = (int)coordUStream[i];
                    unsigned char byteAsChar = byteValue;
                    uvCoordsBuffer.push_back(byteAsChar);
                }

                char const * coordVStream = (char const *)&coordV;
                for (size_t i = 0; i != sizeof(float); ++i)
                {
                    int byteValue = (int)coordVStream[i];
                    unsigned char byteAsChar = byteValue;
                    uvCoordsBuffer.push_back(byteAsChar);
                }

            }
            textureIndex++;
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

    filestr << "{" << '\n';
    filestr << "\"buffer\" : 0," << '\n';
    filestr << "\"byteOffset\" : " << std::to_string(nrOfBytesIndexBufferPlusOffset) << "," << '\n';
    filestr << "\"byteLength\" : " << std::to_string(nrOfBytesVertexBuffer + nrOfBytesNormalsBuffer + nrOfBytesUVBuffer) << "," << '\n';
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

    //-----------------------------------------------------
    //vertex accessor
    filestr << "{" << '\n';
    filestr << "\"bufferView\" : 1," << '\n';
    filestr << "\"byteOffset\" : 0," << '\n';
    filestr << "\"componentType\" : 5126," << '\n';
    filestr << "\"count\" : " << std::to_string(index) << "," << '\n';
    filestr << "\"type\" : \"VEC3\"," << '\n';
    //keep the dot save in the export. Location configuration of the system could lead to comma export of floats
    std::string maxXString = std::to_string(maxX);
    std::replace( maxXString.begin(), maxXString.end(), ',', '.');
    std::string maxYString = std::to_string(maxY);
    std::replace( maxYString.begin(), maxYString.end(), ',', '.');
    std::string maxZString = std::to_string(maxZ);
    std::replace( maxZString.begin(), maxZString.end(), ',', '.');
    std::string minXString = std::to_string(minX);
    std::replace( minXString.begin(), minXString.end(), ',', '.');
    std::string minYString = std::to_string(minY);
    std::replace( minYString.begin(), minYString.end(), ',', '.');
    std::string minZString = std::to_string(minZ);
    std::replace( minZString.begin(), minZString.end(), ',', '.');

    filestr << "\"max\" : [ " << maxXString << "," << maxYString << "," << maxZString << "]," << '\n';
    filestr << "\"min\" : [ " << minXString << "," << minYString << "," << minZString << "]" << '\n';
    filestr << "}," << '\n';
    filestr << "{" << '\n';
    //------------------------------------------------------
    //normal accesor
    filestr << "\"bufferView\" : 1," << '\n';
    //byte offset inside the second buffer view
    filestr << "\"byteOffset\" : " << std::to_string(nrOfBytesVertexBuffer) << "," << '\n';
    filestr << "\"componentType\" : 5126," << '\n';
    filestr << "\"count\" : " << std::to_string(index) << "," << '\n';
    filestr << "\"type\" : \"VEC3\"," << '\n';
    std::string maxNormalXString = std::to_string(maxNormalX);
    std::replace( maxNormalXString.begin(), maxNormalXString.end(), ',', '.');
    std::string maxNormalYString = std::to_string(maxNormalY);
    std::replace( maxNormalYString.begin(), maxNormalYString.end(), ',', '.');
    std::string maxNormalZString = std::to_string(maxNormalZ);
    std::replace( maxNormalZString.begin(), maxNormalZString.end(), ',', '.');
    std::string minNormalXString = std::to_string(minNormalX);
    std::replace( minNormalXString.begin(), minNormalXString.end(), ',', '.');
    std::string minNormalYString = std::to_string(minNormalY);
    std::replace( minNormalYString.begin(), minNormalYString.end(), ',', '.');
    std::string minNormalZString = std::to_string(minNormalZ);
    std::replace( minNormalZString.begin(), minNormalZString.end(), ',', '.');


    filestr << "\"max\" : [ " << maxNormalXString << "," << maxNormalYString << "," << maxNormalZString  << "]," << '\n';
    filestr << "\"min\" : [ " << minNormalXString << "," << minNormalYString << "," << minNormalZString  << "]" << '\n';
    if (!mExportTextureCoordinates){
        filestr << "}" << '\n';
    }
    //-----------------------------------------------------------
    //uv buffer / texture coords accesor
    else{
        filestr << "}," << '\n';
        filestr << "{" << '\n';
        filestr << "\"bufferView\" : 1," << '\n';
        //byte offset inside the second buffer view
        filestr << "\"byteOffset\" : " << std::to_string(nrOfBytesVertexBuffer+nrOfBytesNormalsBuffer) << "," << '\n';
        filestr << "\"componentType\" : 5126," << '\n';
        filestr << "\"count\" : " << std::to_string(index) << "," << '\n';
        filestr << "\"type\" : \"VEC2\"," << '\n';
        std::string maxUString = std::to_string(maxU);
        std::replace( maxUString.begin(), maxUString.end(), ',', '.');
        std::string maxVString = std::to_string(maxV);
        std::replace( maxVString.begin(), maxVString.end(), ',', '.');
        std::string minUString = std::to_string(minU);
        std::replace( minUString.begin(), minUString.end(), ',', '.');
        std::string minVString= std::to_string(minV);
        std::replace( minVString.begin(), minVString.end(), ',', '.');

        filestr << "\"max\" : [ " << maxUString << "," << maxVString << "]," << '\n';
        filestr << "\"min\" : [ " << minUString << "," << minVString << "]" << '\n';
        filestr << "}" << '\n';
    }

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
