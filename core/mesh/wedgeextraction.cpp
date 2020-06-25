
/*
Written by Martin Seiler
Part of the Master Thesis

Status: Experimental

Please look at mesh.cpp for larger and more complete disclaimer by original creators

*/

#include<iostream>
#include<vector>
#include<list>
#include<iterator>
#include<queue>

#include<algorithm>
#include<random>
#include<chrono>
#include<math.h> //for sqrt, can hopefully be omitted

#include<limits> //used for infinity placeholders in RANSAC

#include <GigaMesh/mesh/wedgeextraction.h>

#include <GigaMesh/mesh/vertex.h>
#include <GigaMesh/mesh/mesh.h>


using namespace std;

//helper methods exclusively called in the wedge extraction project
//they all begin with wE


//! Distance calculation for vertices omitting square root for speed up
double wEComputeSquaredDistanceBetweenTwoVertices(Vertex* &vertexNo1, Vertex* &vertexNo2){
/*
	return(	(vertexNo1->getX() - vertexNo2->getX()) * (vertexNo1->getX() - vertexNo2->getX())	+
			(vertexNo1->getY() - vertexNo2->getY()) * (vertexNo1->getY() - vertexNo2->getY())	+
			(vertexNo1->getZ() - vertexNo2->getZ()) * (vertexNo1->getZ() - vertexNo2->getZ())	);
*/

	return(	pow((vertexNo1->getX() - vertexNo2->getX()), 2.0)+
			pow((vertexNo1->getY() - vertexNo2->getY()), 2.0)+
			pow((vertexNo1->getZ() - vertexNo2->getZ()), 2.0)	);

}


//! Distance calculation for normals interpreted as vertices omitting square root for speed up
double wEComputeSquaredDistanceBetweenTwoNormalsTreatedAsVertices(Vertex* &vertexNo1, Vertex* &vertexNo2){
/*
	return(	(vertexNo1->getX() - vertexNo2->getX()) * (vertexNo1->getX() - vertexNo2->getX())	+
			(vertexNo1->getY() - vertexNo2->getY()) * (vertexNo1->getY() - vertexNo2->getY())	+
			(vertexNo1->getZ() - vertexNo2->getZ()) * (vertexNo1->getZ() - vertexNo2->getZ())	);
*/

	return(	pow((vertexNo1->getNormalX() - vertexNo2->getNormalX()), 2.0)+
			pow((vertexNo1->getNormalY() - vertexNo2->getNormalY()), 2.0)+
			pow((vertexNo1->getNormalZ() - vertexNo2->getNormalZ()), 2.0)	);

}


//! Distance calculation for normals interpreted as vertices omitting square root for speed up
//! Input is a vertex and a normal given as a vector of doubles
double wEComputeSquaredDistanceBetweenTwoNormalsTreatedAsVertices(Vertex* &vertexNo1, vector<double> &normalComponents){

	return(	pow((vertexNo1->getNormalX() - normalComponents[0]), 2.0)+
			pow((vertexNo1->getNormalY() - normalComponents[1]), 2.0)+
			pow((vertexNo1->getNormalZ() - normalComponents[2]), 2.0)	);

}


//! Random Selection of a specified number of Vertices from a vector
//! Shuffles the input vector for the random selection, there may be quicker ways
void wERandomlyChooseVerticesFromVector(vector<Vertex*> &inputVector, vector<Vertex*> &outputVector, int howManyVerticesWanted){

	unsigned timeBasedSeed = chrono::system_clock::now().time_since_epoch().count();

	shuffle(inputVector.begin(), inputVector.end(), default_random_engine(timeBasedSeed));

	//will be rewritten to be less absolute soon...
	if(howManyVerticesWanted == 3){
		outputVector.push_back(inputVector[0]);
		outputVector.push_back(inputVector[1]);
		outputVector.push_back(inputVector[2]);
	}

	if(howManyVerticesWanted == 4){
		outputVector.push_back(inputVector[0]);
		outputVector.push_back(inputVector[1]);
		outputVector.push_back(inputVector[2]);
		outputVector.push_back(inputVector[3]);
	}

}

/*
//! currently unused, no necessity because mean is used instead of median
void wEFindMedianInListForGivenMean(list<Vertex*> &inputList, double &meanNormalX, double &meanNormalY, double &meanNormalZ, double &medianNormalX, double &medianNormalY, double &medianNormalZ){

	//check back with supervisor

}
*/

//! Notes down to which mean a Normal is nearest in a euclidian sence. This means, normals are interpreted as points
void wEAssignNormalsToNearestMean(vector<int> &currentClustering, vector<Vertex*> &verticesWithCurrentLabel, vector<vector<double>> &normalMeanComponents){

	for(int i=0; i<verticesWithCurrentLabel.size(); i++){

		double distanceToNormal1 = wEComputeSquaredDistanceBetweenTwoNormalsTreatedAsVertices(verticesWithCurrentLabel[i], normalMeanComponents[0]);
		double distanceToNormal2 = wEComputeSquaredDistanceBetweenTwoNormalsTreatedAsVertices(verticesWithCurrentLabel[i], normalMeanComponents[1]);
		double distanceToNormal3 = wEComputeSquaredDistanceBetweenTwoNormalsTreatedAsVertices(verticesWithCurrentLabel[i], normalMeanComponents[2]);

		if((distanceToNormal1 < distanceToNormal2) && (distanceToNormal1 < distanceToNormal3)) {
			//normal 1 is nearest
			currentClustering[i] = 1;
		} else if ((distanceToNormal2 < distanceToNormal1) && (distanceToNormal2 < distanceToNormal3)){
			//normal 2 is nearest
			currentClustering[i] = 2;
		} else if ((distanceToNormal3 < distanceToNormal1) && (distanceToNormal3 < distanceToNormal2)){
			//normal 3 is nearest
			currentClustering[i] = 3;
		}

	}

}

//! Notes down to which mean a Normal is nearest in an euclidian sence. This means, normals are interpreted as points
void wEAssignNormalsToNearestMean(vector<int> &currentClustering, vector<Vertex*> &verticesWithCurrentLabel, vector<Vertex*> &threeRandomlyChosenVertices){

	for(int i=0; i<verticesWithCurrentLabel.size(); i++){

		double distanceToNormal1 = wEComputeSquaredDistanceBetweenTwoNormalsTreatedAsVertices(verticesWithCurrentLabel[i], threeRandomlyChosenVertices[0]);
		double distanceToNormal2 = wEComputeSquaredDistanceBetweenTwoNormalsTreatedAsVertices(verticesWithCurrentLabel[i], threeRandomlyChosenVertices[1]);
		double distanceToNormal3 = wEComputeSquaredDistanceBetweenTwoNormalsTreatedAsVertices(verticesWithCurrentLabel[i], threeRandomlyChosenVertices[2]);

		if((distanceToNormal1 < distanceToNormal2) && (distanceToNormal1 < distanceToNormal3)) {
			//normal 1 is nearest
			currentClustering[i] = 1;
		} else if ((distanceToNormal2 < distanceToNormal1) && (distanceToNormal2 < distanceToNormal3)){
			//normal 2 is nearest
			currentClustering[i] = 2;
		} else if ((distanceToNormal3 < distanceToNormal1) && (distanceToNormal3 < distanceToNormal2)){
			//normal 3 is nearest
			currentClustering[i] = 3;
		}

	}

}


//! Computes three means of normals according to a given clustering
void wEComputeMeans(vector<int> &currentClustering, vector<Vertex*> &verticesWithCurrentLabel, vector<vector<double>> &normalMeanComponents){

	double firstNormalMeanComponentX = 0.0;
	double firstNormalMeanComponentY = 0.0;
	double firstNormalMeanComponentZ = 0.0;

	int workedOnFirstCounter = 0;

	double secondNormalMeanComponentX = 0.0;
	double secondNormalMeanComponentY = 0.0;
	double secondNormalMeanComponentZ = 0.0;

	int workedOnSecondCounter = 0;

	double thirdNormalMeanComponentX = 0.0;
	double thirdNormalMeanComponentY = 0.0;
	double thirdNormalMeanComponentZ = 0.0;

	int workedOnThirdCounter = 0;

	for(int i=0; i<currentClustering.size(); i++){

		if(currentClustering[i] == 1){

			firstNormalMeanComponentX += verticesWithCurrentLabel[i]->getNormalX();
			firstNormalMeanComponentY += verticesWithCurrentLabel[i]->getNormalY();
			firstNormalMeanComponentZ += verticesWithCurrentLabel[i]->getNormalZ();

			workedOnFirstCounter++;

		} else if (currentClustering[i] == 2){

			secondNormalMeanComponentX += verticesWithCurrentLabel[i]->getNormalX();
			secondNormalMeanComponentY += verticesWithCurrentLabel[i]->getNormalY();
			secondNormalMeanComponentZ += verticesWithCurrentLabel[i]->getNormalX();

			workedOnSecondCounter++;

		} else if (currentClustering[i] == 3) {

			thirdNormalMeanComponentX += verticesWithCurrentLabel[i]->getNormalX();
			thirdNormalMeanComponentY += verticesWithCurrentLabel[i]->getNormalY();
			thirdNormalMeanComponentZ += verticesWithCurrentLabel[i]->getNormalX();

			workedOnThirdCounter++;

		}
	}

	//average

	firstNormalMeanComponentX = firstNormalMeanComponentX/workedOnFirstCounter;
	firstNormalMeanComponentY = firstNormalMeanComponentY/workedOnFirstCounter;
	firstNormalMeanComponentZ = firstNormalMeanComponentZ/workedOnFirstCounter;

	secondNormalMeanComponentX = secondNormalMeanComponentX/workedOnSecondCounter;
	secondNormalMeanComponentY = secondNormalMeanComponentY/workedOnSecondCounter;
	secondNormalMeanComponentZ = secondNormalMeanComponentZ/workedOnSecondCounter;

	thirdNormalMeanComponentX = thirdNormalMeanComponentX/workedOnThirdCounter;
	thirdNormalMeanComponentY = thirdNormalMeanComponentY/workedOnThirdCounter;
	thirdNormalMeanComponentZ = thirdNormalMeanComponentZ/workedOnThirdCounter;

	//write in vector

	normalMeanComponents[0][0] = firstNormalMeanComponentX;
	normalMeanComponents[0][1] = firstNormalMeanComponentY;
	normalMeanComponents[0][2] = firstNormalMeanComponentZ;

	normalMeanComponents[1][0] = secondNormalMeanComponentX;
	normalMeanComponents[1][1] = secondNormalMeanComponentY;
	normalMeanComponents[1][2] = secondNormalMeanComponentZ;

	normalMeanComponents[2][0] = thirdNormalMeanComponentX;
	normalMeanComponents[2][1] = thirdNormalMeanComponentY;
	normalMeanComponents[2][2] = thirdNormalMeanComponentZ;

}


//! computes the shortest distance between a point (arbPoint) and a line defined by two points lying on the line
//! TODO could be rewritten using a class or other helper methods
//! currently uses the square root and I don't like it
double wEComputeShortestDistanceBetweenPointAndLine(Vertex* &arbPoint, Vertex* &point1OnLine, Vertex* &point2OnLine){

	if((arbPoint!=point1OnLine)&&(arbPoint!=point2OnLine)){

		double lineVectorComponentX = point2OnLine->getX() - point1OnLine->getX();
		double lineVectorComponentY = point2OnLine->getY() - point1OnLine->getY();
		double lineVectorComponentZ = point2OnLine->getZ() - point1OnLine->getZ();

		double helperVectorComponentX = arbPoint->getX() - point1OnLine->getX();
		double helperVectorComponentY = arbPoint->getY() - point1OnLine->getY();
		double helperVectorComponentZ = arbPoint->getZ() - point1OnLine->getZ();

		//the magnitude of the cross product of the vectors gives the area of the created parallelogram
		//its height can be calculated by deviding the area by the distance of the points on the line

		//magnitude of the cross product of lineVector and helperVector
		double crossProductComponentX = (lineVectorComponentY * helperVectorComponentZ) - (lineVectorComponentZ * helperVectorComponentY);
		double crossProductComponentY = (lineVectorComponentZ * helperVectorComponentX) - (lineVectorComponentX * helperVectorComponentZ);
		double crossProductComponentZ = (lineVectorComponentX * helperVectorComponentY) - (lineVectorComponentY * helperVectorComponentX);

		//maybe sqrt can be omitted
		double parallelogramArea = sqrt(pow(crossProductComponentX, 2.0) + pow(crossProductComponentY, 2.0) + pow(crossProductComponentZ, 2.0));
		//maybe sqrt can be omitted
		double lineSegmentLength = sqrt(pow(lineVectorComponentX, 2.0) + pow(lineVectorComponentY, 2.0) + pow(lineVectorComponentZ, 2.0));

		return parallelogramArea / lineSegmentLength;
	} else {
		//in case the arbitrary point is one of the vertices given to construct the line
		return 0.0;
	}

}

/*
//! UNUSED Determines on which border of 2 clusters a border vertex lies
void wEGetBorderGroupFromVertexByFeatureVector(Vertex* vertexInQuestion,int &foundBordergroup){

	double borderTo1;
	vertexInQuestion->getFeatureElement(21, &borderTo1);
	double borderTo2;
	vertexInQuestion->getFeatureElement(22, &borderTo2);
	double borderTo3;
	vertexInQuestion->getFeatureElement(23, &borderTo3);

	if((borderTo1 > 0) && (borderTo2 > 0)){
		foundBordergroup = 12;
	} else if ((borderTo2 > 0) && (borderTo3 > 0)){
		foundBordergroup = 23;
	} else if ((borderTo3 > 0) && (borderTo1 > 0)){
		foundBordergroup = 31;
	}

}
*/

//! Determines on which border of 2 clusters a border vertex lies
void wEGetBorderGroupFromVertexByFeatureVector(Vertex* &finalLineVertex1, Vertex* &finalLineVertex2, Vertex* &finalLineVertex3,
				Vertex* &finalTetraederVertexGroup12, Vertex* &finalTetraederVertexGroup23, Vertex* &finalTetraederVertexGroup31){

	double borderGroup1;
	finalLineVertex1->getFeatureElement(21, &borderGroup1);

	double borderGroup2;
	finalLineVertex2->getFeatureElement(21, &borderGroup2);

	double borderGroup3;
	finalLineVertex3->getFeatureElement(21, &borderGroup3);


	if((int)borderGroup1 == 12){
		finalTetraederVertexGroup12 = finalLineVertex1;
	} else if ((int)borderGroup1 == 23){
		finalTetraederVertexGroup23 = finalLineVertex1;
	} else if ((int)borderGroup1 == 31){
		finalTetraederVertexGroup31 = finalLineVertex1;
	}

	if((int)borderGroup2 == 12){
		finalTetraederVertexGroup12 = finalLineVertex2;
	} else if ((int)borderGroup2 == 23){
		finalTetraederVertexGroup23 = finalLineVertex2;
	} else if ((int)borderGroup1 == 31){
		finalTetraederVertexGroup31 = finalLineVertex2;
	}

	if((int)borderGroup3 == 12){
		finalTetraederVertexGroup12 = finalLineVertex3;
	} else if ((int)borderGroup3 == 23){
		finalTetraederVertexGroup23 = finalLineVertex3;
	} else if ((int)borderGroup1 == 31){
		finalTetraederVertexGroup31 = finalLineVertex3;
	}

}


//! Calculates the squared distance between a projection point on a line (the result of projecting a point on a line) and a certain point on the line
//! To determine the point on the line this formula is used: A and B on Line, P arbitrary point, Point on line given by: A + dot(AP,AB) / dot(AB,AB) * AB
void wEComputeSquaredDistanceFromTetraederTopToProjectedPointOnLine(Vertex* &arbPoint, Vertex* &point1OnLineTetraederTop, Vertex* &point2OnLine, double &computedSquaredDistance){

	if((arbPoint != point1OnLineTetraederTop) && (arbPoint != point2OnLine)){

		double vector1ComponentX = arbPoint->getX() - point1OnLineTetraederTop->getX();
		double vector1ComponentY = arbPoint->getY() - point1OnLineTetraederTop->getY();
		double vector1ComponentZ = arbPoint->getZ() - point1OnLineTetraederTop->getZ();

		double vector2ComponentX = point2OnLine->getX() - point1OnLineTetraederTop->getX();
		double vector2ComponentY = point2OnLine->getY() - point1OnLineTetraederTop->getY();
		double vector2ComponentZ = point2OnLine->getZ() - point1OnLineTetraederTop->getZ();



		double dotProduct1 = vector1ComponentX * vector2ComponentX + vector1ComponentY * vector2ComponentY + vector1ComponentZ * vector2ComponentZ;
		double dotProduct2 = vector1ComponentX * vector1ComponentX + vector1ComponentY * vector1ComponentY + vector1ComponentZ * vector1ComponentZ;

		/*

		double projectedPointOnLineComponentX = point1OnLineTetraederTop->getX() + dotProduct1/dotProduct2 * vector2ComponentX;
		double projectedPointOnLineComponentY = point1OnLineTetraederTop->getY() + dotProduct1/dotProduct2 * vector2ComponentY;
		double projectedPointOnLineComponentZ = point1OnLineTetraederTop->getZ() + dotProduct1/dotProduct2 * vector2ComponentZ;

		double distanceComponentX = projectedPointOnLineComponentX - point1OnLineTetraederTop->getX();
		double distanceComponentY = projectedPointOnLineComponentY - point1OnLineTetraederTop->getY();
		double distanceComponentZ = projectedPointOnLineComponentZ - point1OnLineTetraederTop->getZ();

		*/

		//can be shortened down (because of point1OnLineTetraederTop->getX() -point1OnLineTetraederTop->getX() = 0 ) to:

		double distanceComponentX = dotProduct1/dotProduct2 * vector2ComponentX;
		double distanceComponentY = dotProduct1/dotProduct2 * vector2ComponentY;
		double distanceComponentZ = dotProduct1/dotProduct2 * vector2ComponentZ;

		computedSquaredDistance = pow(distanceComponentX, 2.0) + pow(distanceComponentY, 2.0) + pow(distanceComponentZ, 2.0);

	} else if (arbPoint == point1OnLineTetraederTop){
		computedSquaredDistance = 0.0;
	} else {

		computedSquaredDistance = 	pow(point1OnLineTetraederTop->getX()-point2OnLine->getX(), 2.0) +
									pow(point1OnLineTetraederTop->getY()-point2OnLine->getY(), 2.0) +
									pow(point1OnLineTetraederTop->getZ()-point2OnLine->getZ(), 2.0);

	}

}

/*
//! export
void writeWedgeToPlyOrObj(){
}
*/

//copied from mesh.cpp, will be legacy soon
//TODO: could be slimlined and moved to vertex class
bool getSurroundingVerticesInOrder (list<Vertex*> &adjacentVertsInOrder, Vertex* &pi, bool printDebug) {

	if(printDebug) {
		cout << endl << "[Mesh::getSurroundingVerticesInOrder] pi: " << pi->getIndex() << endl;
	}

	set<Face*> facesVisited;
	set<Face*> facesAdjacent;
	pi->getFaces(&facesAdjacent);
	Face* nextFace = nullptr;
	Face* currentFace = (*facesAdjacent.begin());
	facesVisited.insert( currentFace );
	nextFace = currentFace->getNextFaceWith( pi, &facesVisited );
	adjacentVertsInOrder.clear();

	if(printDebug) {
		cout << "[Mesh::getSurroundingVerticesInOrder] Vertex A,B,C: " << currentFace->getVertA()->getIndex() << "\t" <<
		        currentFace->getVertB()->getIndex() << "\t" << currentFace->getVertC()->getIndex() << endl;
		cout << "[Mesh::getSurroundingVerticesInOrder] size of facesAdjacent: " << facesAdjacent.size() << endl;
		cout << "[Mesh::getSurroundingVerticesInOrder] curFaceID1: " << currentFace->getIndex() << endl;
	}

	if(pi == currentFace->getVertA()) {
		adjacentVertsInOrder.push_back(currentFace->getVertA());
		adjacentVertsInOrder.push_back(currentFace->getVertC());
		adjacentVertsInOrder.push_back(currentFace->getVertB());
	}
	if(pi== currentFace->getVertB()) {
		adjacentVertsInOrder.push_back(currentFace->getVertB());
		adjacentVertsInOrder.push_back(currentFace->getVertC());
		adjacentVertsInOrder.push_back(currentFace->getVertA());
	}
	if(pi == currentFace->getVertC()) {
		adjacentVertsInOrder.push_back(currentFace->getVertC());
		adjacentVertsInOrder.push_back(currentFace->getVertA());
		adjacentVertsInOrder.push_back(currentFace->getVertB());
	}

	while( nextFace != nullptr ) {
		currentFace = nextFace;
		nextFace = currentFace->getNextFaceWith( pi, &facesVisited );
		facesVisited.insert( currentFace );

		if(printDebug) {
			cout << "[Mesh::getSurroundingVerticesInOrder] curFaceID2: " << currentFace->getIndex() << endl;
		}

		if(find(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), currentFace->getVertA()) == adjacentVertsInOrder.end()) {
			adjacentVertsInOrder.push_back(currentFace->getVertA());
		}
		if(find(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), currentFace->getVertB()) == adjacentVertsInOrder.end()) {
			adjacentVertsInOrder.push_back(currentFace->getVertB());
		}
		if(find(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), currentFace->getVertC()) == adjacentVertsInOrder.end()) {
			adjacentVertsInOrder.push_back(currentFace->getVertC());
		}
	}

	// in case of a border vertex we also have to dance around in the other direction:
	// in this case, the vertices are pushed to the beginning, to ensure the correct order in the list
	currentFace = (*facesAdjacent.begin());
	nextFace = currentFace->getNextFaceWith( pi, &facesVisited );
	    if(printDebug) {
			cout << "[Mesh::getSurroundingVerticesInOrder] curFaceID3: " << currentFace->getIndex() << endl;
		}

		if(find(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), currentFace->getVertA()) == adjacentVertsInOrder.end()) {
			adjacentVertsInOrder.push_front(currentFace->getVertA());
		}
		if(find(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), currentFace->getVertB()) == adjacentVertsInOrder.end()) {
			adjacentVertsInOrder.push_front(currentFace->getVertB());
		}
		if(find(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), currentFace->getVertC()) == adjacentVertsInOrder.end()) {
			adjacentVertsInOrder.push_front(currentFace->getVertC());
		}

		while( nextFace != nullptr ) {
		currentFace = nextFace;
		nextFace = currentFace->getNextFaceWith( pi, &facesVisited );
		facesVisited.insert( currentFace );

		if(printDebug) {
			cout << "[Mesh::getSurroundingVerticesInOrder] curFaceID4: " << currentFace->getIndex() << endl;
		}

		if(find(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), currentFace->getVertA()) == adjacentVertsInOrder.end()) {
			adjacentVertsInOrder.push_front(currentFace->getVertA());
		}
		if(find(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), currentFace->getVertB()) == adjacentVertsInOrder.end()) {
			adjacentVertsInOrder.push_front(currentFace->getVertB());
		}
		if(find(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), currentFace->getVertC()) == adjacentVertsInOrder.end()) {
			adjacentVertsInOrder.push_front(currentFace->getVertC());
		}
	}

	adjacentVertsInOrder.erase(std::remove(adjacentVertsInOrder.begin(), adjacentVertsInOrder.end(), pi), adjacentVertsInOrder.end());

	if(printDebug) {
		cout << "[Mesh::getSurroundingVerticesInOrder] size of adjacentVertsInOrder: " << adjacentVertsInOrder.size() << endl;
		list<Vertex*>::iterator iter;
		for(iter=adjacentVertsInOrder.begin(); iter!=adjacentVertsInOrder.end();++iter) {
			cout << (*iter)->getIndex() << endl;
		}
	}

	return true;
}




//methods called in mesh.cpp


//! an experimental non maximum suppression method.
//! Takes as input the distance, how far NMS should look (the frontier will progress until vertices are too far away)
//! will enlarge the feature vectors of all vertices to size 18
//! Data will be written at Feature Vector Position 17 (boolean if maximum) and 18 (if maximum, then averaged MSII)
bool experimentalSuppressNonMaxima(double &NMSDistance, vector<Vertex*> &mVertices){

	//these counters are used for feedback in the terminal
	int processedVertexCounter = 0;
	int maximaCounter = 0;

	//look up vector, will be cleared for every vertex.
	vector<bool> checkVisited (mVertices.size());

	//avoid using square root to speed things u
	double squaredDistance = NMSDistance * NMSDistance;

	//loop over all vertices
	for(auto pVertex : mVertices){

		//initialize the checks for visited vertices with false
		//maybe use methods that already exist in vertex.cpp
		for (int i = 0; i < checkVisited.size() ; i++){
			checkVisited[i] = false;
			//maybe this works too: checkVisited.at(i) = false
		}

		//if the feature vector has less than 18 elements, resize it to length 18
		//the current concept is: use the feature vector and write computated values at position 17 and 18
		//resizing pads with zeros
		if(18 > pVertex->getFeatureVectorLen()){
			pVertex->resizeFeatureVector(18);
		}

		//used for relative feature vector element access
		unsigned int currentFeatureVectorLength = pVertex->getFeatureVectorLen();

		//clear data that might sit at these positions, written their by previous calculations with different distances
		//Position 17 (index 16) shows if a vertex is a local maximum
		//Position 18 (index 17) includes the vertex's MSII-value if it is a local maximum
		//pVertex->setFeatureElement( (currentFeatureVectorLength-2) , 0.0);
		//pVertex->setFeatureElement( (currentFeatureVectorLength-1) , 0.0);

		//legacy, initialize with zero, position access deemed too absolute
		//however I like it better at the moment
		pVertex->setFeatureElement(16, 0.0);
		pVertex->setFeatureElement(17, 0.0);


		//per default, it is guessed not to be the (local) maximum
		bool isMax = false;
		//this boolean is used to abort loops quickly if neighbours with larger MSII values are found
		bool biggerValueFound = false;

		//acquire the MSII-value for this vertex
		double MSIIcomponent16;
		//pVertex->getFeatureElement( (currentFeatureVectorLength-3) , &MSIIcomponent16);

		//legacy, position access deemed too absolute
		//however I like i more
		pVertex->getFeatureElement(15, &MSIIcomponent16);

		//Vertices, which will be the input for every search
		queue<Vertex*> marchingFrontier;

		marchingFrontier.push(pVertex);

		//if the list marchingFrontier is empty, there are no vertices to be checked.

		//as long as there are eligible candidates, perform the algorithm
		//only perform the algorithm when no bigger MSII value than the one from the current vertex (pVertex) has been found
		while(!marchingFrontier.empty() && !biggerValueFound){

			list<Vertex*> adjacentVertsInOrder;
			getSurroundingVerticesInOrder(adjacentVertsInOrder, marchingFrontier.front(), false);

			//vertices are marked as visited, when they they are asked for their neighbours
			checkVisited[marchingFrontier.front()->getIndexOriginal()] = true;

			//after the front vertex of the queue was marked as visitied, it is popped
			marchingFrontier.pop();

			//loop over the new found neighbours
			for(auto& adjacentVertex : adjacentVertsInOrder){

				//only perform the algorithm, if no bigger value was found AND the candidate has not been visited yet
				if(!biggerValueFound && !(checkVisited[adjacentVertex->getIndexOriginal()])){

					bool stillInReach = false;

					//helper method for distance calculation is used
					if( wEComputeSquaredDistanceBetweenTwoVertices(adjacentVertex, pVertex) <= squaredDistance){

						stillInReach = true;

					}

					//consider only vertices that are within the reach specified by the user
					if(stillInReach){

						double adjacentVertexsMSIIcomponent16;
						//adjacentVertex->getFeatureElement( (currentFeatureVectorLength-3) , &adjacentVertexsMSIIcomponent16);

						//legacy, position access deemed too absolute
						//however I like it more
						adjacentVertex->getFeatureElement(15, &adjacentVertexsMSIIcomponent16);

						// if a bigger MSII value than the one of the current vertex (pVertex) was found, pVertex is not the (lokal) maximum
						if(MSIIcomponent16 <= adjacentVertexsMSIIcomponent16){

							biggerValueFound = true;
							isMax = false; //not necessary

						}else{ //so it still seems to be the (local) maximum

							isMax = true; //the current Vertex temporarily seems to be a maximum

							//the new found neighbour doesn't have a bigger MSII value, so it is added to the front (put at the end of the queue
							//maybe his neighbours have a bigger MSII value
							marchingFrontier.push(adjacentVertex);

						}
					}
				}
			}
		}
		//if after the while loop the boolean isMax was set to true and remained true, the current vertex pVertex is a maximum

		//if the current vertex is the maximum
		//write at Feature Vector Position 17 a pseudo boolean state.
		//write at Feature Vector Position 18 its MSII value.
		if(isMax){

			//pVertex->setFeatureElement( (currentFeatureVectorLength-2) , 1.0);
			//pVertex->setFeatureElement( (currentFeatureVectorLength-1) , MSIIcomponent16);

			//legacy, position access deemed to absolute
			//however I like it better
			pVertex->setFeatureElement(16, 1.0);
			pVertex->setFeatureElement(17, MSIIcomponent16);


			maximaCounter++;

		}
		//this else-block is important to update the values calculated by this method for example in case another distance is chosen.
		//without this else-block outdated values may remain in the feature vector.
		//alternatively the reset to zero could be implemented at the beginning of the method.
		else {

			//pVertex->setFeatureElement( (currentFeatureVectorLength-2) , 0.0);
			//pVertex->setFeatureElement( (currentFeatureVectorLength-1) , 0.0);

			//legacy, position access deemed to absolute
			//however I like it more
			pVertex->setFeatureElement(16, 0.0);
			pVertex->setFeatureElement(17, 0.0);


		}

		processedVertexCounter++;

		if((processedVertexCounter % 10000) == 0){

			cout << processedVertexCounter << " of " << mVertices.size() << " vertices have been processed until now." << endl;

		}

	}// all vertices were looped over

	cout << "Given the radius of: " << NMSDistance << " these many MSII values (local maxima) survived non maximum suppression: " << maximaCounter << endl;

	return true;

}


//! An experimental Watershed method using values computed in the Non Maximum Suppression Method
//! will enlarge the feature vectors of all vertices to size 20
//! Data will be written at Feature Vector Position 19 (order of processing) and 20 (label)
bool experimentalComputeWatershed(double deletableInput, vector<Vertex*> &mVertices){

	double counterForFutureWork = 1.0;

	//prepare a look up vector to mark visited vertices
	vector<bool> checkVisited (mVertices.size());

	//initialize the checks for visited vertices with false
	for (int i = 0; i < checkVisited.size() ; i++){
		checkVisited[i] = false;
		//maybe this works too: checkVisited.at(i) = false
	}

	//list to retrieve all vertices which are maxima
	//they have 1.0 at feature vector position 17 (index 16)
	list<Vertex*> foundMaxima;

	//labels start with 1 and go up to (the number of maxima)
	double experimentalLabel = 1.0;

	//prepare data
	for(auto pVertex : mVertices){

		//if the feature vector has less than 20 elements, resize it to length 20
		if(20 > pVertex->getFeatureVectorLen())
		{
			pVertex->resizeFeatureVector(20);
		}

		//clear data, that might sit in the feature vectors, remaining from previous runs of the watershed algorithm

		pVertex->setFeatureElement(18, 0.0);
		pVertex->setFeatureElement(19, 0.0);

		double isLocalMaximum;
		pVertex->getFeatureElement(16, &isLocalMaximum);
		//more relative, less absolute
		//pVertex->getFeatureElement( (getVertexNr() - 4) , &isLocalMaximum);

		//add to list of maxima and mark as visited
		//in addition label the vertex
		//a grand counter increments for future work
		if(isLocalMaximum > 0.0){

			foundMaxima.push_back(pVertex);

			pVertex->setFeatureElement(18, counterForFutureWork);
			pVertex->setFeatureElement(19, experimentalLabel);

			int vertexIndex = pVertex->getIndexOriginal();
			checkVisited[vertexIndex] = true;

			counterForFutureWork = counterForFutureWork + 1.0;
			experimentalLabel = experimentalLabel + 1.0;

			//cout << counterForFutureWork << " vertices have been processed." << endl;

		}
	}

	//after this step a list of maxima exists, they are labeled and marked as visited

	//these many clusters will exist
	//int numberOfClusters = foundMaxima.size();

	//the coast, as a map structure
	//this structure was chosen, because it provides ordering
	//the vertices will be ordered by their MSII value
	map<double, Vertex*> watershedCoast;
	//using a multimap may be safer so multiple vertices with the same MSII-value can be included

	//initialize the coast
	//this step can be integrated in the data preparation step, however maybe the separation is useful later
	for(auto& foundMaximum : foundMaxima){

		list<Vertex*> adjacentVertsInOrder;
		getSurroundingVerticesInOrder(adjacentVertsInOrder, foundMaximum, false);

		//retrieve the label, to label coast candidates
		double maximumLabel;
		foundMaximum->getFeatureElement(19, &maximumLabel);

		for(auto& coastCandidate : adjacentVertsInOrder){

			int vertexIndex = coastCandidate->getIndexOriginal();

			//only continue, if the vertex was not visited yet
			if(!checkVisited[vertexIndex]){

				double currentCandidateLabel;
				coastCandidate->getFeatureElement(19, &currentCandidateLabel);

				//label the vertex, if it has no label yet
				//labels start at 1.0, a value below means no label was given yet
				if(currentCandidateLabel < 1.0){

					coastCandidate->setFeatureElement(19, maximumLabel);

					counterForFutureWork = counterForFutureWork + 1.0;

					//add the vertex to the coast
					//the coast is sorted by the MSII-Value

					double coastCandidateMSIIValue;
					coastCandidate->getFeatureElement(15, &coastCandidateMSIIValue);

					//there might be a loss of vertices, if vertices have the same MSII value (at position 16 (index 15)) because the map does not allow multiple equal keys
					watershedCoast.insert( pair<double, Vertex*>(coastCandidateMSIIValue, coastCandidate) );

					//DO NOT mark the vertex as visited
					//A Vertex gets marked as visited ONLY, when its neighbours have been retrieved

				}
				/*
				//add the vertex to the coast
				//the coast is sorted by the MSII-Value

				double coastCandidateMSIIValue;
				coastCandidate->getFeatureElement(15, &coastCandidateMSIIValue);

				//there might be a loss of vertices, if vertices have the same MSII value (at position 16 (index 15)) because the map does not allow multiple equal keys
				watershedCoast.insert( pair<double, Vertex*>(coastCandidateMSIIValue, coastCandidate) );

				//DO NOT mark the vertex as visited
				//A Vertex is visited ONLY, when its neighbours have been retrieved
				*/
				//der Block wurde nach oben gezogen, sobald Nachbarn noch labelbar sind und gerade gelabeled werden, werden sie zur coast hinzugefuegt
			}
		}
	}

	//the following code is very similar to the code above, I consider rewriting this method

	//as long as the coast is not empty
	while(!watershedCoast.empty()){

		//take the Vertex with the largest MSII-value
		Vertex* VertexPWithLargestMSIIValue = watershedCoast.rbegin()->second;

		//store the key of that vertex for later use
		double VPWMSIIValue = watershedCoast.rbegin()->first;

		//that vertex already has a label
		//label its neighbours with that label, add them to the coast

		list<Vertex*> adjacentVertsInOrder;
		getSurroundingVerticesInOrder(adjacentVertsInOrder, VertexPWithLargestMSIIValue, false);

		//retrieve the label to label coast candidates
		double maximumLabel;
		VertexPWithLargestMSIIValue->getFeatureElement(19, &maximumLabel);

		for(auto& newCoastCandidate : adjacentVertsInOrder){

			int candidateIndex = newCoastCandidate->getIndexOriginal();

			//only continue, if the vertex was not visited yet
			if(!checkVisited[candidateIndex]){

				double currentCandidateLabel;
				newCoastCandidate->getFeatureElement(19, &currentCandidateLabel);

				//label the vertex, if it has no label yet
				//labels start at 1.0, a value below means no label was given yet
				if(currentCandidateLabel < 1.0){

					newCoastCandidate->setFeatureElement(19, maximumLabel);

				}

				//add the vertex to the coast
				//the coast is sorted by the MSII-Value

				double coastVertexMSIIValue;
				newCoastCandidate->getFeatureElement(15, &coastVertexMSIIValue);

				//there might be a loss of vertices, if vertices have the same MSII value (at position 16 (index 15)) because the map does not allow multiple equal keys
				watershedCoast.insert( pair<double, Vertex*>(coastVertexMSIIValue, newCoastCandidate) );

			}
		}

		//the current vertex with the largest MSII value (maybe now not anymore) has been searched for neighbours
		//so mark it as visited and note down when it was treated

		VertexPWithLargestMSIIValue->setFeatureElement(18, counterForFutureWork);
		counterForFutureWork = counterForFutureWork + 1.0;

		//mark the Vertex as visited (after its neighbours were added to the coast)
		int vertexIndex = VertexPWithLargestMSIIValue->getIndexOriginal();
		checkVisited[vertexIndex] = true;

		//remove the vertex from the coast
		//DO NOT // watershedCoast.erase(prev(watershedCoast.end())); //THIS IS WRONG, meanwhile the highest element no longer has to be the vertex we looked at, because we inserted other vertices
		watershedCoast.erase(VPWMSIIValue);

		if(((int)counterForFutureWork % 10000) == 0){

			cout << counterForFutureWork << " of " << mVertices.size() << " vertices have been processed." << endl;
		}
	}

	//consider implementation using multimap
	//to avoid the following

	//postprocessing the map solution


	//the following works as long as no unlabeled vertex is surrounded by unlabeled vertices

	int notLabeledYet = 0;
	int postProcessingCouldLabel = 0;

	//find all vertices that don't have a label yet and give them a label of an arbitrary neighbour
	for(auto pVertex : mVertices){

		double checkLabel;
		pVertex->getFeatureElement(19, &checkLabel);

		//no label given
		if(checkLabel < 1.0){

			//note down that it happened
			notLabeledYet++;
			list<Vertex*> adjacentVertsInOrder;
			getSurroundingVerticesInOrder(adjacentVertsInOrder, pVertex, false);

			bool labelCouldBeAssigned = false;

			//use neighbours to assign a label arbitrarily
			for(auto& neighbourVertex : adjacentVertsInOrder){

				if(!labelCouldBeAssigned){

					double possibleLabel;
					neighbourVertex->getFeatureElement(19, &possibleLabel);

					//if the neighbouring vertex has a label, assign it to pVertex
					if(possibleLabel > 0.0){

						pVertex->setFeatureElement(19, possibleLabel);

						postProcessingCouldLabel++;

						labelCouldBeAssigned = true;

					}
				}
			}
		}
	}

	cout << foundMaxima.size() << " maxima formed the base for the watershed algorithm." << endl;

	cout << "The watershed algorithm did not reach " << notLabeledYet << " vertices." << endl;
	cout << "Postprocessing gave " << postProcessingCouldLabel << " of these vertices a label." << endl;

	return true;

}


//! An experimental clustering method
//! It does not use in-cluster variance to judge how good the clustering is
//! Instead it relies on multiple iterations and mean adjusting
//! Even if the user specifies 0 iteration, one iteration is done
//! Will enlarge the feature vectors of all vertices to size 21
//! Data will be written at Feature Vector Position 21 (sub clustering)
bool experimentalComputeClustering(int numberOfIterations, vector<Vertex*> &mVertices){

	//will note down the number of different labels used
	//is likely the same a the number of maxima that non maximum suppression produced
	int numberOfLabels = 0.0;

	int verticesWithoutLabel = 0;

	//prepare data
	//find the number of labels and the number of vertices, which were not labeled
	for(auto pVertex : mVertices){

		//if the feature vector has less than 21 elements, resize it to length 21
		if(21 > pVertex->getFeatureVectorLen()){

			pVertex->resizeFeatureVector(21);

		}

		//clear data, that might sit in the feature vectors, remaining from previous runs of the clustering algorithm
		pVertex->setFeatureElement(20, 0.0);

		double givenLabel;
		pVertex->getFeatureElement(19, &givenLabel);

		if(givenLabel < 1.0){

			//a vertex was not labeled in the watershed step
			verticesWithoutLabel++;

		}else{ //find out how many labels are used

			if(givenLabel > numberOfLabels){

				numberOfLabels = givenLabel;

			}
		}
	}

	//number of clusters is now known
	//it is noted down as int numberOfLabels

	//loop over every cluster
	for(int labelIterator=1;labelIterator<=numberOfLabels;labelIterator++){

		vector<Vertex*> verticesWithCurrentLabel;

		//add vertices of the currently investigated label to a big vector
		for(auto investigatedVertex : mVertices){

			double foundLabel;
			investigatedVertex->getFeatureElement(19, &foundLabel);

			if(labelIterator == round(foundLabel)){

				verticesWithCurrentLabel.push_back(investigatedVertex);

			}
		}
		//The vector verticesWithCurrentLabel now holds all the vertices which will be used for clustering
		//there already is some randomness involved in creating this vector
		//however another shuffling (resulting in a random selection of 3 future cluster means) is done

		vector<Vertex*> threeRandomlyChosenVertices;
		//the random selection is done by a helper method
		wERandomlyChooseVerticesFromVector(verticesWithCurrentLabel, threeRandomlyChosenVertices, 3);

		//the currentClustering will represent the solution after the given number of iterations.
		vector<int> currentClustering (verticesWithCurrentLabel.size());

		//the assignment to the nearest mean is done by a helper method
		wEAssignNormalsToNearestMean(currentClustering, verticesWithCurrentLabel, threeRandomlyChosenVertices);

		//the initial cluster centers
		//Vertex* clusterCNr1 = threeRandomlyChosenVertices[0];
		//Vertex* clusterCNr2 = threeRandomlyChosenVertices[1];
		//Vertex* clusterCNr3 = threeRandomlyChosenVertices[2];

		vector<vector<double>> normalMeans = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};

		for(int iteration=0;iteration<numberOfIterations;iteration++){

			//The mean computation is done by a helper method
			wEComputeMeans(currentClustering, verticesWithCurrentLabel, normalMeans);

			//Determining to which mean a normal is closest is done by a helper method
			wEAssignNormalsToNearestMean(currentClustering, verticesWithCurrentLabel, normalMeans);

		}
		//Clustering did as many iterations as the user wanted.

		//The final clustering is noted down per vertex

		//write the clustering into the feature vector
		for(int i = 0;i<verticesWithCurrentLabel.size();i++){

			Vertex* currentVertex = verticesWithCurrentLabel[i];
			currentVertex->setFeatureElement(20, currentClustering[i]);

		}
	}

	cout << "The clustering algorithm found " << verticesWithoutLabel << " vertices without label." << endl;
	cout << "The clustering algorithm worked on " << numberOfLabels  << " labels, k-means found 3 clusters in each labeled group." << endl;

	return true;

}

//! RANSAC algorithm that fits a tetraeder (into a found wedge)
//! Will enlarge the feature vectors of all vertices to size 22
//! Data will be written at Feature Vector Position 22 if vertices lie on a border between two clusterings
bool experimentalComputeRANSAC(int numberOfIterations, vector<Vertex*> &mVertices){

	//will note down the number of different labels used
	//is likely the same a the number of maxima that non maximum suppression produced
	double numberOfLabels = 0.0;

	int verticesWithoutLabel = 0;

	//prepare data
	//find the number of labels and the number of vertices, which were not labeled
	for(auto pVertex : mVertices){

		//if the feature vector has less than 22 elements, resize it to length 22
		if(22 > pVertex->getFeatureVectorLen()){

			pVertex->resizeFeatureVector(22);

		}

		//clear data, that might sit in the feature vectors, remaining from previous runs of the RANSAC algorithm
		pVertex->setFeatureElement(21, 0.0);

		double givenLabel;
		pVertex->getFeatureElement(19, &givenLabel);

		if(givenLabel < 1.0){

			//a vertex was not labeled in the watershed step
			verticesWithoutLabel++;

		}else{ //find out how many labels are used

			if(givenLabel > numberOfLabels){

				numberOfLabels = givenLabel;

			}
		}
	}

	//number of clusters is now known

	cout << "Number of Clusters is now known" << endl;

	//loop over every labeled group
	for(int labelIterator=1;labelIterator<=(int)numberOfLabels;labelIterator++){

		//vector<Vertex*> verticesWithCurrentLabel;

		//may be used for more elaborate choice of vertices
		//currently can be used to find the tetraeder outer points
		//vector<Vertex*> verticesBorderGroup1And2;
		//vector<Vertex*> verticesBorderGroup2And3;
		//vector<Vertex*> verticesBorderGroup3And1;

		vector<Vertex*> verticesBorderGroup;

		//add vertices of the currently investigated label to a big vector
		for(auto currentVertex : mVertices){

			double foundLabel;
			currentVertex->getFeatureElement(19, &foundLabel);

			//smelly, comparing doubles for equality, however I don't know a better way at the moment
			//if((double)labelIterator == foundLabel){
			if(labelIterator == (int)foundLabel){

				//so a vertex was found to be part of the current label group

				//verticesWithCurrentLabel.push_back(currentVertex);

				//collect all vertices that have a neighbour with a different label

				double currentVertexClusterGroup;
				currentVertex->getFeatureElement(20, &currentVertexClusterGroup);

				list<Vertex*> adjacentVertsInOrder;
				getSurroundingVerticesInOrder(adjacentVertsInOrder, currentVertex, false);

				bool borderVertexFound = false;

				//bool borderVertexWasAddedToDistinctBorderGroup = false;

				for(auto& adjacentVertex : adjacentVertsInOrder){

					//to abort as soon as a borderVertex is found
					if(!borderVertexFound){

						//only consider neighbours in the same label group
						//this is important because on the border of a labeled group neighbours may belong to different clusters,
						//but we only care about the current label group
						double adjacentVertexLabel;
						adjacentVertex->getFeatureElement(19, &adjacentVertexLabel);

						if(labelIterator == (int)adjacentVertexLabel){

							//so the neighbour belongs to the same label group

							double adjacentVertexClusterGroup;
							adjacentVertex->getFeatureElement(20, &adjacentVertexClusterGroup);

							if((int)currentVertexClusterGroup!=(int)adjacentVertexClusterGroup){

								//the current Vertex was found to be a border vertex

								borderVertexFound = true;

								//find out on what border the vertex lies

								//border case 1 and 2 (or 2 and 1)
								if( (((int)currentVertexClusterGroup == 1) && ((int)adjacentVertexClusterGroup == 2))
									|| (((int)currentVertexClusterGroup == 2 ) && ((int)adjacentVertexClusterGroup == 1)) ){

									currentVertex->setFeatureElement(21, 12.0);
									//currentVertex->setFeatureElement(22, 2.0);

								//border case 2 and 3 (or 3 and 2)
								} else if ( (((int)currentVertexClusterGroup == 2) && ((int)adjacentVertexClusterGroup == 3))
									|| (((int)currentVertexClusterGroup == 3 ) && ((int)adjacentVertexClusterGroup == 2)) ){

									currentVertex->setFeatureElement(21, 23.0);
									//currentVertex->setFeatureElement(23, 3.0);

								//border case 1 and 3 (or 3 and 1)
								} else if ( (((int)currentVertexClusterGroup == 3) && ((int)adjacentVertexClusterGroup == 1))
									|| (((int)currentVertexClusterGroup == 1 ) && ((int)adjacentVertexClusterGroup == 3)) ){

									currentVertex->setFeatureElement(21, 31.0);
									//currentVertex->setFeatureElement(23, 3.0);
								}
							}
						}
					}

				}

				//if the current vertex belongs to the border group add it to the vector verticesBordergroup
				if(borderVertexFound){

					verticesBorderGroup.push_back(currentVertex);

				}

			}
		}
		//The vector verticesBorderGroup now holds all the vertices which will be used for RANSAC

		cout << "RANSAC can now start working with one wedge." << endl;

		//the sum of distances represents the quality of the chosen points and their corresponding rays
		//is initialized with a infinity placeholder
		double smallestSumOfDistances = numeric_limits<double>::max();

		cout << "max ini successful" << endl;

		//unassigned pointer may be dangerous
		//Werde ueberdenken
		//could be filled with dummy data
		Vertex* finalTetraederTop;
		Vertex* finalLineVertex1;
		Vertex* finalLineVertex2;
		Vertex* finalLineVertex3;

		//only work on a border group if it includes at least 4 vertices. This is a necessary requirement, as RANSAC needs 4 randomly chosen vertices to start
		if(verticesBorderGroup.size()>=4){

			//RANSAC begins
			for(int iteration=0;iteration<numberOfIterations;iteration++){

				double currentDistanceForThisCoice = 0.0;

				//randomly choose four vertices for RANSAC
				vector<Vertex*> fourRandomlyChosenBorderVertices;
				wERandomlyChooseVerticesFromVector(verticesBorderGroup, fourRandomlyChosenBorderVertices, 4);

				cout << "random choice successful" << endl;

				Vertex* randomlyChosenTetraederTop = fourRandomlyChosenBorderVertices[0];
				Vertex* randomlyChosenTetraederVertex1 = fourRandomlyChosenBorderVertices[1];
				Vertex* randomlyChosenTetraederVertex2 = fourRandomlyChosenBorderVertices[2];
				Vertex* randomlyChosenTetraederVertex3 = fourRandomlyChosenBorderVertices[3];

				for(auto borderVertex : verticesBorderGroup){

					//find minimum distance to one of the rays
					double minimumDistance = min(min(wEComputeShortestDistanceBetweenPointAndLine(borderVertex, randomlyChosenTetraederTop, randomlyChosenTetraederVertex1),
												wEComputeShortestDistanceBetweenPointAndLine(borderVertex, randomlyChosenTetraederTop, randomlyChosenTetraederVertex2)),
												wEComputeShortestDistanceBetweenPointAndLine(borderVertex, randomlyChosenTetraederTop, randomlyChosenTetraederVertex3));
					currentDistanceForThisCoice += minimumDistance;
				}

				cout << "minimum distance computed" << endl;

				if(currentDistanceForThisCoice < smallestSumOfDistances){

					smallestSumOfDistances = currentDistanceForThisCoice;
					finalTetraederTop = randomlyChosenTetraederTop;
					finalLineVertex1 = randomlyChosenTetraederVertex1;
					finalLineVertex2 = randomlyChosenTetraederVertex2;
					finalLineVertex3 = randomlyChosenTetraederVertex3;

				}

				cout << "One RANSAC Iteration finished" << endl;

			}
			//RANSAC now has noted down the current best fit for rays.

			cout << "RANSAC found a best fit for one wedge" << endl;

			//possible bordergroups
			/*
			12
			23
			31
			*/
			/*
			int borderGroupFinalTetVertex1;
			wEGetBorderGroupFromVertexByFeatureVector(finalTetraederVertex1, borderGroupFinalTetVertex1);

			int borderGroupFinalTetVertex2;
			wEGetBorderGroupFromVertexByFeatureVector(finalTetraederVertex2, borderGroupFinalTetVertex2);

			int borderGroupFinalTetVertex3;
			wEGetBorderGroupFromVertexByFeatureVector(finalTetraederVertex3, borderGroupFinalTetVertex3);
			*/

			//determine to which bordergoup which finalTetraederVertex belongs

			Vertex* finalTetraederVertexGroup12;
			Vertex* finalTetraederVertexGroup23;
			Vertex* finalTetraederVertexGroup31;

			wEGetBorderGroupFromVertexByFeatureVector(finalLineVertex1, finalLineVertex2, finalLineVertex3, finalTetraederVertexGroup12, finalTetraederVertexGroup23, finalTetraederVertexGroup31);


			//Determine which projected point on each line is farthest away
			//A faster but less precise way is to simply calculate the distance between the tetraeder top and the vertex itself and NOT the projected point on the line

			double biggestDistanceGroup12 = 0.0;
			Vertex* finalVertexGroup12;

			double biggestDistanceGroup23 = 0.0;
			Vertex* finalVertexGroup23;

			double biggestDistanceGroup31 = 0.0;
			Vertex* finalVertexGroup31;

			for(auto borderVertex : verticesBorderGroup){

				//int currentborderVertexGroup;
				//wEGetBorderGroupFromVertexByFeatureVector(borderVertex, currentborderVertexGroup);

				double currentBorderVertexGroup;
				borderVertex->getFeatureElement(21, &currentBorderVertexGroup);

				//keep of the biggest distance per border group
				//note down the vertex that has the projection farthest away

				if((int)currentBorderVertexGroup == 12){

					double calculatedDistance;

					wEComputeSquaredDistanceFromTetraederTopToProjectedPointOnLine(borderVertex, finalTetraederTop, finalVertexGroup12 , calculatedDistance);
					if(calculatedDistance>biggestDistanceGroup12){

						biggestDistanceGroup12 = calculatedDistance;
						finalVertexGroup12 = borderVertex;

					}

				} else if ((int)currentBorderVertexGroup == 23){

					double calculatedDistance;

					wEComputeSquaredDistanceFromTetraederTopToProjectedPointOnLine(borderVertex, finalTetraederTop, finalVertexGroup23 , calculatedDistance);
					if(calculatedDistance>biggestDistanceGroup23){

						biggestDistanceGroup23 = calculatedDistance;
						finalVertexGroup23 = borderVertex;

					}

				} else if ((int)currentBorderVertexGroup == 31){

					double calculatedDistance;

					wEComputeSquaredDistanceFromTetraederTopToProjectedPointOnLine(borderVertex, finalTetraederTop, finalVertexGroup31 , calculatedDistance);
					if(calculatedDistance>biggestDistanceGroup31){

						biggestDistanceGroup31 = calculatedDistance;
						finalVertexGroup31 = borderVertex;

					}

				}
			}


			/*

			A Tetraeder is:
			finalTetraederTop
			finalVertexGroup12
			finalVertexGroup23
			finalVertexGroup31

			*/

			//writeWedgeToPlyOrObj();

			cout << "A Wedge was extracted" << endl;
		}



	}//all labeled groups have been worked upon

    cout << "RANSAC terminated successfully!" << endl;

	return true;

}


bool experimentalReorderFeatureVector(vector<Vertex*> &mVertices){

	cout << "unfinished" << endl;

	return true;

}
