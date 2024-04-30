//
// Created by timo.homburg on 29.04.2024.
//
#include "annotation.h"
#include "GigaMesh/mesh/mesh.h"
#include "meshQt.h"

Annotation::Annotation(QJsonObject annojsonn,QString annoid,MeshQt* mesh) {
    if(annojson.contains("body")){
        if(annojson.find("body")->isObject()){
            annotationbody=QJsonArray();
            annotationbody.append(annojson.find("body")->toObject());
        }else if(annojson.find("body")->isArray()){
            annotationbody=annojson.find("body")->toArray();
        }
    }
    annotationid=annoid.toStdString();
    themesh=mesh;
    annojson=annojsonn;
    QJsonObject selectorObject=annojson.find("target")->toObject().find("selector")->toObject();
    QString annotype=selectorObject.find("type")->toString();
    QString annovalue = selectorObject.find("value")->toString();
    bool hasbbox=false;
    if(annotype=="WKTSelector" || annotype=="WktSelector"){
        annovalue=annovalue.replace("POLYGON Z ((","").replace("POLYGON Z((","").replace("))","").trimmed();
        double res[6];
        mesh->wktStringToBBOX(annovalue.toStdString(),res);
        minX=res[0];
        minY=res[1];
        minZ=res[2];
        maxX=res[3];
        maxY=res[4];
        maxZ=res[5];
        bboxToVertexIds(mesh,false);
    }else if(annotype=="SvgSelector" || annotype=="SVGSelector"){
        annovalue=annovalue.replace("<svg><polygon","").replace("points=\"","").replace("\"></polygon></svg>","").trimmed();
        double imgheight=0.0;
        double imgwidth=0.0;
        if(selectorObject.contains("dimensions")){
            imgwidth=selectorObject.find("dimensions")->toObject().find("x")->toDouble();
            imgheight=selectorObject.find("dimensions")->toObject().find("y")->toDouble();
        }
        //qDebug()<<annovalue;
        double res[4];
        mesh->svgStringTo2DBBOX(annovalue.toStdString(),imgheight,imgwidth,res);
        minX=res[0];
        minY=res[1];
        maxX=res[2];
        maxY=res[3];
        bboxToVertexIds(mesh,true);
    }

}

bool Annotation::determineZBBOXFromVertices(){
    double minZz=DBL_MAX;
    double maxZz=DBL_MIN;
    for(Vertex* vert:vertices){
        if(vert->getZ()<minZz){
            minZz=vert->getZ();
        }
        if(vert->getZ()>maxZz){
            maxZz=vert->getZ();
        }
    }
    this->minZ=minZz;
    this->maxZ=maxZz;
    return true;
}

bool Annotation::bboxToVertexIds(MeshQt* meshToTest,bool twodimensional){
    if(twodimensional){
        this->vertices=meshToTest->getVerticesIn2DBBOX(minX,maxX,minY,maxY);
        determineZBBOXFromVertices();
    }else{
        this->vertices=meshToTest->getVerticesInBBOX(minX,maxX,minY,maxY,minZ,maxZ);
    }
    return true;
}

bool Annotation::pointInAnnotationBBOX3D(double x, double y, double z){
    if(x>minX && x<maxX && y>minY && y<maxY && z>minZ && z<maxZ){
        return true;
    }
    return false;
}
bool Annotation::pointInAnnotationBBOX2D(double x, double y){
    if(x>minX && x<maxX && y>minY && y<maxY){
        return true;
    }
    return false;
}

std::string Annotation::toString(){
    return QJsonDocument(annotationbody).toJson(QJsonDocument::Compact).toStdString();
}


QJsonObject Annotation::getAnnotation(std::string exports){
    //QJsonObject result=QJsonObject();
    //QString annoid=QString::fromStdString(annotationid);
    //find(annoid)->toObject()
    QJsonObject target=annojson.find("target")->toObject();
    if(exports=="MeshIDSelector"){
        target.insert("selector",QJsonObject());
        QJsonObject selector=target.find("selector")->toObject();
        selector.remove("type");
        selector.insert("type","MeshIDSelector");
        selector.remove("value");
        QJsonArray valuearray=selector.find("value")->toArray();
        for(Vertex* vert:vertices){
            valuearray.append(vert->getIndex());
        }
        selector.insert("value",valuearray);
    }else if(exports=="MeshVertexSelector"){
        target.insert("selector",QJsonObject());
        QJsonObject selector=target.find("selector")->toObject();
        selector.remove("type");
        selector.insert("type","MeshVertexSelector");
        selector.remove("value");
        QJsonArray valuearray=selector.find("value")->toArray();
        for(Vertex* vert:vertices){
            QJsonArray xyzarray=QJsonArray();
            xyzarray.append(vert->getX());
            xyzarray.append(vert->getY());
            xyzarray.append(vert->getZ());
            valuearray.append(xyzarray);
        }
        selector.insert("value",valuearray);
    }else if(exports=="WKTSelector"){
        target.insert("selector",QJsonObject());
        QJsonObject selector=target.find("selector")->toObject();
        selector.insert("type","SvgSelector");
        selector.insert("value",QString::fromStdString("POLYGON Z(("+std::to_string(minX)+" "+std::to_string(minY)+" "+std::to_string(minZ)+","+std::to_string(maxX)+" "+std::to_string(maxY)+" "+std::to_string(minZ)+","+std::to_string(minX)+" "+std::to_string(maxY)+" "+std::to_string(minZ)+","+std::to_string(minX)+" "+std::to_string(minY)+" "+std::to_string(maxZ)+","+std::to_string(maxX)+" "+std::to_string(minY)+" "+std::to_string(maxZ)+","+std::to_string(maxX)+" "+std::to_string(maxY)+" "+std::to_string(maxZ)+","+std::to_string(minX)+" "+std::to_string(minY)+" "+std::to_string(maxZ)+"))"));

    }else if(exports=="SVGSelector"){
        target.insert("selector",QJsonObject());
        QJsonObject selector=target.find("selector")->toObject();
        selector.insert("type","SvgSelector");
        selector.insert("value",QString::fromStdString("<svg><polygon points=\""+std::to_string(minX)+","+std::to_string(minY)+" "+std::to_string(minX)+","+std::to_string(maxY)+" "+std::to_string(maxX)+","+std::to_string(maxY)+" "+std::to_string(maxX)+","+std::to_string(minY)+"\"></polygon></svg>"));
    }
    return annojson;
}


