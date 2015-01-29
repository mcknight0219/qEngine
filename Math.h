#ifndef _MY_MATH_H
#define _MY_MATH_H

#include <assert.h>
#include <cmath>
#ifdef __linux__
    #include <stdlib.h>
#endif

#ifndef M_PI
	#define M_PI 3.1415926
#endif
#define DEG_TO_RAD	(2 * M_PI / 360.0f)


#ifndef MI_EPSILON
	#define MI_EPSILON 1.0e-6
#endif

/*
==================================================================

4 dimension vector

==================================================================
*/
class Vec4
{
private:
	float vec[4];

public:
					Vec4();
					Vec4(float x, float y, float z, float w);
					Vec4(const Vec4& a);

	const float& 	operator[](int i) const;
	float&			operator[](int i);
	Vec4&			operator=(const Vec4& a);
	Vec4			operator-(const Vec4& a) const;
	Vec4			operator+(const Vec4& a) const;

	Vec4			Normalize();
	Vec4			Scale(float s);
	float			DotProduct(const Vec4& other) const;
};

inline Vec4::Vec4() {
	vec[0] = vec[1] = vec[2] = vec[3] = 0.0f;
}

inline Vec4::Vec4(float x, float y, float z, float w) {
	vec[0] = x; vec[1] = y; vec[2] = z; vec[3] = w;
}

inline Vec4::Vec4(const Vec4& a) {
	vec[0] = a.vec[0]; vec[1] = a.vec[1]; vec[2] = a.vec[2]; vec[3] = a[3];
}

inline float& Vec4::operator[](int i) {
	assert(i >= 0 && i < 4);
	return vec[i];
}

inline const float& Vec4::operator[](int i) const {
	assert(i >= 0 && i < 4);
	return vec[i];
}

inline Vec4& Vec4::operator=(const Vec4& a) {
	vec[0] = a.vec[0]; vec[1] = a.vec[1]; vec[2] = a.vec[2]; vec[3] = a.vec[3];
	return *this;
}

inline Vec4 Vec4::Scale(float s) {
	Vec4 res(vec[0] * s, vec[1] * s, vec[2] * s, vec[3] * s);
	return res;
}

inline Vec4 Vec4::Normalize() {
	float normInv = 1 / sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);
	return Scale(normInv);
}

inline Vec4 Vec4::operator-(const Vec4& a) const {
	Vec4 res = *this;
	res[0] -= a[0]; res[1] -= a[1]; res[2] -= a[2]; res[3] -= a[3];
	return res;	
}

inline Vec4 Vec4::operator+(const Vec4& a) const {
	Vec4 res = *this;
	res[0] += a[0]; res[1] += a[1]; res[2] += a[2]; res[3] += a[3];
	return res;
}

inline float Vec4::DotProduct(const Vec4& other) const
{
	return vec[0] * other[0] + vec[1] * other[1] + vec[2] * other[2] + vec[3] * other[3];
}


/*
===============================================

Vectotr class

===============================================
*/
class Vec3 
{
private:
	float vec[3];
public:
					Vec3();
					Vec3(float x, float y, float z);
					Vec3(float x);
					Vec3(const Vec3& a);

	float&			operator[](int i);
	const float& 	operator[](int i) const;
	Vec3&			operator=(const Vec3& a);
	Vec3			operator-(const Vec3& a) const;
	Vec3			operator+(const Vec3& a) const;
    bool    		operator<(const Vec3& a) const;
    bool    		operator==(const Vec3& a) const;
    bool    		operator!=(const Vec3& a) const;

	void			Zero();
	Vec3			Normalize();
	Vec3			Scale(float s);
	Vec3			CrossProduct(const Vec3& a) const;
	float			DotProduct(const Vec3& a) const;
	bool			IsZero() const;
    Vec4            ToV4() const;
};

inline Vec3::Vec3() {
	vec[0] = vec[1] = vec[2] = 0.0f;
}

inline Vec3::Vec3(float x, float y, float z) {
	vec[0] = x; vec[1] = y; vec[2] = z;
}

inline Vec3::Vec3(float x) {
	vec[0] = x; vec[1] = x; vec[2] = x;
}

inline Vec3::Vec3(const Vec3& a) {
	vec[0] = a.vec[0]; vec[1] = a.vec[1]; vec[2] = a.vec[2];
}

inline float& Vec3::operator[](int i) {
	assert(i >= 0 && i < 3);
	return vec[i];
}

inline const float& Vec3::operator[](int i) const {
	assert(i >= 0 && i < 3);
	return vec[i];
}

inline Vec3& Vec3::operator=(const Vec3& a) {
	vec[0] = a.vec[0]; vec[1] = a.vec[1]; vec[2] = a.vec[2];
	return *this;
}

inline void Vec3::Zero()
{
	vec[0] = vec[1] = vec[2] = 0;
}

inline Vec3 Vec3::Scale(float s) {
	Vec3 res(vec[0] * s, vec[1] * s, vec[2] * s);
	return res;
}

inline Vec3 Vec3::Normalize() {
	float normInv = 1 / sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	return Scale(normInv);
}

inline Vec3 Vec3::operator-(const Vec3& a) const {
	Vec3 res = *this;
	res[0] -= a[0]; res[1] -= a[1]; res[2] -= a[2];
	return res;	
}

inline Vec3 Vec3::operator+(const Vec3& a) const {
	Vec3 res = *this;
	res[0] += a[0]; res[1] += a[1]; res[2] += a[2];
	return res;
}

inline bool Vec3::operator<(const Vec3& a) const {
    return vec[0] < a[0] && vec[1] < a[1] && vec[2] < a[2]; 
}

inline bool Vec3::operator==(const Vec3& a) const {
    return std::abs(vec[0] - a[0]) < MI_EPSILON &&
        std::abs(vec[1] - a[1]) < MI_EPSILON &&
        std::abs(vec[2] - a[2]) < MI_EPSILON;
}

inline bool Vec3::operator!=(const Vec3& a) const {
    return !( *this == a );
}

inline float Vec3::DotProduct(const Vec3& other) const
{
	return vec[0] * other[0] + vec[1] * other[1] + vec[2] * other[2];
}

inline Vec3 Vec3::CrossProduct(const Vec3& other) const
{
	float r_1 = vec[1] * other[2] - vec[2] * other[1];
	float r_2 = vec[2] * other[0] - vec[0] * other[2];
	float r_3 = vec[0] * other[1] - vec[1] * other[0];

	return Vec3(r_1, r_2, r_3);
}

inline bool Vec3::IsZero() const
{
	return	(std::abs(vec[0]) <= MI_EPSILON) && 
			(std::abs(vec[1]) <= MI_EPSILON) &&
			(std::abs(vec[2]) <= MI_EPSILON);
}

// Get homogeneous coordinates
inline Vec4 Vec3::ToV4() const
{
    Vec4 ret(vec[0], vec[1], vec[2], 1);

    return ret;
}

/*
================================================

Matrix are all column major as in OpenGL
Column major means same column occupy consecutive
memory locations

=================================================
*/
class Mat3
{
private:
	Vec3 mat[3];
public:
			Mat3();
			Mat3(float xx, float xy, float xz,
				 float yx, float yy, float yz,
				 float zx, float zy, float zz);
			Mat3(const Vec3 col_1, const Vec3 col_2, const Vec3 col_3);
			Mat3(const Mat3& m);

	Mat3&	operator=(const Mat3& m);
	const Vec3&	operator[](int j) const;
	Vec3&	operator[](int j);
	void	Ident();	// Load identity matrix
	float	Determinant() const ;
	Mat3	Transpose();
	void	TransposeSelf();
	Mat3	Add(const Mat3& m) const;
	Mat3	Sub(const Mat3& m) const;
	// m x this
	Mat3	LeftMul(const Mat3& m) const;
	// this x m
	Mat3	RightMul(const Mat3& m) const;
	// Right multiply a column
	Vec3	Mul(const Vec3 v) const;
	Mat3	Inverse() const;
	const float * GetRawPtr() const;
};

inline Mat3::Mat3() {
}

inline Mat3::Mat3(float xx, float xy, float xz,
				  float yx, float yy, float yz,
				  float zx, float zy, float zz) {
	mat[0][0] = xx; mat[0][1] = yx; mat[0][2] = zx;
	mat[1][0] = xy; mat[1][1] = yy; mat[1][2] = zy;
	mat[2][0] = xz; mat[2][1] = yz; mat[2][2] = zz;
}

inline Mat3::Mat3(const Mat3& m) {
	*this = m;
}

inline Mat3::Mat3(const Vec3 col_1, const Vec3 col_2, const Vec3 col_3) {
	mat[0] = col_1;
	mat[1] = col_2;
	mat[2] = col_3;
}

inline Mat3& Mat3::operator=(const Mat3& m) {
	for( int i = 0; i < 3; ++i ) {
		for( int j = 0; j < 3; ++j )
			mat[i][j] = m[i][j];
	}
	return *this;
}

inline Vec3& Mat3::operator[](int j) {
	assert( j >= 0 && j < 3 );
	return mat[j];
}

inline const Vec3& Mat3::operator[](int j) const {
	assert(j >= 0 && j < 3);
	return mat[j];
}

inline void Mat3::Ident()
{
	for( int i = 0; i < 3; ++i )
		mat[i][i] = 1.0f;
}

inline Mat3 Mat3::Transpose()
{
	Mat3 ret;
	for( int i = 0; i < 3; ++i )
		for( int j = 0; j < 3; ++j )
			ret[i][j] = mat[j][i];

	return ret;
}

inline void Mat3::TransposeSelf() {
	Mat3 tmp = *this;
	tmp.Transpose();
	*this = tmp;
}

inline Mat3 Mat3::Add(const Mat3& m) const {
	Mat3 sum;
	for( int i = 0; i < 3; ++i ) {
		for( int j = 0; j < 3; ++j ) {
			sum[i][j] = mat[i][j] + m[i][j];
		}
	}
	return sum;
}

inline Mat3 Mat3::Sub(const Mat3& m) const{
	Mat3 sum;
	for( int i = 0; i < 3; ++i ) {
		for( int j = 0; j < 3; ++j ) {
			sum[i][j] = mat[i][j] - m[i][j];
		}
	}
	return sum;
}

inline Vec3 Mat3::Mul(const Vec3 v) const{
	Vec3 res;
	
	res[0] = mat[0][0] * v[0] + mat[1][0] * v[1] + mat[2][0] * v[2];
	res[1] = mat[0][1] * v[0] + mat[1][1] * v[1] + mat[2][1] * v[2];
	res[2] = mat[0][2] * v[0] + mat[1][2] * v[1] + mat[2][2] * v[2];
	return res;
}

inline Mat3 Mat3::LeftMul(const Mat3& m) const{
	Vec3 col1 = m.Mul(mat[0]);
	Vec3 col2 = m.Mul(mat[1]);
	Vec3 col3 = m.Mul(mat[2]);
	return Mat3(col1, col2, col3);
}


inline Mat3 Mat3::RightMul(const Mat3& m) const{
	return m.LeftMul(*this);
}

inline float Mat3::Determinant() const {
	float det2_12_01 = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];
	float det2_12_02 = mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0];
	float det2_12_12 = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];

	return mat[0][0] * det2_12_12 - mat[0][1] * det2_12_02 + mat[0][2] * det2_12_01;
}

inline Mat3 Mat3::Inverse() const 
{
    float det, detInv;
    Mat3 inverse;

    inverse[0][0] = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];
    inverse[1][0] = mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2];
    inverse[2][0] = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];

    det = inverse[0][0] * mat[0][0] + inverse[1][0] * mat[0][1] +  inverse[2][0] * mat[0][2];
    if( std::abs(det) <= MI_EPSILON ) {
    	return *this;	// Orthogonal matrix
    }

    detInv = 1.0f / det;
    inverse[0][1] = mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2];
    inverse[1][1] = mat[0][1] * mat[2][2] - mat[0][2] * mat[2][0];
    inverse[2][1] = mat[0][1] * mat[2][0] - mat[0][0] * mat[2][1];
    inverse[0][2] = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
    inverse[1][2] = mat[0][2] * mat[1][0] - mat[0][0] * mat[1][2];
    inverse[2][2] = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

    for( int i = 0; i < 3; ++i ) {
    	for( int j = 0; j < 3; ++j ) {
    		inverse[i][j] *= detInv;
    	}
    }

    return inverse;
}

inline const float * Mat3::GetRawPtr() const 
{
	return reinterpret_cast<const float*>(mat);
}

class Mat4
{
private:
	Vec4 mat[4];

public:
			Mat4();
			Mat4(float xx, float xy, float xz, float xw,
				 float yx, float yy, float yz, float yw,
				 float zx, float zy, float zz, float zw,
				 float wx, float wy, float wz, float ww);
			Mat4(const Vec4 col_1, const Vec4 col_2, const Vec4 col_3, const Vec4 col_4);
			Mat4(const Mat4& m);
			Mat4(const float *);

	Mat4&	operator=(const Mat4& m);
	Vec4&	operator[](int j);
	const Vec4& operator[](int j) const;
	void	Ident();	// Load identity matrix
	float	Determinant() const ;
	Mat4	Transpose();
	void	TransposeSelf();
	Mat4	Add(const Mat4& m) const;
	Mat4	Sub(const Mat4& m) const;
	// m x this
	Mat4	LeftMul(const Mat4& m) const;
	// this x m
	Mat4	RightMul(const Mat4& m) const;
	// Right multiply a column
	Vec4	Mul(const Vec4) const;
	Vec4	Mul(const Vec3) const;
	const float * GetRawPtr() const;
};

inline Mat4::Mat4() {
}

inline Mat4::Mat4(float xx, float xy, float xz, float xw,
				 float yx, float yy, float yz, float yw,
				 float zx, float zy, float zz, float zw,
				 float wx, float wy, float wz, float ww) {
	mat[0][0] = xx; mat[0][1] = yx; mat[0][2] = zx; mat[0][3] = xw;
	mat[1][0] = xy; mat[1][1] = yy; mat[1][2] = zy; mat[1][3] = yw;
	mat[2][0] = xz; mat[2][1] = yz; mat[2][2] = zz; mat[2][3] = zw;
	mat[3][0] = wx; mat[3][1] = wy; mat[3][2] = wz; mat[3][3] = ww;
}

inline Mat4::Mat4(const Vec4 col_1, const Vec4 col_2, const Vec4 col_3, const Vec4 col_4) {
	mat[0] = col_1; mat[1] = col_2; mat[2] = col_3; mat[3] = col_4;
}

inline Mat4::Mat4(const Mat4& m) {
	mat[0] = m[0]; mat[1] = m[1]; mat[2] = m[2]; mat[3] = m[3];
}

inline Mat4::Mat4(const float * m)
{
	mat[0][0] = m[0]; mat[0][1] = m[1]; mat[0][2] = m[2]; mat[0][3] = m[3];
	mat[1][0] = m[4]; mat[1][1] = m[5]; mat[1][2] = m[6]; mat[1][3] = m[7];
	mat[2][0] = m[8]; mat[2][1] = m[9]; mat[2][2] = m[10]; mat[2][3] = m[11];
	mat[3][0] = m[12]; mat[3][1] = m[13]; mat[3][2] = m[14]; mat[3][3] = m[15];
}

inline Mat4& Mat4::operator=(const Mat4& m) {
	for( int i = 0; i < 4; ++i ) {
		for( int j = 0; j < 4; ++j )
			mat[i][j] = m[i][j];
	}
	return *this;
}

inline Vec4& Mat4::operator[](int j) {
	assert( j >= 0 && j < 4 );
	return mat[j];
}

inline const Vec4& Mat4::operator[](int j) const {
	assert(j >= 0 && j < 4);
	return mat[j];
}

inline void Mat4::Ident() {
	for( int i = 0; i < 4; ++i ) {
		mat[i][i] = 1.0f;
	}
}

// Copied from Doom-BGF source code
inline float Mat4::Determinant() const {
	// 2x2 sub-determinants
	float det2_01_01 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	float det2_01_02 = mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0];
	float det2_01_03 = mat[0][0] * mat[1][3] - mat[0][3] * mat[1][0];
	float det2_01_12 = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
	float det2_01_13 = mat[0][1] * mat[1][3] - mat[0][3] * mat[1][1];
	float det2_01_23 = mat[0][2] * mat[1][3] - mat[0][3] * mat[1][2];

	// 3x3 sub-determinants
	float det3_201_012 = mat[2][0] * det2_01_12 - mat[2][1] * det2_01_02 + mat[2][2] * det2_01_01;
	float det3_201_013 = mat[2][0] * det2_01_13 - mat[2][1] * det2_01_03 + mat[2][3] * det2_01_01;
	float det3_201_023 = mat[2][0] * det2_01_23 - mat[2][2] * det2_01_03 + mat[2][3] * det2_01_02;
	float det3_201_123 = mat[2][1] * det2_01_23 - mat[2][2] * det2_01_13 + mat[2][3] * det2_01_12;

	return ( - det3_201_123 * mat[3][0] + det3_201_023 * mat[3][1] - det3_201_013 * mat[3][2] + det3_201_012 * mat[3][3] );
}


inline Mat4	Mat4::Transpose() {
	Mat4 ret;
	for( int i = 0; i < 4; ++i )
		for( int j = 0; j < 4; ++j )
			ret[i][j] = mat[j][i];

	return ret;
} 

inline void Mat4::TransposeSelf() {
	Mat4 tmp = *this;
	tmp.Transpose();
	*this = tmp;
}

inline Mat4 Mat4::Add(const Mat4& m) const {
	Mat4 sum;
	for( int i = 0; i < 4; ++i ) {
		for( int j = 0; j < 4; ++j ) {
			sum[i][j] = mat[i][j] + m[i][j];
		}
	}

	return sum;
}

inline Mat4 Mat4::Sub(const Mat4& m) const {
	Mat4 sub;
	for( int i = 0; i < 4; ++i ) {
		for( int j = 0; j < 4; ++j ) {
			sub[i][j] = mat[i][j] - m[i][j];
		}
	}
	return sub;
}

inline Vec4	Mat4::Mul(const Vec4 v) const {
	Vec4 res;
	res[0] = mat[0][0] * v[0] + mat[1][0] * v[1] + mat[2][0] * v[2] + mat[3][0] * v[3];
	res[1] = mat[0][1] * v[0] + mat[1][1] * v[1] + mat[2][1] * v[2] + mat[3][1] * v[3];
	res[2] = mat[0][2] * v[0] + mat[1][2] * v[1] + mat[2][2] * v[2] + mat[3][2] * v[3];
	res[3] = mat[0][3] * v[0] + mat[1][3] * v[1] + mat[2][3] * v[2] + mat[3][3] * v[3];
	return res;
}

inline Vec4 Mat4::Mul(const Vec3 v) const 
{
	Vec4 w(v[0], v[1], v[2], 1.0f);
	return Mul(w);
}

inline Mat4 Mat4::LeftMul(const Mat4& m) const {
	Vec4 col1 = m.Mul(mat[0]);
	Vec4 col2 = m.Mul(mat[1]);
	Vec4 col3 = m.Mul(mat[2]);
	Vec4 col4 = m.Mul(mat[3]);
	return Mat4(col1, col2, col3, col4);
}


inline Mat4 Mat4::RightMul(const Mat4& m) const {
	return m.LeftMul(*this);
}

inline const float * Mat4::GetRawPtr() const 
{
	return reinterpret_cast<const float*>(mat);
}

#endif
