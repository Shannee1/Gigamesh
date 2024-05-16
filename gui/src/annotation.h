//
// Created by timo.homburg on 29.04.2024.
//

#ifndef GIGAMESH_ANNOTATION_H
#define GIGAMESH_ANNOTATION_H

#include <QJsonObject>
#include <QJsonDocument>
#include "GigaMesh/mesh/mesh.h"
#include "meshQt.h"

class Annotation {
    public:
        enum exportFormats{WKTSelector,WKTPolygonSelector, SVGSelector, PLYSelector,PLYFiles, MeshIDSelector,MeshLabelSelector,MeshVertexSelector};
        enum annotationType{Character, Word, Line, Surface};
        Annotation();
        Annotation(QJsonObject annojson,QString annoid, MeshQt* mesh,QString side="noside");
        double minX;
        double maxX;
        double minY;
        double maxY;
        double minZ;
        double maxZ;
        QJsonObject annojson;
        std::set<Vertex*> vertices;
        std::set<Vertex*> bboxVertices;
        Mesh* getAnnotationMesh(Mesh* themesh);
        std::string annotationid;
        MeshQt* themesh;
        bool isempty;

        bool determineZBBOXFromVertices(std::string side);

        bool pointInAnnotationBBOX3D(double x, double y, double z);

        bool pointInAnnotationBBOX2D(double x, double y);

        bool bboxToVertexIds(MeshQt* meshToTest, bool twodimensional,std::string side);

        QJsonArray getAnnotationBody();

        void setAnnotationBody(QJsonArray newbody);

        void getRelativePositions(std::list<Annotation*> otherannotations);

        void setLabelIDs(double labelid);

        QJsonObject getAnnotation(std::string exportFormats,Mesh* themesh,QString outpath,bool borderOnly);

        std::string toHTML();

        std::string toString();

        QJsonArray annotationbody;

        std::set<std::string> fieldnames;

        Annotation* leftOf;
        Annotation* rightOf;
        Annotation* above;
        Annotation* below;

    private:



        std::string annotationtarget;

};


#endif //GIGAMESH_ANNOTATION_H
