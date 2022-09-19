/* FortuneAlgorithm
 * Copyright (C) 2018 Pierre Vigier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// STL
#include <vector>
#include <list>
// My includes
#include "Box.h"
//#include "Arc.h"

class FortuneAlgorithm;
class Arc;
class Boundp;
class VoronoiDiagram
{
public:
    struct HalfEdge;
    struct Face;
	
    struct Site
    {
        std::size_t index;
        Vector2 point;
        Face* face;
    };
	
	struct Circle
	{
		double y;
		Vector2 point;
		Arc* arc;
	};
    struct Vertex
    {
        Vector2 point;

    private:
        friend VoronoiDiagram;
        std::list<Vertex>::iterator it;
    };
	struct ParentHalfEdge
	{
		bool isMarginEdge = false; //用于判断该边是否是临界边
		HalfEdge* parent = nullptr;//对应的父边地址索引
		std::size_t edgeid = -1;//该边在父边中所处的位置 
	};
	struct ChildrenHalfEdge
	{
		bool isMarginEdge = false; //用于判断该边是否是临界边
		std::vector<HalfEdge*> childrens;//顺序存储子边索引
	};
    struct HalfEdge
    {
        Vertex* origin = nullptr;
        Vertex* destination = nullptr;
        HalfEdge* twin = nullptr;
        Face* incidentFace;
        HalfEdge* prev = nullptr;
        HalfEdge* next = nullptr;
		double angle; //与生成点间的夹角
		ParentHalfEdge parentEdge; //父边内容
		ChildrenHalfEdge chldrensEdge;//子边内容
		bool isbound = false;//是否是临界边
    private:
        friend VoronoiDiagram;
        std::list<HalfEdge>::iterator it;
    };
    struct Face
    {
        Site* site;
        HalfEdge* outerComponent;
    };

    VoronoiDiagram(const std::vector<Vector2>& points);
	VoronoiDiagram();
    // Remove copy operations
    VoronoiDiagram(const VoronoiDiagram&) = delete;
    VoronoiDiagram& operator=(const VoronoiDiagram&) = delete;

    // Move operations
    VoronoiDiagram(VoronoiDiagram&&) = default;
    VoronoiDiagram& operator=(VoronoiDiagram&&) = default;

    // Accessors
    Site* getSite(std::size_t i);
    std::size_t getNbSites() const;
    Face* getFace(std::size_t i);
    const std::list<Vertex>& getVertices() const;
    const std::list<HalfEdge>& getHalfEdges() const;

    // Intersection with a box
    bool intersect(Box box);

	std::vector<Vector2> getCentroidPos(int retainsize);
	std::vector<Vector2> getCfCVDPos(int retainsize, int &n);
	void getBoundpos(VoronoiDiagram &parentDiagram, VoronoiDiagram &childrenDiagram);
	void getAngle(VoronoiDiagram::HalfEdge* halfEdge);
	std::vector<Vector2> mBoundSitesid;
	VoronoiDiagram::HalfEdge* mboundBegin = nullptr;
private:
    std::vector<Site> mSites;
    std::vector<Face> mFaces;
    std::list<Vertex> mVertices;
	std::list<HalfEdge> mHalfEdges;
	

    // Diagram construction
    friend FortuneAlgorithm;

    Vertex* createVertex(Vector2 point);
	Vertex* createCorner(Box box, Box::Side side);
	Vertex* createCorner2(Box box, int side);
    HalfEdge* createHalfEdge(Face* face);

    // Intersection with a box
	void link(Box box, HalfEdge* start, Box::Side startSide, HalfEdge* end, Box::Side endSide);
	void link2(Box box, HalfEdge* start, int startSide, HalfEdge* end, int endSide);
    void removeVertex(Vertex* vertex);
    void removeHalfEdge(HalfEdge* halfEdge);
};
