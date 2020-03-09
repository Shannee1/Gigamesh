#ifndef MESHQTCSVIMPORTEXPORT_H
#define MESHQTCSVIMPORTEXPORT_H

#include <functional>

#include <QTextStream>

#include "meshGL/meshGL.h"

class MeshQtCSVImportExport
{

public:

	MeshQtCSVImportExport(MeshGL* meshGLPtr,
	                        const std::function<std::vector<Vertex*>*()>& verticesAccessorFunction,
	                        const std::function<std::vector<std::tuple<Vector3D, Primitive*, bool>>*()>&
	                                                                        selectedPositionsAccessorFunction,
	                        const std::function<void(const double, const std::string&)>& showProgressFunction,
	                        const std::function<void(const std::string&)>& showProgressStartFunction,
	                        const std::function<void(const std::string&)>& showProgressStopFunction):
	                                                                                            m_meshGLPtr{meshGLPtr},
	                                                                                            m_verticesAccessorFunction{verticesAccessorFunction},
	                                                                                            m_selectedPositionsAccessorFunction{
	                                                                                                        selectedPositionsAccessorFunction},
	                                                                                            m_showProgressFunction{showProgressFunction},
	                                                                                            m_showProgressStartFunction{showProgressStartFunction},
	                                                                                            m_showProgressStopFunction{showProgressStopFunction}
	{
	}

	virtual bool   importVertexDataFromCSV( const QString& outputFilenameString,
	                                            const std::function<void()>& boundingBoxChangedCallback );
	virtual bool   importVertexDataFromCSVDialog( const std::function<void()>& boundingBoxChangedCallback );
	bool           exportVertexCoordinatesToCSV( const QString& outputFileNameString,
	                                             const bool     selectedVerticesOnly);
	bool           exportVertexCoordinatesToCSVDialog( const bool selectedVerticesOnly );
	bool           exportSelPrimsPositionsToCSV( const QString& outputFilenameString );
	bool           exportSelPrimsPositionsToCSVDialog();

	void           exportPositionsSelPrimsToCSV( QTextStream& outputTextStream );
	void           exportSelMVertsCoordinatesToCSV( QTextStream& outputTextStream );
	void           exportMVerticesCoordinatesToCSV( QTextStream& outputTextStream );

private:

	MeshGL* m_meshGLPtr = nullptr;

	const std::function<std::vector<Vertex*>*()> m_verticesAccessorFunction;

	const std::function<std::vector<std::tuple<Vector3D, Primitive*, bool>>*()>
	                                                m_selectedPositionsAccessorFunction;

	const std::function<void(const double, const std::string&)> m_showProgressFunction;
	const std::function<void(const std::string&)> m_showProgressStartFunction;
	const std::function<void(const std::string&)> m_showProgressStopFunction;

};

#endif // MESHQTCSVIMPORTEXPORT_H
