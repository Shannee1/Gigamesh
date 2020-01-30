#include <GigaMesh/mesh/polyedge.h>

#define POLYEDGEINITDEFAULTS                \
	mVertPoly( nullptr ),               \
	mFromFace( nullptr ),               \
	mFromEdge( Face::EDGE_NONE ),       \
	mCurvature( _NOT_A_NUMBER_ )

using namespace std;

//! Constructor
PolyEdge::PolyEdge( Vertex* rSomeVert, Face* rFromSomeFace, Face::eEdgeNames rFromSomeEdge )
	: POLYEDGEINITDEFAULTS {
	mVertPoly = rSomeVert;
	mFromFace = rFromSomeFace;
	mFromEdge = rFromSomeEdge;
}

//! Destructor
//!
//! Does NOT remove any vertices, because this will be done by the Mesh:
PolyEdge::~PolyEdge() {
	mVertPoly  = nullptr;
	mFromFace  = nullptr;
	mFromEdge  = Face::EDGE_NONE;
	mCurvature = _NOT_A_NUMBER_;
}

// --- Position and normal vector ------------------------------------------------------------------------------------------------------------------------------

//! Return the position vector of the Vertex.
//!
//! @returns false in case of an error. True otherwise.
bool PolyEdge::getPosition( Vector3D* rPos ) {
	// Sanity check
	if( rPos == nullptr ) {
		return( false );
	}
	// Write
	rPos->set( mVertPoly->getCenterOfGravity() );
	return( true );
}

//! Return the position vector of the Vertex and
//! apply the given transformation matrix.
//!
//! @returns false in case of an error. True otherwise.
bool PolyEdge::getPositionTransformed( Vector3D* rPos, const Matrix4D& rTransform ) {
	// Sanity check
	if( rPos == nullptr ) {
		return( false );
	}
	// Write
	rPos->set( mVertPoly->getCenterOfGravity() * rTransform );
	return( true );
}

//! Return the normal vector of the Vertex, when present.
//! @returns false in case of an error including an undefined normal. True otherwise.
bool PolyEdge::getNormal( Vector3D* rNormal ) {
	if( rNormal == nullptr ) {
		return false;
	}
	Vector3D vertNormal = mVertPoly->getNormal();
	if( !isnormal( vertNormal.getLength3() ) ) {
		return false;
	}
	rNormal->set( vertNormal );
	return true;
}

// --- Function Value (of underlying vertex) -------------------------------------------------------------------------------------------------------------------

//! Set function value to underlying vertex.
bool PolyEdge::setFuncValue( double  rVal ) {
	return mVertPoly->setFuncValue( rVal );
}

//! Get function value to underlying vertex.
bool PolyEdge::getFuncValue( double* rVal ) const {
	if( rVal == nullptr ) {
		return false;
	}
	return mVertPoly->getFuncValue( rVal );
}
