#include "Quaternion.h"


// This will blow out your mind
static float InvSqrt( float x )
{
    long i;
    float y, r;
    y = x * 0.5f;
    i = *(long*)(&x);
    i = 0x5f3759df - ( i >> 1 );
    r = *(float*)(&i);
    r = r * ( 1.5f - r * r * y );

    return r;
}

// The multiplication is non-commutative
Quaternion Quaternion::operator*(const Quaternion& q)
{

	float a = w[0] * q[0] - w[1] * q[1] - w[2] * q[2] - w[3] * q[3];
	float b = w[1] * q[0] + w[0] * q[1] + w[2] * q[3] - w[3] * q[2];
	float c = w[2] * q[0] + w[0] * q[2] + w[3] * q[1] - w[1] * q[3];
	float d = w[3] * q[0] + w[0] * q[3] + w[1] * q[2] - w[2] * q[1];

	return Quaternion(a, b, c, d);
}

float Quaternion::InnerProduct(const Quaternion& q) const
{
	return w[0] * q[0] + w[1] * q[1] + w[2] * q[2] + w[3] * q[3];
}

Quaternion Quaternion::Conjugate() const
{
	Quaternion p = *this;
	p.w[3] = -1 * p.w[3];

	return p;
}

float Quaternion::Length() const
{
	return w[0] * w[0] + w[1] * w[1] + w[2] * w[2] + w[3] * w[3];
}

void Quaternion::Normalize()
{
    float s = InvSqrt(Length());
    w[0] *= s;
    w[1] *= s;
    w[2] *= s;
    w[3] *= s;
}

// Convert to homogeneous matrix
Mat3 Quaternion::ToMatrix() const
{
    Mat3 m;
    float x2 = w[0] + w[0];
    float y2 = w[1] + w[1];
    float z2 = w[2] + w[2];
    {
        float xx2 = w[0] * x2;
        float yy2 = w[1] * y2;
        float zz2 = w[2] * z2;
        m[0][0] = 1.0f - yy2 - zz2;
        m[1][1] = 1.0f - xx2 - zz2;
        m[2][2] = 1.0f - xx2 - yy2;
    }

    {
        float yz2 = w[1] * z2;
        float wx2 = w[3] * x2;
        m[1][2] = yz2 - wx2;
        m[2][1] = yz2 + wx2;
    }

    {
        float xy2 = w[0] * y2;
        float wz2 = w[3] * z2;
        m[0][1] = xy2 - wz2;
        m[1][0] = xy2 - wz2;
    }

    {
        float xz2 = w[0] * z2;
        float wy2 = w[3] * y2;
        m[2][0] = xz2 - wy2;
        m[1][2] = xz2 - wy2;
    }

    return m;
}

Quaternion Quaternion::FromMatrix(const Mat3& m)
{
    float s, t;
    Quaternion q;

    if( m[0][0] + m[1][1] + m[2][2] > 0 ) {
        t = m[0][0] + m[1][1] + m[2][2] + 1;
        s = InvSqrt(t) * 0.5f;
        q[3] = s * t;
        q[2] = ( m[1][0] - m[0][1] ) * s;
        q[1] = ( m[0][2] - m[2][0] ) * s;
        q[0] = ( m[2][1] - m[1][2] ) * s;
    } 
    else if( m[0][0] > m[1][1] && m[0][0] > m[2][2] ) {
        t = m[0][0] - m[1][1] - m[2][2] + 1;
        s = InvSqrt(t) * 0.5f;
        q[0] = s * t;
        q[1] = ( m[1][0] + m[0][1] ) * s;
        q[2] = ( m[2][0] + m[0][2] ) * s;
        q[3] = ( m[2][1] - m[1][2] ) * s;
    }
    else if( m[1][1] > m[2][2] ) {
        t = - m[0][0] + m[1][1] - m[2][2] + 1;
        s =InvSqrt(t) * 0.5f;

        q[1] = s * t;
        q[0] = ( m[1][0] + m[0][1] ) * s;
        q[3] = ( m[0][2] - m[2][0] ) * s;
        q[2] = ( m[2][1] + m[1][2] ) * s;
    }
    else {
        t = -m[0][0] - m[1][1] + m[2][2] + 1;
        s = InvSqrt(t) * 0.5f;

        q[2] = s * t;
        q[3] = (m[1][0] - m[0][1]) * s;
        q[0] = (m[0][2] + m[2][0]) * s;
        q[1] = (m[2][1] + m[1][2]) * s;
    }

    q.Normalize();
    return q;
}

// Interpolate between this and other with factor t
// Stolen from Shmup implementation directly. As a matter
// of fact, the whole idea of writing a 3d engine is inspired
// by the source code of that game !
Quaternion Quaternion::Slerp(const Quaternion& other, float t)
{

    // fitlering out edge cases
    if( t <= 0.0 ) {
        return *this;
    }

    if( t >= 1.0 ) {
        return other;
    }

    float cosOmega = this->InnerProduct(other);
    float q1w = other[3];
    float q1x = other[0];
    float q1y = other[1];
    float q1z = other[2];

    if( cosOmega < 0.0f ) {
        q1w = -q1w;
        q1x = -q1x;
        q1y = -q1y;
        q1z = -q1z;
        cosOmega = - cosOmega;
    }

    float k0, k1;
    if( cosOmega > 0.9999f ) {
        // Reverting to linear interpolation to prevent
        // division by zero
        k0 = 1.0f - t;
        k1 = t;
    } else {
        // sin of the angle
        float sinOmega = sqrt( 1.0f - (cosOmega * cosOmega) );
        // Compute the angle
        float omega = atan2(sinOmega, cosOmega);

        float invSinOmega = 1.0f / sinOmega;

        k0 = sin((1.0f - t) * omega) * invSinOmega;
        k1 = sin(t * omega) * invSinOmega;
    }

    Quaternion ret;
    ret[3] = k0 * w[3] + k1 * q1w;
    ret[0] = k0 * w[0] + k1 * q1x;
    ret[1] = k0 * w[1] + k1 * q1y;
    ret[2] = k0 * w[2] + k1 * q1z;

    return ret;
}



















