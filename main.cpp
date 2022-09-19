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

 // STL
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
// opengl
#include <GL/glew.h> // Include the GLEW header file
#include <glut.h> // Include the GLUT header file
// My includes
#include "recursiveVoronoi.h"
#include <windows.h>
#include <stack>

//constexpr float WINDOW_WIDTH = 600.0f;
//constexpr float WINDOW_HEIGHT = 600.0f;
//constexpr float POINT_RADIUS = 0.005f;
//double EPSILON = std::numeric_limits<double>::epsilon();
#define EPSILON std::numeric_limits<double>::epsilon()
//constexpr float OFFSET = 1.0f;
double w = 600;
double ww = 800;
void display(void);
void onEF(int n);
void reshape(int width, int height);
std::vector<Vector2> corpos = { Vector2(0.0, w/2),Vector2(w / 2, 0.0),Vector2(w, w / 2) ,Vector2(w / 2, w) };
DiscreteGlobalGrid discreteGlobalGrid;
/*
std::vector<Vector2> generatePoints(Box box,int nbPoints);
// Generate points
//std::vector<Vector2> points;

std::vector<Vector2> points = generatePoints(Box{ corpos, -w, -w, 2 * w, 2 * w },10);

std::vector<Vector2> generatePoints(Box box,int nbPoints)
{
	uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::cout << "seed: " << seed << '\n';
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	std::vector<Vector2> points;
	while (points.size() < nbPoints)
	{
		Vector2 pos(w * (double)rand() / (double)RAND_MAX, w * (double)rand() / (double)RAND_MAX);
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
	for (size_t i = 0; i < box.corpos.size(); i++)
	{
		points.push_back(box.corpos[i]);
	}
	return points;
}

void drawPoint()
{
	//sf::CircleShape shape(POINT_RADIUS);
	//shape.setPosition(sf::Vector2f(point.x - POINT_RADIUS, 1 - point.y - POINT_RADIUS));
	//shape.setFillColor(color);
	//window.draw(shape);
}

void drawEdge(Vector2 origin, Vector2 destination)
{
	glBegin(GL_LINES);
	glVertex2f(-1 + 2 * (origin.x + (ww - w) / 2) / ww, -1 + 2 * (origin.y + (ww - w) / 2) / ww);
	glVertex2f(-1 + 2 * (destination.x + (ww - w) / 2) / ww, -1 + 2 * (destination.y + (ww - w) / 2) / ww);
	glEnd();
}

void drawPoints(VoronoiDiagram& diagram)
{

	glBegin(GL_QUADS);
	for (std::size_t i = 0; i < diagram.getNbSites(); ++i)
	{
		VoronoiDiagram::Site *p = diagram.getSite(i);
		glVertex2f(-1 + 2 * (p->point.x + (ww - w) / 2) / ww - 0.01, -1 + 2 * (p->point.y + (ww - w) / 2) / ww - 0.01);
		glVertex2f(-1 + 2 * (p->point.x + (ww - w) / 2) / ww + 0.01, -1 + 2 * (p->point.y + (ww - w) / 2) / ww - 0.01);
		glVertex2f(-1 + 2 * (p->point.x + (ww - w) / 2) / ww + 0.01, -1 + 2 * (p->point.y + (ww - w) / 2) / ww + 0.01);
		glVertex2f(-1 + 2 * (p->point.x + (ww - w) / 2) / ww - 0.01, -1 + 2 * (p->point.y + (ww - w) / 2) / ww + 0.01);
	}
	glEnd();
	//for (std::size_t i = 0; i < diagram.getNbSites(); ++i)
	//    drawPoint(window, diagram.getSite(i)->point, sf::Color(100, 250, 50));
}
int siteindex = 0;
void drawBoundpos(VoronoiDiagram& diagram)
{
	std::vector<int> boundpos = diagram.getBoundpos();
	glBegin(GL_QUADS);
	for (std::size_t i = 0; i < boundpos.size(); ++i)
	{
		VoronoiDiagram::Site *p = diagram.getSite(boundpos[i]);
		glVertex2f(-1 + 2 * (p->point.x + (ww - w) / 2) / ww - 0.01, -1 + 2 * (p->point.y + (ww - w) / 2) / ww - 0.01);
		glVertex2f(-1 + 2 * (p->point.x + (ww - w) / 2) / ww + 0.01, -1 + 2 * (p->point.y + (ww - w) / 2) / ww - 0.01);
		glVertex2f(-1 + 2 * (p->point.x + (ww - w) / 2) / ww + 0.01, -1 + 2 * (p->point.y + (ww - w) / 2) / ww + 0.01);
		glVertex2f(-1 + 2 * (p->point.x + (ww - w) / 2) / ww - 0.01, -1 + 2 * (p->point.y + (ww - w) / 2) / ww + 0.01);
	}
	glEnd();

}
void drawTIN(VoronoiDiagram& diagram)
{
	std::stack<int> q;
	std::vector<bool> visited;
	for (size_t i = 0; i < diagram.getNbSites(); i++)
	{
		visited.push_back(false);
	}
	q.push(0);
	while (!q.empty())
	{
		int id = q.top();
		q.pop();
		const VoronoiDiagram::Site* site = diagram.getSite(id);
		Vector2 center = site->point;
		visited[id] = true;
		VoronoiDiagram::HalfEdge* halfEdge = site->face->outerComponent;
		VoronoiDiagram::HalfEdge* start = halfEdge;
		while (true)
		{
			if (halfEdge->twin != nullptr)
			{
				int id2 = halfEdge->twin->incidentFace->site->index;
				if (!visited[id2])
				{
					q.push(id2);
					//deal something
					drawEdge(site->point, diagram.getSite(id2)->point);
				}
			}
			halfEdge = halfEdge->next;
			if (halfEdge == start)
				break;
		}
	}

}
void drawDiagram(VoronoiDiagram& diagram)
{
	//std::size_t i = (siteindex++) % diagram.getNbSites();
	
	for (std::size_t i = 0; i < diagram.getNbSites(); ++i)
	{
		//Sleep(1000);
		const VoronoiDiagram::Site* site = diagram.getSite(i);
		Vector2 center = site->point;
		VoronoiDiagram::Face* face = site->face;
		VoronoiDiagram::HalfEdge* halfEdge = face->outerComponent;
		if (halfEdge == nullptr)
			continue;
		while (halfEdge->prev != nullptr)
		{
			halfEdge = halfEdge->prev;
			if (halfEdge == face->outerComponent)
				break;
		}
		VoronoiDiagram::HalfEdge* start = halfEdge;
		while (halfEdge != nullptr && (unsigned int)halfEdge != 0xDDDDDDDD)
		{
			if ((unsigned int)halfEdge->origin != 0xDDDDDDDD && halfEdge->origin != nullptr && halfEdge->destination != nullptr)
			{
				Vector2 origin = (halfEdge->origin->point - center) * OFFSET + center;
				Vector2 destination = (halfEdge->destination->point - center) * OFFSET + center;
				drawEdge(origin, destination);
			}
			halfEdge = halfEdge->next;
			if (halfEdge == start || halfEdge == nullptr)
				break;
		}
	}
	//siteindex = (siteindex++) % diagram.getNbSites();
}

VoronoiDiagram generateRandomDiagram(std::size_t nbPoints)
{
	//std::vector<Vector2> corpos = { Vector2(0.0, 0.0),Vector2(w, 0.0) ,Vector2(w, w),Vector2(0.0, w) };
	// Generate points
	//if (nbPoints > 128)
	//{
	//	int a = 0;
	//	std::cin >> a;
	//	points.clear();
	//	points = generatePoints(Box{ corpos, 0.0, 0.0, w, w }, nbPoints);
	//	
	//}
	

	// Construct diagram
	FortuneAlgorithm algorithm(points);
	auto start = std::chrono::steady_clock::now();
	algorithm.construct();
	auto duration = std::chrono::steady_clock::now() - start;
	std::cout << "construction: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';

	// Bound the diagram
	start = std::chrono::steady_clock::now();
	
	algorithm.bound(Box{ corpos, 0.0, 0.0, 0.0, 0.0 }); // Take the bounding box slightly bigger than the intersection box
	duration = std::chrono::steady_clock::now() - start;
	std::cout << "bounding: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
	VoronoiDiagram diagram = algorithm.getDiagram();
	//diagram.setBoundpos(algorithm.getBoundpos());

	// Intersect the diagram with a box
	start = std::chrono::steady_clock::now();
	bool valid = diagram.intersect(Box{ corpos, 0.0, 0.0, 0.0, 0.0 });
	duration = std::chrono::steady_clock::now() - start;
	std::cout << "intersection: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
	if (!valid)
		throw std::runtime_error("An error occured in the box intersection algorithm");

	return diagram;
}
std::size_t nbPoints = 130;
VoronoiDiagram diagram = generateRandomDiagram(nbPoints);
*/

void drawPoints()
{
	std::vector<Vector2> points = discreteGlobalGrid.points;
	glBegin(GL_QUADS);
	for (std::size_t i = 0; i < points.size(); ++i)
	{
		Vector2 p = points[i];
		glVertex2f(-1 + 2 * (p.x + (ww - w) / 2) / ww - 0.01, -1 + 2 * (p.y + (ww - w) / 2) / ww - 0.01);
		glVertex2f(-1 + 2 * (p.x + (ww - w) / 2) / ww + 0.01, -1 + 2 * (p.y + (ww - w) / 2) / ww - 0.01);
		glVertex2f(-1 + 2 * (p.x + (ww - w) / 2) / ww + 0.01, -1 + 2 * (p.y + (ww - w) / 2) / ww + 0.01);
		glVertex2f(-1 + 2 * (p.x + (ww - w) / 2) / ww - 0.01, -1 + 2 * (p.y + (ww - w) / 2) / ww + 0.01);
	}
	glEnd();
}
void drawOtherPoints()
{
	std::vector<Vector2> points = discreteGlobalGrid.otherTestPoints;
	glBegin(GL_QUADS);
	for (std::size_t i = 0; i < points.size(); ++i)
	{
		Vector2 p = points[i];
		glVertex2f(-1 + 2 * (p.x + (ww - w) / 2) / ww - 0.01, -1 + 2 * (p.y + (ww - w) / 2) / ww - 0.01);
		glVertex2f(-1 + 2 * (p.x + (ww - w) / 2) / ww + 0.01, -1 + 2 * (p.y + (ww - w) / 2) / ww - 0.01);
		glVertex2f(-1 + 2 * (p.x + (ww - w) / 2) / ww + 0.01, -1 + 2 * (p.y + (ww - w) / 2) / ww + 0.01);
		glVertex2f(-1 + 2 * (p.x + (ww - w) / 2) / ww - 0.01, -1 + 2 * (p.y + (ww - w) / 2) / ww + 0.01);
	}
	glEnd();
}
void drawDiagram()
{
	//drawPoints();
	//drawOtherPoints();
	std::vector<Edge> edges = discreteGlobalGrid.edges;
	int dcount = edges.size();
	for (size_t i = 0; i < dcount; i++)
	{
		Edge e = edges[i];
		std::vector<Vector2> origin = e.origin;
		std::vector<Vector2> destination = e.destination;
		int vcount = origin.size();
		for (size_t j = 0; j < vcount; j++)
		{
			glBegin(GL_LINES);
			glVertex2f(-1 + 2 * (origin[j].x + (ww - w) / 2) / ww, -1 + 2 * (origin[j].y + (ww - w) / 2) / ww);
			glVertex2f(-1 + 2 * (destination[j].x + (ww - w) / 2) / ww, -1 + 2 * (destination[j].y + (ww - w) / 2) / ww);
			glEnd();
		}
	}
}

void drawVoronoi()
{
	//std::cout << nbPoints << std::endl;
	//VoronoiDiagram diagram = generateRandomDiagram(nbPoints);
	//if (nbPoints > 128)
	//{
	//	nbPoints--;
	//}
	//points.clear();
	//points = diagram.getCentroidPos(corpos.size());
	
	//diagram = generateRandomDiagram(nbPoints);
	//drawBoundpos(diagram);
	//drawPoints(diagram);
	drawDiagram();
	//drawTIN(diagram);
}

void display(void)
{
	std::cout << "display\n";
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations  
	glTranslatef(0.0f, 0.0f, -5.0f);

	glFlush();
}
void onEF(int n)
{

	glutTimerFunc(20, onEF, 0);
	glClear(GL_COLOR_BUFFER_BIT);//Clear the screen
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f); // Clear the background of our window to red  

	drawVoronoi();
	glutSwapBuffers();
	//Draw everything to the screen
}

void reshape(int width, int height)
{

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
	glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
	glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
	gluPerspective(22.5, (GLfloat)width / (GLfloat)height, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
	glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}

double get_angle(double x1, double y1, double x2, double y2, double x3, double y3)
{
	double theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
	if (theta > std::_Pi)
		theta -= 2 * std::_Pi;
	if (theta < -std::_Pi)
		theta += 2 * std::_Pi;

	theta = abs(theta * 180.0 / std::_Pi);
	return theta;
}

int main(int argc, char **argv)
{
	double a = get_angle(373.5030592, 0, 294.2053188, 62.68338888, 338.1511887, 0.750755333);
	double b = get_angle(294.2053188,62.68338888,283.9666317,40.06746694,338.1511887,0.750755333);
	double c = get_angle(283.9666317,40.06746694,282.3138898,0,338.1511887,0.750755333);
	double d = get_angle(282.3138898,0,373.5030592,0,338.1511887,0.750755333);
	recursiveVoronoi *rv = new recursiveVoronoi;
	rv->begin(corpos, 1);
	discreteGlobalGrid = rv->getDiscreteGlobalGrid();

	glutInit(&argc, argv); // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE); // Set up a basic display buffer (only single buffered for now)
	glutInitWindowSize(600, 600); // Set the width and height of the window
	glutInitWindowPosition(100, 100); // Set the position of the window
	glutCreateWindow("first OpenGL Window"); // Set the title for the window

	glutTimerFunc(100, onEF, 0);
	glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering

	glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping

	//glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses
	//glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events

	glutMainLoop(); // Enter GLUT's main loop
	return 0;
}
