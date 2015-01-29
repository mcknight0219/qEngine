#ifndef _ENTITY_H
#define _ENTITY_H

#ifdef _WIN32
#include <GLES\egl.h>
#include <GLES\gl.h>
#elif __linux__
#include <GLES/egl.h>
#include <GLES/gl.h>

#include <string.h>	// for memcpy
#endif

#include "IL/il.h"	// For loading texure resources
#include "File.h"
#include "String.h"
#include "Common.h"
#include "Math.h"
#include "qArr.h"

#include <stdio.h>
#include <vector>

typedef enum { ENT_PLAYER, ENT_ENEMY } entity_type_t;
typedef unsigned char byte;

typedef struct {
	float 	texel[2];
	// weights
	int 		start;
	int			num;
} md5_vertex_t;

typedef struct {
	int joint;
	float bias;
	float pos[3];
} md5_weight_t;


// Congregation of data uploaded to GPU
typedef struct {
	Vec3 			pos;
	unsigned short 	st[2];	// Texture coordinates
	Vec3 			normal;
} vertex_t;

// p2-p1 is the ccw about normal to triangle plane
typedef struct {
    unsigned short  p1, p2;
    int             tri;
    byte            marked;
} edge_t;

/*
==============================================

Mesh object. Only one instance for each type 
of model

==============================================
*/
class Mesh
{
public:

						Mesh(qStr path);
						~Mesh();

	bool 				LoadMD5();
	qStr				GetName() const;
	vertex_t * 			GetVertexArray() const;
	unsigned short *	GetIndexArray() const;
	unsigned short		GetNumIndex() const;
	unsigned short		GetNumVert() const;

	qStr				GetTexName() const;
	unsigned int& 		GetVboId() const;
	bool				IsUploaded() const;
	void				UploadGPU();
	void				UnBind();

    qArr<edge_t>        GenEdgeList();

private:
	void				ReadJoin(LexerFile *lex);
	void				ReadMesh(LexerFile *lex);
	md5_vertex_t		ReadVertex(LexerFile *lex);
	void				ReadTriangle(LexerFile *lex, std::vector<unsigned short>& tris);
	md5_weight_t		ReadWeight(LexerFile *lex);

	void				CalcNormal(vertex_t * varr, const unsigned short * iarr, const int vsize, const int isize);
	
	// Merge vertex, texture, normal into one big chunk and
	// then feed into GPU pipeline
	void 				MingleData(std::vector<float>& vVert, std::vector<float>& vTexture, std::vector<unsigned short>& vTris);

private:
	qStr					name;
	qStr					meshFileName;
	qStr					textureFileName;
	
	// Everything in bundle to upload to GPU
	vertex_t *				vertexArray;				
	// Indices is separate stream of data
	unsigned short *		indexArray;
	unsigned int 			vboId;
	// If the data is in GPU
	bool					isBind;
	unsigned short			nIndex; 
	unsigned short			nVert;
};

inline qStr Mesh::GetName() const {
	return name;
}

inline vertex_t * Mesh::GetVertexArray() const
{
	return vertexArray;
}

inline unsigned short * Mesh::GetIndexArray() const
{
	return indexArray;
}

inline bool Mesh::IsUploaded() const
{
	return isBind;
}

inline unsigned short Mesh::GetNumIndex() const
{
	return nIndex;
}

inline unsigned short Mesh::GetNumVert() const
{
	return nVert;
}

inline qStr Mesh::GetTexName() const
{
	return textureFileName;
}


typedef enum { TEXTURE_GL_RGBA, TEXTURE_GL_RGB } texture_format_t;
/*
===========================================================

Texture resource management class.
It's awkward for MD5 file contains the texture coordinates.
So this class just handles load and upload actual texture
file. 

============================================================
*/
class Texture
{
public:
					Texture(qStr path);
					~Texture();

	bool			IsUploaded();
	void 			UploadGPU();
	unsigned int 	GetHeight();
	unsigned int 	GetWidth();
	qStr			GetName() const;
	bool			LoadPNG();

private:
	void * 			apiData;
	unsigned int 	size;
	unsigned int 	apiId;
	qStr			texFileName;
	qStr			name;
	bool			isBind;

	unsigned int 	height;
	unsigned int 	width;
	texture_format_t format;

private:
	Texture() {}
	Texture(const Texture&) {}

};

inline Texture::~Texture()
{
	if (apiData)
		free(apiData);
}

inline Texture::Texture(qStr path) : apiData(NULL), isBind(false), height(0), width(0) 
{
	texFileName = path;
	name = texFileName.GetFileName();
}


inline bool Texture::IsUploaded()
{
	return isBind;
}

inline unsigned int Texture::GetHeight()
{
	return height;
}

inline unsigned int Texture::GetWidth()
{
	return width;
}

inline qStr Texture::GetName() const
{
	return name;
}

#endif /* !_ENTITY_H */
