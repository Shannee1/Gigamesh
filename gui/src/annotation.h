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
        enum exportFormats{WKTSelector,WKTPolygonSelector, SVGSelector, MeshIDSelector,MeshLabelSelector,MeshVertexSelector};
        enum annotationType{Character, Word, Line, Surface};
        Annotation(QJsonObject annojson,QString annoid, MeshQt* mesh,QString side="noside");
        double minX;
        double maxX;
        double minY;
        double maxY;
        double minZ;
        double maxZ;
        QJsonObject annojson;
        std::set<Vertex*> vertices;
        std::string annotationid;
        MeshQt* themesh;

        bool determineZBBOXFromVertices();

        bool pointInAnnotationBBOX3D(double x, double y, double z);

        bool pointInAnnotationBBOX2D(double x, double y);

        bool bboxToVertexIds(MeshQt* meshToTest, bool twodimensional);

        QJsonObject getAnnotationBody();

        void setAnnotationBody(QJsonObject newbody);

        QJsonObject getAnnotation(std::string exportFormats);

        std::string toString();

        QJsonArray  annotationbody;

    private:

        std::string annotationtarget;





};


#endif //GIGAMESH_ANNOTATION_H
