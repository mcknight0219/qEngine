#ifndef _QUATERNION_H
#define _QUATERNION_H

#include <assert.h>
#include "Math.h"

class Quaternion
{
public:
						Quaternion();
						Quaternion(float x, float y, float z, float q);

	Quaternion 			operator*(const Quaternion& q);		// Multiplication
	Quaternion& 		operator=(Quaternion q);
	float				InnerProduct(const Quaternion& q) const;
	Quaternion 			Conjugate() const;
	float				Length() const;
    void        		Normalize();
    Quaternion 			Slerp(const Quaternion&, float);
    Mat3        		ToMatrix() const;
    static Quaternion 	FromMatrix(const Mat3& m);

    float&      		operator[]( int i );
    const float&		operator[]( int i ) const;

private:
	Vec4				w;
};

inline Quaternion::Quaternion()
{
	// Vec4 is initialized to 0
}

inline Quaternion::Quaternion(float x, float y, float z, float q)
{
	w = Vec4(x, y, z, q);
}

inline Quaternion& Quaternion::operator=(Quaternion p)
{
	w[0] = p[0];
	w[1] = p[1];
	w[2] = p[2];
	w[3] = p[3];

	return *this;
} 

inline const float& Quaternion::operator[]( int i ) const
{
	assert( i >= 0 && i <= 3 );
    return w[i];
}

inline float& Quaternion::operator[]( int i ) 
{
	assert( i >= 0 && i <= 3 );
	return w[i];
}

#endif
