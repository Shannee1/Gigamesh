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

#include <GigaMesh/mesh/octnode.h>
#include <algorithm>

// Constructor for the root node.
Octnode::Octnode( Vector3D* center, double scale ) : \
        OCTNODEDEFAULTS,                    \
        mparent( nullptr ),                    \
        mlevel( 0 ),                        \
        mCube( *center, scale ) {
    // do nothing
}

// Constructor for a child node.
Octnode::Octnode( Octnode* parent, int nr ) : \
       OCTNODEDEFAULTS,              \
       mparent( parent ),            \
       mlevel( parent->mlevel+1 ),   \
       mCube( Cube(mparent->mCube.mcenter, 0.5*mparent->mCube.mscale).getVertex(nr), 0.5*mparent->mCube.mscale ) {
    // do nothing
}

Octnode::Octnode(Octnode &copy) :\
    OCTNODEDEFAULTS,              \
    mlevel(copy.mlevel),       \
    mCube(copy.mCube),            \
    mVertices(copy.mVertices)
{
   // do nothing
}



//!get all the leaf nodes of the octnode
//! traverse all children recursive until the leafs were found
std::vector<Octnode*> Octnode::getLeafNodes(){
    std::vector<Octnode*> leafNodes;
    //check all children of the node
    for( unsigned int i=0; i < 8; i++){
        if(mchildren[i]->misleaf == true){
            //nodes without vertices or faces are not interesting
            if(mchildren[i]->mVertices.size() > 0 || mchildren[i]->mFaces.size() > 0){
                leafNodes.push_back(mchildren[i]);
            }
        }
        else{
            //search in the children
            std::vector<Octnode*> tmpNodes;
            tmpNodes = mchildren[i]->getLeafNodes();
            leafNodes.insert(leafNodes.end(), tmpNodes.begin(), tmpNodes.end());
        }
    }

    return leafNodes;
}

//! traverse all children recursive until all nodes were added
std::vector<Octnode*> Octnode::getNodeList(){
    std::vector<Octnode*> nodes;
    //check all children of the node
    for( unsigned int i=0; i < 8; i++){
        if(mchildren[i]->misleaf == false){
            //search in the child
            std::vector<Octnode*> tmpNodes;
            tmpNodes = mchildren[i]->getNodeList();
            nodes.insert(nodes.end(), tmpNodes.begin(), tmpNodes.end());
        }
        nodes.push_back(mchildren[i]);
    }

    return nodes;
}

bool Octnode::isFaceInside(Face *face){
    if(std::find(mFaces.begin(), mFaces.end(), face) != mFaces.end()) {
        return true;
    } else {
        return false;
    }

}

/// comparison function to sort nodes by level
bool Octnode::compareoctnode( Octnode* rNode1, Octnode* rNode2 ) {
    return ( rNode1->mlevel < rNode2->mlevel );
}