//
// Created by timo.homburg on 29.04.2024.
//
#include "annotation.h"
#include "GigaMesh/mesh/mesh.h"
#include "meshQt.h"
#include "../../core/mesh/MeshIO/PlyWriter.h"


Annotation::Annotation(){
    isempty=true;
}

Annotation::Annotation(QJsonObject annojsonn,QString annoid,MeshQt* mesh,QString side) {
    annotationid=annoid.toStdString();
    isempty=false;
    themesh=mesh;
    annojson=annojsonn;
    this->bboxVertices=std::set<Vertex*>();
    if(annojsonn.contains("body")){
        if(annojsonn.find("body")->isObject()){
            annotationbody=QJsonArray();
            annotationbody.append(annojsonn.find("body")->toObject());
        }else if(annojsonn.find("body")->isArray()){
            annotationbody=annojsonn.find("body")->toArray();
        }
    }
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
        bboxToVertexIds(mesh,false,"");
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
        mesh->svgStringTo2DBBOX(annovalue.toStdString(),imgheight,imgwidth,side.toStdString(),res);
        minX=res[0];
        minY=res[1];
        maxX=res[2];
        maxY=res[3];
        bboxToVertexIds(mesh,true,side.toStdString());
    }

}

bool Annotation::determineZBBOXFromVertices(std::string side){
    double minZz=DBL_MAX;
    double maxZz=DBL_MIN;
    for (auto it = vertices.begin(); it != vertices.end();){
        if(side=="front" && (*it)->getZ()<0){
            it=vertices.erase(it);
            continue;
        }else if(side=="back" && (*it)->getZ()>0){
            it=vertices.erase(it);
            continue;
        }
        if((*it)->getZ()<minZz){
            minZz=(*it)->getZ();
        }
        if((*it)->getZ()>maxZz){
            maxZz=(*it)->getZ();
        }
        ++it;
    }
    if(side=="front" && minZz<0){
        minZz=0;
    }else if(side=="back" && maxZz>0){
        maxZz=0;
    }
    this->minZ=minZz;
    this->maxZ=maxZz;
    return true;
}

Mesh* Annotation::getAnnotationMesh(Mesh* themesh){
    auto* someFaces=new std::set<Face*>();
    themesh->getFaceContainsVert(this->vertices,*someFaces);
    return new Mesh(someFaces);
}

bool Annotation::bboxToVertexIds(MeshQt* meshToTest,bool twodimensional,const std::string side){
    if(twodimensional){
        this->vertices=meshToTest->getVerticesIn2DBBOX(minX,maxX,minY,maxY,this->bboxVertices);
        determineZBBOXFromVertices(side);
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

std::string Annotation::toHTML(){
    std::string result="<b>Annotation "+annotationid+"</b>";
    //result+="["+std::to_string(minX)+","+std::to_string(maxX)+","+std::to_string(minY)+","+std::to_string(maxY)+","+std::to_string(minZ)+","+std::to_string(maxZ)+"]<br/>";                                                       "<ul>";
    for(QJsonValue elem:annotationbody){
        result+="<li>"+elem.toObject().find("purpose").value().toString().toStdString()+": "+elem.toObject().find("value").value().toString().toStdString()+"</li>";
    }
    result+="</ul>";
    return result;
}


QJsonObject Annotation::getAnnotation(std::string exports,Mesh* themesh,QString outpath,bool borderOnly){
    QJsonObject result=QJsonObject();
    QString annoid=QString::fromStdString(annotationid);
    QJsonObject annotation;
    QJsonArray body;
    QJsonObject target;
    QJsonObject annoresult=result.find(annoid)->toObject();
    if(annojson.find("body")->isArray()) {
        body=annojson.find("body")->toArray();
    }else{
        QJsonArray thearray=QJsonArray();
        thearray.append(annojson.find("body")->toObject());
        body=thearray;
    }
    annotation.insert("body",body);
    std::cout << "Export in format "<< exports << "\n";
    QJsonObject selector;
    if(exports=="PLYFiles"){
        Mesh* resmesh=this->getAnnotationMesh(themesh);
        resmesh->writeFile(themesh->getBaseName().string()+"_anno_"+this->annotationid+".ply");
    }else if(exports=="MeshIDSelector"){
        std::cout << exports << "\n" ;
        selector["type"]="MeshIDSelector";
        QJsonArray valuearray=selector.find("value")->toArray();
        if(borderOnly){
            for(Vertex* vert:bboxVertices){
                valuearray.append(vert->getIndex());
            }
        }else{
            for(Vertex* vert:vertices){
                valuearray.append(vert->getIndex());
            }
        }
        selector["value"]=valuearray;
    }else if(exports=="MeshVertexSelector"){
        selector["type"]="MeshVertexSelector";
        QJsonArray valuearray=selector.find("value")->toArray();
        if(borderOnly){
            for(Vertex* vert:bboxVertices){
                QJsonArray xyzarray=QJsonArray();
                xyzarray.append(vert->getX());
                xyzarray.append(vert->getY());
                xyzarray.append(vert->getZ());
                valuearray.append(xyzarray);
            }
        }else{
            for(Vertex* vert:vertices){
                QJsonArray xyzarray=QJsonArray();
                xyzarray.append(vert->getX());
                xyzarray.append(vert->getY());
                xyzarray.append(vert->getZ());
                valuearray.append(xyzarray);
            }
        }

        selector["value"]=valuearray;
    }else if(exports=="WKTSelector"){
        selector.insert("type","WKTSelector");
        selector.insert("value",QString::fromStdString("POLYGON Z(("+std::to_string(minX)+" "+std::to_string(minY)+" "+std::to_string(minZ)+","+std::to_string(maxX)+" "+std::to_string(maxY)+" "+std::to_string(minZ)+","+std::to_string(minX)+" "+std::to_string(maxY)+" "+std::to_string(minZ)+","+std::to_string(minX)+" "+std::to_string(minY)+" "+std::to_string(maxZ)+","+std::to_string(maxX)+" "+std::to_string(minY)+" "+std::to_string(maxZ)+","+std::to_string(maxX)+" "+std::to_string(maxY)+" "+std::to_string(maxZ)+","+std::to_string(minX)+" "+std::to_string(minY)+" "+std::to_string(maxZ)+"))"));

    }else if(exports=="PLYSelector"){
        selector.insert("type","PLYSelector");
        //selector.insert("value",this->getAnnotationMesh(themesh).));
    }else if(exports=="SVGSelector"){
        selector.insert("type","SvgSelector");
        selector.insert("value",QString::fromStdString("<svg><polygon points=\""+std::to_string(minX)+","+std::to_string(minY)+" "+std::to_string(minX)+","+std::to_string(maxY)+" "+std::to_string(maxX)+","+std::to_string(maxY)+" "+std::to_string(maxX)+","+std::to_string(minY)+"\"></polygon></svg>"));
        target.insert("selector",selector);
    }
    target.insert("selector",selector);
    annotation.insert("target",target);
    result.insert(annoid,annotation);
    return result;
}


