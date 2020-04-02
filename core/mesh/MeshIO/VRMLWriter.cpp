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

#include "VRMLWriter.h"
#include <fstream>
#include <iostream>
#include <locale>

using namespace std;

bool VRMLWriter::writeFile(const std::filesystem::path& rFilename, const std::vector<sVertexProperties>& rVertexProps, const std::vector<sFaceProperties>& rFaceProps, MeshSeedExt& rMeshSeed)
{
	fstream filestr;
	filestr.imbue(std::locale("C"));
	filestr.open( rFilename, fstream::out );
	if( !filestr.is_open() ) {
		cerr << "[VRMLWriter] Could not open file: '" << rFilename << "'.\n";
		return false;
	}

	cout << "[MeshIO] File open for writing: '" << rFilename << "'.\n";


	filestr << "#VRML V2.0 utf8\n";
	filestr << "#===============================================================================\n";
	filestr << "# File generated by the GigaMesh Software Framework\n";
	filestr << "#-------------------------------------------------------------------------------\n";
	filestr << "# WebSite: http://gigamesh.eu\n";
	filestr << "# Email:   info@gigamesh.eu\n";
	filestr << "#-------------------------------------------------------------------------------\n";
	filestr << "# Contact: Hubert MARA <hubert.mara@iwr.uni-heidelberg.de>\n";
	filestr << "#          IWR - Heidelberg University, Germany\n";
	filestr << "#===============================================================================\n";
	filestr << "Shape {\n";
	filestr << "\tgeometry IndexedFaceSet {\n";
	// old=dead: if( vertexTexture != nullptr ) {
	filestr << "\t\tcolorPerVertex TRUE\n";
	//} else {
	//	filestr << "\t\tcolorPerVertex FALSE\n";
	//}
	filestr << "\t\tcoord Coordinate {\n";
	filestr << "\t\t\tpoint [\n";
	// vertexIdx -OBJs start with ONE - VRMLs with ZERO!
	for( const auto& vertexProp : rVertexProps) {
		filestr << "\t\t\t\t" << vertexProp.mCoordX << " "
				<< vertexProp.mCoordY << " " << vertexProp.mCoordZ << "\n";
	}
	filestr << "\t\t\t] # point\n";
	filestr << "\t\t} # coord Cordinate\n";

	filestr << "\t\tcoordIndex [\n";
	for( const auto& faceProp : rFaceProps) {
		// VRML index for vertices start with ZERO!!!:
		filestr << "\t\t\t" << faceProp.vertexIndices[0] << " "
		        << faceProp.vertexIndices[1] << " " << faceProp.vertexIndices[2] << " -1\n";
	}
	filestr << "\t\t] # coordIndex\n";

	// old=dead: if( vertexTexture != nullptr ) {
	filestr << "\t\tcolor Color {\n";
	filestr << "\t\t\tcolor [\n";
	for( const auto& vertexProp : rVertexProps) {
		filestr << "\t\t\t\t" << vertexProp.mColorRed << ", "
				<< vertexProp.mColorGrn << ", "
				<< vertexProp.mColorBle  << ", \n";
	}
	filestr << "\t\t\t] # color\n";
	filestr << "\t\t} # color Color\n";
	// old=dead: }
	filestr << "\t} # IndexedFaceSet\n";
	filestr << "} # Shape \n";
	filestr.close();

	cout << "[VRMLWriter] VRML written to: " << rFilename << "\n";
	return true;
}
