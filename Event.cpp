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

#include "Event.h"

Event::Event(VoronoiDiagram::Site* site) : type(Type::SITE), y(site->point.y), index(-1), site(site)
{

}

Event::Event(VoronoiDiagram::Circle* circle) : type(Type::CIRCLE), y(circle->y), index(-1), point(circle->point), arc(circle->arc)
{


}
bool operator<(const Event& lhs, const Event& rhs)
{
	bool is = false;
	//if (lhs.y < rhs.y || (std::abs(lhs.y - rhs.y) < 0.00000001 && lhs.site->point.x < rhs.site->point.x))
	if (lhs.y < rhs.y)
		is = true;
    return is;
}

std::ostream& operator<<(std::ostream& os, const Event& event)
{
    if(event.type == Event::Type::SITE)
        os << "S(" << event.site->index << ", " << event.y << ")";
    else
        os << "C(" << event.arc << ", " << event.y << ", " << event.point << ")";
    return os;
}

