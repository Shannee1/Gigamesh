﻿//
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

#include <GigaMesh/mesh/octree.h>



const auto NUM_THREADS = std::thread::hardware_concurrency() * 2;


//! @param vertexlist  @param  center @param maxnr represents the maximum number of vertices per cube
//! @param edgelen contains the length of edge of the largest cube
//! Octree is primarily a octree of vertices. A Faceoctree can be created on the base of the vertex octree.
Octree::Octree(std::vector<Vertex*> &vertexlist,std::vector<Face*> &facelist, Vector3D* center, unsigned int maxnr, double edgelen, double EdgeLenMax, bool copyelements): mmaxnr(maxnr) {

    //max EdgeLen of current mesh
    mEdgeLenMax = EdgeLenMax;
    //current level is 0, as we have done no subdivision so far
    mmaxlevel = 0;
    //create root node
    mRootVertices = new Octnode(center, 0.5*edgelen);
    //assign elementlist to root node
    mRootVertices->mVertices = vertexlist;
    //mcopyelements true means that an element may copied to more than one node,
    //e.g. if it is in on the border of the nodes
    mcopyelements = copyelements;
    //start recursive initialization of octree
    initialize(mRootVertices, 1);

    //create face octree
    mRootFaces = new Octnode(center, 0.5*edgelen);
    //for all faces inside this vector we have to check in which nodes they intersect
    generateFacesOctreeHypothesis(mRootFaces,mRootVertices,1,&mIncompleteFaces);
    //delete dulicates
    sort(mIncompleteFaces.begin(),mIncompleteFaces.end());
    mIncompleteFaces.erase( unique( mIncompleteFaces.begin(), mIncompleteFaces.end()),mIncompleteFaces.end());
    correctFacesOctree(mIncompleteFaces);
}

//! destruct
Octree::~Octree() {
    std::vector<Octnode*> nodeVertPtrList;
    std::vector<Octnode*> nodeFacePtrList;
    getnodelist(nodeVertPtrList,VERTEX_OCTREE);
    getnodelist(nodeFacePtrList,FACE_OCTREE);
    // The following causes a CRASH, because .CLEAR() will try to delete the already deleted entries!!!
    //for(vector<Octnode*>::iterator it= nodelist.begin(); it!= nodelist.end(); ++it) {
    //	delete (*it);
    //}

    //delete Vertex octree
    for(Octnode* nodePtr : nodeVertPtrList)
    {
        delete nodePtr;
    }
    nodeVertPtrList.clear();

    //delete Face octree
    for(Octnode* nodePtr : nodeFacePtrList)
    {
        delete nodePtr;
    }
    nodeFacePtrList.clear();
}



void Octree::getnodelist(std::vector<Octnode*>& nodelist, eTreeType treeType) {
    if( treeType == VERTEX_OCTREE ){
        nodelist = mRootVertices->getNodeList();
    }
    else if( treeType == FACE_OCTREE){
        nodelist = mRootFaces->getNodeList();
    }
}

bool Octree::getNodesOfFace(std::vector<Octnode *> &nodelist, Face *face){
    std::vector<Octnode *> allLeafNodes;
    allLeafNodes = mRootFaces->getLeafNodes();

    for(Octnode* node : allLeafNodes){
        if(node->isFaceInside(face)){
            nodelist.push_back(node);
        }
    }
    if( nodelist.size() == 0){
        return false;
    }
    return true;
}
void Octree::getnodesinlevel(std::vector<Octnode*>& nodelist, unsigned int i) {
    std::vector<Octnode*> nodes;
    nodes = mRootVertices->getNodeList();
    for(Octnode* node : nodes){
        if( node->mlevel == i){
            nodelist.push_back(node);
        }
    }
}
//!get all nodes of the octree with vertices inside (leafnodes)
void Octree::getleafnodes(std::vector<Octnode*>& nodelist, eTreeType treeType) {
    if( treeType == VERTEX_OCTREE ){
        nodelist = mRootVertices->getLeafNodes();
    }
    else if( treeType == FACE_OCTREE){
        nodelist = mRootFaces->getLeafNodes();
    }

}





//! @param[out] sif contains pointers to self intersectiing faces
//!
void Octree::detectselfintersections(std::vector<Face*>& sif) {
    ShowProgress progress = ShowProgress("[Octree]");
    progress.showProgressStart( "Detect Face-Intersection" );
    std::vector<Octnode *> allLeafNodes;
    allLeafNodes = mRootFaces->getLeafNodes();
    //check for all faces in leafnodes if they intersect each other
    for(unsigned int nodeId = 0; nodeId < allLeafNodes.size(); nodeId++){
        //we have to convert the face set into vector because only with a vector we can access the values by index
        std::vector<Face*> nodeFaces{allLeafNodes[nodeId]->mFaces.begin(),allLeafNodes[nodeId]->mFaces.end()};
        std::vector<std::thread> threads(NUM_THREADS);
        unsigned int i = 0;
        while(i < nodeFaces.size()){
            // spawn n threads:
            for (unsigned int threadId = 0; threadId < NUM_THREADS ; threadId++) {
                if(i < nodeFaces.size()){
                    i++;
                }
                threads[threadId] = std::thread(&Octree::checkIntersectionOfFaceToOtherFaces, this, nodeFaces, i);
            }
            //wait until the threads are finished
            for (auto& th : threads) {
                th.join();
            }
        }
        progress.showProgress( static_cast<double>(nodeId)/static_cast<double>(allLeafNodes.size()),
                               "Detect Face-Intersection" );
    }
    //mSelfIntersectedFaces is filled in checkIntersectionOfFaceToOtherFaces
    sif = mSelfIntersectedFaces;
    progress.showProgressStop( "Detect Face-Intersection" );
}

void Octree::gettriangleintersection(std::vector<Octnode *> &nodelist, std::vector<Octnode *> &cnodelist, std::vector<Line> &drawlines, TriangularPrism &tri)
{
    std::vector<Octnode*> nodes;
    nodes = mRootVertices->getNodeList();
    for(Octnode* node : nodes){
        if(node->misleaf) {
            relativePosition tmp;
            tmp = node->mCube.CubeisintriangularPrism(tri, drawlines);

            switch(tmp) {
                case CONTAINED:
                    cnodelist.push_back(node);
                    break;

                case INTERSECTION:
                    nodelist.push_back(node);
                    break;


                case DISJOINT:
                    break;

                //should not be reached
                default:
                    std::cout<<"OCTREE triangle intersection something is incorrect"<<std::endl;

            }
        }
    }

}






//! prints number of levels and number of nodes in each level and total number of nodes
bool Octree::dumpInfo() {
    std::cout<<"-------------------------------------------------------"<<std::endl;
    std::cout<<"--------------- Octree Information --------------------"<<std::endl<<std::endl;
    std::cout<<"Octree maximum number of Vertices per cube: "<<mmaxnr<<std::endl;
    std::cout<<"Octree maximum depth:       "<<mmaxlevel<<std::endl;
    std::vector<Octnode*> nodelist;
    size_t temp = 0;
    unsigned int i = 1;
    size_t totalnodes=0;
    do {
        temp = nodelist.size();
        getnodesinlevel(nodelist, i);
        ++i;
        totalnodes += nodelist.size() - temp;
        std::cout<<"In Level " << i-1 << " are " << nodelist.size() - temp <<" Octnodes "<<std::endl;
    }
    while (temp != nodelist.size() && i<= mmaxlevel);
    std::cout<<"In total there are "<< totalnodes << " Octnodes in the whole Octree"<<std::endl;
    std::cout<<"--------------- End Octree Information ----------------"<<std::endl;
    std::cout<<"-------------------------------------------------------"<<std::endl;
    nodelist.clear();
    return true;
}


//! prints all centers of octnodes
bool Octree::dumpInfov() {
    std::vector<Octnode*> nodelist;
    getnodelist(nodelist,VERTEX_OCTREE);
    for ( typename std::vector<Octnode*>::iterator it = nodelist.begin(); it!=nodelist.end(); ++it) {
        (*it)->mCube.mcenter.dumpInfo();
        std::cout<<(*it)->mCube.mscale<<std::endl;
    }
    nodelist.clear();
    return true;
}

//! prints all centers of octnodes and corresponding vertices
bool Octree::dumpInfovlong() {
    std::vector<Octnode*> nodelist;
    getnodelist(nodelist,VERTEX_OCTREE);
    for ( typename std::vector<Octnode*>::iterator it = nodelist.begin(); it!=nodelist.end(); ++it) {
        (*it)->mCube.mcenter.dumpInfo();
        std::cout<<(*it)->mCube.mscale<<std::endl;
        for ( typename std::vector<Vertex*>::iterator jt = (*it)->mVertices.begin(); jt!=(*it)->mVertices.end(); ++jt) {
            (*jt)->dumpInfo();
        }
    }
    nodelist.clear();
    return true;
}




//!oointers - object octree intersection
// returns true if there is an intersection between object and cnode
// has to be implemented for each object type, as long as there is no general approach
// to decide if a object intersects cube or not
//for Vertex*
void Octree::oointers(Vertex* object, Octnode* cnode) {
    double x = object->getX();
    double y = object->getY();
    double z = object->getZ();

    for (int j=0; j<8; j++) {
        if ( fabs( x - cnode->mchildren[j]->mCube.mcenter.getX() ) <= 0.5*cnode->mCube.mscale &&
             fabs( y - cnode->mchildren[j]->mCube.mcenter.getY() ) <= 0.5*cnode->mCube.mscale &&
             fabs( z - cnode->mchildren[j]->mCube.mcenter.getZ() ) <= 0.5*cnode->mCube.mscale   ) {

            cnode->mchildren[j]->mVertices.push_back(object);
            if(mcopyelements==false) {
                break;
            }
        }
    }
}

//for Face*
void Octree::oointers(Face* object, Octnode* cnode) {
    Vector3D c = object->getCenterOfGravity();
    double x = c.getX();
    double y = c.getY();
    double z = c.getZ();

    for (int j=0; j<8; j++) {
        if ( fabs( x - cnode->mchildren[j]->mCube.mcenter.getX() ) <= cnode->mCube.mscale &&
             fabs( y - cnode->mchildren[j]->mCube.mcenter.getY() ) <= cnode->mCube.mscale &&
             fabs( z - cnode->mchildren[j]->mCube.mcenter.getZ() ) <= cnode->mCube.mscale   ) {

            //cnode->mchildren[j]->mElements.push_back(object);
            if(mcopyelements==false) {
                break;
            }
        }
    }
}

//helper function for recursive construction (we do not want to call the constructor recursively)
bool Octree::initialize(Octnode* cnode, unsigned int clevel) {

    //set current level (depth of octree)
    mmaxlevel = clevel;

    //decide if to further subdivide or mark octnode as leaf
    if ( mmaxnr < cnode->mVertices.size() &&
         clevel < mmaxlevelpermitted &&
         mEdgeLenMax < cnode->mCube.mscale ) {

        //create children
        for ( unsigned int i=0; i<8; ++i ) {
            cnode->mchildren[i] = new Octnode(cnode, i);
        }

        //assign vertices to children
        for (typename std::vector<Vertex*>::iterator it = cnode->mVertices.begin(); it!=cnode->mVertices.end(); ++it) {
            oointers(*it, cnode);
        }


        //delete vertices in node above
        cnode->mVertices.clear();


        //next level
        for ( unsigned int i=0; i<8; ++i ) {
            initialize(cnode->mchildren[i], clevel + 1);
        }

    }
    //mark as leaf
    else {
        cnode->misleaf = true;
    }
    return true;
}

void Octree::generateFacesOctreeHypothesis(Octnode* parentNodeFace,Octnode* parentNodeVertex, unsigned int treeLevel, std::vector<Face*> *incompleteFaces){
    //traverse through the vertex octree
    //create the same nodes for the face octree
    //if the node is a leaf node, then add all involved faces of the vertices to the new node
    if( parentNodeVertex->misleaf ){
        parentNodeFace->misleaf = true;
        // get all faces of the vertices inside the node
        for( unsigned int i=0; i < parentNodeVertex->mVertices.size(); i++){
            std::vector<Face*> facesOfVertex;
            parentNodeVertex->mVertices[i]->getFaces(&facesOfVertex);
            //not using insert because you have to check if the face is already in the node
            for(Face* face : facesOfVertex){
                //only add face if not already in mfaces
                if(!(parentNodeFace->mFaces.find(face) != parentNodeFace->mFaces.end())){
                    parentNodeFace->mFaces.insert(face);
                    //check here if the face is completely in octnode/cube --> if not it is an interesting face for correction
                    if(!parentNodeFace->mCube.isFaceCompletelyInCube(face)){
                        incompleteFaces->push_back(face);
                    }
                }
            }

            //parentNodeFace->mFaces.insert(parentNodeFace->mFaces.end(), facesOfVertex.begin(), facesOfVertex.end());
        }
    }
    else{
        //create children
        parentNodeFace->misleaf = false;
        for ( unsigned int i=0; i<8; ++i ) {
            parentNodeFace->mchildren[i] = new Octnode(parentNodeFace, i);
        }

        //next level
        for ( unsigned int i=0; i<8; ++i ) {
            std::vector<Face*> tmpIncompleteFaces;
            generateFacesOctreeHypothesis(parentNodeFace->mchildren[i],parentNodeVertex->mchildren[i], treeLevel + 1, &tmpIncompleteFaces);
            incompleteFaces->insert(incompleteFaces->begin(),tmpIncompleteFaces.begin(),tmpIncompleteFaces.end());
        }
    }

}

void Octree::correctFace(Face *face)
{
    //get the nodes where the face is aligned
    std::vector<Octnode*> faceNodes;
    if (!getNodesOfFace(faceNodes,face)) {
        //the face isn't in any node --> faces octree is not correct
        //TODO: error handling
    }

    //face is not completely inside the node
    //get the level of the octree in that the face is completely inside
    Octnode* parent;
    parent = faceNodes[0]->mparent;
    while(!parent->mCube.isFaceCompletelyInCube(face)){
        parent = parent->mparent;
        //loop emergency exit
        if( parent->mlevel == 1 ){
            break;
        }
    }
    //add face to all nodes that intersects their cube
    addFaceToAllIntersectedChildren(parent,face);

}
//!obsolete and not tested
bool Octree::areFacesIntersected(Face *faceA, Face *faceB)
{
    //get vertices as position of face A
    Vector3D vA0 = faceA->getVertA()->getPositionVector();
    Vector3D vA1 = faceA->getVertB()->getPositionVector();
    Vector3D vA2 = faceA->getVertC()->getPositionVector();
    //get vertices as position of face B
    Vector3D vB0 = faceB->getVertA()->getPositionVector();
    Vector3D vB1 = faceB->getVertB()->getPositionVector();
    Vector3D vB2 = faceB->getVertC()->getPositionVector();

    //1: simple rejection test: all vertices of the faceA is on the other side of the plane of faceB
    //and the otherway around

    //plane face A
    Vector3D normalA = (vA1 - vA0) * (vA2 - vA0);
    double distanceA = (-1 * normalA)*vA0;

    //plae face B
    Vector3D normalB = (vB1 - vB0) * (vB2 - vB0);
    double distanceB = (-1 * normalB)*vB0;

    //calculate distance of the vertices face A to the plane B
    double d_vA0 = (normalB * vA0) + distanceB;
    double d_vA1 = (normalB * vA1) + distanceB;
    double d_vA2 = (normalB * vA2) + distanceB;

    //calculate distance of the vertices face B to the plane A
    double d_vB0 = (normalA * vB0) + distanceA;
    double d_vB1 = (normalA * vB1) + distanceA;
    double d_vB2 = (normalA * vB2) + distanceA;

    if(((d_vA0 > 0 && d_vA1 > 0 && d_vA2 > 0) || (d_vA0 < 0 && d_vA1 < 0 && d_vA2 < 0)) &&
          ((d_vB0 > 0 && d_vB1 > 0 && d_vB2 > 0)||(d_vB0 < 0 && d_vB1 < 0 && d_vB2 < 0))){
        // face A is completely on the other side of the plane B and the otherway around
        // --> intersection not possible
        return false;
    }

    //2: calculate plane intersection --> line
    // then calculate the intervall for both faces which results from the intersection of the edges with the line
    //if the intervalls overlaps -> the faces intersects
    Vector3D directionPlaneIntersection = normalA * normalB;
    //projected vertices on intersection Lin
    Vector3D projectedVA0 = vA0;
    Vector3D projectedVA1 = vA1;
    Vector3D projectedVA2 = vA2;
    Vector3D projectedVB0 = vB0;
    Vector3D projectedVB1 = vB1;
    Vector3D projectedVB2 = vB2;

    double Dx = std::abs(directionPlaneIntersection.getX());
    double Dy = std::abs(directionPlaneIntersection.getY());
    double Dz = std::abs(directionPlaneIntersection.getZ());
    double maxD = std::max({Dx,Dy,Dz});
    if( Dx == maxD){
        projectedVA0.setX(Dx);
        projectedVA1.setX(Dx);
        projectedVA2.setX(Dx);
        projectedVB0.setX(Dx);
        projectedVB1.setX(Dx);
        projectedVB2.setX(Dx);
    }
    if( Dy == maxD){
        projectedVA0.setY(Dy);
        projectedVA1.setY(Dy);
        projectedVA2.setY(Dy);
        projectedVB0.setY(Dy);
        projectedVB1.setY(Dy);
        projectedVB2.setY(Dy);
    }
    if( Dz == maxD){
        projectedVA0.setZ(Dz);
        projectedVA1.setZ(Dz);
        projectedVA2.setZ(Dz);
        projectedVB0.setZ(Dz);
        projectedVB1.setZ(Dz);
        projectedVB2.setZ(Dz);
    }

    //calculate the intervals
    //face A
    Vector3D tA1 = projectedVA0 + ((projectedVA1 - projectedVA0)*(d_vA0/(d_vA0-d_vA1)));
    Vector3D tA2 = projectedVA2 + ((projectedVA1 - projectedVA2)*(d_vA2/(d_vA2-d_vA1)));
    //face B
    Vector3D tB1 = projectedVB0 + ((projectedVB1 - projectedVB0)*(d_vB0/(d_vB0-d_vB1)));
    Vector3D tB2 = projectedVB2 + ((projectedVB1 - projectedVB2)*(d_vB2/(d_vB2-d_vB1)));

    Line TA = Line(tA1,tA2);
    Line TB = Line(tB1,tB2);
    Line result; //no relevance
    if(TA.getLineIntersection(TB,result)){
        return true;
    }
    return false;
}

void Octree::checkIntersectionOfFaceToOtherFaces(std::vector<Face *> nodeFaces, unsigned int faceIterator)
{
    for(unsigned int j=faceIterator+1; j<nodeFaces.size();j++){
        if(nodeFaces[faceIterator]->intersectsFace(nodeFaces[j])){
            mSelfIntersectedFaces.push_back(nodeFaces[faceIterator]);
            mSelfIntersectedFaces.push_back(nodeFaces[j]);
        }
    }
}


void Octree::correctFacesOctree(std::vector<Face *> &facelist){
    // check all problematic faces (facelist = all faces with more then one node )
    ShowProgress progress = ShowProgress("[Octree]");
    progress.showProgressStart( "Generate octree of faces" );
    unsigned int i = 0;
    while(i < facelist.size()){
        std::vector<std::thread> threads(NUM_THREADS);
        // spawn n threads:
        for (int threadId = 0; threadId < NUM_THREADS ; threadId++) {
            if(i < facelist.size()){
                i++;
            }
            threads[threadId] = std::thread(&Octree::correctFace, this, std::ref(facelist[i]));

        }
        //wait until the threads are finished
        for (auto& th : threads) {
            th.join();
        }
        progress.showProgress( static_cast<double>(i)/static_cast<double>(facelist.size()),
                      "Generate octree of faces" );

    }
    progress.showProgressStop( "Generate octree of faces" );
}

void Octree::addFaceToAllIntersectedChildren(Octnode *parentNode, Face *face){
    //get all children that the face intersects
    for ( unsigned int i=0; i<8; ++i ) {
        if( parentNode->mchildren[i]->mCube.trianglecubeintersection(face) ){
            // deeper into the tree if not a leaf node
            if(parentNode->mchildren[i]->misleaf){
                //add if face is not inside
                if(!parentNode->mchildren[i]->isFaceInside(face)){
                  parentNode->mchildren[i]->mFaces.insert(face);
                }
            }
            else{
                addFaceToAllIntersectedChildren(parentNode->mchildren[i],face);
            }

        }
    }
}

//! @param copyelements ==true => elements on the border of a cube are copied to all adjacent cubes
//! if copyelements ==false => elements on the border of a cube are copied to a single cube
//! should be true for correct collision detection etc.
bool mcopyelements;

double mEdgeLenMax;
