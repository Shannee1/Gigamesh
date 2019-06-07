#include "PinRenderer.h"
#include <vector>
#include <QOpenGLFunctions_3_3_Core>
#include "glmacros.h"

PinRenderer::PinRenderer() : mIsInitialized(false),
							 mAllocatedBufferSize(0),
							 mPinPositionBuffer(QOpenGLBuffer::VertexBuffer),
							 mPinNormalBuffer(QOpenGLBuffer::VertexBuffer),
							 mPinHeadFlags(QOpenGLBuffer::VertexBuffer),
							 mPinFaceIndexBuffer(QOpenGLBuffer::IndexBuffer),
							 mInstancedPinBuffer(QOpenGLBuffer::VertexBuffer)
{

}

PinRenderer::~PinRenderer()
{
	if(mIsInitialized)
		destroy();
}

bool PinRenderer::init()
{
	if(mIsInitialized)
		return true;

	mGL.initializeOpenGLFunctions();

	if(!mShaderPins.isLinked())
	{
		if(!initializeShader())
		{
			return false;
		}
	}

	if(!initializePinGeometry())
		return false;

	mIsInitialized = true;
	return true;
}

void PinRenderer::destroy()
{
	if(!mIsInitialized)
		return;

	mPinPositionBuffer.destroy();
	mPinNormalBuffer.destroy();
	mPinHeadFlags.destroy();
	mPinFaceIndexBuffer.destroy();
	mInstancedPinBuffer.destroy();
	mVAO.destroy();

	mIsInitialized = false;
	mAllocatedBufferSize = 0;
}

void PinRenderer::render(std::vector<PinVertex> &pinPoints, const QMatrix4x4& projectionMatrix, const QMatrix4x4& modelViewMatrix, float pinSize)
{
	if(!mIsInitialized || pinPoints.empty())
	{
		return;
	}
	GLint prevVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);

	mShaderPins.bind();
	mVAO.bind();

	if(mAllocatedBufferSize < pinPoints.size())
	{
		resizePinPointsBuffer(pinPoints.size());
	}

	mInstancedPinBuffer.bind();
	mInstancedPinBuffer.write(0,pinPoints.data(), sizeof(PinVertex) * pinPoints.size());
	PRINT_OPENGL_ERROR( "Writing pin instenced data" );


	mGL.glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Draw
	mShaderPins.setUniformValue("modelview", modelViewMatrix);
	mShaderPins.setUniformValue("projection", projectionMatrix);

	mShaderPins.setUniformValue("scaleFactor", pinSize );
	mGL.glDrawElementsInstanced(GL_TRIANGLES, mPinFaceIndicesSize, GL_UNSIGNED_INT,  nullptr, pinPoints.size());
	PRINT_OPENGL_ERROR( "glDrawArrays( ... )" );

	mVAO.release();
	mShaderPins.release();

	mGL.glBindVertexArray(prevVAO);
}

bool PinRenderer::isInitialized()
{
	return mIsInitialized;
}

bool PinRenderer::initializeShader()
{

	if(!mShaderPins.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GMShaders/vertex_pin.vert"))
	{
		QString errMsg = mShaderPins.log();
		errMsg = errMsg.left( errMsg.indexOf( "***"));
		std::cerr << "[PinRenderer::" << __FUNCTION__ << "] ERROR: compiling shader program (" << "PIN" << "/vert): " << errMsg.toStdString() << std::endl;
		mShaderPins.removeAllShaders();
		return false;
	}

	if(!mShaderPins.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GMShaders/vertex_pin.frag"))
	{
		QString errMsg = mShaderPins.log();
		errMsg = errMsg.left( errMsg.indexOf( "***"));
		std::cerr << "[PinRenderer::" << __FUNCTION__ << "] ERROR: compiling shader program (" << "PIN" << "/vert): " << errMsg.toStdString() << std::endl;
		mShaderPins.removeAllShaders();
		return false;
	}

	if(!mShaderPins.link())
	{
		QString errMsg = mShaderPins.log();
		errMsg = errMsg.left( errMsg.indexOf( "***"));
		std::cerr << "[PinRenderer::" << __FUNCTION__ << "] ERROR: compiling shader program (" << "PIN" << "/vert): " << errMsg.toStdString() << std::endl;
		mShaderPins.removeAllShaders();
		return false;
	}
	return true;
}

bool PinRenderer::initializePinGeometry()
{

	GLint prevVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);

	mVAO.create();
	mVAO.bind();
	std::vector<float> positions {0, 0.032895, 0, 0, 0.075515, 0.276868, 0.028898, 0.069767, 0.276868, 0.012588, 0.030391, 0, 0.053397, 0.053397, 0.276868, 0.02326, 0.02326, 0, 0.069767, 0.028898, 0.276868, 0.030391, 0.012588, 0, 0.075515, -0, 0.276868, 0.032895, -0, 0, 0.069767, -0.028898, 0.276868, 0.030391, -0.012588, 0, 0.053397, -0.053397, 0.276868, 0.02326, -0.02326, 0, 0.028898, -0.069767, 0.276868, 0.012588, -0.030391, 0, 0, -0.075515, 0.276868, 0, -0.032895, 0, -0.028898, -0.069767, 0.276868, -0.012588, -0.030391, 0, -0.053397, -0.053397, 0.276868, -0.02326, -0.02326, 0, -0.069767, -0.028898, 0.276868, -0.030391, -0.012588, 0, -0.075515, 0, 0.276868, -0.032895, 0, 0, -0.069767, 0.028898, 0.276868, -0.030391, 0.012588, 0, -0.053397, 0.053397, 0.276868, -0.02326, 0.02326, 0, -0, 0.138434, 2.9828, 0.052976, 0.127896, 2.9828, -0.028898, 0.069767, 0.276868, -0.012588, 0.030391, 0, 0.097888, 0.097888, 2.9828, -0.052976, 0.127896, 2.9828, -0.097888, 0.097888, 2.9828, -0.127896, 0.052976, 2.9828, -0.138434, 0, 2.9828, -0.127896, -0.052976, 2.9828, -0.097888, -0.097888, 2.9828, -0.052976, -0.127896, 2.9828, -0, -0.138434, 2.9828, 0.052976, -0.127896, 2.9828, 0.097888, -0.097888, 2.9828, 0.127896, -0.052976, 2.9828, 0.138434, -0, 2.9828, 0.127896, 0.052976, 2.9828, 0, 0.36402, 3.00985, 0, 0.475615, 3.17687, 0.18201, 0.439411, 3.17687, 0.139304, 0.33631, 3.00985, 0, 0.514801, 3.37387, 0, 0.475615, 3.57088, 0.18201, 0.439411, 3.57088, 0.197006, 0.475615, 3.37387, 0, 0.36402, 3.73789, 0, 0.197006, 3.84949, 0.075391, 0.18201, 3.84949, 0.139304, 0.33631, 3.73789, 0, 0.197006, 2.89826, 0.075391, 0.18201, 2.89826, 0, 0, 3.88867, 0, 0, 2.85907, 0.257401, 0.257401, 3.00985, 0.139304, 0.139304, 2.89826, 0.36402, 0.36402, 3.37387, 0.33631, 0.33631, 3.17687, 0.257401, 0.257401, 3.73789, 0.33631, 0.33631, 3.57088, 0.139304, 0.139304, 3.84949, 0.33631, 0.139304, 3.73789, 0.43941, 0.18201, 3.57088, 0.18201, 0.075391, 3.84949, 0.18201, 0.075391, 2.89826, 0.43941, 0.18201, 3.17687, 0.33631, 0.139304, 3.00985, 0.475614, 0.197006, 3.37387, 0.197006, 0, 2.89826, 0.475614, 0, 3.17687, 0.36402, 0, 3.00985, 0.475614, 0, 3.57088, 0.514801, 0, 3.37387, 0.197006, 0, 3.84949, 0.36402, 0, 3.73789, 0.18201, -0.075391, 3.84949, 0.33631, -0.139304, 3.73789, 0.33631, -0.139304, 3.00985, 0.18201, -0.075391, 2.89826, 0.475614, -0.197006, 3.37387, 0.43941, -0.18201, 3.17687, 0.43941, -0.18201, 3.57088, 0.257401, -0.257401, 3.00985, 0.139304, -0.139304, 2.89826, 0.364019, -0.364019, 3.37387, 0.33631, -0.33631, 3.17687, 0.257401, -0.257401, 3.73789, 0.33631, -0.33631, 3.57088, 0.139304, -0.139304, 3.84949, 0.075391, -0.18201, 3.84949, 0.075391, -0.18201, 2.89826, 0.18201, -0.43941, 3.17687, 0.139304, -0.33631, 3.00985, 0.18201, -0.43941, 3.57088, 0.197006, -0.475614, 3.37387, 0.139304, -0.33631, 3.73789, -0, -0.475614, 3.17687, -0, -0.364019, 3.00985, -0, -0.475614, 3.57088, -0, -0.514801, 3.37387, -0, -0.197006, 3.84949, -0, -0.364019, 3.73789, -0, -0.197006, 2.89826, -0.139304, -0.33631, 3.00985, -0.075391, -0.18201, 2.89826, -0.197006, -0.475614, 3.37387, -0.18201, -0.43941, 3.17687, -0.139304, -0.33631, 3.73789, -0.18201, -0.43941, 3.57088, -0.075391, -0.18201, 3.84949, -0.36402, -0.364019, 3.37387, -0.33631, -0.33631, 3.17687, -0.257401, -0.2574, 3.73789, -0.33631, -0.33631, 3.57088, -0.139304, -0.139304, 3.84949, -0.139304, -0.139304, 2.89826, -0.257401, -0.2574, 3.00985, -0.18201, -0.075391, 2.89826, -0.43941, -0.182009, 3.17687, -0.33631, -0.139304, 3.00985, -0.43941, -0.182009, 3.57088, -0.475614, -0.197006, 3.37387, -0.18201, -0.075391, 3.84949, -0.33631, -0.139304, 3.73789, -0.475614, 0, 3.57088, -0.514801, 0, 3.37387, -0.197006, 0, 3.84949, -0.364019, 0, 3.73789, -0.364019, 0, 3.00985, -0.197006, 0, 2.89826, -0.475614, 0, 3.17687, -0.33631, 0.139305, 3.00985, -0.18201, 0.075391, 2.89826, -0.475614, 0.197006, 3.37387, -0.43941, 0.18201, 3.17687, -0.33631, 0.139305, 3.73789, -0.43941, 0.18201, 3.57088, -0.18201, 0.075391, 3.84949, -0.2574, 0.257401, 3.73789, -0.33631, 0.336311, 3.57088, -0.139304, 0.139304, 3.84949, -0.139304, 0.139304, 2.89826, -0.33631, 0.336311, 3.17687, -0.2574, 0.257401, 3.00985, -0.364019, 0.36402, 3.37387, -0.075391, 0.18201, 2.89826, -0.182009, 0.439411, 3.17687, -0.139304, 0.33631, 3.00985, -0.182009, 0.439411, 3.57088, -0.197006, 0.475615, 3.37387, -0.075391, 0.18201, 3.84949, -0.139304, 0.33631, 3.73789 };
	std::vector<float> normals {0, 0.693289, -0.720634, 0, 0.996185, -0.087161, 0.381207, 0.920347, -0.087161, 0.265297, 0.640522, -0.720634, 0.704398, 0.704398, -0.087161, 0.490219, 0.490219, -0.720634, 0.920347, 0.381207, -0.087161, 0.640522, 0.265297, -0.720634, 0.996185, 0, -0.087161, 0.693289, 0, -0.720634, 0.920347, -0.381207, -0.087161, 0.640522, -0.265297, -0.720634, 0.704398, -0.704398, -0.087161, 0.490219, -0.490219, -0.720634, 0.381207, -0.920347, -0.087161, 0.265297, -0.640522, -0.720634, 0, -0.996185, -0.087161, 0, -0.693289, -0.720634, -0.381207, -0.920347, -0.087161, -0.265297, -0.640522, -0.720634, -0.704398, -0.704398, -0.087161, -0.490219, -0.490219, -0.720634, -0.920347, -0.381207, -0.087161, -0.640522, -0.265297, -0.720634, -0.996185, 0, -0.087161, -0.693289, 0, -0.720634, -0.920347, 0.381207, -0.087161, -0.640522, 0.265297, -0.720634, -0.704398, 0.704398, -0.087161, -0.490219, 0.490219, -0.720634, 0, 0.753777, 0.657094, 0.288461, 0.696402, 0.657094, -0.381207, 0.920347, -0.087161, -0.265297, 0.640522, -0.720634, 0.533006, 0.533006, 0.657094, -0.288461, 0.696402, 0.657094, -0.533006, 0.533006, 0.657094, -0.696402, 0.288461, 0.657094, -0.753777, 0, 0.657094, -0.696402, -0.288461, 0.657094, -0.533006, -0.533006, 0.657094, -0.288461, -0.696402, 0.657094, 0, -0.753777, 0.657094, 0.288461, -0.696402, 0.657094, 0.533006, -0.533006, 0.657094, 0.696402, -0.288461, 0.657094, 0.753777, 0, 0.657094, 0.696402, 0.288461, 0.657094, 0, 0.718833, -0.695151, 0, 0.927274, -0.37434, 0.354839, 0.856685, -0.37434, 0.275094, 0.664113, -0.695151, 0, 0.999969, 0, 0, 0.927274, 0.37434, 0.354839, 0.856685, 0.37434, 0.382672, 0.923856, 0, 0, 0.718833, 0.695151, 0, 0.403088, 0.915159, 0.154241, 0.372387, 0.915159, 0.275094, 0.664113, 0.695151, 0, 0.403088, -0.915159, 0.154241, 0.372387, -0.915159, 0, 0, 1, 0, 0, -1, 0.508286, 0.508286, -0.695151, 0.285012, 0.285012, -0.915159, 0.707083, 0.707083, 0, 0.65566, 0.65566, -0.37434, 0.508286, 0.508286, 0.695151, 0.65566, 0.65566, 0.37434, 0.285012, 0.285012, 0.915159, 0.664113, 0.275094, 0.695151, 0.856685, 0.354839, 0.37434, 0.372387, 0.154241, 0.915159, 0.372387, 0.154241, -0.915159, 0.856685, 0.354839, -0.37434, 0.664113, 0.275094, -0.695151, 0.923856, 0.382672, 0, 0.403088, 0, -0.915159, 0.927274, 0, -0.37434, 0.718833, 0, -0.695151, 0.927274, 0, 0.37434, 1, 0, 0, 0.403088, 0, 0.915159, 0.718833, 0, 0.695151, 0.372387, -0.154241, 0.915128, 0.664113, -0.275094, 0.695151, 0.664113, -0.275094, -0.695151, 0.372387, -0.154241, -0.915159, 0.923856, -0.382672, 0, 0.856685, -0.354839, -0.37434, 0.856685, -0.354839, 0.37434, 0.508286, -0.508286, -0.695151, 0.285012, -0.285012, -0.915159, 0.707083, -0.707083, 0, 0.65566, -0.65566, -0.37434, 0.508286, -0.508286, 0.695151, 0.65566, -0.65566, 0.37434, 0.285012, -0.285012, 0.915159, 0.154241, -0.372387, 0.915159, 0.154241, -0.372387, -0.915159, 0.354839, -0.856685, -0.37434, 0.275094, -0.664113, -0.695151, 0.354839, -0.856685, 0.37434, 0.382672, -0.923856, 0, 0.275094, -0.664113, 0.695151, 0, -0.927274, -0.37434, 0, -0.718833, -0.695151, 0, -0.927274, 0.37434, 0, -1, 0, 0, -0.403088, 0.915159, 0, -0.718833, 0.695151, 0, -0.403088, -0.915159, -0.275094, -0.664113, -0.695151, -0.154241, -0.372387, -0.915159, -0.382672, -0.923856, 0, -0.354839, -0.856685, -0.37434, -0.275094, -0.664113, 0.695151, -0.354839, -0.856685, 0.37434, -0.154241, -0.372387, 0.915159, -0.707083, -0.707083, 0, -0.65566, -0.65566, -0.37434, -0.508286, -0.508286, 0.695151, -0.65566, -0.65566, 0.37434, -0.285012, -0.285012, 0.915159, -0.285012, -0.285012, -0.915159, -0.508286, -0.508286, -0.695151, -0.372387, -0.154241, -0.915159, -0.856685, -0.354839, -0.37434, -0.664113, -0.275094, -0.695151, -0.856685, -0.354839, 0.37434, -0.923856, -0.382672, 0, -0.372387, -0.154241, 0.915159, -0.664113, -0.275094, 0.695151, -0.927274, 0, 0.37434, -1, 0, 0, -0.403088, 0, 0.915159, -0.718833, 0, 0.695151, -0.718833, 0, -0.695151, -0.403088, 0, -0.915159, -0.927274, 0, -0.37434, -0.664113, 0.275094, -0.695151, -0.372387, 0.154241, -0.915159, -0.923856, 0.382672, 0, -0.856685, 0.354839, -0.37434, -0.664113, 0.275094, 0.695151, -0.856685, 0.354839, 0.37434, -0.372387, 0.154241, 0.915159, -0.508286, 0.508286, 0.695151, -0.65566, 0.65566, 0.37434, -0.285012, 0.285012, 0.915159, -0.285012, 0.285012, -0.915159, -0.65566, 0.65566, -0.37434, -0.508286, 0.508286, -0.695151, -0.707083, 0.707083, 0, -0.154241, 0.372387, -0.915159, -0.354839, 0.856685, -0.37434, -0.275094, 0.664113, -0.695151, -0.354839, 0.856685, 0.37434, -0.382672, 0.923856, 0, -0.154241, 0.372387, 0.915159, -0.275094, 0.664113, 0.695151 };
	std::vector<float> headVertexFlag {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	std::vector<unsigned int> faceIndices {0, 1, 2, 0, 2, 3, 3, 2, 4, 3, 4, 5, 5, 4, 6, 5, 6, 7, 7, 6, 8, 7, 8, 9, 9, 8, 10, 9, 10, 11, 11, 10, 12, 11, 12, 13, 13, 12, 14, 13, 14, 15, 15, 14, 16, 15, 16, 17, 17, 16, 18, 17, 18, 19, 19, 18, 20, 19, 20, 21, 21, 20, 22, 21, 22, 23, 23, 22, 24, 23, 24, 25, 25, 24, 26, 25, 26, 27, 27, 26, 28, 27, 28, 29, 2, 1, 30, 2, 30, 31, 29, 28, 32, 29, 32, 33, 33, 32, 1, 33, 1, 0, 33, 0, 3, 3, 5, 7, 7, 9, 11, 11, 13, 15, 15, 17, 19, 19, 21, 23, 23, 25, 27, 27, 29, 33, 33, 3, 7, 7, 11, 15, 15, 19, 23, 23, 27, 33, 33, 7, 15, 15, 23, 33, 34, 31, 30, 30, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 34, 34, 30, 36, 36, 38, 40, 40, 42, 44, 44, 46, 34, 34, 36, 40, 40, 44, 34, 26, 24, 38, 26, 38, 37, 12, 10, 45, 12, 45, 44, 22, 20, 40, 22, 40, 39, 1, 32, 35, 1, 35, 30, 8, 6, 47, 8, 47, 46, 18, 16, 42, 18, 42, 41, 28, 26, 37, 28, 37, 36, 4, 2, 31, 4, 31, 34, 14, 12, 44, 14, 44, 43, 24, 22, 39, 24, 39, 38, 10, 8, 46, 10, 46, 45, 20, 18, 41, 20, 41, 40, 32, 28, 36, 32, 36, 35, 6, 4, 34, 6, 34, 47, 16, 14, 43, 16, 43, 42, 48, 49, 50, 48, 50, 51, 52, 53, 54, 52, 54, 55, 56, 57, 58, 56, 58, 59, 60, 48, 51, 60, 51, 61, 49, 52, 55, 49, 55, 50, 53, 56, 59, 53, 59, 54, 57, 62, 58, 63, 60, 61, 61, 51, 64, 61, 64, 65, 50, 55, 66, 50, 66, 67, 54, 59, 68, 54, 68, 69, 58, 62, 70, 63, 61, 65, 51, 50, 67, 51, 67, 64, 55, 54, 69, 55, 69, 66, 59, 58, 70, 59, 70, 68, 69, 68, 71, 69, 71, 72, 70, 62, 73, 63, 65, 74, 64, 67, 75, 64, 75, 76, 66, 69, 72, 66, 72, 77, 68, 70, 73, 68, 73, 71, 65, 64, 76, 65, 76, 74, 67, 66, 77, 67, 77, 75, 63, 74, 78, 76, 75, 79, 76, 79, 80, 77, 72, 81, 77, 81, 82, 71, 73, 83, 71, 83, 84, 74, 76, 80, 74, 80, 78, 75, 77, 82, 75, 82, 79, 72, 71, 84, 72, 84, 81, 73, 62, 83, 84, 83, 85, 84, 85, 86, 78, 80, 87, 78, 87, 88, 79, 82, 89, 79, 89, 90, 81, 84, 86, 81, 86, 91, 83, 62, 85, 63, 78, 88, 80, 79, 90, 80, 90, 87, 82, 81, 91, 82, 91, 89, 88, 87, 92, 88, 92, 93, 90, 89, 94, 90, 94, 95, 91, 86, 96, 91, 96, 97, 85, 62, 98, 63, 88, 93, 87, 90, 95, 87, 95, 92, 89, 91, 97, 89, 97, 94, 86, 85, 98, 86, 98, 96, 98, 62, 99, 63, 93, 100, 92, 95, 101, 92, 101, 102, 94, 97, 103, 94, 103, 104, 96, 98, 99, 96, 99, 105, 93, 92, 102, 93, 102, 100, 95, 94, 104, 95, 104, 101, 97, 96, 105, 97, 105, 103, 102, 101, 106, 102, 106, 107, 104, 103, 108, 104, 108, 109, 105, 99, 110, 105, 110, 111, 100, 102, 107, 100, 107, 112, 101, 104, 109, 101, 109, 106, 103, 105, 111, 103, 111, 108, 99, 62, 110, 63, 100, 112, 112, 107, 113, 112, 113, 114, 106, 109, 115, 106, 115, 116, 108, 111, 117, 108, 117, 118, 110, 62, 119, 63, 112, 114, 107, 106, 116, 107, 116, 113, 109, 108, 118, 109, 118, 115, 111, 110, 119, 111, 119, 117, 116, 115, 120, 116, 120, 121, 118, 117, 122, 118, 122, 123, 119, 62, 124, 63, 114, 125, 113, 116, 121, 113, 121, 126, 115, 118, 123, 115, 123, 120, 117, 119, 124, 117, 124, 122, 114, 113, 126, 114, 126, 125, 63, 125, 127, 126, 121, 128, 126, 128, 129, 120, 123, 130, 120, 130, 131, 122, 124, 132, 122, 132, 133, 125, 126, 129, 125, 129, 127, 121, 120, 131, 121, 131, 128, 123, 122, 133, 123, 133, 130, 124, 62, 132, 131, 130, 134, 131, 134, 135, 133, 132, 136, 133, 136, 137, 127, 129, 138, 127, 138, 139, 128, 131, 135, 128, 135, 140, 130, 133, 137, 130, 137, 134, 132, 62, 136, 63, 127, 139, 129, 128, 140, 129, 140, 138, 139, 138, 141, 139, 141, 142, 140, 135, 143, 140, 143, 144, 134, 137, 145, 134, 145, 146, 136, 62, 147, 63, 139, 142, 138, 140, 144, 138, 144, 141, 135, 134, 146, 135, 146, 143, 137, 136, 147, 137, 147, 145, 146, 145, 148, 146, 148, 149, 147, 62, 150, 63, 142, 151, 141, 144, 152, 141, 152, 153, 143, 146, 149, 143, 149, 154, 145, 147, 150, 145, 150, 148, 142, 141, 153, 142, 153, 151, 144, 143, 154, 144, 154, 152, 63, 151, 155, 153, 152, 156, 153, 156, 157, 154, 149, 158, 154, 158, 159, 148, 150, 160, 148, 160, 161, 151, 153, 157, 151, 157, 155, 152, 154, 159, 152, 159, 156, 149, 148, 161, 149, 161, 158, 150, 62, 160, 161, 160, 57, 161, 57, 56, 155, 157, 48, 155, 48, 60, 156, 159, 52, 156, 52, 49, 158, 161, 56, 158, 56, 53, 160, 62, 57, 63, 155, 60, 157, 156, 49, 157, 49, 48, 159, 158, 53, 159, 53, 52 };

	mPinFaceIndicesSize = faceIndices.size();

	mPinPositionBuffer.create();
	mPinPositionBuffer.bind();
	mPinPositionBuffer.allocate( positions.data(), sizeof(float) * positions.size());
	mShaderPins.setAttributeBuffer( "position", GL_FLOAT, 0, 3, 0 );
	mShaderPins.enableAttributeArray( "position" );


	mPinNormalBuffer.create();
	mPinNormalBuffer.bind();
	mPinNormalBuffer.allocate( normals.data(), sizeof(float) * normals.size());
	mShaderPins.setAttributeBuffer( "vNormal", GL_FLOAT, 0, 3, 0 );
	mShaderPins.enableAttributeArray( "vNormal" );

	mPinHeadFlags.create();
	mPinHeadFlags.bind();
	mPinHeadFlags.allocate( headVertexFlag.data(), sizeof(float) * headVertexFlag.size());
	mShaderPins.setAttributeBuffer( "vHeadFlag", GL_FLOAT, 0, 1, 0 );
	mShaderPins.enableAttributeArray( "vHeadFlag" );


	mPinFaceIndexBuffer.create();
	mPinFaceIndexBuffer.bind();
	mPinFaceIndexBuffer.allocate( faceIndices.data(), sizeof(unsigned int) * faceIndices.size());

	mInstancedPinBuffer.create();
	mInstancedPinBuffer.bind();
	mInstancedPinBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	mInstancedPinBuffer.allocate(0);
	PRINT_OPENGL_ERROR( "enabling attribute Array vHeadFlag" );


	mShaderPins.setAttributeBuffer( "offsetInstanced", GL_FLOAT, offsetof(PinVertex, position), 3, static_cast<int>(sizeof(PinVertex)) );
	PRINT_OPENGL_ERROR( "Shader setting Attribute buffer offsetInstanced" );
	mShaderPins.enableAttributeArray( "offsetInstanced" );
	PRINT_OPENGL_ERROR( "Shader enableAttributeArray offsetInstanced" );

	mShaderPins.setAttributeBuffer( "colorInstanced", GL_UNSIGNED_BYTE, offsetof(PinVertex, color), 3, static_cast<int>(sizeof(PinVertex)));
	mShaderPins.enableAttributeArray("colorInstanced");
	PRINT_OPENGL_ERROR( "Shader enableAttributeArray colorInstanced" );


	mShaderPins.setAttributeBuffer( "pinDirectionInstanced", GL_FLOAT, offsetof(PinVertex, normal), 3, static_cast<int>(sizeof(PinVertex)));
	mShaderPins.enableAttributeArray( "pinDirectionInstanced");
	PRINT_OPENGL_ERROR( "Shader enableAttributeArray pinDirectionInstanced" );

	mGL.glVertexAttribDivisor(mShaderPins.attributeLocation("offsetInstanced"),1);
	mGL.glVertexAttribDivisor(mShaderPins.attributeLocation("colorInstanced"),1);
	//PRINT_OPENGL_ERROR( "Shader attribdivisor colorInstanced" );

	mGL.glVertexAttribDivisor(mShaderPins.attributeLocation("pinDirectionInstanced"),1);
	PRINT_OPENGL_ERROR( "Shader attribdivisor pinDirectionInstanced" );

	mVAO.release();

	mGL.glBindVertexArray(prevVAO);
	return true;
}

bool PinRenderer::resizePinPointsBuffer(unsigned int size)
{

	mAllocatedBufferSize = size;
	mInstancedPinBuffer.bind();
	mInstancedPinBuffer.allocate(size * sizeof (PinVertex));
	return true;
}
