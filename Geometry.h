/*
 * ===============================================================
 *
 * A group of geometric structures and related algorithms
 * Reference paper: http://fabiensanglard.net/polygon_codec/clippingdocument/Clipping.pdf
 * 
 *================================================================
 */
#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "Math.h"
#include <vector>

#ifdef __linux__
    #include <stdlib.h>
#endif

/**
 * Line segment definition
 */
struct Line
{
	Line(const Vec3 a, const Vec3 b) { ends[0] = a; ends[1] = b; }
    float Length() const;

	Vec3 ends[2];
};

inline float Line::Length() const
{
    float x = ends[0][0] - ends[1][0];
    float y = ends[0][1] - ends[1][1];
    float z = ends[0][2] - ends[1][2];
    return x * x + y * y + z * z;
}


// Clipping result
enum clipping_t { CLIP_IN, CLIP_OUT, CLIP_CROSS };
enum {PS_OUT, PS_IN, PS_ON};

/**
 * Plane definition
 */
class Plane
{

public :
				Plane() : normal(Vec3(0)), dist(0) {}
	            Plane(Vec3 n, float d) : normal(n), dist(d) {}
    explicit    Plane(Vec3 n1, Vec3 n2, Vec3 n3);
				
	void		operator=(const Plane& other);
public:
    Vec3        GetNormal() const { return normal; }
    float       GetDist() const { return dist; }
	clipping_t 	Clip(const Line l, Vec3& intersect);
	int 	    Side(const Vec3 p) const;						// 0 on, 1 inside, 2 outside						

private:
	Vec3	normal;	// Normal vector
	float	dist;	// distance from origin
};



/**
 * Axis Aligned Bounding Box (AABB)
 */
class BBox
{
public:
	BBox(const Vec3 vmin, const Vec3 vmax) : min_(vmin), max_(vmax) { 
		Vec3 diff = max_ - min_;
		volumn = abs(diff[0]) * abs(diff[1]) * abs(diff[2]);
	}

	float               Volumn() const; 
    int                 Side(const Plane p) const;        // 0 intersect, 1 inside, 2 outside   
    bool                Contain(const BBox b);      // if b is contained inside
    Vec3                GetMin() const { return min_; }
    Vec3                GetMax() const { return max_; }
    // Those two are for debugging purpose only
    std::vector<Vec3>   GetVertex() const;

private:
	Vec3 min_;
	Vec3 max_;
	float volumn;
};

inline float BBox::Volumn() const
{
	return volumn;
}

/**
 * Simple implementation for polygon.
 */
class Poly
{
public:
    // Assume input is in certain winding order, be it CW or CCW
    Poly(const std::vector<Vec3>& vertices) : vert(vertices), numVert(vert.size()) {}
	Poly() { numVert = 0; }

	void operator=(const Poly& other);


    Poly	Clip(const Plane p) const;
    void    Add(const Vec3 v) { vert.push_back(v); }
    int     Size() const { return numVert; }

private:
    std::vector<Vec3> vert;
    int numVert;
};


/**
 * Viewing pyramid used for culling, not for projection
 */
class Frustum
{
public:

    Frustum(float zNear, float zFar, float viewX, float viewY);
    Poly	ClipPoly(Poly p);
    bool    ClipBBox(const BBox b) const;

private:
    // near, far, top, down, left, right
    Plane side[6];
};


#endif
