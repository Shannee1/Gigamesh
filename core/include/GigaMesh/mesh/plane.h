#ifndef PLANE_H
#define PLANE_H

#include "vertex.h"
#include "face.h"

class Vertex;

//!
//! \brief Plane line class. (Layer 0)
//!
//! ....
//! ....
//!
//! Layer 0
//!

class Plane : public Primitive {
	public:
		// Constructor and deconstructor:
		Plane( Vector3D rPosA, Vector3D rPosB, Vector3D rPosC );
		Plane( Vector3D rPosA, Vector3D rNormal );
		Plane( Vector3D* rPlaneHNF );
		Plane( Face& f );
		Plane( const Plane& rOther );
		Plane();
		~Plane();

		// Information retrival:
		virtual double   getX() const;
		virtual double   getY() const;
		virtual double   getZ() const;
		virtual double   getNormalX();
		virtual double   getNormalY();
		virtual double   getNormalZ();
		Vector3D         getHNF() const;

		        double   getHNFA() const;
		        double   getHNFB() const;
		        double   getHNFC() const;
		        double   getHNFD() const;

		virtual	int      getType();

		        bool     isValid();

		// Transformation
		virtual bool     applyTransfrom( Matrix4D* transMat );

		enum ePlaneVerts{ PLANE_VERT_A, PLANE_VERT_B, PLANE_VERT_C };

		enum ePlaneDefinedBy{
			PLANE_UNDEFINED,               //! The plane object was created, but not properly defined.
			THREE_POSITIONS,               //! Plane defined by three points in R3.
			POSITION_AND_NORMAL,           //! Plane defined using a position vector and a normal (direction - not an axis!).
			HESSE_NORMAL_FORM,             //! Plane defined as HNF.
			AXIS_POINTS_AND_POSITION       //! Plane using the tip ("Spitze") and bottom ("Schaft") of an axis and the center of gravity of a Primitive.
		};

		// Retrieve information
		        bool            getVertCoords( double* rCoords ) const;
		        bool            getVertPositions( Vector3D& rPosA, Vector3D& rPosB, Vector3D& rPosC ) const;
		        bool            getVertHNFCOG( Vector3D& rPosHNF, Vector3D& rPosCOG ) const;
		        bool            getFlipped() const;
		        ePlaneDefinedBy getDefinedBy() const;

		// Operations:
		double classifyPoint( Vertex* point );
		bool   setData( ePlaneVerts rPosIdx, Vector3D* rPos );
		bool   setPlaneHNF( Vector3D* rPlaneHNF );
		bool   setPlaneHNF( Vector3D* rPosA, Vector3D* rPlaneHNF );
		bool   setPlaneByAxisAndPosition( const Vector3D& rAxisTop, const Vector3D& rAxisBottom, const Vector3D& rPos );
		bool   flipPlane();

		// Calculations
		bool   getPointProjected( Vector3D rPoint, Vector3D* rProjection );
		bool   getChangeOfBasisTrans( Matrix4D* rTransMat, bool rUseAxisToY );
		bool   getPlaneIntersection( Plane& rOther, Vector3D& rLinePosA, Vector3D& rLinePosB );

	private:
		// Vertices
		Vertex* mVertA;            //!< reference to Vertex A
		Vertex* mVertB;            //!< reference to Vertex B
		Vertex* mVertC;            //!< reference to Vertex C

		        void   setPositions(  const Vector3D& rPosA, const Vector3D& rPosB, const Vector3D& rPosC  );
		virtual double calcNormalX();
		virtual double calcNormalY();
		virtual double calcNormalZ();
		        bool   updateHNFCOG();

		Vector3D mHNF;             //! Normal of plane and distance from origin
		Vector3D mCog;             //! Center of gravity vector

		bool            mWasFlipped;     //!< Rember that this plane had another orientation.
		ePlaneDefinedBy mDefinitionType; //!< Store the type of definition used.
};

bool solve3dimLinearSystem(const Vector3D lg1, const Vector3D lg2, const Vector3D lg3, double& result1, double& result2, double &result3 );
bool solve2dimLinearSystem( const Vector3D lg1, const Vector3D lg2, double& result1, double& result2 );

std::ostream& operator<<( std::ostream& o, const Plane& p );

#endif