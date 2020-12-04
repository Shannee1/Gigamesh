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

#include "PlyWriter.h"
#include <fstream>
#include <iostream>     //! \todo remove
#include <sstream>
#include <chrono>
#include <locale>
#include <filesystem>
#include <GigaMesh/mesh/primitive.h>
#include "PlyEnums.h"

#include <GigaMesh/logging/Logging.h>

using namespace std;


PlyWriter::PlyWriter()
{

}


bool PlyWriter::writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed)
{
	//! Supports:
	fstream filestr;
	filestr.imbue(std::locale("C"));

	// for performance mesurement
	using namespace std::chrono;
	high_resolution_clock::time_point tStart, tEnd;


	bool exportTextureId = MeshWriter::getModelMetaDataRef().getTexturefilesRef().size() > 1;
	// Measure total time:
	tStart = high_resolution_clock::now();
	LOG::info() << "[PlyWriter::" << __FUNCTION__ << "] ------------------------------------------------------------\n";

	filestr.open( rFilename, fstream::out | fstream::binary);
	if( !filestr.is_open() ) {
		LOG::error() << "[PlyWriter::" << __FUNCTION__ <<"] ERROR: Could not open file: '" << rFilename << "'!\n";
		return false;
	} else {
		LOG::info() << "[PlyWriter::" << __FUNCTION__ <<"] File open for writing: '" << rFilename << "'.\n";
	}

	// if we got a vertex texture_
	//! \todo Make these hard-coded flags available thru the UI.
	//! \todo Detect if color per vertex is present.
	// old=dead: if( vertexTexture == nullptr ) {
	mExportVertColor = true; //why?
	if( rMeshSeed.getFeatureVecLenMax( Primitive::IS_VERTEX ) <= 0 ) {
		mExportVertFeatureVector  = false;
	}

	// Header:
	filestr << "ply\n";
	if( mExportBinary ) {
		if( mIsBigEndian ) {
			filestr << "format binary_big_endian 1.0\n";
		} else {
			filestr << "format binary_little_endian 1.0\n";
		}
	} else {
		filestr << "format ascii 1.0\n";
	}
	filestr << "comment +-------------------------------------------------------------------------------+\n";
	filestr << "comment | PLY file generated by GigaMesh Software Framework                             |\n";
	filestr << "comment +-------------------------------------------------------------------------------+\n";
	filestr << "comment | WebSite: https://gigamesh.eu                                                  |\n";
	filestr << "comment | EMail:   info@gigamesh.eu                                                     |\n";
	filestr << "comment +-------------------------------------------------------------------------------+\n";
	filestr << "comment | Contact: Hubert MARA <hubert.mara@iwr.uni-heidelberg.de>                      |\n";
	filestr << "comment |          IWR - Heidelberg University, Germany                                 |\n";
	filestr << "comment +-------------------------------------------------------------------------------+\n";
	filestr << "comment | GigaMesh compiled                                                             |\n";

#ifdef COMP_USER
	filestr << "comment | .... by: " << COMP_USER << "\n";
#else
	filestr << "comment | .... by: UNKNOWN                                                              |\n";
#endif
#ifdef COMP_DATE
	filestr << "comment | .... at: " << COMP_DATE << "\n";
#else
	filestr << "comment | .... at: UNKNOWN                                                              |\n";
#endif
#ifdef COMP_EDIT
	filestr << "comment | ... for: " << COMP_EDIT << "\n";
#else
	filestr << "comment | ... for: UNKNOWN                                                              |\n";
#endif
#ifdef COMP_GITHEAD
	filestr << "comment | ... git-head: " << COMP_GITHEAD << "\n";
#else
	filestr << "comment | ... git-head: UNKNOWN                                                         |\n";
#endif
#ifdef VERSION_PACKAGE
	filestr << "comment | .... Version: " << VERSION_PACKAGE << "\n";
#else
	filestr << "comment | .... Version: UNKNOWN                                                              |\n";
#endif

	filestr << "comment +-------------------------------------------------------------------------------+\n";
	filestr << "comment | Meta information:                                                             |\n";
	filestr << "comment +-------------------------------------------------------------------------------+\n";
	for( uint64_t i=0; i<ModelMetaData::META_STRINGS_COUNT; i++ ) {
		auto metaId = static_cast<ModelMetaData::eMetaStrings>( i );
		if( metaId == ModelMetaData::META_FILENAME ) { // Ignore the filename!
			continue;
		}
		string metaStr = MeshWriter::getModelMetaDataRef().getModelMetaString( metaId );
		if( metaStr.empty()) { // Ignore empty strings!
			continue;
                }

		string metaName;
		if( MeshWriter::getModelMetaDataRef().getModelMetaStringName( metaId, metaName ) ) {
			if(metaId == ModelMetaData::META_TEXTUREFILE)
			{
				continue;	//we use the textures stored in getTexturefilesRef instead
                        }
                        if( metaId == ModelMetaData::META_USER_DATA ) { // write user data to several lines

                                std::string userMetaString;
                                std::istringstream ss(metaStr);
                                std::string line;
                                while(std::getline(ss, line)){
                                    userMetaString += line + "\ncomment ";
                                }

                                filestr << "comment " << metaName << " " << userMetaString << "\n";
                                continue;
                        }

			filestr << "comment " << metaName << " " << metaStr << "\n";
		}
	}

	if(!MeshWriter::getModelMetaDataRef().getTexturefilesRef().empty())
	{
		for(const auto& texName : MeshWriter::getModelMetaDataRef().getTexturefilesRef())
		{
			auto prevPath = std::filesystem::current_path();
			std::filesystem::current_path(std::filesystem::absolute(rFilename).parent_path());
			filestr << "comment TextureFile " << std::filesystem::relative(texName).string() << "\n";
			std::filesystem::current_path(prevPath);
		}
	}
	filestr << "comment +-------------------------------------------------------------------------------+\n";

	filestr << "element vertex " << rVertexProps.size() << "\n";
	//! .) Vertices: x, y and z coordinates.
	filestr << "property float x\n";
	filestr << "property float y\n";
	filestr << "property float z\n";
	//! .) Vertex quality will be set to the function value.
	filestr << "property float quality\n";
	//! .) Vertex flags will be exported.
	if( mExportVertFlags ) {
		filestr << "property int flags\n";
	}
	//! .) Texture: Color per vertex, when present.
	if( mExportVertColor ) {
		filestr << "property uint8 red\n";
		filestr << "property uint8 green\n";
		filestr << "property uint8 blue\n";
		//! \todo Alpha is ignored and not written to output.
	}
	//! .) Normal per vertex
	if( mExportVertNormal ) {
		filestr << "property float nx\n";
		filestr << "property float ny\n";
		filestr << "property float nz\n";
	}
	//! .) Label ID per vertex
	if( mExportVertLabel ) {
		filestr << "property uint32 labelid\n";
	}
	//! .) Proprietary support for feature vectors, when present.
	if( mExportVertFeatureVector ) {
		filestr << "property list uint8 float feature_vector\n";
	}

	//! .) Faces referencing to Vertices - optional, because they are not required by point clouds.
	if( rFaceProps.size() > 0 ) {
		filestr << "element face " << rFaceProps.size() << "\n";
		filestr << "property list uchar int32 vertex_indices\n";

		if(mExportTextureCoordinates)
		{
			filestr << "property list uchar float texcoord\n";
		}
		if(exportTextureId)
		{
			filestr << "property int texnumber\n";
		}
	}

	//! .) Proprietary support for polygonal lines (generated by user and/or algorithm), when present.
	if( ( mExportPolyline ) && ( rMeshSeed.getPolyLineNr() > 0 ) ) {
		filestr << "element line " << rMeshSeed.getPolyLineNr() << "\n";
		filestr << "property float x\n";
		filestr << "property float y\n";
		filestr << "property float z\n";
		filestr << "property float nx\n";
		filestr << "property float ny\n";
		filestr << "property float nz\n";
		filestr << "property uint32 labelid\n";
		filestr << "property list int32 int32 vertex_indices\n";
	}

	//! .) End of header tag.
	filestr << "end_header\n";

	if( !mExportBinary ) {
		//! \todo Test ASCII export with and without texture.
		// === ASCII mode ===========================================================
		// --- Vertices -------------------------------------------------------------
		uint64_t featureVecLenMax = rMeshSeed.getFeatureVecLenMax( Primitive::IS_VERTEX );

		int currVertNum = 0;
		for(const auto& vertexProp : rVertexProps)
		{
			filestr << vertexProp.mCoordX  << " ";   // x-coordinate
			filestr << vertexProp.mCoordY  << " ";   // y-coordinate
			filestr << vertexProp.mCoordZ  << " ";   // z-coordinate
			filestr << vertexProp.mFuncVal;          // function value stored within PLY_QUALITY
			if( mExportVertFlags ) {
				filestr << " ";
				filestr << static_cast<unsigned int>(vertexProp.mFlags); // flags lower half (int32 vs long64!)
			}
			if( mExportVertColor ) {
				filestr << " ";
				filestr << static_cast<unsigned int>(vertexProp.mColorRed)   << " ";
				filestr << static_cast<unsigned int>(vertexProp.mColorGrn) << " ";
				filestr << static_cast<unsigned int>(vertexProp.mColorBle);
				//! \todo Alpha is ignored and not written to output.
			}
			if( mExportVertNormal ) {
				filestr << " ";
				filestr << vertexProp.mNormalX << " ";
				filestr << vertexProp.mNormalY << " ";
				filestr << vertexProp.mNormalZ;
			}
			if( mExportVertLabel ) {
				filestr << " ";
				filestr << vertexProp.mLabelId;
			}
			if( mExportVertFeatureVector ) {
				//! \todo test: write ASCII PLY with features
				filestr << " ";
				filestr << featureVecLenMax << " ";
				for( uint64_t j=0; j<rMeshSeed.getFeatureVecLenMax( Primitive::IS_VERTEX ); j++ ) {
					auto featureToWrite = static_cast<float>(rMeshSeed.getFeatureVecVerticesRef().at( featureVecLenMax*currVertNum+j ));
					//filestr << getFeatureVertex( i, j ) << " ";
					filestr << featureToWrite << " ";
				}
			}
			filestr << "\n";
			++currVertNum;
		}

		// --- Faces ----------------------------------------------------------------
		for(const auto & rFaceProp : rFaceProps) {
			// PLYs start with ZERO! So no +1 needed (in contrast to OBJ)
			filestr << rFaceProp.vertexIndices.size();

			for(auto index : rFaceProp.vertexIndices)
			{
				filestr << " " << index;
			}

			if(mExportTextureCoordinates)
			{
				filestr << " " << rFaceProp.textureCoordinates.size();
				for(auto texCoord : rFaceProp.textureCoordinates)
				{
					filestr << " " << texCoord;
				}
			}
			if(exportTextureId)
			{
				filestr << " " << static_cast<unsigned short>(rFaceProp.textureId);
			}
			filestr << "\n";
		}
		// ---- Polygonal lines -----------------------------------------------------
		for( unsigned int i=0; i<rMeshSeed.getPolyLineNr(); i++ ) {
			PrimitiveInfo primInfo = rMeshSeed.getPolyLinePrimInfo( i );
			filestr << primInfo.mPosX << " ";
			filestr << primInfo.mPosY << " ";
			filestr << primInfo.mPosZ << " ";
			filestr << primInfo.mNormalX << " ";
			filestr << primInfo.mNormalY << " ";
			filestr << primInfo.mNormalZ << " ";
			// The label ID:                      property uint32 labelid
			unsigned int polyLabelID = rMeshSeed.getPolyLineLabel( i );
			filestr << polyLabelID;
			filestr << " ";
			// Than a list of vertex references:  property list int32 int32 vertex_indices
			filestr << rMeshSeed.getPolyLineLength( i );
			// Finally the references to the vertices:
			for( unsigned int j=0; j<rMeshSeed.getPolyLineLength( i ); j++ ) {
				filestr << " " << rMeshSeed.getPolyLineVertIdx( i, j );
			}
			filestr << "\n";
		}
	} else {
			// === Binary mode ===
			// --- Vertices -------------------------------------------------------------
			high_resolution_clock::time_point tStartVertices = high_resolution_clock::now();
			unsigned char texVal;
			unsigned long featureVecLenMax = rMeshSeed.getFeatureVecLenMax( Primitive::IS_VERTEX );
			int currVertNum = 0;
			for( const auto& vertexProp : rVertexProps) {
				float someFloat = static_cast<float>( vertexProp.mCoordX );
				filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 ); // floats have 4 bytes in a binary PLY
				someFloat = static_cast<float>( vertexProp.mCoordY );
				filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 );
				someFloat = static_cast<float>( vertexProp.mCoordZ );
				filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 );
				someFloat = static_cast<float>( vertexProp.mFuncVal );   // function value stored ...
				filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 ); // ... within PLY_QUALITY
				if( mExportVertFlags) {
					int vertFlags = static_cast<int>(vertexProp.mFlags);
					filestr.write( reinterpret_cast<char*>(&vertFlags), PLY_INT32 );
				}
				if( mExportVertColor ) {
					texVal = vertexProp.mColorRed;
					filestr.write( reinterpret_cast<char*>(&texVal), PLY_UINT8 );
					texVal = vertexProp.mColorGrn;
					filestr.write( reinterpret_cast<char*>(&texVal), PLY_UINT8 );
					texVal = vertexProp.mColorBle;
					filestr.write( reinterpret_cast<char*>(&texVal), PLY_UINT8 );
					//! \todo Alpha is ignored and not written.
				}
				if( mExportVertNormal ) {
					float someFloat = static_cast<float>(vertexProp.mNormalX);
					filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 ); // floats have 4 bytes in a binary PLY
					someFloat = static_cast<float>(vertexProp.mNormalY);
					filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 );
					someFloat = static_cast<float>(vertexProp.mNormalZ);
					filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 );
				}
				if( mExportVertLabel ) {
					unsigned int vertLabelID = vertexProp.mLabelId;
					filestr.write( reinterpret_cast<char*>(&vertLabelID), PLY_UINT32 );
				}
				if( mExportVertFeatureVector ) {
					filestr << static_cast<unsigned char>(featureVecLenMax);
					for( uint64_t j=0; j<featureVecLenMax; j++ ) {
						float featureToWrite = static_cast<float>(rMeshSeed.getFeatureVecVerticesRef().at( featureVecLenMax*currVertNum+j ));
						filestr.write( reinterpret_cast<char*>(&(featureToWrite)), PLY_FLOAT32 );
					}
				}
				++currVertNum;
			}
			high_resolution_clock::time_point tEndVertices = high_resolution_clock::now();
			duration<double> time_span_Vertices = duration_cast<duration<double>>( tEndVertices - tStartVertices );
			LOG::info() << "[PlyWriter::" << __FUNCTION__ << "] write Vertices:         " << time_span_Vertices.count() << " seconds.\n";
			// --- Faces ----------------------------------------------------------------
			{
				high_resolution_clock::time_point tStartFaces = high_resolution_clock::now();
				for(const auto & rFaceProp : rFaceProps) {
					const char numberOfVerticesPerFace = static_cast<char>(rFaceProp.vertexIndices.size());
					// PLYs start with ZERO!
					filestr.write( &numberOfVerticesPerFace, PLY_UCHAR ); // uchar have 1 byte in a binary PLY

					for(auto index : rFaceProp.vertexIndices)
					{
						uint32_t someIdx = static_cast<uint32_t>(index);
						filestr.write( reinterpret_cast<char*>(&someIdx), PLY_INT32 ); // floats have 4 bytes in a binary PLY
					}

					if(mExportTextureCoordinates)
					{
						const char numberOfTexcoords = static_cast<char>(rFaceProp.textureCoordinates.size());
						filestr.write(&numberOfTexcoords, PLY_UCHAR);

						for(auto texCoord : rFaceProp.textureCoordinates)
						{
							filestr.write( reinterpret_cast<char*>(&texCoord), PLY_FLOAT32);
						}
					}
					if(exportTextureId)
					{
						int texID = rFaceProp.textureId;
						filestr.write( reinterpret_cast<char*>(&texID), PLY_INT32);
					}
				}
				high_resolution_clock::time_point tEndFaces = high_resolution_clock::now();
				duration<double> time_span_Faces = duration_cast<duration<double>>( tEndFaces - tStartFaces );
				LOG::info() << "[PlyWriter::" << __FUNCTION__ << "] write Faces:            " << time_span_Faces.count() << " seconds.\n";
			}
			// ---- Polygonal lines -----------------------------------------------------
			if( ( mExportPolyline ) && ( rMeshSeed.getPolyLineNr() > 0 ) ) {
				high_resolution_clock::time_point tStartPolylines = high_resolution_clock::now();
				for( unsigned int i=0; i<rMeshSeed.getPolyLineNr(); i++ ) {
					PrimitiveInfo primInfo = rMeshSeed.getPolyLinePrimInfo( i );
					float someFloat = static_cast<float>(primInfo.mPosX);
					filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 ); // floats have 4 bytes in a binary PLY
					someFloat = static_cast<float>(primInfo.mPosY);
					filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 ); // floats have 4 bytes in a binary PLY
					someFloat = static_cast<float>(primInfo.mPosZ);
					filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 ); // floats have 4 bytes in a binary PLY
					someFloat = static_cast<float>(primInfo.mNormalX);
					filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 ); // floats have 4 bytes in a binary PLY
					someFloat = static_cast<float>(primInfo.mNormalY);
					filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 ); // floats have 4 bytes in a binary PLY
					someFloat = static_cast<float>(primInfo.mNormalZ);
					filestr.write( reinterpret_cast<char*>(&someFloat), PLY_FLOAT32 ); // floats have 4 bytes in a binary PLY
					// The label ID:                      property uint32 labelid
					unsigned int polyLabelID = rMeshSeed.getPolyLineLabel( i );
					filestr.write( reinterpret_cast<char*>(&polyLabelID), PLY_UINT32 );
					// Than a list of vertex references:  property list int32 int32 vertex_indices
					int polyLen = static_cast<int>(rMeshSeed.getPolyLineLength( i ));
					filestr.write( reinterpret_cast<char*>(&polyLen), PLY_INT32 );
					// Finally the references to the vertices:
					for( unsigned int j=0; j<rMeshSeed.getPolyLineLength( i ); j++ ) {
						int vertIdx = rMeshSeed.getPolyLineVertIdx( i, j );
						filestr.write( reinterpret_cast<char*>(&vertIdx), PLY_INT32 );
					}
				}
				high_resolution_clock::time_point tEndPolylines = high_resolution_clock::now();
				duration<double> time_span_Polylines = duration_cast<duration<double>>( tEndPolylines - tStartPolylines );
				LOG::info() << "[PlyWriter::" << __FUNCTION__ << "] write Polylines:        " << time_span_Polylines.count() << " seconds.\n";
			} else {
				LOG::info() << "[PlyWriter::" << __FUNCTION__ << "] write Polylines:        None present or export disabled.\n";
			}
		}

		filestr.close();

		tEnd = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>( tEnd - tStart );
		LOG::info() << "[PlyWriter::" << __FUNCTION__ << "] ------------------------------------------------------------\n";
		LOG::info() << "[PlyWriter::" << __FUNCTION__ << "] write TOTAL:            " << time_span.count() << " seconds.\n";
		LOG::info() << "[PlyWriter::" << __FUNCTION__ << "] ------------------------------------------------------------\n";

	return true;
}
