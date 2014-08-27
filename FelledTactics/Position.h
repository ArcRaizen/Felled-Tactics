#ifndef POSITION_S
#define POSITION_S
#include <math.h>

struct Position
{
	int x;
	int y;
	Position(){ x = 0;	y = 0; }
	Position(int _x, int _y){ x = _x;	y = _y; }
	bool operator==(const Position& other)
	{
		return x == other.x && y == other.y;
	}
	bool operator!=(const Position& other)
	{
		return x != other.x || y != other.y;
	}
	Position operator+=(const Position other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	Position operator+=(const D3DXVECTOR3 other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	Position operator*(float scalar) const
	{
		return Position(x * scalar, y * scalar);
	}
	Position operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}
	Position operator+(const Position& other) const
	{
		return Position(x + other.x, y + other.y);
	}
	Position operator-(const Position& other) const
	{
		return Position(x - other.x, y - other.y);
	}
	float operator-=(const Position& other)
	{
		return (abs(other.x - x) + abs(other.y - y));
	}
	int DistanceTo(const Position& other)
	{
		return (abs(other.x - x) + abs(other.y - y));
	}
	void Reset()
	{
		x = y = -1;
	}
};
#endif

/*----------------------------------------------------
|                                                    |
|                                                    |
|                                                    |
|                                                    |
|                                                    |		
|			DO NOT WRITE IN THIS SPACE				 |
|                                                    |
|                                                    |
|                                                    |
|                                                    |
|                                                    |
-----------------------------------------------------*/