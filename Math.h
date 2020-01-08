#pragma once

#include <SDL/SDL.h>
#include <cmath>

#define PI										3.141592654

#define BULLET_DRAG								0.99f
#define BULLET_DIAMETER							12.5f
#define BULLET_MUZZLE_VELOCITY					25.0f	
#define BULLET_KINETIC_ENERGY_CUTOFF			10.0f

#define MAX_VERTICES							64

enum class lateral
{
	LEFT,
	RIGHT
};

enum class cardinal
{
	NORTH,
	SOUTH,
	WEST,
	EAST
};

template <class T>
void swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}


struct screenCoord
{
	int x = 0;
	int y = 0;

	screenCoord()
	{
	}

	screenCoord(int x_, int y_):
		x(x_), y(y_)
	{
	}
};


struct boundingBoxScreen
{
	screenCoord topLeft;
	screenCoord bottomRight;

	boundingBoxScreen()
	{
		topLeft.x		= 0;
		topLeft.y		= 0;
		bottomRight.x	= 0;
		bottomRight.y	= 0;
	}

	boundingBoxScreen(const int& x1, const int& y1, const int& x2, const int& y2)
	{
		topLeft.x		= x1;
		topLeft.y		= y1;
		bottomRight.x	= x2;
		bottomRight.y	= y2;

		if (bottomRight.x < topLeft.x) { swap(topLeft.x, bottomRight.x); }
		if (bottomRight.y < topLeft.y) { swap(topLeft.y, bottomRight.y); }
	}

	boundingBoxScreen(const screenCoord& tl, const screenCoord& br)
	{
		topLeft		= tl;
		bottomRight = br;

		if (bottomRight.x < topLeft.x) { swap(topLeft.x, bottomRight.x); }
		if (bottomRight.y < topLeft.y) { swap(topLeft.y, bottomRight.y); }
	}

	inline void clipToScreen(const int& w, const int& h)
	{
		if (topLeft.x < 0) { topLeft.x = 0; }
		if (topLeft.y < 0) { topLeft.y = 0; }
		if (bottomRight.x >= w) { bottomRight.x = w - 1; }
		if (bottomRight.y >= h) { bottomRight.y = h - 1; }
		if ((topLeft.x < 0 && bottomRight.x < 0)	||
			(topLeft.y < 0 && bottomRight.y < 0)	||
			(topLeft.x >= w && bottomRight.x >= w)	||
			(topLeft.y >= h && bottomRight.y >= h)		)
		{
			topLeft.x		= 0;
			topLeft.y		= 0;
			bottomRight.x	= 0;
			bottomRight.y	= 0;
		}
	}
};


struct vect2
{
	double x = 0.0f;
	double y = 0.0f;

	vect2()
	{
	}

	vect2(double x_, double y_):
		x(x_), y(y_)
	{
	}

	inline vect2	operator +	(const vect2& v)	{ return vect2(this->x + v.x, this->y + v.y);	}
	inline vect2	operator -	(const vect2& v)	{ return vect2(this->x - v.x, this->y - v.y);	}
	inline vect2	operator += (const vect2& v)	{ return vect2(this->x += v.x, this->y += v.y); }
	inline vect2	operator -= (const vect2& v)	{ return vect2(this->x -= v.x, this->y -= v.y); }
	inline double	operator *	(const vect2& v)	{ return this->x * v.x + this->y * v.y;			}
	inline vect2	operator *	(const double& s)	{ return vect2(this->x * s, this->y * s);		}
	inline vect2	operator *=	(const double& s)	{ return vect2(this->x *= s, this->y *= s);		}
	inline vect2	operator /	(const double& s)	{ return vect2(this->x / s, this->y / s);		}
	inline vect2	operator /=	(const double& s)	{ return vect2(this->x /= s, this->y /= s);		}
	
	inline double len() { return sqrt(this->x * this->x + this->y * this->y); }
	inline double lenSquared() { return this->x * this->x + this->y * this->y; }
	inline vect2 norm() { return *this / this->len(); }
	inline vect2 rot(const double& a) { return vect2(this->x * cos(a) - this->y * sin(a), this->y * cos(a) + this->x * sin(a)); }
	inline screenCoord onScreen(const double& s) { return screenCoord( (int)(this->x * s), (int)(this->y * s)); }

};


struct polygon
{
	unsigned int	n						= 0;
	vect2			vertices[MAX_VERTICES]	= { vect2(0.0f, 0.0f) };
	Uint32			colour					= 0;
	polygon*		leftChild				= nullptr;
	polygon*		rightChild				= nullptr;
};


struct edge
{
	vect2 startP;
	vect2 endP;

	vect2 normal;

	edge()
	{
		startP	= vect2(0.0f, 0.0f);
		endP	= vect2(0.0f, 0.0f);
		normal	= vect2(0.0f, 0.0f);
	}

	edge(const vect2& p, const vect2& q)
	{
		startP	= p;
		endP	= q;
		normal	= (endP - startP).norm().rot(PI * 0.5);
	}

	void flip()
	{
		//swap(startP, endP);
		//normal = (endP - startP).norm().rot(PI * 0.5);
		normal *= -1.0f;
	}

	void move(vect2 m)
	{
		startP	+= m;
		endP	+= m;
	}
};


struct triangle2
{
	screenCoord a;
	screenCoord b;
	screenCoord c;
};



