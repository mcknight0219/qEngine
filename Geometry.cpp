#include "Geometry.h"

// Deriving plane from triangle
// see paper http://fabiensanglard.net/doom3_documentation/37729-293751.pdf
Plane::Plane(Vec3 n1, Vec3 n2, Vec3 n3)
{
    Vec3 d1 = n2 - n1;
    Vec3 d2 = n3 - n1;
    Vec3 n = d2.CrossProduct(d1);
    n = n.Normalize();
    dist = -(n[0] * n1[0] + n[1] * n1[1] + n[2] * n1[2]);
}

void Plane::operator=(const Plane& other)
{
	this->normal = other.normal;
	this->dist = other.dist;
}

// intersect is intact if not cross
clipping_t Plane::Clip(const Line l, Vec3& intersect)
{
    // find a point on plane
    Vec3 p = normal.Scale(sqrt(dist));
    Vec3 q1 = l.ends[0];
    Vec3 q2 = l.ends[1];

    float d1 = (q1 - p).DotProduct(normal);
    float d2 = (q2 - p).DotProduct(normal);

    if( (d1 >= 0 && d2 > MI_EPSILON) || (d1 > MI_EPSILON && d2 >= 0) )
        return CLIP_IN;

    if( (d1 <= 0 && d2 < -MI_EPSILON) || (d1 < -MI_EPSILON && d2 <= 0) )
        return CLIP_OUT; 

    float t = 0.0f;
    // q1 lies inside and q2 lies outside
    if( d1 > MI_EPSILON && d2 < -MI_EPSILON ) {
        // calculate t
        t = d1 / (d1 - d2);
    } else if( d1 < -MI_EPSILON && d2 > MI_EPSILON ) {
        t = d1 / (d1 - d2);
    }
    intersect = q1 + (q2 - q1).Scale(t);
    return CLIP_CROSS;
}

int Plane::Side(const Vec3 p) const
{
	float d = p.DotProduct(normal);
	float diff = abs(d - dist);
	if( diff < MI_EPSILON ) {
		return PS_ON;
	}

	return d - diff > 0 ? PS_OUT : PS_IN;
}



int BBox::Side(const Plane p) const
{
    int sign1 = p.Side(min_);
    int sign2 = p.Side(max_);
    if( (sign1 & 0x7FFFFFFF) != (sign2 & 0x7FFFFFFF) ) {
        return 0;
    }

    return sign1 > 0 ? 1 : 2; 
}

bool BBox::Contain(const BBox b)
{
    if( min_ == b.min_ ) {
        if( max_ == b.max_ ) {
            return true;    // box contains itself
        }
        if( max_ < b.max_ ) {
            return true;
        }
    } else if( min_ < b.min_ ) {
        if( b.max_ < max_ ) {
            return true;
        }
    }

    return false;
}

// Generate the vertex list for box
std::vector<Vec3> BBox::GetVertex() const
{
    std::vector<Vec3> vert;
    vert.reserve(8 * 3);

    vert.push_back( min_ );
    vert.push_back( Vec3(max_[0], min_[1], min_[2]) );

	vert.push_back( Vec3(max_[0], min_[1], min_[2]) );
    vert.push_back( Vec3(max_[0], max_[1], min_[2]) );

	vert.push_back( Vec3(max_[0], max_[1], min_[2]) );
    vert.push_back( Vec3(min_[0], max_[1], min_[2]) );

	vert.push_back( min_ );
    vert.push_back( Vec3(min_[0], max_[1], min_[2]) );

	vert.push_back( Vec3(min_[0], min_[1], max_[2]) );
	vert.push_back( Vec3(min_[0], max_[1], max_[2]) );

	vert.push_back( Vec3(min_[0], max_[1], max_[2]) );
    vert.push_back( max_ );

	vert.push_back( max_ );
    vert.push_back( Vec3(max_[0], min_[1], max_[2]) );

	vert.push_back( Vec3(max_[0], min_[1], max_[2]) );
	vert.push_back( Vec3(min_[0], min_[1], max_[2]) );

	vert.push_back( max_ );
	vert.push_back( Vec3(max_[0], max_[1], min_[2]) );

	vert.push_back( Vec3(min_[0], min_[1], max_[2]) );
	vert.push_back( min_);

	vert.push_back( Vec3(max_[0], min_[1], min_[2]) );
	vert.push_back( Vec3(max_[0], min_[1], max_[2]) );

	vert.push_back( Vec3(min_[0], max_[1], min_[2]) );
	vert.push_back( Vec3(min_[0], max_[1], max_[2]) );

    return vert;
}


void Poly::operator=(const Poly& other)
{
	this->vert = other.vert;
	this->numVert = this->vert.size();
}


// Return the clipped polygon by the plane.
// Return an empty polygon if whole clipped outside
// and itself if whole clipped inside. 
Poly Poly::Clip(const Plane p) const
{
    // Find p first
    Vec3 n = p.GetNormal();
    Vec3 q = n.Scale(sqrt(p.GetDist()));

    float pdot = 0;
    float idot = n.DotProduct(vert[0] - q);
    Poly inside;

    for( int i = 1; i < numVert; ++i ) {
        float dot = n.DotProduct(vert[i] - q);
        // Have a cross 
        if( pdot * idot < -MI_EPSILON ) {
			Vec3 intersect = vert[i-1] + (vert[i] - vert[i-1]).Scale(pdot / (pdot - dot));
            inside.Add(intersect);
        }
        // inside
        if( dot > MI_EPSILON ) {
            inside.Add(vert[i]);
        }
        pdot = dot;
    }
    // for line sect Qn-1,Q0
    if( pdot * idot < -MI_EPSILON ) {
		Vec3 intersect = vert[numVert - 1] + (vert[0] - vert[numVert - 1]).Scale(pdot / (pdot - idot));
        inside.Add(intersect);
    }  

    return inside;
}



Frustum::Frustum(float zNear, float zFar, float viewX, float viewY)
{
    // All normals point to the inside of pyramid
    // Near plane
    side[0] = Plane( Vec3(0, 0, -1), zNear );
    // Far plane
    side[1] = Plane( Vec3(0, 0, 1), zFar );
    // Top plane
    side[2] = Plane( Vec3(0, -cos(viewY / 2), -sin(viewY / 2)), 0 );
    // Bottom plane
    side[3] = Plane( Vec3(0, cos(viewY / 2), sin(viewY / 2)), 0 );
    // Left plane
    side[4] = Plane( Vec3(cos(viewX / 2), 0, -sin(viewX / 2)), 0 );
    // Right plane
    side[5] = Plane( Vec3(-cos(viewX / 2), 0, -sin(viewX / 2)), 0 );
}

// Clip polygon against viewing frustum.
Poly Frustum::ClipPoly(Poly p)
{
    // p against each plane of frustum utilizing previous clipping result
    if( p.Size() == 0 )
        return p;
        
    for( int i = 0; i < 6; ++i ) {
        Plane pl = side[i];
        p = p.Clip(pl);
        // p is totally clipped out, 
        if( p.Size() == 0 ) {
            break;
        }
    }

    return p;
}

// Quickly check if bounding box is culled
// Just check box against each plane
bool Frustum::ClipBBox(const BBox b) const
{
    Vec3 vmin = b.GetMin();
    Vec3 vmax = b.GetMax();    

    bool inside = false;
    for( int i = 0; i < 6; ++i ) {
        int d1 = side[i].Side(vmin);
        int d2 = side[i].Side(vmax);
        if( d1 * d2 < -MI_EPSILON ) {
            // Crossed one plane, surely inside frustum
            return true;
        } 

        inside = inside || ( d1 > MI_EPSILON && d2 > MI_EPSILON );       
    }

    // Culled if b falls outside of payramid
    return inside;
}

