#include "definitions.h"
#include "Point.h"

Point::Point(int x_, int y_) : x(x_), y(y_) {}

// Point::Point(std::initializer_list<int> list) {
// 	this->x = *list.begin();
// 	this->y = *(list.end() - 1);
// }

Point::Point() {}
Point::~Point()
{
}

Point Point::operator+(const Point & right) {
	int newx = this->x + right.x;
	int newy = this->y + right.y;
	return Point(newx, newy);
}