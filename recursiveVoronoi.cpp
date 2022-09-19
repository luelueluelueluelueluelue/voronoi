#include "recursiveVoronoi.h"
#include <chrono>
#include <random>
#include <iostream>
#define EPSILON std::numeric_limits<double>::epsilon()
#define OFFSET 1.0f
DiscreteGlobalGrid* recursiveVoronoi::begin(std::vector<Vector2> corpos, int level)
{
	//corpos = {
	//Vector2(429.42040830999633,353.40710861466050),
	//Vector2(310.56373249626904,600.00000000000000),
	//Vector2(0.0000000000000000,600.00000000000000)
	//};
	bool isaddcor = false;
	std::vector<Vector2> points = generatePoints(Box{ corpos, 0, 0, 0, 0 }, 20, isaddcor);
	/*
	std::vector<Vector2> points = {
	//Vector2(297.01185825879151,401.12542129650279),
	//Vector2(352.36391568305328,157.62271039965862),
	//Vector2(179.19306546687844,431.24072179216046),
	//Vector2(461.05023171306260,257.22415720221295),
	//Vector2(209.75540861498672,343.85409767413068),
	//Vector2(374.90459677275123,462.48313755971179),
	//Vector2(217.17610631009782,365.25286688308120),
	//Vector2(91.485912298919089,437.53695835654270),
	//Vector2(282.69717605850911,462.56763658559680),
	//Vector2(109.66733375071621,288.92515570696230),
	//Vector2(409.84785761325389,495.51658418210894),
	//Vector2(446.76292835241935,360.46656265902800),
	//Vector2(140.76761986060083,180.79161430237676),
	//Vector2(225.82774813146429,154.29869945577974),
	//Vector2(242.01323574294420,391.15052181877093),
	//Vector2(144.15952348967107,253.88417434442226),
	//Vector2(105.21091147363474,358.36396195350517),
	//Vector2(160.97033595471896,336.33529376047630),
	//Vector2(461.95272456434424,443.11630986306028),
	//Vector2(238.91164839267938,248.53590888636822),
	//Vector2(422.08840920856051,89.135972179911036),
	//Vector2(352.36436749804602,375.92533414281002),
	//Vector2(348.52997215054592,273.75111919505218),
	//Vector2(251.91968969979686,78.513298827252456),
	//Vector2(325.47252177027383,427.20640748192312),
	Vector2(458.89651120204690,493.20913661680004),
	Vector2(281.97740865125928,339.89292356021167),
	//Vector2(461.90977161337923,112.20958487391326),
	//Vector2(338.15118869594409,0.0000000000000000),
	//Vector2(600.00000000000000,210.17487105929746),
	//Vector2(599.99999999999989,104.46485793633839),
	//Vector2(0.0000000000000000,182.39692373424481),
	//Vector2(99.539170506912441,0.0000000000000000),
	//Vector2(267.41538743247781,600.00000000000000),
	//Vector2(0.0000000000000000,71.449934385204628),
	//Vector2(226.72811059907835,0.0000000000000000),
	//Vector2(364.61073641163364,0.0000000000000000),
	Vector2(436.00573747978149,600.00000000000000),
	//Vector2(555.43076876125372,600.00000000000011),
	//Vector2(125.76067384868924,600.00000000000000),
	//Vector2(599.99999999999989,467.79381694998017),
	Vector2(599.81688894314402,600.00000000000000)
	//Vector2(0.0000000000000000,504.08642841883602),
	//Vector2(0.0000000000000000,225.51957762382887),
	//Vector2(0.0000000000000000,406.32343516342661),
	//Vector2(551.27414777062290,0.0000000000000000),
	//Vector2(600.00000000000000,274.77645191808830),
	//Vector2(135.06271553697317,0.0000000000000000),
	//Vector2(599.99999999999989,310.26337473677785)
	//Vector2(0.0000000000000000,295.13840144047367)
	};
	*/
	//std::vector<Vector2> points = {Vector2(0,0),Vector2(0,600),Vector2(600,600)};
	//VoronoiDiagram diagram = getDiagram(corpos, points);
	//VoronoiDiagram diagram = getCVDiagram(corpos, points);
	VoronoiDiagram diagram = getCfCVDiagram(corpos, points);
	
	if (level == 0)
	{
		//diagram.getBoundpos();
		setDiscreteGlobalGrid(diagram);
		mDiagrams.push_back(std::move(diagram));
	}
	else
	{
		generateVoronoi(diagram, level - 1);
	}
	return nullptr;
}
DiscreteGlobalGrid * recursiveVoronoi::begin_dynamic(std::vector<Vector2> corpos, int level)
{
	bool isaddcor = true;
	std::vector<Vector2> points = generatePoints(Box{ corpos, 0, 0, 0, 0 }, 10, isaddcor);
	VoronoiDiagram diagram = getCfCVDiagram_dynamic(corpos, points);
	if (level == 0)
	{
		setDiscreteGlobalGrid(diagram);
		mDiagrams.push_back(std::move(diagram));
	}
	else
	{
		generateVoronoi(diagram, level - 1);
	}
	return nullptr;
}
#include <stack>
void recursiveVoronoi::generateVoronoi(VoronoiDiagram &lastdiagram, int level)
{
	if (level-- < 0)
	{
		return;
	}
	std::stack<int> q;
	std::vector<bool> visited;
	for (size_t i = 0; i < lastdiagram.getNbSites(); i++)
	{
		visited.push_back(false);
	}
	q.push(0);
	visited[0] = true;
	VoronoiDiagram newdiagram = subVoronoi(lastdiagram, 0);
	while (!q.empty())
	{
		int id = q.top();
		q.pop();
		const VoronoiDiagram::Site* site = lastdiagram.getSite(id);
		Vector2 center = site->point;
		VoronoiDiagram::HalfEdge* halfEdge = site->face->outerComponent;
		VoronoiDiagram::HalfEdge* start = halfEdge;
		while (true)
		{
			if (halfEdge->twin != nullptr)
			{
				int id2 = halfEdge->twin->incidentFace->site->index;
				if (!visited[id2])
				{
					visited[id2] = true;
					q.push(id2);
					//deal something
					VoronoiDiagram diagram = subVoronoi(lastdiagram,id2);
					//将diagram合并至newdiagram
					mergeVoronoi(lastdiagram, newdiagram, diagram);
				}
			}
			halfEdge = halfEdge->next;
			if (halfEdge == start)
				break;
		}
	}
	//必须一层一层地做，要保证上层建筑的稳定
	//用完上一层之后把对象移至容器存储起来
	setDiscreteGlobalGrid(lastdiagram);
	mDiagrams.push_back(std::move(lastdiagram));
	generateVoronoi(newdiagram, level);
}
void recursiveVoronoi::mergeVoronoi(VoronoiDiagram &parentDiagram, VoronoiDiagram &newDiagram, VoronoiDiagram &diagram)
{
	//先找出diagram的环边点，并根据最后几个点进行分割
}
DiscreteGlobalGrid recursiveVoronoi::getDiscreteGlobalGrid()
{
	return mDiscreteGlobalGrid;
}
void recursiveVoronoi::setDiscreteGlobalGrid(VoronoiDiagram & diagram)
{
	for (std::size_t i = 0; i < diagram.getNbSites(); ++i)
	{
		const VoronoiDiagram::Site* site = diagram.getSite(i);
		Vector2 center = site->point;
		mDiscreteGlobalGrid.points.push_back(center);
		VoronoiDiagram::HalfEdge* halfEdge = site->face->outerComponent;
		VoronoiDiagram::HalfEdge* start = halfEdge;
		Edge e;
		double testangle = 0;
		while (true && halfEdge != nullptr && halfEdge->origin != nullptr && halfEdge->destination != nullptr)
		{
			testangle += halfEdge->angle;
			Vector2 origin = (halfEdge->origin->point - center) * OFFSET + center;
			Vector2 destination = (halfEdge->destination->point - center) * OFFSET + center;
			e.origin.push_back(origin);
			e.destination.push_back(destination);
			halfEdge = halfEdge->next;
			if (halfEdge == start || halfEdge == nullptr || halfEdge->origin == nullptr || halfEdge->destination == nullptr)
				break;
		}
		mDiscreteGlobalGrid.edges.push_back(e);
	}
	mDiscreteGlobalGrid.otherTestPoints.insert(mDiscreteGlobalGrid.otherTestPoints.end(),
		diagram.mBoundSitesid.begin(), diagram.mBoundSitesid.end());
}
void recursiveVoronoi::clearDiscreteGlobalGrid(VoronoiDiagram & diagram)
{

}
VoronoiDiagram recursiveVoronoi::getCVDiagram(std::vector<Vector2> corpos, std::vector<Vector2> points, bool isadd)
{
	double error = 0.1;
	VoronoiDiagram diagram;
	while (true)
	{
		diagram = getDiagram(corpos, points);
		std::vector<Vector2> newpoints;
		if (isadd)
			newpoints = diagram.getCentroidPos(corpos.size());
		else
			newpoints = diagram.getCentroidPos(0);
		//std::vector<Vector2> newpoints = diagram.getCentroidPos(corpos.size());
		size_t i = 0;
		for (; i < newpoints.size(); i++)
		{
			double x1 = newpoints[i].x;
			double y1 = newpoints[i].y;
			double x2 = points[i].x;
			double y2 = points[i].y;
			double d = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
			if (d > error)
			{
				points = newpoints;
				break;
			}
		}
		if (i == newpoints.size())
			break;
	}
	return std::move(diagram);
}
VoronoiDiagram recursiveVoronoi::getCfCVDiagram(std::vector<Vector2> corpos, std::vector<Vector2> points, bool isadd)
{
	double error = 0.1;
	VoronoiDiagram diagram;
	int n = 0;
	while (true)
	{
		diagram = getDiagram(corpos, points);
		std::vector<Vector2> newpoints = diagram.getCfCVDPos(corpos.size() + n, n);
		//std::vector<Vector2> newpoints = diagram.getCentroidPos(corpos.size());
		size_t i = 0;
		for (; i < newpoints.size(); i++)
		{
			double x1 = newpoints[i].x;
			double y1 = newpoints[i].y;	
			double x2 = points[i].x;
			double y2 = points[i].y;
			double d = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
			if (d > error)
			{
				points = newpoints;
				break;
			}
		}
		if (i == newpoints.size())
			break;
	}
	return std::move(diagram);
}
VoronoiDiagram recursiveVoronoi::getCfCVDiagram_dynamic(std::vector<Vector2> corpos, std::vector<Vector2> points, bool isadd)
{
	double error = 0.1;
	VoronoiDiagram diagram;
	int n = 0;
	while (true)
	{
		diagram = getDiagram(corpos, points);
		setDiscreteGlobalGrid(diagram);
		std::vector<Vector2> newpoints = diagram.getCfCVDPos(corpos.size() + n, n);
		size_t i = 0;
		for (; i < newpoints.size(); i++)
		{
			double x1 = newpoints[i].x;
			double y1 = newpoints[i].y;
			double x2 = points[i].x;
			double y2 = points[i].y;
			double d = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
			if (d > error)
			{
				points = newpoints;
				break;
			}
		}
		if (i == newpoints.size())
			break;
	}
	return std::move(diagram);
}
VoronoiDiagram recursiveVoronoi::getDiagram(std::vector<Vector2> corpos, std::vector<Vector2> points)
{
	FortuneAlgorithm algorithm(points);
	algorithm.construct();
	algorithm.bound(Box{ corpos, 0.0, 0.0, 0.0, 0.0 });
	VoronoiDiagram diagram = algorithm.getDiagram();
	bool valid = diagram.intersect(Box{ corpos, 0.0, 0.0, 0.0, 0.0 });
	if (!valid)
		throw std::runtime_error("An error occured in the box intersection algorithm");
	return std::move(diagram);
}
VoronoiDiagram recursiveVoronoi::subVoronoi(VoronoiDiagram &lastdiagram,int id)
{
	std::vector<Vector2> corpos;
	const VoronoiDiagram::Site* site = lastdiagram.getSite(id);
	VoronoiDiagram::HalfEdge* halfEdge = site->face->outerComponent;
	VoronoiDiagram::HalfEdge* start = halfEdge;
	while (true)
	{
		corpos.push_back(halfEdge->origin->point);
		halfEdge = halfEdge->next;
		if (halfEdge == start)
			break;
	}
	std::vector<Vector2> points = generatePoints(Box{ corpos, 0, 0, 0, 0 }, 10,true);
	//VoronoiDiagram diagram = getDiagram(corpos, points);
	//VoronoiDiagram diagram = getCVDiagram(corpos, points);
	VoronoiDiagram diagram = getCfCVDiagram(corpos, points);
	setDiscreteGlobalGrid(diagram);
	return std::move(diagram);
}

std::vector<Vector2> recursiveVoronoi::generatePoints(Box box, int nbPoints, bool isAddCor)
{
	uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<double> distribution(0.0, 1.0);
	box.left = box.corpos[0].x;
	box.bottom = box.corpos[0].y;
	box.right = box.corpos[0].x;
	box.top = box.corpos[0].y;
	for (size_t i = 0; i < box.corpos.size(); i++)
	{
		box.left = std::min(box.corpos[i].x, box.left);
		box.bottom = std::min(box.corpos[i].y, box.bottom);
		box.right = std::max(box.corpos[i].x, box.right);
		box.top = std::max(box.corpos[i].y, box.top);
	}
	double w = box.right - box.left;
	double h = box.top - box.bottom;

	std::vector<Vector2> points;
	while (points.size() < nbPoints)
	{
		Vector2 pos(box.left + w * (double)rand() / (double)RAND_MAX, box.bottom + h * (double)rand() / (double)RAND_MAX);
		if (box.contains(pos) >= 0)
		{
			bool ret = true;
			for (size_t i = 0; i < points.size(); i++)
			{
				if (std::abs(points[i].y - pos.y) < EPSILON)
				{
					ret = false;
					break;
				}
			}
			if (ret)
			{
				points.push_back(pos);
			}
		}
	}
	if (isAddCor)
	{
		for (size_t i = 0; i < box.corpos.size(); i++)
		{
			points.push_back(box.corpos[i]);
		}
	}
	return points;
}