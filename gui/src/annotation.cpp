//
// Created by timo.homburg on 29.04.2024.
//
#include "annotation.h"

#include <utility>
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
    for(QJsonValue elem:annotationbody){
        fieldnames.insert(elem.toObject().find("purpose").value().toString().toStdString());
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
    auto minZz=DBL_MAX;
    auto maxZz=DBL_MIN;
    for (auto it = vertices.begin(); it != vertices.end();){
        if(side=="front" && (*it)->getZ()<0){
            it=vertices.erase(it);
            continue;
        }
        if(side=="back" && (*it)->getZ()>0){
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
    for(auto it= bboxVertices.begin();it!=bboxVertices.end();){
        if(side=="front" && (*it)->getZ()<0){
            it=bboxVertices.erase(it);
            continue;
        }
        if(side=="back" && (*it)->getZ()>0){
            it=bboxVertices.erase(it);
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

void Annotation::setAnnotationBody(QJsonArray newbody){
    this->annotationbody=std::move(newbody);
}

void Annotation::getRelativePositions(std::list<Annotation*> otherannotations){
    std::cout << "GET RELATIVE POSITIONS!!!!! " << endl;
    auto leftOfMaxX=DBL_MAX, rightOfMinX=DBL_MAX,aboveMinY=DBL_MAX, belowMaxY=DBL_MIN;
    below=new Annotation();
    above=new Annotation();
    leftOf=new Annotation();
    rightOf=new Annotation();
    for(Annotation* oanno:otherannotations){
        if(this->maxX<oanno->minX && !(oanno->maxY < this->minY || oanno->minY > this->maxY)){
            if(oanno->minX<leftOfMaxX){
                std::cout << "FOUND NEW LEFT OF!!!!!! " << std::to_string(leftOfMaxX) << " "<< this->minX << " " << this->maxX << "\n";
                if(leftOf->isempty || (!leftOf->isempty && oanno->minX<leftOf->minX)){
                    leftOfMaxX=oanno->minX;
                    leftOf=oanno;
                    std::cout << "FOUND NEW LEFT OF!!!!!!22222 " << std::to_string(leftOfMaxX) << " "<< this->minX << " " << this->maxX << "\n";
                }
            }
        }
        if(this->minX<oanno->maxX && !(oanno->maxX < this->minX || oanno->minX > this->maxX)){
            if(oanno->maxX>rightOfMinX){
                std::cout << "FOUND NEW RIGHT OF!!!!!!\n";
                rightOfMinX=oanno->maxX;
                rightOf=oanno;
            }
        }
        if(this->maxY<oanno->minY && !(oanno->maxX < this->minX || oanno->minX > this->maxX)){
            if(oanno->minY<aboveMinY){
                std::cout << "FOUND NEW ABOVE OF!!!!!!\n";
                aboveMinY=oanno->minY;
                above=oanno;
            }
        }
        if(this->maxY<oanno->minY && !(oanno->maxX < this->minX || oanno->minX > this->maxX)){
            if(oanno->minY>belowMaxY){
                std::cout << "FOUND NEW BELOW OF!!!!!!\n";
                belowMaxY=oanno->minY;
                below=oanno;
            }
        }
    }
}

void Annotation::setLabelIDs(uint64_t labelid){
    for(auto* vert:this->vertices){
        vert->setLabel(labelid);
    }
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

bool Annotation::convexHull(){
    // Find the bottommost point
    /*int ymin = points[0].y, min = 0;
    for (auto* vert:this->vertices)
    {
        int y = vert->getY();

        // Pick the bottom-most or choose the left
        // most point in case of tie
        if ((y < ymin) || (ymin == y &&
                           points[i].x < points[min].x))
            ymin = points[i].y, min = i;
    }

    // Place the bottom-most point at first position
    swap(points[0], points[min]);

    // Sort n-1 points with respect to the first point.
    // A point p1 comes before p2 in sorted output if p2
    // has larger polar angle (in counterclockwise
    // direction) than p1
    p0 = points[0];
    qsort(&points[1], n-1, sizeof(Point), compare);

    // If two or more points make same angle with p0,
    // Remove all but the one that is farthest from p0
    // Remember that, in above sorting, our criteria was
    // to keep the farthest point at the end when more than
    // one points have same angle.
    int m = 1; // Initialize size of modified array
    for (int i=1; i<n; i++)
    {
        // Keep removing i while angle of i and i+1 is same
        // with respect to p0
        while (i < n-1 && orientation(p0, points[i],
                                      points[i+1]) == 0)
            i++;


        points[m] = points[i];
        m++;  // Update size of modified array
    }

    // If modified array of points has less than 3 points,
    // convex hull is not possible
    if (m < 3) return;

    // Create an empty stack and push first three points
    // to it.
    stack<Point> S;
    S.push(points[0]);
    S.push(points[1]);
    S.push(points[2]);

    // Process remaining n-3 points
    for (int i = 3; i < m; i++)
    {
        // Keep removing top while the angle formed by
        // points next-to-top, top, and points[i] makes
        // a non-left turn
        while (S.size()>1 && orientation(nextToTop(S), S.top(), points[i]) != 2)
            S.pop();
        S.push(points[i]);
    }

    // Now stack has the output points, print contents of stack
    while (!S.empty())
    {
        Point p = S.top();
        cout << "(" << p.x << ", " << p.y <<")" << endl;
        S.pop();
    }*/
}

bool Annotation::getBBOXVertices(double borderThickness){
    for(auto* vert:this->vertices){
        if(vert->getX() >= minX && vert->getX() <= (minX+borderThickness) && vert->getY() > minY && vert->getY() < maxY){
            this->bboxVertices.insert(vert);
        }
        else if(vert->getX() <= maxX && vert->getX()>=(maxX-borderThickness) && vert->getY() > minY && vert->getY() < maxY){
            this->bboxVertices.insert(vert);
        }
        else if(vert->getY() >= minY && vert->getY()<=(minY+borderThickness) && vert->getX() > minX && vert->getX() < maxX){
            this->bboxVertices.insert(vert);
        }
        else if(vert->getY() <= maxY && vert->getY()>=(maxY-borderThickness) && vert->getX() > minX && vert->getX() < maxX){
            this->bboxVertices.insert(vert);
        }
    }
    return true;
}

bool Annotation::getBBOXFromVertices(){
    double lminX=DBL_MAX,lmaxX=DBL_MIN,lminY=DBL_MAX,lmaxY=DBL_MIN,lminZ=DBL_MAX,lmaxZ=DBL_MIN;
    for(auto* vert:this->vertices){
        if(vert->getX()<minX){
            minX=vert->getX();
        }
        if(vert->getX()>maxX){
            maxX=vert->getX();
        }
        if(vert->getY()<minY){
            minY=vert->getY();
        }
        if(vert->getY()>maxY) {
            maxY = vert->getY();
        }
        if(vert->getZ()<minZ){
            minZ=vert->getZ();
        }
        if(vert->getZ()>maxZ){
            maxZ=vert->getZ();
        }
    }
    this->minX=lminX;
    this->maxX=lmaxX;
    this->minY=lminY;
    this->maxY=lmaxY;
    this->minZ=lminZ;
    this->maxZ=lmaxZ;
    return true;
}

std::string Annotation::toString(){
    return QJsonDocument(annotationbody).toJson(QJsonDocument::Compact).toStdString();
}

QJsonArray Annotation::getAnnotationBody(){
    return this->annotationbody;
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


