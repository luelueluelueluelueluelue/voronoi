#pragma once
#include "FortuneAlgorithm.h"
struct Edge {
	std::vector<Vector2> origin;
	std::vector<Vector2> destination;
};
struct DiscreteGlobalGrid
{
	std::vector<Vector2> points;
	std::vector<Edge> edges;
	std::vector<Vector2> otherTestPoints;
};
class recursiveVoronoi
{
public:
	
	DiscreteGlobalGrid* begin(std::vector<Vector2> corpos, int level);
	DiscreteGlobalGrid* begin_dynamic(std::vector<Vector2> corpos, int level);
	void generateVoronoi(VoronoiDiagram &lastdiagram, int level);
	DiscreteGlobalGrid getDiscreteGlobalGrid();
	void setDiscreteGlobalGrid(VoronoiDiagram &diagram);
	void clearDiscreteGlobalGrid(VoronoiDiagram &diagram);
	VoronoiDiagram subVoronoi(VoronoiDiagram &lastdiagram, int id);
	void mergeVoronoi(VoronoiDiagram &parentDiagram, VoronoiDiagram &newDiagram, VoronoiDiagram &diagram);
	VoronoiDiagram getCVDiagram(std::vector<Vector2> corpos, std::vector<Vector2> points, bool isadd = true);
	VoronoiDiagram getCfCVDiagram(std::vector<Vector2> corpos, std::vector<Vector2> points, bool isadd = true);
	VoronoiDiagram getCfCVDiagram_dynamic(std::vector<Vector2> corpos, std::vector<Vector2> points, bool isadd = true);
	VoronoiDiagram getDiagram(std::vector<Vector2> corpos, std::vector<Vector2> points);
	std::vector<Vector2> generatePoints(Box box, int nbPoints, bool isAddCor);
private:
	//VoronoiDiagram* diagram;
	std::vector<VoronoiDiagram> mDiagrams;
	DiscreteGlobalGrid mDiscreteGlobalGrid;
};

