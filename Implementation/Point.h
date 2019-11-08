#pragma once
#include "definitions.h"

class Point
{
private:
public:
	int x, y;
	Point(int x, int y);
	Point(std::initializer_list<int> list);
	Point();
	Point operator+(const Point & right);

	~Point();
};
