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
#include <array>
#include <limits>
// My includes
#include "Vector2.h"
#include <vector>
class Box
{
public:
    // Be careful, y-axis is oriented to the top like in math
    enum class Side : int {LEFT, BOTTOM, RIGHT, TOP};

    struct Intersection
    {
		int id;
        Side side;
        Vector2 point;
    };
	struct Line
	{
		Vector2 p1, p2;
		double a, b, c;
	};
	std::vector<Vector2> corpos;
    double left;
    double bottom;
    double right;
    double top;

    int contains(const Vector2& point);
    Intersection getFirstIntersection(const Vector2& origin, const Vector2& direction) const; // Useful for Fortune's algorithm
    int getIntersections(const Vector2& origin, const Vector2& destination, std::array<Intersection, 2>& intersections) ; // Useful for diagram intersection


	bool IsLineSegmentCross(const Vector2 &pFirst1, const Vector2 &pFirst2, const Vector2 &pSecond1, const Vector2 &pSecond2);
	bool getCrossPoint(const Vector2 &p1, const Vector2 &p2, const Vector2 &q1, const Vector2 &q2, Vector2& v, double& d);
	bool IsRectCross(const Vector2 &p1, const Vector2 &p2, const Vector2 &q1, const Vector2 &q2);
	Vector2 getCrossPoint(Line &l1, Line &l2);
	void GetLinePara(Line &l);
private:
    static constexpr double EPSILON = std::numeric_limits<double>::epsilon();
};

