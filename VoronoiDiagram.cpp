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

#include "VoronoiDiagram.h"
// STL
#include <unordered_set>
#include <iostream>
#include <random>
#define EPSILON 0.00000001


VoronoiDiagram::VoronoiDiagram(const std::vector<Vector2>& points)
{
    mSites.reserve(points.size());
    mFaces.reserve(points.size());
    for(std::size_t i = 0; i < points.size(); ++i)
    {
        mSites.push_back(VoronoiDiagram::Site{i, points[i], nullptr});
        mFaces.push_back(VoronoiDiagram::Face{&mSites.back(), nullptr});
        mSites.back().face = &mFaces.back();
    }
}

VoronoiDiagram::VoronoiDiagram()
{
}

VoronoiDiagram::Site* VoronoiDiagram::getSite(std::size_t i)
{
    return &mSites[i];
}

std::size_t VoronoiDiagram::getNbSites() const
{
    return mSites.size();
}

VoronoiDiagram::Face* VoronoiDiagram::getFace(std::size_t i)
{
    return &mFaces[i];
}

const std::list<VoronoiDiagram::Vertex>& VoronoiDiagram::getVertices() const
{
    return mVertices;
}

const std::list<VoronoiDiagram::HalfEdge>& VoronoiDiagram::getHalfEdges() const
{
    return mHalfEdges;
}

bool VoronoiDiagram::intersect(Box box)
{
    bool error = false;
    std::unordered_set<HalfEdge*> processedHalfEdges;
    std::unordered_set<Vertex*> verticesToRemove;
	int vaildsize = mSites.size() - box.corpos.size();
    for (const Site& site : mSites)
    {
        HalfEdge* halfEdge = site.face->outerComponent;
		int ret = box.contains(halfEdge->origin->point);
		bool inside;
		if (ret == -1)
			inside = false;
		else
			inside = true;
        bool outerComponentDirty = !inside;
        HalfEdge* incomingHalfEdge = nullptr; // First half edge coming in the box
        HalfEdge* outgoingHalfEdge = nullptr; // Last half edge going out the box
        int incomingSide, outgoingSide;
        do
        {
            std::array<Box::Intersection, 2> intersections;
            int nbIntersections = box.getIntersections(halfEdge->origin->point, halfEdge->destination->point, intersections);
			//std::cout << nbIntersections << std::endl;
			int ret2 = box.contains(halfEdge->destination->point);
			bool nextInside;
			if (ret2 == -1)
				nextInside = false;
			else
				nextInside = true;
			
            HalfEdge* nextHalfEdge = halfEdge->next;
			bool isremove = false;
            // The two points are outside the box 
            if (!inside && !nextInside)
            {
                // The edge is outside the box
                if (nbIntersections == 0)
                {
                    verticesToRemove.emplace(halfEdge->origin);
                    removeHalfEdge(halfEdge);
					isremove = true;
                }
                // The edge crosses twice the frontiers of the box
                else if (nbIntersections == 2)
                {
                    verticesToRemove.emplace(halfEdge->origin);
                    if (processedHalfEdges.find(halfEdge->twin) != processedHalfEdges.end())
                    {
                        halfEdge->origin = halfEdge->twin->destination;
                        halfEdge->destination = halfEdge->twin->origin;
                    }
                    else
                    {
                        halfEdge->origin = createVertex(intersections[0].point);
                        halfEdge->destination = createVertex(intersections[1].point);
                    }
                    if (outgoingHalfEdge != nullptr)
                        link2(box, outgoingHalfEdge, outgoingSide, halfEdge, intersections[0].id);
                    if (incomingHalfEdge == nullptr)
                    {
                       incomingHalfEdge = halfEdge;
                       incomingSide = intersections[0].id;
                    }
                    outgoingHalfEdge = halfEdge;
                    outgoingSide = intersections[1].id;
                    processedHalfEdges.emplace(halfEdge);
                }
                else
                    error = true;
            }
            // The edge is going outside the box
            else if (inside && !nextInside)
            {
                if (nbIntersections == 1)
                {
                    if (processedHalfEdges.find(halfEdge->twin) != processedHalfEdges.end())
                        halfEdge->destination = halfEdge->twin->origin;
                    else
                        halfEdge->destination = createVertex(intersections[0].point);
                    outgoingHalfEdge = halfEdge;
                    outgoingSide = intersections[0].id;
                    processedHalfEdges.emplace(halfEdge);
                }
                else
                    error = true;
            }
            // The edge is coming inside the box
            else if (!inside && nextInside)
            {
                if (nbIntersections == 1)
                {
                    verticesToRemove.emplace(halfEdge->origin);
                    if (processedHalfEdges.find(halfEdge->twin) != processedHalfEdges.end())
                        halfEdge->origin = halfEdge->twin->destination;
                    else
                        halfEdge->origin = createVertex(intersections[0].point);
                    if (outgoingHalfEdge != nullptr)
                        link2(box, outgoingHalfEdge, outgoingSide, halfEdge, intersections[0].id);
                    if (incomingHalfEdge == nullptr)
                    {
                       incomingHalfEdge = halfEdge;
                       incomingSide = intersections[0].id;
                    }
                    processedHalfEdges.emplace(halfEdge);
                }
                else
                    error = true;
            }
			if (site.index < vaildsize && !isremove)
			{
				getAngle(halfEdge);
			}
            halfEdge = nextHalfEdge;
            // Update inside
            inside = nextInside;
        } while (halfEdge != site.face->outerComponent && halfEdge != nullptr);
        // Link the last and the first half edges inside the box
        if (outerComponentDirty && incomingHalfEdge != nullptr)
            link2(box, outgoingHalfEdge, outgoingSide, incomingHalfEdge, incomingSide);
        // Set outer component
        if (outerComponentDirty)
            site.face->outerComponent = incomingHalfEdge;
    }
    // Remove vertices
    for (auto& vertex : verticesToRemove)
        removeVertex(vertex);
    // Return the status
    return !error;
}

std::vector<Vector2> VoronoiDiagram::getCentroidPos(int retainsize)
{
	std::vector<Vector2> pos;
	for (size_t i = 0; i < getNbSites() - retainsize; i++)
	{
		const VoronoiDiagram::Site* site = getSite(i);
		//计算质心点
		VoronoiDiagram::HalfEdge* halfEdge = site->face->outerComponent;
		VoronoiDiagram::HalfEdge* start = halfEdge;
		double x = 0,y = 0;
		int nbv = 0;
		while (true)
		{
			x += halfEdge->origin->point.x;
			y += halfEdge->origin->point.y;
			nbv++;
			halfEdge = halfEdge->next;
			if (halfEdge == start)
				break;
		}
		x /= nbv;
		y /= nbv;
		pos.push_back(Vector2(x, y));
	}
	for (size_t i = getNbSites() - retainsize; i < getNbSites(); i++)
	{
		const VoronoiDiagram::Site* site = getSite(i);
		pos.push_back(site->point);
	}
	return pos;
}
Vector2 getlineprojection(Vector2 P, Vector2 A, Vector2 B) {
	Vector2 v = B - A;
	return A + v * (v.dot(P - A) / v.dot(v));
}
std::vector<Vector2> VoronoiDiagram::getCfCVDPos(int retainsize, int &n)
{
	std::vector<Vector2> pos;
	std::vector<Vector2> tpos;
	std::vector<int> ids;
	n = 0;
	VoronoiDiagram::HalfEdge* boundBegin = nullptr;
	for (size_t i = 0; i < getNbSites() - retainsize; i++)
	{
		const VoronoiDiagram::Site* site = getSite(i);
		std::vector<VoronoiDiagram::HalfEdge*> angles;
		//计算质心点
		VoronoiDiagram::HalfEdge* halfEdge = site->face->outerComponent;
		VoronoiDiagram::HalfEdge* start = halfEdge;
		double x = 0, y = 0;
		int nbv = 0;
		while (true)
		{
			if (boundBegin == nullptr && halfEdge->twin == nullptr)
			{
				boundBegin = halfEdge;
			}
			x += halfEdge->origin->point.x;
			y += halfEdge->origin->point.y;
			nbv++;
			halfEdge = halfEdge->next;
			if (halfEdge == start)
				break;
		}
		x /= nbv;
		y /= nbv;
		tpos.push_back(Vector2(x, y));
	}
	//计算边界点
	if (boundBegin != nullptr)
	{
		std::vector<VoronoiDiagram::HalfEdge*> bounds;
		VoronoiDiagram::HalfEdge* corner = nullptr;
		VoronoiDiagram::HalfEdge* start = boundBegin;
		VoronoiDiagram::HalfEdge* halfEdge = boundBegin;
		while (true)
		{
			//do somthing
			double x;
			double y;
			/*
			Vector2 p = halfEdge->incidentFace->site->point;
			double ang = halfEdge->angle;
			//halfEdge边斜率
			double k = (halfEdge->origin->point.y - halfEdge->destination->point.y) / 
				(halfEdge->origin->point.x - halfEdge->destination->point.x);
			
			
			if (std::abs(halfEdge->origin->point.y - halfEdge->destination->point.y) < EPSILON)
			{
				x = p.x;
				y = halfEdge->origin->point.y;
			}
			else if (std::abs(halfEdge->origin->point.x - halfEdge->destination->point.x) < EPSILON)
			{
				y = p.y;
				x = halfEdge->origin->point.x;
			}
			else
			{
				double b = halfEdge->origin->point.y / (k * halfEdge->origin->point.x);
				x = (k * (p.y - b) + p.x) / (k*k + 1);
				y = k * x + b;
			}
			*/
			
			Vector2 tp;
			//投影计算
			//tp = getlineprojection(halfEdge->incidentFace->site->point,halfEdge->origin->point, halfEdge->destination->point);
			//中点计算
			tp = (halfEdge->origin->point + halfEdge->destination->point) / 2;
			x = tp.x;
			y = tp.y;
			//排除不调整点
			if (halfEdge->incidentFace->site->index < tpos.size())
			{
				ids.push_back(halfEdge->incidentFace->site->index);
				tpos[halfEdge->incidentFace->site->index] = Vector2(x, y);
			}

			if (halfEdge->next->twin != nullptr)
			{
				halfEdge = halfEdge->next->twin->next;
				bounds.push_back(halfEdge);
			}
			else
			{
				//遇到拐角点，不对拐点进行操作
				halfEdge = halfEdge->next;
				bounds.push_back(nullptr);
			}
			if (halfEdge == start)
				break;
		}
	}
	//处理bounds
	for (size_t i = 0; i < getNbSites() - retainsize; i++)
	{
		if (std::find(ids.begin(), ids.end(), i) == ids.end())
			pos.insert(pos.begin(), tpos[i]);
		else
		{
			pos.push_back(tpos[i]);
			n++;
		}

	}
	for (size_t i = getNbSites() - retainsize; i < getNbSites(); i++)
		pos.push_back(getSite(i)->point);
	
	return pos;
}

void VoronoiDiagram::getBoundpos(VoronoiDiagram &parentDiagram, VoronoiDiagram &childrenDiagram)
{
	//先找到一条边缘边后直接跳出
	VoronoiDiagram::HalfEdge* start = nullptr;
	for (int i = mSites.size() - 1; i >= 0; i--)
	{
		Site site = mSites[i];
		VoronoiDiagram::HalfEdge* halfEdge = site.face->outerComponent;
		VoronoiDiagram::HalfEdge* end = halfEdge;
		while (true)
		{
			if (halfEdge->twin == nullptr)
			{
				//找到，跳出
				start = halfEdge;
				i = -1;
				break;
			}
			halfEdge = halfEdge->next;
			if (halfEdge == end)
				break;
		}
	}
	//开始遍历临界边点集
	VoronoiDiagram::HalfEdge* end = start;
	start = start->next->twin->prev;
	while (start != end)
	{

	}
	if (start->next->twin->prev == nullptr)
	{
		VoronoiDiagram::HalfEdge* halfEdge = start->next->twin->prev;
	}
}

void VoronoiDiagram::getAngle(VoronoiDiagram::HalfEdge* halfEdge)
{
	Vector2 A = halfEdge->incidentFace->site->point;
	Vector2 B = halfEdge->origin->point;
	Vector2 C = halfEdge->destination->point;
	double x1 = B.x;
	double x2 = C.x;
	double x3 = A.x;
	double y1 = B.y;
	double y2 = C.y;
	double y3 = A.y;
	double theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
	if (theta > std::_Pi)
		theta -= 2 * std::_Pi;
	if (theta < -std::_Pi)
		theta += 2 * std::_Pi;

	theta = abs(theta * 180.0 / std::_Pi);
	halfEdge->angle = theta;
	/*
	//计算对应的角度
	Vector2 A = halfEdge->incidentFace->site->point;
	Vector2 B = halfEdge->origin->point;
	Vector2 C = halfEdge->destination->point;
	//AB=(B.x-A.x, B.y-A.y)
	Vector2 AB = B - A;
	//AC=(C.x-A.x, C.y-A.y)
	Vector2 AC = C - A;
	//cosA = (AB*AC)/(|AB|*|AC|)
	double ang = std::acos((AB.dot(AC)) / (AB.getNorm()*AC.getNorm())) * 180 / std::_Pi;
	halfEdge->angle = ang;*/
}

VoronoiDiagram::Vertex* VoronoiDiagram::createVertex(Vector2 point)
{
    mVertices.emplace_back();
    mVertices.back().point = point;
    mVertices.back().it = std::prev(mVertices.end());
    return &mVertices.back();
}
VoronoiDiagram::Vertex* VoronoiDiagram::createCorner2(Box box, int side)
{
	return createVertex(box.corpos[side]);
}
VoronoiDiagram::Vertex* VoronoiDiagram::createCorner(Box box, Box::Side side)
{
    switch (side)
    {
        case Box::Side::LEFT:
            return createVertex(Vector2(box.left, box.top));
        case Box::Side::BOTTOM:
            return createVertex(Vector2(box.left, box.bottom));
        case Box::Side::RIGHT:
            return createVertex(Vector2(box.right, box.bottom));
        case Box::Side::TOP:
            return createVertex(Vector2(box.right, box.top));
        default:
            return nullptr;
    }
}

VoronoiDiagram::HalfEdge* VoronoiDiagram::createHalfEdge(Face* face)
{
    mHalfEdges.emplace_back();
    mHalfEdges.back().incidentFace = face;
    mHalfEdges.back().it = std::prev(mHalfEdges.end());
    if(face->outerComponent == nullptr)
        face->outerComponent = &mHalfEdges.back();
    return &mHalfEdges.back();
}

void VoronoiDiagram::link(Box box, HalfEdge* start, Box::Side startSide, HalfEdge* end, Box::Side endSide)
{
	HalfEdge* halfEdge = start;
	int side = static_cast<int>(startSide);
	while (side != static_cast<int>(endSide))
	{
		side = (side + 1) % 4;
		halfEdge->next = createHalfEdge(start->incidentFace);
		halfEdge->next->prev = halfEdge;
		halfEdge->next->origin = halfEdge->destination;
		halfEdge->next->destination = createCorner(box, static_cast<Box::Side>(side));
		halfEdge = halfEdge->next;
	}
	halfEdge->next = createHalfEdge(start->incidentFace);
	halfEdge->next->prev = halfEdge;
	end->prev = halfEdge->next;
	halfEdge->next->next = end;
	halfEdge->next->origin = halfEdge->destination;
	halfEdge->next->destination = end->origin;
}
void VoronoiDiagram::link2(Box box, HalfEdge* start, int startSide, HalfEdge* end, int endSide)
{
	HalfEdge* halfEdge = start;
	int side = startSide;
	while (side != endSide)
	{
		side = (side + 1) % box.corpos.size();
		halfEdge->next = createHalfEdge(start->incidentFace);
		halfEdge->next->prev = halfEdge;
		halfEdge->next->origin = halfEdge->destination;
		halfEdge->next->destination = createCorner2(box, side);
		halfEdge = halfEdge->next;
	}
	halfEdge->next = createHalfEdge(start->incidentFace);
	halfEdge->next->prev = halfEdge;
	end->prev = halfEdge->next;
	halfEdge->next->next = end;
	halfEdge->next->origin = halfEdge->destination;
	halfEdge->next->destination = end->origin;
	getAngle(halfEdge->next);
}
void VoronoiDiagram::removeVertex(Vertex* vertex)
{
    mVertices.erase(vertex->it);
}

void VoronoiDiagram::removeHalfEdge(HalfEdge* halfEdge)
{
    mHalfEdges.erase(halfEdge->it);
}

