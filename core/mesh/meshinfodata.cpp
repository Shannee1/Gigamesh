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

#include <GigaMesh/mesh/meshinfodata.h>

#include <sstream>      // std::stringstream
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision
#include <iostream>     // std::cout
#include <string>       // std::string, std::to_string
#include <math.h>       // sqrt

#include <GigaMesh/mesh/gmcommon.h>

//! Constructer calls MeshInfoData::reset() and sets the names for the enumerators.
MeshInfoData::MeshInfoData() {
	// String names
	mStringName[FILENAME]             = "Filename";
	mStringName[MODEL_ID]             = "Model Id";
	mStringName[MODEL_MATERIAL]       = "Model Material";
	mStringName[MODEL_WEBREFERENCE]   = "Web-Reference";

	// Unsigned long names
	mCountULongName[VERTICES_TOTAL] = "Total number of vertices";
	mCountULongName[VERTICES_NAN] = "Vertices not-a-number";
	mCountULongName[VERTICES_NORMAL_LEN_NORMAL] = "Vertex normal vector length normal";
	mCountULongName[VERTICES_SOLO] = "Vertices solo";
	mCountULongName[VERTICES_POLYLINE] = "Vertices of polylines";
	mCountULongName[VERTICES_BORDER] = "Border vertices";
	mCountULongName[VERTICES_NONMANIFOLD] = "Vertices non-manifold";
	mCountULongName[VERTICES_SINGULAR] = "Vertices singular";
	mCountULongName[VERTICES_ON_INVERTED_EDGE] = "Vertices along an inverted edge";
	mCountULongName[VERTICES_PART_OF_ZERO_FACE] = "Vertices part of a zero area face";
	mCountULongName[VERTICES_SYNTHETIC] = "Vertices synthetic";
	mCountULongName[VERTICES_MANUAL] = "Vertices manually added";
	mCountULongName[VERTICES_CIRCLE_CENTER] = "Vertices circle center";
	mCountULongName[VERTICES_SELECTED] = "Vertices selected";
	mCountULongName[VERTICES_FUNCVAL_FINITE] = "Vertices with finite function value";
	mCountULongName[VERTICES_FUNCVAL_LOCAL_MIN] = "Vertices with local function value minimum";
	mCountULongName[VERTICES_FUNCVAL_LOCAL_MAX] = "Vertices with local function value maximum";
	mCountULongName[FACES_TOTAL] = "Total number of faces";
	mCountULongName[FACES_SOLO] = "Solo faces";
	mCountULongName[FACES_BORDER] = "Border faces";
	mCountULongName[FACES_BORDER_THREE_VERTICES] = "Faces with three border vertices";
	mCountULongName[FACES_BORDER_BRDIGE_TRICONN] = "Border faces bridge tri-connection";
	mCountULongName[FACES_BORDER_BRDIGE] = "Bridge border faces";
	mCountULongName[FACES_BORDER_DANGLING] = "Dangling border faces";
	mCountULongName[FACES_MANIFOLD] = "Manifold faces";
	mCountULongName[FACES_NONMANIFOLD] = "Non-manifold faces";
	mCountULongName[FACES_STICKY] = "Sticky faces";
	mCountULongName[FACES_ZEROAREA] = "Faces with zero area";
	mCountULongName[FACES_INVERTED] = "Inverted Faces";
	mCountULongName[FACES_SELECTED] = "Selected Faces";
	mCountULongName[FACES_WITH_SYNTH_VERTICES] = "Faces with synthetic vertices";
	mCountULongName[CONNECTED_COMPONENTS] = "Connected components";

	// Double names
	mmCountDoubleName[BOUNDINGBOX_MIN_X]    = "Minimum x coordinate";
	mmCountDoubleName[BOUNDINGBOX_MIN_Y]    = "Minimum y coordinate";
	mmCountDoubleName[BOUNDINGBOX_MIN_Z]    = "Minimum z coordinate";
	mmCountDoubleName[BOUNDINGBOX_MAX_X]    = "Maximum x coordinate";
	mmCountDoubleName[BOUNDINGBOX_MAX_Y]    = "Maximum y coordinate";
	mmCountDoubleName[BOUNDINGBOX_MAX_Z]    = "Maximum z coordinate";
	mmCountDoubleName[BOUNDINGBOX_WIDTH]    = "Bounding box width";
	mmCountDoubleName[BOUNDINGBOX_HEIGHT]   = "Bounding box height";
	mmCountDoubleName[BOUNDINGBOX_THICK]    = "Bounding box thickness";
	mmCountDoubleName[TOTAL_AREA]           = "Total area";
	mmCountDoubleName[TOTAL_VOLUME_DX]      = "Total volume (dx)";
	mmCountDoubleName[TOTAL_VOLUME_DY]      = "Total volume (dy)";
	mmCountDoubleName[TOTAL_VOLUME_DZ]      = "Total volume (dz)";

	reset();
}

//! Reset all values.
void MeshInfoData::reset() {
	for( std::string& countValue : this->mStrings ) {
		countValue = "";
	}
	for( auto& countValue : this->mCountULong ) {
		countValue = _NOT_A_NUMBER_ULONG_;
	}
	for( double& countValue : this->mCountDouble ) {
		countValue = _NOT_A_NUMBER_DBL_;
	}
}

bool MeshInfoData::getMeshInfoXML(std::string& rInfoXML){
    std::string infoStr = "<?xml version=\"1.0\"?>\n<GigaMeshInfo xmlns=\"http://www.gigamesh.eu/ont#\">\n";
    infoStr+="<VertexInformation>\n";
    infoStr+="<TotalNumberOfVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_TOTAL])+"</TotalNumberOfVertices>\n";
    infoStr+="<NaNVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_NAN])+"</NaNVertices>\n";
    infoStr+="<NormalLengthVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_NORMAL_LEN_NORMAL])+"</NormalLengthVertices>\n";
    infoStr+="<IsolatedVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SOLO])+"</IsolatedVertices>\n";
    infoStr+="<BorderVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_BORDER])+"</BorderVertices>\n";
    infoStr+="<NonManifoldVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_NONMANIFOLD])+"</NonManifoldVertices>\n";
    infoStr+="<SingularVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SINGULAR])+"</SingularVertices>\n";
    infoStr+="<VerticesOnInvertedEdge>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_ON_INVERTED_EDGE])+"</VerticesOnInvertedEdge>\n";
    infoStr+="<VerticesPartOfZeroAreaFace>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_PART_OF_ZERO_FACE])+"</VerticesPartOfZeroAreaFace>\n";
    infoStr+="<SyntheticVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SYNTHETIC])+"</SyntheticVertices>\n";
    infoStr+="<ManuallyAddedVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_MANUAL])+"</ManuallyAddedVertices>\n";
    infoStr+="<CircleCenterVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_CIRCLE_CENTER])+"</CircleCenterVertices>\n";
    infoStr+="<SelectedVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SELECTED])+"</SelectedVertices>\n";
    infoStr+="<FiniteFunctionValueVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_FINITE])+"</FiniteFunctionValueVertices>\n";
    infoStr+="<LocalFunctionMinValueVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MIN])+"</LocalFunctionMinValueVertices>\n";
    infoStr+="<LocalFunctionMaxValueVertices>"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MAX])+"</LocalFunctionMaxValueVertices>\n";
    infoStr+="</VertexInformation>\n";
    infoStr+="<FacesInformation>\n";
    infoStr+="<TotalNumberOfFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_TOTAL])+"</TotalNumberOfFaces>\n";
    infoStr+="<SoloFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_SOLO])+"</SoloFaces>\n";
    infoStr+="<BorderFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER])+"</BorderFaces>\n";
    infoStr+="<ThreeBorderVertexFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_THREE_VERTICES])+"</ThreeBorderVertexFaces>\n";
    infoStr+="<BorderFacesBridgeTriConnection>"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_BRDIGE_TRICONN])+"</BorderFacesBridgeTriConnection>\n";
    infoStr+="<BridgeBorderFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_BRDIGE])+"</BridgeBorderFaces>\n";
    infoStr+="<DanglingBorderFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_DANGLING])+"</DanglingBorderFaces>\n";
    infoStr+="<ManifoldFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_MANIFOLD])+"</ManifoldFaces>\n";
    infoStr+="<NonManifoldFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_NONMANIFOLD])+"</NonManifoldFaces>\n";
    infoStr+="<StickyFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_STICKY])+"</StickyFaces>\n";
    infoStr+="<ZeroAreaFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_ZEROAREA])+"</ZeroAreaFaces>\n";
    infoStr+="<InvertedFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_INVERTED])+"</InvertedFaces>\n";
    infoStr+="<SelectedFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_SELECTED])+"</SelectedFaces>\n";
    infoStr+="<SyntheticVertexFaces>"+std::to_string(this->mCountULong[MeshInfoData::FACES_WITH_SYNTH_VERTICES])+"</SyntheticVertexFaces>\n";
    infoStr+="</FacesInformation>\n";
    infoStr+="<BoundingBox>\n";
    infoStr+="<MinimumXCoordinate>"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_X])+"</MinimumXCoordinate>\n";
    infoStr+="<MinimumYCoordinate>"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Y])+"</MinimumYCoordinate>\n";
    infoStr+="<MinimumZCoordinate>"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Z])+"</MinimumZCoordinate>\n";
    infoStr+="<MaximumXCoordinate>"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_X])+"</MaximumXCoordinate>\n";
    infoStr+="<MaximumYCoordinate>"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Y])+"</MaximumYCoordinate>\n";
    infoStr+="<MaximumZCoordinate>"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Z])+"</MaximumZCoordinate>\n";
    infoStr+="<BoundingBoxWidth>"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_WIDTH])+"</BoundingBoxWidth>\n";
    infoStr+="<BoundingBoxHeight>"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_HEIGHT])+"</BoundingBoxHeight>\n";
    infoStr+="</BoundingBox>\n";    
    infoStr+="<Metadata>\n";
    infoStr+="<Filename>"+this->mStrings[MeshInfoData::FILENAME]+"</Filename>\n";
    infoStr+="<GigaMeshVersion> </GigaMeshVersion>\n";
    infoStr+="<TotalArea>"+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_AREA])+"</TotalArea>\n";    
    infoStr+="<TotalVolumeDX>"+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_VOLUME_DX])+"</TotalVolumeDX>\n"; 
    infoStr+="<TotalVolumeDY>"+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_VOLUME_DY])+"</TotalVolumeDY>\n";
    infoStr+="<TotalVolumeDZ>"+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_VOLUME_DZ])+"</TotalVolumeDZ>\n"; 
    infoStr+="</Metadata>\n";
    infoStr+="</GigaMeshInfo>";
    rInfoXML = infoStr;
	return( true );    
}


bool MeshInfoData::getMeshInfoJSON(std::string& rInfoJSON){
    std::string infoStr = "{\n";
    infoStr+="\"@context\":\"http://www.gigamesh.eu/ont#\",\n"; 
    infoStr+="\"@type\":\"GigaMeshInfo\",\n"; 
    infoStr+="\"FileName\":\""+this->mStrings[MeshInfoData::FILENAME]+"\",\n";
    infoStr+="\"GigaMeshVersion\":\"\",\n";
    infoStr+="\"VertexInformation\":{\n";
    infoStr+="\"TotalNumberOfVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_TOTAL])+",\n";   
    infoStr+="\"NaNVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_NAN])+",\n";    
    infoStr+="\"NormalLengthVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_NORMAL_LEN_NORMAL])+",\n";    
    infoStr+="\"IsolatedVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SOLO])+",\n";
    infoStr+="\"BorderVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_BORDER])+",\n"; 
    infoStr+="\"NonManifoldVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_NONMANIFOLD])+",\n"; 
    infoStr+="\"SingularVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SINGULAR])+",\n"; 
    infoStr+="\"VerticesOnInvertedEdge\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_ON_INVERTED_EDGE])+",\n"; 
    infoStr+="\"VerticesPartOfZeroAreaFace\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_PART_OF_ZERO_FACE])+",\n"; 
    infoStr+="\"SyntheticVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SYNTHETIC])+",\n"; 
    infoStr+="\"ManuallyAddedVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_MANUAL])+",\n"; 
    infoStr+="\"CircleCenterVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_CIRCLE_CENTER])+",\n";
    infoStr+="\"SelectedVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SELECTED])+",\n";
    infoStr+="\"FiniteFunctionValueVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_FINITE])+",\n";
    infoStr+="\"LocalFunctionMinValueVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MIN])+",\n";
    infoStr+="\"LocalFunctionMaxValueVertices\":"+std::to_string(this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MAX])+"\n";
    infoStr+="},\n";
    infoStr+="\"FacesInformation\":{\n";
    infoStr+="\"TotalNumberOfFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_TOTAL])+",\n";     
    infoStr+="\"SoloFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_SOLO])+",\n";   
    infoStr+="\"BorderFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER])+",\n";   
    infoStr+="\"ThreeBorderVertexFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_THREE_VERTICES])+",\n";   
    infoStr+="\"BorderFacesBridgeTriConnection\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_BRDIGE_TRICONN])+",\n"; 
    infoStr+="\"BridgeBorderFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_BRDIGE])+",\n"; 
    infoStr+="\"DanglingBorderFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_DANGLING])+",\n"; 
    infoStr+="\"ManifoldFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_MANIFOLD])+",\n"; 
    infoStr+="\"NonManifoldFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_NONMANIFOLD])+",\n"; 
    infoStr+="\"StickyFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_STICKY])+",\n"; 
    infoStr+="\"ZeroAreaFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_ZEROAREA])+",\n"; 
    infoStr+="\"InvertedFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_INVERTED])+",\n"; 
    infoStr+="\"SelectedFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_SELECTED])+",\n"; 
    infoStr+="\"SyntheticVertexFaces\":"+std::to_string(this->mCountULong[MeshInfoData::FACES_WITH_SYNTH_VERTICES])+"\n"; 
    infoStr+="},\n";    
    infoStr+="\"BoundingBox\":{\n";
    infoStr+="\"MinimumXCoordinate\":"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_X])+",\n";    
    infoStr+="\"MinimumYCoordinate\":"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Y])+",\n"; 
    infoStr+="\"MinimumZCoordinate\":"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Z])+",\n";
    infoStr+="\"MaximumXCoordinate\":"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_X])+",\n"; 
    infoStr+="\"MaximumYCoordinate\":"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Y])+",\n"; 
    infoStr+="\"MaximumZCoordinate\":"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Z])+",\n"; 
    infoStr+="\"BoundingBoxWidth\":"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_WIDTH])+",\n"; 
    infoStr+="\"BoundingBoxHeight\":"+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_HEIGHT])+"\n"; 
    infoStr+="},\n";
    infoStr+="\"Metadata\":{\n";
    infoStr+="\"TotalArea\":"+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_AREA])+",\n";   
    infoStr+="\"TotalVolumeDX\":"+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_VOLUME_DX])+",\n";   
    infoStr+="\"TotalVolumeDY\":"+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_VOLUME_DY])+",\n";  
    infoStr+="\"TotalVolumeDZ\":"+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_VOLUME_DZ])+"\n";  
    infoStr+="}\n";
    infoStr+="}";
    rInfoJSON = infoStr;
	return( true );
}

bool MeshInfoData::getMeshInfoTTL(std::string& rInfoTTL){
    std::string indid=this->mStrings[MeshInfoData::FILENAME];
    std::string infoStr = "@prefix rdf:<http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n@prefix skos: <http://www.w3.org/2004/02/skos/core#> .\n@prefix xsd:<http://www.w3.org/2001/XMLSchema#> .\n@prefix rdfs:<http://www.w3.org/2000/01/rdf-schema#> .\n@prefix owl:<http://www.w3.org/2002/07/owl#> .\n@prefix dcat:<http://www.w3.org/ns/dcat#> .\n@prefix prov:<http://www.w3.org/ns/prov#> .\n@prefix giga:<http://www.gigamesh.eu/ont#> .\n@prefix ex:<http://purl.org/net/ns/ex#> .\n@prefix geo:<http://www.opengis.net/ont/geosparql#> .\n@prefix wdt:<http://www.wikidata.org/prop/direct/> .\n";
    infoStr+="giga:GigameshInfo rdf:type owl:Class .\n";
    infoStr+="giga:TotalNumberOfVertices rdf:type owl:Class .\n";    
    infoStr+="giga:TotalNumberOfVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_totalNumberOfVertices rdf:type giga:TotalNumberOfVertices .\n";   
    infoStr+="giga:"+indid+"_totalNumberOfVertices giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_TOTAL])+"\"^^xsd:integer .\n";   
    infoStr+="giga:totalNumberOfVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:totalNumberOfVertices rdfs:label \"Total number of vertices\"@en .\n";
    infoStr+="giga:totalNumberOfVertices skos:definition \"The total number of vertices as represented in the mesh\" .\n";
    infoStr+="giga:totalNumberOfVertices rdfs:comment \"Gigamesh Info: Total number of vertices\" .\n";
    infoStr+="giga:"+indid+" giga:totalNumberOfVertices giga:"+indid+"_totalNumberOfVertices .\n"; 
    infoStr+="giga:NaNVertices rdf:type owl:Class .\n";    
    infoStr+="giga:NaNVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_NaNVertices rdf:type giga:NaNVertices .\n";   
    infoStr+="giga:"+indid+"_NaNVertices giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_NAN])+"\"^^xsd:integer .\n";   
    infoStr+="giga:NaNVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:NaNVertices rdfs:label \"NaN Vertices\"@en .\n";
    infoStr+="giga:NaNVertices skos:definition \"The total number of vertices which are not represented by a number\" .\n";
    infoStr+="giga:NaNVertices rdfs:comment \"Gigamesh Info: Vertices not-a-number\" .\n";
    infoStr+="giga:"+indid+" giga:NaNVertices giga:"+indid+"_NaNVertices .\n";       
    infoStr+="giga:"+indid+"_NaNVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:VertexNormalLength rdf:type owl:Class .\n";    
    infoStr+="giga:VertexNormalLength rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_vertexNormalLength rdf:type giga:VertexNormalLength .\n";   
    infoStr+="giga:"+indid+"_vertexNormalLength giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_NORMAL_LEN_NORMAL])+"\"^^xsd:integer .\n";   
    infoStr+="giga:vertexNormalLength rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:vertexNormalLength rdfs:label \"Vertex normal vector length normal\"@en .\n";
    infoStr+="giga:vertexNormalLength skos:definition \"The toal number of vertices with a normal vector length\" .\n";
    infoStr+="giga:vertexNormalLength rdfs:comment \"Gigamesh Info: Vertex normal vector length normal\" .\n";
    infoStr+="giga:"+indid+" giga:vertexNormalLength giga:"+indid+"_vertexNormalLength .\n"; 
    infoStr+="giga:"+indid+"_vertexNormalLength wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:VertexIsolated rdf:type owl:Class .\n";    
    infoStr+="giga:VertexIsolated rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_vertexIsolated rdf:type giga:VertexIsolated .\n";   
    infoStr+="giga:"+indid+"_vertexIsolated giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SOLO])+"\"^^xsd:integer .\n";   
    infoStr+="giga:vertexIsolated rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:vertexIsolated rdfs:label \"Vertices solo\"@en .\n";
    infoStr+="giga:vertexIsolated skos:definition \"The total number of isolated vertices\" .\n";
    infoStr+="giga:vertexIsolated rdfs:comment \"Gigamesh Info: Vertices solo\" .\n";
    infoStr+="giga:"+indid+" giga:vertexIsolated giga:"+indid+"_vertexIsolated .\n"; 
    infoStr+="giga:"+indid+"_vertexIsolated wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:BorderVertices rdf:type owl:Class .\n";    
    infoStr+="giga:BorderVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_borderVertices rdf:type giga:BorderVertices .\n";   
    infoStr+="giga:"+indid+"_borderVertices giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_BORDER])+"\"^^xsd:integer .\n";   
    infoStr+="giga:borderVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:borderVertices rdfs:label \"Border vertices\"@en .\n";
    infoStr+="giga:borderVertices skos:definition \"The total number of vertices at borders of the mesh\" .\n";
    infoStr+="giga:borderVertices rdfs:comment \"Gigamesh Info: Border vertices\" .\n";
    infoStr+="giga:"+indid+" giga:borderVertices giga:"+indid+"_borderVertices .\n";
    infoStr+="giga:"+indid+"_borderVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:NonManifoldVertices rdf:type owl:Class .\n";    
    infoStr+="giga:NonManifoldVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_nonManifoldVertices rdf:type giga:NonManifoldVertices .\n";   
    infoStr+="giga:"+indid+"_nonManifoldVertices giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_NONMANIFOLD])+"\"^^xsd:integer .\n";  
    infoStr+="giga:nonManifoldVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:nonManifoldVertices rdfs:label \"Vertices non-manifold\"@en .\n";
    infoStr+="giga:nonManifoldVertices skos:definition \"The total number of vertices which are not part of a manifold\" .\n";
    infoStr+="giga:nonManifoldVertices rdfs:comment \"Gigamesh Info: Vertices non-manifold\" .\n";
    infoStr+="giga:"+indid+" giga:nonManifoldVertices giga:"+indid+"_nonManifoldVertices .\n"; 
    infoStr+="giga:"+indid+"_nonManifoldVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:SingularVertices rdf:type owl:Class .\n";    
    infoStr+="giga:SingularVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_singularVertices rdf:type giga:SingularVertices .\n";   
    infoStr+="giga:"+indid+"_singularVertices giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SINGULAR])+"\"^^xsd:integer .\n";  
    infoStr+="giga:singularVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:singularVertices rdfs:label \"Vertices singular\"@en .\n";
    infoStr+="giga:singularVertices skos:definition \"The total number of vertices which are singular\" .\n";
    infoStr+="giga:singularVertices rdfs:comment \"Gigamesh Info: Vertices singular\" .\n";
    infoStr+="giga:"+indid+" giga:singularVertices giga:"+indid+"_singularVertices .\n"; 
    infoStr+="giga:"+indid+"_singularVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:VerticesOnInvertedEdge rdf:type owl:Class .\n";    
    infoStr+="giga:VerticesOnInvertedEdge rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_verticesOnInvertedEdge rdf:type giga:VerticesOnInvertedEdge .\n";   
    infoStr+="giga:"+indid+"_verticesOnInvertedEdge giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_ON_INVERTED_EDGE])+"\"^^xsd:integer .\n";  
    infoStr+="giga:verticesOnInvertedEdge rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:verticesOnInvertedEdge rdfs:label \"Vertices along an inverted edge\"@en .\n";
    infoStr+="giga:verticesOnInvertedEdge skos:definition \"The total number of vertices on an inverted edge\" .\n";
    infoStr+="giga:verticesOnInvertedEdge rdfs:comment \"Gigamesh Info: Vertices along an inverted edge\" .\n";
    infoStr+="giga:"+indid+" giga:verticesOnInvertedEdge giga:"+indid+"_verticesOnInvertedEdge .\n";  
    infoStr+="giga:"+indid+"_verticesOnInvertedEdge wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";    
    infoStr+="giga:VerticesPartOfZeroAreaFace rdf:type owl:Class .\n";    
    infoStr+="giga:VerticesPartOfZeroAreaFace rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_verticesPartOfZeroAreaFace rdf:type giga:VerticesPartOfZeroAreaFace .\n";   
    infoStr+="giga:"+indid+"_verticesPartOfZeroAreaFace giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_PART_OF_ZERO_FACE])+"\"^^xsd:integer .\n";  
    infoStr+="giga:verticesPartOfZeroAreaFace rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:verticesPartOfZeroAreaFace rdfs:label \"Vertices part of a zero area face\"@en .\n";
    infoStr+="giga:verticesPartOfZeroAreaFace skos:definition \"The total number of vertices which are part of a zero area face\" .\n";
    infoStr+="giga:verticesPartOfZeroAreaFace rdfs:comment \"Gigamesh Info: Vertices part of a zero area face\" .\n";
    infoStr+="giga:"+indid+" giga:verticesPartOfZeroAreaFace giga:"+indid+"_verticesPartOfZeroAreaFace .\n"; 
    infoStr+="giga:"+indid+"_verticesPartOfZeroAreaFace wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";    
    infoStr+="giga:SyntheticVertices rdf:type owl:Class .\n";    
    infoStr+="giga:SyntheticVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_syntheticVertices rdf:type giga:SyntheticVertices .\n";   
    infoStr+="giga:"+indid+"_syntheticVertices giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SYNTHETIC])+"\"^^xsd:integer .\n";  
    infoStr+="giga:syntheticVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:syntheticVertices rdfs:label \"Vertices synthetic\"@en .\n";
    infoStr+="giga:syntheticVertices skos:definition \"The total number of vertices which are synthetic\" .\n";
    infoStr+="giga:syntheticVertices rdfs:comment \"Gigamesh Info: Vertices synthetic\" .\n";
    infoStr+="giga:"+indid+" giga:syntheticVertices giga:"+indid+"_syntheticVertices .\n";  
    infoStr+="giga:"+indid+"_syntheticVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:ManuallyAddedVertices rdf:type owl:Class .\n";    
    infoStr+="giga:ManuallyAddedVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_manuallyAddedVertices rdf:type giga:ManuallyAddedVertices .\n";   
    infoStr+="giga:"+indid+"_manuallyAddedVertices giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_MANUAL])+"\"^^xsd:integer .\n";  
    infoStr+="giga:manuallyAddedVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:manuallyAddedVertices rdfs:label \"Vertices manually added\"@en .\n";
    infoStr+="giga:manuallyAddedVertices skos:definition \"The total number of vertices which have been manually added\" .\n";
    infoStr+="giga:manuallyAddedVertices rdfs:comment \"Gigamesh Info: Vertices manually added\" .\n";
    infoStr+="giga:"+indid+" giga:manuallyAddedVertices giga:"+indid+"_manuallyAddedVertices .\n"; 
    infoStr+="giga:"+indid+"_manuallyAddedVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:CircleCenterVertices rdf:type owl:Class .\n";    
    infoStr+="giga:CircleCenterVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_circleCenterVertices rdf:type giga:CircleCenterVertices .\n";   
    infoStr+="giga:"+indid+"_circleCenterVertices  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_CIRCLE_CENTER])+"\"^^xsd:integer .\n";  
    infoStr+="giga:circleCenterVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:circleCenterVertices rdfs:label \"Vertices circle center\"@en .\n";
    infoStr+="giga:circleCenterVertices skos:definition \"The total number of vertices which comprise the center of a circle\" .\n";
    infoStr+="giga:circleCenterVertices rdfs:comment \"Gigamesh Info: Vertices circle center\" .\n";
    infoStr+="giga:"+indid+" giga:circleCenterVertices giga:"+indid+"_circleCenterVertices .\n"; 
    infoStr+="giga:"+indid+"_circleCenterVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:SelectedVertices rdf:type owl:Class .\n";    
    infoStr+="giga:SelectedVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_selectedVertices rdf:type giga:SelectedVertices .\n";   
    infoStr+="giga:"+indid+"_selectedVertices  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_SELECTED])+"\"^^xsd:integer .\n";  
    infoStr+="giga:selectedVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:selectedVertices rdfs:label \"Vertices selected\"@en .\n";
    infoStr+="giga:selectedVertices skos:definition \"The total number of vertices which are selected\" .\n";
    infoStr+="giga:selectedVertices rdfs:comment \"Gigamesh Info: Vertices selected\" .\n";
    infoStr+="giga:"+indid+" giga:selectedVertices giga:"+indid+"_selectedVertices .\n"; 
    infoStr+="giga:"+indid+"_selectedVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:FiniteFunctionValueVertices rdf:type owl:Class .\n";    
    infoStr+="giga:FiniteFunctionValueVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_finiteFunctionValueVertices rdf:type giga:FiniteFunctionValueVertices .\n";   
    infoStr+="giga:"+indid+"_finiteFunctionValueVertices  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_FINITE])+"\"^^xsd:integer .\n";  
    infoStr+="giga:finiteFunctionValueVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:finiteFunctionValueVertices rdfs:label \"Vertices with finite function value\"@en .\n";
    infoStr+="giga:finiteFunctionValueVertices skos:definition \"The total number of vertices which have a finite function value\" .\n";
    infoStr+="giga:finiteFunctionValueVertices rdfs:comment \"Gigamesh Info: Vertices with finite function value\" .\n";
    infoStr+="giga:"+indid+" giga:finiteFunctionValueVertices giga:"+indid+"_finalFunctionValueVertices .\n"; 
    infoStr+="giga:"+indid+"_finiteFunctionValueVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:LocalFunctionValueMinVertices rdf:type owl:Class .\n";    
    infoStr+="giga:LocalFunctionValueMinVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_localFunctionMinValueVertices rdf:type giga:LocalFunctionMinValueVertices .\n";   
    infoStr+="giga:"+indid+"_localFunctionMinValueVertices  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MIN])+"\"^^xsd:integer .\n";  
    infoStr+="giga:localFunctionValueMinVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:localFunctionValueMinVertices rdfs:label \"Vertices with local function value minimum\"@en .\n";
    infoStr+="giga:localFunctionValueMinVertices skos:definition \"The total number of vertices which have a local function value minimum\" .\n";
    infoStr+="giga:localFunctionValueMinVertices rdfs:comment \"Gigamesh Info: Vertices with local function value minimum\" .\n";
    infoStr+="giga:"+indid+" giga:localFunctionValueMinVertices giga:"+indid+"_localFunctionMinValueVertices .\n";   
    infoStr+="giga:"+indid+"_localFunctionMinValueVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:LocalFunctionValueMaxVertices rdf:type owl:Class .\n";    
    infoStr+="giga:LocalFunctionValueMaxVertices rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_localFunctionMaxValueVertices rdf:type giga:LocalFunctionMaxValueVertices .\n";   
    infoStr+="giga:"+indid+"_localFunctionMaxValueVertices  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MAX])+"\"^^xsd:integer .\n";  
    infoStr+="giga:localFunctionValueMaxVertices rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:localFunctionValueMaxVertices rdfs:label \"Vertices with local function value maximum\"@en .\n";
    infoStr+="giga:localFunctionValueMaxVertices skos:definition \"The total number of vertices which have a local function value maximum\" .\n";
    infoStr+="giga:localFunctionValueMaxVertices rdfs:comment \"Gigamesh Info: Vertices with local function value maximum\" .\n";
    infoStr+="giga:"+indid+" giga:localFunctionValueMaxVertices giga:"+indid+"_localFunctionMaxValueVertices .\n";  
    infoStr+="giga:"+indid+"_localFunctionMaxValueVertices wdt:P361 giga:"+indid+"_totalNumberOfVertices .\n";
    infoStr+="giga:TotalNumberOfFaces rdf:type owl:Class .\n";    
    infoStr+="giga:TotalNumberOfFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_totalNumberOfFaces rdf:type giga:TotalNumberOfFaces .\n";   
    infoStr+="giga:"+indid+"_totalNumberOfFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_TOTAL])+"\"^^xsd:integer .\n";  
    infoStr+="giga:totalNumberOfFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:totalNumberOfFaces rdfs:label \"Total number of faces\"@en .\n";
    infoStr+="giga:totalNumberOfFaces skos:definition \"The total number of faces in the given mesh\" .\n";
    infoStr+="giga:totalNumberOfFaces rdfs:comment \"Gigamesh Info: Total number of faces\" .\n";
    infoStr+="giga:"+indid+" giga:totalNumberOfFaces giga:"+indid+"_totalNumberOfFaces .\n";  
    infoStr+="giga:SoloFaces rdf:type owl:Class .\n";    
    infoStr+="giga:SoloFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_soloFaces rdf:type giga:SoloFaces .\n";   
    infoStr+="giga:"+indid+"_soloFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_SOLO])+"\"^^xsd:integer .\n";  
    infoStr+="giga:soloFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:soloFaces rdfs:label \"Solo faces\"@en .\n";
    infoStr+="giga:soloFaces skos:definition \"The total number of solo faces\" .\n";
    infoStr+="giga:soloFaces rdfs:comment \"Gigamesh Info: Solo faces\" .\n";
    infoStr+="giga:"+indid+" giga:soloFaces giga:"+indid+"_soloFaces .\n";  
    infoStr+="giga:"+indid+"_soloFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:BorderFaces rdf:type owl:Class .\n";    
    infoStr+="giga:BorderFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_borderFaces rdf:type giga:BorderFaces .\n";   
    infoStr+="giga:"+indid+"_borderFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER])+"\"^^xsd:integer .\n";  
    infoStr+="giga:borderFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:borderFaces rdfs:label \"Border faces\"@en .\n";
    infoStr+="giga:borderFaces skos:definition \"The total number of border faces\" .\n";
    infoStr+="giga:borderFaces rdfs:comment \"Gigamesh Info: Border faces\" .\n";
    infoStr+="giga:"+indid+" giga:borderFaces giga:"+indid+"_borderFaces . \n";  
    infoStr+="giga:"+indid+"_borderFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:ThreeBorderVertexFaces rdf:type owl:Class .\n";    
    infoStr+="giga:ThreeBorderVertexFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_threeBorderVertexFaces rdf:type giga:ThreeBorderVertexFaces .\n";   
    infoStr+="giga:"+indid+"_threeBorderVertexFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_THREE_VERTICES])+"\"^^xsd:integer .\n";  
    infoStr+="giga:threeBorderVertexFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:threeBorderVertexFaces rdfs:label \"Faces with three border vertices\"@en .\n";
    infoStr+="giga:threeBorderVertexFaces skos:definition \"The total number of faces with three border vertices\" .\n";
    infoStr+="giga:threeBorderVertexFaces rdfs:comment \"Gigamesh Info: Faces with three border vertices\" .\n";
    infoStr+="giga:"+indid+" giga:threeBorderVertexFaces giga:"+indid+"_threeBorderVertexFaces .\n";      
    infoStr+="giga:"+indid+"_threeBorderVertexFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:BorderFacesBridgeTriConnection rdf:type owl:Class .\n";    
    infoStr+="giga:BorderFacesBridgeTriConnection rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_threeBorderVertexFaces rdf:type giga:BorderFacesBridgeTriConnection .\n";   
    infoStr+="giga:"+indid+"_threeBorderVertexFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_BRDIGE_TRICONN])+"\"^^xsd:integer .\n";  
    infoStr+="giga:borderFacesBridgeTriConnection rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:borderFacesBridgeTriConnection rdfs:label \"Border faces bridge tri-connection\"@en .\n";
    infoStr+="giga:borderFacesBridgeTriConnection skos:definition \"The total number of faces with three border vertices\" .\n";
    infoStr+="giga:borderFacesBridgeTriConnection rdfs:comment \"Gigamesh Info: Border faces bridge tri-connection\" .\n";
    infoStr+="giga:"+indid+" giga:borderFacesBridgeTriConnection giga:"+indid+"_borderFacesBridgeTriConnection .\n";   
    infoStr+="giga:"+indid+"_borderFacesBridgeTriConnection wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:BridgeBorderFaces rdf:type owl:Class .\n";    
    infoStr+="giga:BridgeBorderFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_bridgeBorderFaces rdf:type giga:BridgeBorderFaces .\n";   
    infoStr+="giga:"+indid+"_bridgeBorderFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_BRDIGE])+"\"^^xsd:integer .\n";  
    infoStr+="giga:bridgeBorderFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:bridgeBorderFaces rdfs:label \"Bridge border faces\"@en .\n";
    infoStr+="giga:bridgeBorderFaces skos:definition \"The total number of bridge border faces\" .\n";
    infoStr+="giga:bridgeBorderFaces rdfs:comment \"Gigamesh Info: Bridge border faces\" .\n";
    infoStr+="giga:"+indid+" giga:bridgeBorderFaces giga:"+indid+"_bridgeBorderFaces .\n";   
    infoStr+="giga:"+indid+"_bridgeBorderFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:DanglingBorderFaces rdf:type owl:Class .\n";    
    infoStr+="giga:DanglingBorderFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_danglingBorderFaces rdf:type giga:BridgeBorderFaces .\n";   
    infoStr+="giga:"+indid+"_danglingBorderFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_BORDER_DANGLING])+"\"^^xsd:integer .\n";  
    infoStr+="giga:danglingBorderFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:danglingBorderFaces rdfs:label \"Dangling border faces\"@en .\n";
    infoStr+="giga:danglingBorderFaces skos:definition \"The total number of dangling border faces\" .\n";
    infoStr+="giga:danglingBorderFaces rdfs:comment \"Gigamesh Info: Dangling border faces\" .\n";
    infoStr+="giga:"+indid+" giga:danglingBorderFaces giga:"+indid+"_danglingBorderFaces .\n";
    infoStr+="giga:"+indid+"_danglingBorderFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:ManifoldFaces rdf:type owl:Class .\n";    
    infoStr+="giga:ManifoldFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_manifoldFaces rdf:type giga:ManifoldFaces .\n";   
    infoStr+="giga:"+indid+"_manifoldFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_MANIFOLD])+"\"^^xsd:integer .\n";  
    infoStr+="giga:manifoldFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:manifoldFaces rdfs:label \"Manifold faces\"@en .\n";
    infoStr+="giga:manifoldFaces skos:definition \"The total number of manifold faces\" .\n";
    infoStr+="giga:manifoldFaces rdfs:comment \"Gigamesh Info: Manifold faces\" .\n";
    infoStr+="giga:"+indid+" giga:manifoldFaces giga:"+indid+"_manifoldFaces .\n";
    infoStr+="giga:"+indid+"_manifoldFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:NonManifoldFaces rdf:type owl:Class .\n";    
    infoStr+="giga:NonManifoldFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_nonManifoldFaces rdf:type giga:ManifoldFaces .\n";   
    infoStr+="giga:"+indid+"_nonManifoldFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_NONMANIFOLD])+"\"^^xsd:integer .\n";  
    infoStr+="giga:nonManifoldFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:nonManifoldFaces rdfs:label \"Non-Manifold faces\"@en .\n";
    infoStr+="giga:nonManifoldFaces skos:definition \"The total number of non-manifold faces\" .\n";
    infoStr+="giga:nonManifoldFaces rdfs:comment \"Gigamesh Info: Non-Manifold faces\" .\n";
    infoStr+="giga:"+indid+" giga:nonManifoldFaces giga:"+indid+"_nonManifoldFaces .\n";
    infoStr+="giga:"+indid+"_nonManifoldFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:StickyFaces rdf:type owl:Class .\n";    
    infoStr+="giga:StickyFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_stickyFaces rdf:type giga:StickyFaces .\n";   
    infoStr+="giga:"+indid+"_stickyFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_STICKY])+"\"^^xsd:integer .\n";  
    infoStr+="giga:stickyFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:stickyFaces rdfs:label \"Sticky faces\"@en .\n";
    infoStr+="giga:stickyFaces skos:definition \"The total number of sticky faces\" .\n";
    infoStr+="giga:stickyFaces rdfs:comment \"Gigamesh Info: Sticky faces\" .\n";
    infoStr+="giga:"+indid+" giga:stickyFaces giga:"+indid+"_stickyFaces .\n";
    infoStr+="giga:"+indid+"_stickyFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:ZeroAreaFaces rdf:type owl:Class .\n";    
    infoStr+="giga:ZeroAreaFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_zeroAreaFaces rdf:type giga:ZeroAreaFaces .\n";   
    infoStr+="giga:"+indid+"_zeroAreaFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_ZEROAREA])+"\"^^xsd:integer .\n";  
    infoStr+="giga:zeroAreaFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:zeroAreaFaces rdfs:label \"Faces with zero area\"@en .\n";
    infoStr+="giga:zeroAreaFaces skos:definition \"The total number of faces with zero area\" .\n";
    infoStr+="giga:zeroAreaFaces rdfs:comment \"Gigamesh Info: Faces with zero area\" .\n";
    infoStr+="giga:"+indid+" giga:zeroAreaFaces giga:"+indid+"_zeroAreaFaces . \n";
    infoStr+="giga:"+indid+"_zeroAreaFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:InvertedFaces rdf:type owl:Class .\n";    
    infoStr+="giga:InvertedFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_invertedFaces rdf:type giga:InvertedFaces .\n";   
    infoStr+="giga:"+indid+"_invertedFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_INVERTED])+"\"^^xsd:integer .\n";  
    infoStr+="giga:invertedFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:invertedFaces rdfs:label \"Inverted Faces\"@en .\n";
    infoStr+="giga:invertedFaces skos:definition \"The total number of faces which are inverted\" .\n";
    infoStr+="giga:invertedFaces rdfs:comment \"Gigamesh Info: Inverted Faces\" .\n";
    infoStr+="giga:"+indid+" giga:invertedFaces giga:"+indid+"_invertedFaces .\n";
    infoStr+="giga:"+indid+"_invertedFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:SelectedFaces rdf:type owl:Class .\n";    
    infoStr+="giga:SelectedFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_selectedFaces rdf:type giga:SelectedFaces .\n";   
    infoStr+="giga:"+indid+"_selectedFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_SELECTED])+"\"^^xsd:integer .\n";  
    infoStr+="giga:selectedFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:selectedFaces rdfs:label \"Selected Faces\"@en .\n";
    infoStr+="giga:selectedFaces skos:definition \"The total number of selected faces\" .\n";
    infoStr+="giga:selectedFaces rdfs:comment \"Gigamesh Info: Selected Faces\" .\n";
    infoStr+="giga:"+indid+" giga:selectedFaces giga:"+indid+"_selectedFaces .\n";
    infoStr+="giga:"+indid+"_selectedFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:SyntheticVertexFaces rdf:type owl:Class .\n";    
    infoStr+="giga:SyntheticVertexFaces rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_syntheticVertexFaces rdf:type giga:SyntheticVertexFaces .\n";   
    infoStr+="giga:"+indid+"_syntheticVertexFaces  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::FACES_WITH_SYNTH_VERTICES])+"\"^^xsd:integer .\n";  
    infoStr+="giga:syntheticVertexFaces rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:syntheticVertexFaces rdfs:label \"Faces with synthetic vertices\"@en .\n";
    infoStr+="giga:syntheticVertexFaces skos:definition \"The total number of faces with synthetic vertices\" .\n";
    infoStr+="giga:syntheticVertexFaces rdfs:comment \"Gigamesh Info: Faces with synthetic vertices\" .\n";
    infoStr+="giga:"+indid+" giga:syntheticVertexFaces giga:"+indid+"_syntheticVertexFaces .\n";
    infoStr+="giga:"+indid+"_syntheticVertexFaces wdt:P361 giga:"+indid+"_totalNumberOfFaces .\n";
    infoStr+="giga:ConnectedComponents rdf:type owl:Class .\n";    
    infoStr+="giga:ConnectedComponents rdfs:subClassOf giga:GigameshInfo .\n";    
    infoStr+="giga:"+indid+"_connectedComponents rdf:type giga:ConnectedComponents .\n";   
    infoStr+="giga:"+indid+"_connectedComponents  giga:value \""+std::to_string(this->mCountULong[MeshInfoData::CONNECTED_COMPONENTS])+"\"^^xsd:integer .\n";  
    infoStr+="giga:amountOfConnectedComponents rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:amountOfConnectedComponents rdfs:label \"Connected components\"@en .\n";
    infoStr+="giga:amountOfConnectedComponents skos:definition \"The total number of connected components\" .\n";
    infoStr+="giga:amountOfConnectedComponents rdfs:comment \"Gigamesh Info: Connected components\" .\n";
    infoStr+="giga:"+indid+" giga:amountOfConnectedComponents giga:"+indid+"_connectedComponents .\n";
    infoStr+="giga:minimumXCoordinate rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:minimumXCoordinate rdfs:label \"Minimum X Coordinate\"@en . \n";
    infoStr+="giga:minimumXCoordinate skos:definition \"The minimum x coordinate of the mesh\" .\n";
    infoStr+="giga:minimumXCoordinate rdfs:comment \"Gigamesh Info: Minimum X Coordinate\" .\n";
    infoStr+="giga:"+indid+" giga:minimumXCoordinate \""+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_X])+"\"^^xsd:double .\n";
    infoStr+="giga:minimumYCoordinate rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:minimumYCoordinate rdfs:label \"Minimum Y Coordinate\"@en . \n";
    infoStr+="giga:minimumYCoordinate skos:definition \"The minimum y coordinate of the mesh\" .\n";
    infoStr+="giga:minimumYCoordinate rdfs:comment \"Gigamesh Info: Minimum Y Coordinate\" .\n";
    infoStr+="giga:"+indid+" giga:minimumYCoordinate \""+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Y])+"\"^^xsd:double .\n";
    infoStr+="giga:minimumZCoordinate rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:minimumZCoordinate rdfs:label \"Minimum Z Coordinate\"@en . \n";
    infoStr+="giga:minimumZCoordinate skos:definition \"The minimum z coordinate of the mesh\" .\n";
    infoStr+="giga:minimumZCoordinate rdfs:comment \"Gigamesh Info: Minimum Z Coordinate\" .\n";
    infoStr+="giga:"+indid+" giga:minimumZCoordinate \""+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Z])+"\"^^xsd:double .\n";
    infoStr+="giga:maximumXCoordinate rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:maximumXCoordinate rdfs:label \"Maximum X Coordinate\"@en . \n";
    infoStr+="giga:maximumXCoordinate skos:definition \"The maximum x coordinate of the mesh\" .\n";
    infoStr+="giga:maximumXCoordinate rdfs:comment \"Gigamesh Info: Maximum X Coordinate\" .\n";
    infoStr+="giga:"+indid+" giga:maximumXCoordinate \""+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_X])+"\"^^xsd:double .\n";
    infoStr+="giga:maximumYCoordinate rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:maximumYCoordinate rdfs:label \"Maximum Y Coordinate\"@en . \n";
    infoStr+="giga:maximumYCoordinate skos:definition \"The maximum y coordinate of the mesh\" .\n";
    infoStr+="giga:maximumYCoordinate rdfs:comment \"Gigamesh Info: Maximum Y Coordinate\" .\n";
    infoStr+="giga:"+indid+" giga:maximumYCoordinate \""+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Y])+"\"^^xsd:double .\n";
    infoStr+="giga:maximumZCoordinate rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:maximumZCoordinate rdfs:label \"Maximum Z Coordinate\"@en . \n";
    infoStr+="giga:maximumZCoordinate skos:definition \"The maximum y coordinate of the mesh\" .\n";
    infoStr+="giga:maximumZCoordinate rdfs:comment \"Gigamesh Info: Maximum Z Coordinate\" .\n";
    infoStr+="giga:"+indid+" giga:maximumZCoordinate \""+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Z])+"\"^^xsd:double .\n";
    infoStr+="giga:boundingBox rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:boundingBox rdfs:label \"Bounding Box\"@en . \n";
    infoStr+="giga:boundingBox skos:definition \"The bounding box of the mesh\" .\n";
    infoStr+="giga:boundingBox rdfs:comment \"Gigamesh Info: The bounding box of the mesh\" .\n";
    infoStr+="giga:"+indid+"_geom rdf:type geo:Geometry .\n";
    infoStr+="giga:"+indid+" giga:boundingBox giga:"+indid+"_geom . giga:"+indid+"_geom geo:asWKT \"ENVELOPE("+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_X])+" "+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Y])+" "+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Z])+","+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_X])+" "+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Y])+" "+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Z])+")\"^^geo:wktLiteral .\n";
    infoStr+="giga:boundingBoxWidth rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:boundingBoxWidth rdfs:label \"Bounding Box Width\"@en . \n";
    infoStr+="giga:boundingBoxWidth skos:definition \"The width of the bounding box encompassing the mesh\" .\n";
    infoStr+="giga:boundingBoxWidth rdfs:comment \"Gigamesh Info: Bounding Box Width\" .\n";
    infoStr+="giga:"+indid+" giga:boundingBoxWidth \""+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_WIDTH])+"\"^^xsd:double .\n";
    infoStr+="giga:boundingBoxHeight rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:boundingBoxHeight rdfs:label \"Bounding Box Height\"@en. \n";
    infoStr+="giga:boundingBoxHeight skos:definition \"The height of the bounding box encompassing the mesh\" .\n";
    infoStr+="giga:boundingBoxHeight rdfs:comment \"Gigamesh Info: Bounding Box Height\" .\n";
    infoStr+="giga:"+indid+" giga:boundingBoxHeight \""+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_HEIGHT])+"\"^^xsd:double .\n";
    infoStr+="giga:boundingBoxThickness rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:boundingBoxThickness rdfs:label \"Bounding Box Thickness\"@en . \n";
    infoStr+="giga:boundingBoxThickness skos:definition \"The thickness of the bounding box encompassing the mesh\" .\n";
    infoStr+="giga:boundingBoxThickness rdfs:comment \"Gigamesh Info: Bounding Box Thickness\" .\n";
    infoStr+="giga:"+indid+" giga:boundingBoxThickness \""+std::to_string(this->mCountDouble[MeshInfoData::BOUNDINGBOX_THICK])+"\"^^xsd:double .\n";
    infoStr+="giga:totalArea rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:totalArea rdfs:label \"Total Area\"@en . \n";
    infoStr+="giga:totalArea skos:definition \"The total area of the mesh\" .\n";
    infoStr+="giga:totalArea rdfs:comment \"Gigamesh Info: Total Area\" .\n";
    infoStr+="giga:"+indid+" giga:totalArea \""+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_AREA])+"\"^^xsd:double .\n";
    infoStr+="giga:totalVolumeDX rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:totalVolumeDX rdfs:label \"Total Volume DX\"@en . \n";
    infoStr+="giga:totalVolumeDX skos:definition \"The total volume DX of the mesh\" .\n";
    infoStr+="giga:totalVolumeDX rdfs:comment \"Gigamesh Info: Total Volume DX\" .\n";
    infoStr+="giga:"+indid+" giga:totalVolumeDX \""+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_VOLUME_DX])+"\"^^xsd:double .\n";
    infoStr+="giga:totalVolumeDY rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:totalVolumeDY rdfs:label \"Total Volume DY\"@en. \n";
    infoStr+="giga:totalVolumeDY skos:definition \"The total volume DY of the mesh\" .\n";
    infoStr+="giga:totalVolumeDY rdfs:comment \"Gigamesh Info: Total Volume DY\" .\n";
    infoStr+="giga:"+indid+" giga:totalVolumeDY \""+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_VOLUME_DY])+"\"^^xsd:double .\n";
    infoStr+="giga:totalVolumeDZ rdf:type owl:ObjectProperty .\n";
    infoStr+="giga:totalVolumeDZ rdfs:label \"Total Volume DZ\"@en . \n";
    infoStr+="giga:totalVolumeDZ skos:definition \"The total volume DZ of the mesh\" .\n";
    infoStr+="giga:totalVolumeDZ rdfs:comment \"Gigamesh Info: Total Volume DZ\" .\n";
    infoStr+="giga:"+indid+" giga:totalVolumeDZ \""+std::to_string(this->mCountDouble[MeshInfoData::TOTAL_VOLUME_DZ])+"\"^^xsd:double .\n";
    rInfoTTL = infoStr;
	return( true );
}

//! Format mesh information as HTML.
//!
//! See Mesh::dumpMeshInfo for a related plain text method.
//!
//! @returns false in case of an error. True otherwise.
bool MeshInfoData::getMeshInfoHTML(
        std::string&          rInfoHTML    //!< Output: String to be passed e.g. for Qt Infobox
) {
	// Compute and format relative amount
	//-----------------------------------------------------------
	std::stringstream fractionsFormatted[MeshInfoData::ULONG_COUNT];
	for( int i=0; i<=MeshInfoData::VERTICES_FUNCVAL_LOCAL_MAX; i++ ) {
		fractionsFormatted[i] << std::fixed << std::setprecision(2) << this->mCountULong[i]*100.0 / this->mCountULong[MeshInfoData::VERTICES_TOTAL];
	}
	for( int i=MeshInfoData::FACES_SOLO; i<MeshInfoData::ULONG_COUNT; i++ ) {
		fractionsFormatted[i] << std::fixed << std::setprecision(2) << this->mCountULong[i]*100.0 / this->mCountULong[MeshInfoData::FACES_TOTAL];
	}

	// Compute the area and the resolution of the mesh
	//-----------------------------------------------------------
	double areaAcq = this->mCountDouble[MeshInfoData::TOTAL_AREA];
	std::string areaAcqStr   = std::to_string( areaAcq );
	std::string avgResMetric = std::to_string( this->mCountULong[MeshInfoData::VERTICES_TOTAL]/areaAcq );
	std::string avgResDPI    = std::to_string( 2.54*sqrt(this->mCountULong[MeshInfoData::VERTICES_TOTAL]/areaAcq) );
	// Format the numbers
	std::size_t foundDot = areaAcqStr.find('.');
	if( foundDot > 4 ) { // Cut-off fractions for large values
		areaAcqStr = areaAcqStr.substr( 0 , foundDot );
	} else { // Show only 5 digits including the fraction
		areaAcqStr = areaAcqStr.substr( 0 , 6 );
	}
	foundDot = avgResMetric.find('.');
	if( foundDot > 4 ) { // Cut-off fractions for large values
		avgResMetric = avgResMetric.substr( 0 , foundDot );
	} else { // Show only 5 digits including the fraction
		avgResMetric = avgResMetric.substr( 0 , 6 );
	}
	foundDot = avgResDPI.find('.');
	if( foundDot > 4 ) { // Cut-off fractions for large values
		avgResDPI = avgResDPI.substr( 0 , foundDot );
	} else { // Show only 5 digits including the fraction
		avgResDPI = avgResDPI.substr( 0 , 6 );
	}
	//-----------------------------------------------------------

	std::string tableBorder = "0"; // For visual debugging set to 1 - 0 (zero) for release!
	std::string infoStr = "<!DOCTYPE html>\n";
	infoStr += "<html>\n";
	infoStr += "<head>\n";
	infoStr += "<title>GigaMesh Information about [" + this->mStrings[MeshInfoData::FILENAME] + "]</title>\n";
	infoStr += "</head>\n";
	infoStr += "<body>\n";

	infoStr += "<b>Filename:</b> " + this->mStrings[MeshInfoData::FILENAME] + "<br />\n";
	infoStr += "<br />\n";

	infoStr += "Connected components: ";
	if( isnormal( static_cast<double>(this->mCountULong[MeshInfoData::CONNECTED_COMPONENTS]) ) ) {
		infoStr += std::to_string( this->mCountULong[MeshInfoData::CONNECTED_COMPONENTS] );
	} else {
		infoStr += "not determined";
	}
	infoStr += "<br />\n";

	// Outer table - Row I, Col I
	infoStr += "<table align='center' border='" + tableBorder + "'>\n";
	infoStr += "<tr><td align='center'>\n";

	infoStr += "<b>Bounding Box</b> in mm (assumed)\n";
	infoStr += "<table border='" + tableBorder + "'>\n";
	infoStr += "<tr><td>X:</td><td align='right'>" + std::to_string( this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_X] ) + "</td>";
	infoStr += "<td align='center'>&nbsp;&#8722;&nbsp;</td>";
	infoStr += "<td align='left'>" + std::to_string( this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_X] ) + "</td>";
	infoStr += "<td align='center'>&nbsp;=&nbsp;</td>";
	infoStr += "<td align='center'>&nbsp;" + std::to_string( this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_X] - this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_X] ) + "</td></tr>\n";
	infoStr += "<tr><td>Y:</td><td align='right'>" + std::to_string( this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Y] ) + "</td>";
	infoStr += "<td align='center'>&nbsp;&#8722;&nbsp;</td>";
	infoStr += "<td align='left'>" + std::to_string( this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Y] ) + "</td>";
	infoStr += "<td align='center'>&nbsp;=&nbsp;</td>";
	infoStr += "<td align='center'>&nbsp;" + std::to_string( this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Y] - this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Y] ) + "</td></tr>\n";
	infoStr += "<tr><td>Z:</td><td align='right'>" + std::to_string( this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Z] ) + "</td>";
	infoStr += "<td align='center'>&nbsp;&#8722;&nbsp;</td>";
	infoStr += "<td align='left'>" + std::to_string( this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Z] ) + "</td>";
	infoStr += "<td align='center'>&nbsp;=&nbsp;</td>";
	infoStr += "<td align='center'>&nbsp;" + std::to_string( this->mCountDouble[MeshInfoData::BOUNDINGBOX_MAX_Z] - this->mCountDouble[MeshInfoData::BOUNDINGBOX_MIN_Z] ) + "</td></tr>\n";
	infoStr += "</table>\n";

	// Outer table - Row I, Col II
	infoStr += "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td align='center'>\n";

	infoStr += "(units assumed)\n";
	infoStr += "<table align=\"center\" width=\"99%\" border='" + tableBorder + "'>\n";
	infoStr += "<tr>\n";
	infoStr += "<td align=\"left\"><b>Resolution,&nbsp;average:</b></td>";
	infoStr += "<td align=\"right\">" + avgResMetric + "</td>";
	infoStr += "<td align=\"left\">cm<sup>-2</sup></td>";
	infoStr += "</tr>\n";
	infoStr += "<tr>\n";
	infoStr += "<td align=\"left\"></td>";
	infoStr += "<td align=\"right\">" + avgResDPI + "</td>";
	infoStr += "<td align=\"left\">DPI</td>";
	infoStr += "</tr>\n";
	infoStr += "<tr>\n";
	infoStr += "<td align=\"left\"><b>Total&nbsp;surface area:</b></td>";
	infoStr += "<td align=\"right\">" + areaAcqStr + "</td>";
	infoStr += "<td align=\"left\">cm<sup>2</sup></td>";
	infoStr += "</tr>\n";
	infoStr += "</table>\n";

	// Outer table - Row II, Col I
	infoStr += "</td></tr>\n"
	           "<tr><td colspan='3'><hr /></tr>\n"
	           "<tr><td>\n";

	infoStr += "<table align=\"center\" width=\"99%\" border='" + tableBorder + "'>\n";
	infoStr += "<tr><td colspan=\"3\" align=\"center\"><b>Vertices</b></td></tr>\n";
	infoStr += "<tr><td>Total:</td><td align=\"right\">"                                + std::to_string( this->mCountULong[MeshInfoData::VERTICES_TOTAL] )                  + "</td><td align=\"center\">-</td></tr>\n";
	infoStr += "<tr><td>NaN<sup>a)</sup>&ensp;coordinate(s):</td><td align=\"right\">"  + std::to_string( this->mCountULong[MeshInfoData::VERTICES_NAN] )                    + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_SOLO].str()                   + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Normal&nbsp;length&nbsp;not&nbsp;normal:</td><td align=\"right\">"      + std::to_string( this->mCountULong[MeshInfoData::VERTICES_NORMAL_LEN_NORMAL] )      + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_NORMAL_LEN_NORMAL].str()      + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Solo:</td><td align=\"right\">"                                 + std::to_string( this->mCountULong[MeshInfoData::VERTICES_SOLO] )                   + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_SOLO].str()                   + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Polyline:</td><td align=\"right\">"                             + std::to_string( this->mCountULong[MeshInfoData::VERTICES_POLYLINE] )               + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_POLYLINE].str()               + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Border:</td><td align=\"right\">"                               + std::to_string( this->mCountULong[MeshInfoData::VERTICES_BORDER] )                 + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_BORDER].str()                 + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Non-manifold:</td><td align=\"right\">"                         + std::to_string( this->mCountULong[MeshInfoData::VERTICES_NONMANIFOLD] )            + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_NONMANIFOLD].str()            + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Singular:</td><td align=\"right\">"                             + std::to_string( this->mCountULong[MeshInfoData::VERTICES_SINGULAR] )               + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_SINGULAR].str()               + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Inverted edge<sup>b)</sup>:</td><td align=\"right\">"           + std::to_string( this->mCountULong[MeshInfoData::VERTICES_ON_INVERTED_EDGE] )       + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_ON_INVERTED_EDGE].str()       + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Part&nbsp;of&nbsp;zero&nbsp;area&nbsp;face:</td><td align=\"right\">"   + std::to_string( this->mCountULong[MeshInfoData::VERTICES_PART_OF_ZERO_FACE] )      + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_PART_OF_ZERO_FACE].str()      + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Synthetic:</td><td align=\"right\">"                            + std::to_string( this->mCountULong[MeshInfoData::VERTICES_SYNTHETIC] )              + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_SYNTHETIC].str()              + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Manual:</td><td align=\"right\">"                               + std::to_string( this->mCountULong[MeshInfoData::VERTICES_MANUAL] )                 + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_MANUAL].str()                 + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Circle Centers:</td><td align=\"right\">"                       + std::to_string( this->mCountULong[MeshInfoData::VERTICES_CIRCLE_CENTER] )          + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_CIRCLE_CENTER].str()          + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Selected:</td><td align=\"right\">"                             + std::to_string( this->mCountULong[MeshInfoData::VERTICES_SELECTED] )               + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_SELECTED].str()               + "&#37;</td></tr>\n";
	infoStr += "<tr><td colspan=\"3\"><i><u>Function values:</u></i></td></tr>\n";
	infoStr += "<tr><td>...&ensp;non finite:</td><td align=\"right\">"                           + std::to_string( this->mCountULong[MeshInfoData::VERTICES_TOTAL] - this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_FINITE] )         + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_FUNCVAL_FINITE].str()         + "&#37;</td></tr>\n";
	infoStr += "<tr><td>...&ensp;local&ensp;minimum:</td><td align=\"right\">"                   + std::to_string( this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MIN] )      + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MIN].str()      + "&#37;</td></tr>\n";
	infoStr += "<tr><td>...&ensp;local&ensp;maximum:</td><td align=\"right\">"                   + std::to_string( this->mCountULong[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MAX] )      + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::VERTICES_FUNCVAL_LOCAL_MAX].str()      + "&#37;</td></tr>\n";
	infoStr += "</table>\n";

	// Outer table - Row II, Col II
	infoStr += "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>\n";

	infoStr += "<table align=\"center\" width=\"99%\" border='" + tableBorder + "'>\n";
	infoStr += "<tr><td colspan=\"3\" align=\"center\"><b>Faces</b></td></tr>\n";
	infoStr += "<tr><td>Total:</td><td align=\"right\">"                                       + std::to_string( this->mCountULong[MeshInfoData::FACES_TOTAL] )                   + "</td><td align=\"center\">-</td></tr>\n";
	infoStr += "<tr><td>Manifold:</td><td align=\"right\">"                                    + std::to_string( this->mCountULong[MeshInfoData::FACES_MANIFOLD] )                + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_MANIFOLD].str()              + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Non-manifold:</td><td align=\"right\">"                                + std::to_string( this->mCountULong[MeshInfoData::FACES_NONMANIFOLD] )             + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_NONMANIFOLD].str()           + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Inverted<sup>c)</sup>:</td><td align=\"right\">"                       + std::to_string( this->mCountULong[MeshInfoData::FACES_INVERTED] )                + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_INVERTED].str()              + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Sticky:</td><td align=\"right\">"                                      + std::to_string( this->mCountULong[MeshInfoData::FACES_STICKY] )                  + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_STICKY].str()                + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Zero&ensp;area:</td><td align=\"right\">"                              + std::to_string( this->mCountULong[MeshInfoData::FACES_ZEROAREA] )                + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_ZEROAREA].str()              + "&#37;</td></tr>\n";
	infoStr += "<tr><td>Border:</td><td align=\"right\">"                                      + std::to_string( this->mCountULong[MeshInfoData::FACES_BORDER] )                  + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_BORDER].str()                + "&#37;</td></tr>\n";
	infoStr += "<tr><td>...&nbsp;3&nbsp;Vertices&nbsp;(3V):</td><td align=\"right\">"          + std::to_string( this->mCountULong[MeshInfoData::FACES_BORDER_THREE_VERTICES] )   + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_BORDER_THREE_VERTICES].str() + "&#37;</td></tr>\n";
	infoStr += "<tr><td>&nbsp;&nbsp;&nbsp;...&nbsp;Bridge&nbsp;triconn.&nbsp;(3V0E):</td><td align=\"right\">"   + std::to_string( this->mCountULong[MeshInfoData::FACES_BORDER_BRDIGE_TRICONN] )   + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_BORDER_BRDIGE_TRICONN].str() + "&#37;</td></tr>\n";
	infoStr += "<tr><td>&nbsp;&nbsp;&nbsp;...&nbsp;Bridge&nbsp;(3V1E):</td><td align=\"right\">"                 + std::to_string( this->mCountULong[MeshInfoData::FACES_BORDER_BRDIGE] )           + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_BORDER_BRDIGE].str()         + "&#37;</td></tr>\n";
	infoStr += "<tr><td>&nbsp;&nbsp;&nbsp;...&nbsp;Dangling&nbsp;(3V2E):</td><td align=\"right\">"               + std::to_string( this->mCountULong[MeshInfoData::FACES_BORDER_DANGLING] )         + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_BORDER_DANGLING].str()       + "&#37;</td></tr>\n";
	infoStr += "<tr><td>&nbsp;&nbsp;&nbsp;...&nbsp;3&nbsp;Edges&nbsp;(Solo,3E):</td><td align=\"right\">"        + std::to_string( this->mCountULong[MeshInfoData::FACES_SOLO] )                    + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_SOLO].str()                  + "&#37;</td></tr>\n";
	//infoStr += "<tr><td></td><td></td><td></td></tr>\n"; // Empty line
	infoStr += "<tr><td>Selected:</td><td align=\"right\">"                                    + std::to_string( this->mCountULong[MeshInfoData::FACES_SELECTED] )                + "</td><td align=\"right\">" + fractionsFormatted[MeshInfoData::FACES_SELECTED].str()              + "&#37;</td></tr>\n";
	infoStr += "</table>\n";

	// Outer table - End
	infoStr += "</td></tr>\n";
	infoStr += "</table>\n";

	//! \todo provide more information about polylines - maybe using another dialog.
	//infoStr += "<p align=\"center\"><b>Polylines:</b>&nbsp;" + std::to_string( getPolyLineNr() ) + "</p>\n";

	// Footnotes
	infoStr += "<p align='left'>";
	infoStr += "<i>a)</i> Not a Number.<br />\n";
	infoStr += "<i>b)</i> Some non-manifold edges are counted as inverted.<br />\n";
	infoStr += "<i>c)</i> Contains non-manifold faces.<br />\n";
	infoStr += "</p>";

	infoStr += "</body>\n";
	infoStr += "</html>\n";
	// std::cout << infoStr << std::endl; // For debugging the HTML code.

	rInfoHTML = infoStr;
	return( true );
}

bool MeshInfoData::getMeshInfoPropertyName(
                const MeshInfoData::eMeshPropertyString rPropId,
                std::string& rPropName
) {
	if( rPropId == STRING_COUNT ) {
		return( false );
	}
	rPropName = mStringName[rPropId];
	return( true );
}


bool MeshInfoData::getMeshInfoPropertyName(
                const MeshInfoData::eMeshPropertyULongCount rPropId,
                std::string& rPropName
) {
	if( rPropId == ULONG_COUNT ) {
		return( false );
	}
	rPropName = mCountULongName[rPropId];
	return( true );
}


bool MeshInfoData::getMeshInfoPropertyName(
                const MeshInfoData::eMeshPropertyDouble rPropId,
                std::string& rPropName
) {
	if( rPropId == DOUBLE_COUNT ) {
		return( false );
	}
	rPropName = mmCountDoubleName[rPropId];
	return( true );
}
