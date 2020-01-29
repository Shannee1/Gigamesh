#include "meshGL_params.h"

#include <iostream>

#include "gmcommon.h"

using namespace std;

//! Constructor
MeshGLParams::MeshGLParams()
{
	// Initialize flags:
	for( bool& paramFlag : mParamFlag ) {
		paramFlag = false;
	}
	mParamFlag[SHOW_SMOOTH]               = true;
	mParamFlag[SHOW_FACES]                = true;
	mParamFlag[SHOW_FACES_CULLED]         = true;
	mParamFlag[BACKFACE_LIGHTING]         = true;
	mParamFlag[SHOW_SELECTION_SINGLE]     = true;
	mParamFlag[SHOW_VERTICES_SOLO]        = true;
	mParamFlag[SHOW_NPR_HATCHLINES]       = true;
	mParamFlag[SHOW_NPR_OUTLINES]         = true;
	mParamFlag[SHOW_NPR_TOON]             = true;
	mParamFlag[NPR_USE_SPECULAR]          = true;
	mParamFlag[NPR_HATCHLINE_LIGHT_INFLUENCE] = true;

	// Initalize integer parameters:
	mParamInt[SHADER_CHOICE]                = SHADER_MONOLITHIC;
	mParamInt[TEXMAP_CHOICE_FACES]          = TEXMAP_VERT_RGB;
	mParamInt[TEXMAP_CHOICE_VERETX_SPRITES] = TEXMAP_VERT_MONO;
	mParamInt[GLSL_COLMAP_CHOICE]           = GLSL_COLMAP_HOT;
	mParamInt[FUNCVAL_CUTOFF_CHOICE]        = FUNCVAL_CUTOFF_QUANTIL;
	mParamInt[COLMAP_LABEL_OFFSET]          = 0;
	mParamInt[VERTEX_SPRITE_SHAPE]          = SPRITE_SHAPE_STAR_ROUNDED;
	mParamInt[NPR_HATCH_STYLE]              = 0;
    mParamInt[NPR_OUTLINE_SOURCE]           = 0;
    mParamInt[NPR_HATCH_SOURCE]             = 0;
    mParamInt[NPR_TOON_SOURCE]              = 0;
    mParamInt[NPR_TOON_TYPE]                = 0;
    mParamInt[NPR_TOON_LIGHTING_STEPS]      = 6;
    mParamInt[NPR_TOON_HUE_STEPS]           = 40;
    mParamInt[NPR_TOON_SAT_STEPS]           = 10;
    mParamInt[NPR_TOON_VAL_STEPS]           = 10;
    mParamInt[TRANSPARENCY_NUM_LAYERS]      = 10;
	mParamInt[TRANSPARENCY_TRANS_FUNCTION]  = 3;
    mParamInt[TRANSPARENCY_SEL_LABEL]       = 0;
    mParamInt[TRANSPARENCY_OVERFLOW_HANDLING] = 0;
    mParamInt[TRANSPARENCY_BUFFER_METHOD]   = 2;
	mParamInt[DEFAULT_FRAMEBUFFER_ID]       = 0;

	// Initalize float parameters:
	mParamFlt[DATUM_SPHERE_TRANS]    =  0.9;
	mParamFlt[WAVES_COLMAP_LEN]      =  1.0;
	mParamFlt[TEXMAP_QUANTIL_MIN]    =  0.01;
	mParamFlt[TEXMAP_QUANTIL_MAX]    =  0.99;
	mParamFlt[TEXMAP_FIXED_MIN]      = _NOT_A_NUMBER_DBL_; // Nan means not set.
	mParamFlt[TEXMAP_FIXED_MAX]      = _NOT_A_NUMBER_DBL_; // Nan means not set.
	mParamFlt[TEXMAP_AUTO_MIN]       = _NOT_A_NUMBER_DBL_; // Nan means not set.
	mParamFlt[TEXMAP_AUTO_MAX]       = _NOT_A_NUMBER_DBL_; // Nan means not set.
	mParamFlt[POLYLINE_NORMAL_SCALE] =  1.0f;
	mParamFlt[POLYLINE_WIDTH]         = 0.4f;
	mParamFlt[FUNC_VALUE_LOG_GAMMA]   = 1.0f;
	mParamFlt[ISOLINES_DISTANCE]      = 4.0f;
	mParamFlt[ISOLINES_OFFSET]        = 0.0f;
	mParamFlt[ISOLINES_PIXEL_WIDTH]   = 1.5f;
	mParamFlt[BOUNDING_BOX_LINEWIDTH] = 0.3f;
	mParamFlt[NORMALS_LENGTH]         = 1.5f;
	mParamFlt[NORMALS_WIDTH]          = 0.1f;
	mParamFlt[NPR_OUTLINE_WIDTH]      = 0.0f;
	mParamFlt[NPR_HATCH_ROTATION]     = 0.0f;
	mParamFlt[NPR_HATCH_SCALE]        = 1.0f;
	mParamFlt[NPR_OUTLINE_THRESHOLD]  = 0.0f;
    mParamFlt[NPR_SPECULAR_SIZE]      = 0.2f;
	mParamFlt[TRANSPARENCY_UNIFORM_ALPHA] = 0.2f;
    mParamFlt[TRANSPARENCY_ALPHA2] = 1.0f;
    mParamFlt[TRANSPARENCY_GAMMA] = 1.0f;
    mParamFlt[BADLIT_LOWER_THRESHOLD] = 0.05f;
    mParamFlt[BADLIT_UPPER_THRESHOLD] = 0.995f;
	mParamFlt[PIN_SIZE] = 1.0f;
	mParamFlt[PIN_LINE_HEIGHT] = 0.5f;
	mParamFlt[POINTCLOUD_POINTSIZE] = 3.0f;
}

//! Get the state of a flag controlling the display of Primitives, etc.
bool MeshGLParams::getParamFlagMeshGL( MeshGLParams::eParamFlag rParamID, bool* rValue ) {
#ifdef DEBUG_SHOW_ALL_METHOD_CALLS
	cout << "[MeshGLParams::" << __FUNCTION__ << "]" << endl;
#endif
	(*rValue) = mParamFlag[rParamID];
	return true;
}

//! Set flag controlling the display of Primitives, etc.
bool MeshGLParams::setParamFlagMeshGL( MeshGLParams::eParamFlag rParamID, bool rState ) {
	// Prevent infinite loops and unnecessary changes.
	if( mParamFlag[rParamID] == rState ) {
		return false;
	}
	mParamFlag[rParamID] = rState;
	// Set usefull things:
	if( rParamID == SHOW_FUNC_VALUES_ISOLINES_SOLID ) {
		setParamFlagMeshGL( SHOW_FUNC_VALUES_ISOLINES, true );
	}
	if( rParamID == SHOW_FUNC_VALUES_ISOLINES_ONLY ) {
		setParamFlagMeshGL( SHOW_FUNC_VALUES_ISOLINES, true );
	}
	//cout << "[MeshGLParams::" << __FUNCTION__ << "] " << showFlagNr << " : " << setState << endl;
	return true;
}

//! Get an integer parameter.
bool MeshGLParams::getParamIntMeshGL( MeshGLParams::eParamInt rParamID, int* rValue ) {
#ifdef DEBUG_SHOW_ALL_METHOD_CALLS
	cout << "[MeshGLParams::" << __FUNCTION__ << "]" << endl;
#endif
	(*rValue) = mParamInt[rParamID];
	return true;
}

//! Set an integer parameter.
bool MeshGLParams::setParamIntMeshGL( MeshGLParams::eParamInt rParamID, int rValue ) {
#ifdef DEBUG_SHOW_ALL_METHOD_CALLS
	cout << "[MeshGLParams::" << __FUNCTION__ << "]" << endl;
#endif
	//cout << "[MeshGLParams::" << __FUNCTION__ << "] rParamID: " << rParamID << " rValue: " << rValue << endl;
	// Prevent infinite loops and unnecessary changes.
	if( mParamInt[rParamID] == rValue ) {
		return false;
	}
	if( rParamID == TEXMAP_CHOICE_VERETX_SPRITES ) {
		setParamFlagMeshGL( SHOW_VERTICES_ALL, true );
	}
	mParamInt[rParamID] = rValue;
	return true;
}

//! Retrive a floating point parameter.
bool MeshGLParams::getParamFloatMeshGL( MeshGLParams::eParamFlt rParamID, double* rValue ) {
#ifdef DEBUG_SHOW_ALL_METHOD_CALLS
	cout << "[MeshGLParams::" << __FUNCTION__ << "]" << endl;
#endif
	if( rValue == nullptr ) {
		return false;
	}
	(*rValue) = mParamFlt[rParamID];
	return true;
}

//! Set the parameters floating point value
//! @returns true when the value was changed. false otherwise.
bool MeshGLParams::setParamFloatMeshGL( MeshGLParams::eParamFlt rParamID, double rValue ) {
	//! Sets the parameters for ambient light, materials, etc.
	//! Ignores values out of range.
#ifdef DEBUG_SHOW_ALL_METHOD_CALLS
	cout << "[MeshGLParams::" << __FUNCTION__ << "]" << endl;
#endif
	// Prevent infinite loops:
	if( rValue == mParamFlt[rParamID] ) {
		return false;
	}
	switch( rParamID ) {
		case DATUM_SPHERE_TRANS:
			if( rValue < 0.0 ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] DATUM_SPHERE_TRANS to low: " << rValue << endl;
				return false;
			}
			if( rValue > 1.0 ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] DATUM_SPHERE_TRANS to high: " << rValue << endl;
				return false;
			}
			break;
		case WAVES_COLMAP_LEN:
			if( rValue <= 0.0 ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] WAVES_COLMAP_LEN has to be >= 0.0: " << rValue << endl;
				return false;
			}
			break;
		case TEXMAP_QUANTIL_MIN:
			if( ( rValue < 0.0 ) || ( rValue > 1.0 ) ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] TEXMAP_QUANTIL_MIN has to be > 0.0 && < 1.0: " << rValue << endl;
				return false;
			}
			break;
		case TEXMAP_QUANTIL_MAX:
			if( ( rValue < 0.0 ) || ( rValue > 1.0 ) ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] TEXMAP_QUANTIL_MAX has to be > 0.0 && < 1.0: " << rValue << endl;
				return false;
			}
			break;
		case TEXMAP_QUANTIL_MIN_ABSOLUT:
		case TEXMAP_QUANTIL_MAX_ABSOLUT:
		case TEXMAP_FIXED_MIN:
		case TEXMAP_FIXED_MAX:
		case TEXMAP_AUTO_MIN:
		case TEXMAP_AUTO_MAX:
			break;
		case POLYLINE_NORMAL_SCALE:
			if( rValue <= 0.0 ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] POLYLINE_NORMAL_SCALE has to be > 0.0: " << rValue << endl;
				return false;
			}
			break;
		case POLYLINE_WIDTH:
			if( rValue <= 0.0 ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] EDGE_WIDTH has to be > 0.0: " << rValue << endl;
				return false;
			}
			break;
		case FUNC_VALUE_LOG_GAMMA:
			//cout << "[MeshGLParams::" << __FUNCTION__ << "] : " << rValue << endl;
			break;
		case ISOLINES_DISTANCE:
			if( rValue <= 0.0 ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] ISOLINES_DISTANCE has to be > 0.0: " << rValue << endl;
				return( false );
			}
			setParamFlagMeshGL( SHOW_FUNC_VALUES_ISOLINES, true );
			break;
		case ISOLINES_OFFSET: // Should be lower than ISOLINES_DISTANCE. However, nothing happens when not. => freedom of choice.
			setParamFlagMeshGL( SHOW_FUNC_VALUES_ISOLINES, true );
			break;
		case ISOLINES_PIXEL_WIDTH:
			if( rValue <= 0.0 ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] ISOLINES_PIXEL_WIDTH has to be > 0.0: " << rValue << endl;
				return( false );
			}
			setParamFlagMeshGL( SHOW_FUNC_VALUES_ISOLINES, true );
			break;
		case BOUNDING_BOX_LINEWIDTH:
			if( rValue <= 0.0 ) {
				cerr << "[MeshGLParams::" << __FUNCTION__ << "] BOUNDING_BOX_LINEWIDTH has to be > 0.0: " << rValue << endl;
				return false;
			}
			break;
		case NORMALS_LENGTH:
		case NORMALS_WIDTH:
			break;

        case NPR_OUTLINE_WIDTH:
        case NPR_OUTLINE_THRESHOLD:
        case NPR_HATCH_ROTATION:
        case NPR_HATCH_SCALE:
        case NPR_SPECULAR_SIZE:
        case TRANSPARENCY_UNIFORM_ALPHA:
        case TRANSPARENCY_ALPHA2:
        case TRANSPARENCY_GAMMA:
        case BADLIT_LOWER_THRESHOLD:
        case BADLIT_UPPER_THRESHOLD:
		case PIN_SIZE:
		case PIN_LINE_HEIGHT:
		case POINTCLOUD_POINTSIZE:
            break;

		default:
			cerr << "[MeshGLParams::" << __FUNCTION__ << "] ERROR: Unknown parameter ID: " << rParamID << endl;
			break;
	}
	mParamFlt[rParamID] = rValue;
	return true;
}
