#include "Mesh.h"
#include <algorithm>
#include <assert.h>

extern Common * common;

Mesh::Mesh(const qStr sPath) : vertexArray(NULL), indexArray(NULL), isBind(false) 
{
	meshFileName = sPath;
	name = sPath.GetFileName();
}

Mesh::~Mesh()
{
	if( vertexArray )
		free(vertexArray);
	if( indexArray )
		free(indexArray);
}

// checkout md5 format spec http://tfc.duke.free.fr/coding/md5-specs-en.html
bool Mesh::LoadMD5()
{
	if( meshFileName.Empty() ) {
		printf("Model name doesn't exist or is empty");
		return false;
	}

	LexerFile lex(meshFileName);
	if( !lex.Good() ) {
		printf("Failed reading md5 file");
		return false;
	}

	lex.ReadToken();
	if( lex.TokenValue() != "MD5Version") {
		printf("Corrupted md5 file. In wrong format");
		return false;
	}
	lex.ReadToken();	// Don't care version

	int numJoints, numMeshes;
	// Read numJoints
	lex.ReadToken(); lex.ReadToken();
	numJoints = lex.TokenValue().ToInteger();
	// Read numMeshes
	lex.ReadToken(); lex.ReadToken();
	numMeshes = lex.TokenValue().ToInteger();

	// big parsing loop !
	qStr val;
	while( lex.MoreToken() ) {
		lex.ReadToken();
		val = lex.TokenValue();
		if (val == "joints") {
			ReadJoin(&lex);
		} 
		else if (val == "mesh") {
			ReadMesh(&lex);
		}
		else if (val == "}") {
			break;
		}
	}

	return true;
}

void Mesh::ReadJoin(LexerFile *lex)
{
	// For now, just skip joints
	while (lex->MoreToken()) {
		lex->ReadToken();
		qStr val = lex->TokenValue();
		if( val == "}" )
			break;
	}
}

void Mesh::ReadMesh(LexerFile *lex)
{
	std::vector<float>		verts;
	std::vector<float>		texels;
	std::vector<unsigned short>		tris;

	lex->ReadToken(); // '{'

	// texture file name
	lex->ReadToken();
	assert(lex->TokenValue() == "shader");
	lex->ReadToken();
	textureFileName = lex->TokenValue();
	textureFileName.RemoveQuotes();

	// numverts
	lex->ReadToken();
	assert(lex->TokenValue() == "numverts");
	lex->ReadToken();
	int numVerts = lex->TokenValue().ToInteger();

	// vVertes is later used to generate vertex position
	std::vector<md5_vertex_t> vVerts;
	vVerts.reserve(numVerts);
	for( int j = 0; j < numVerts; ++j ) {
		md5_vertex_t v = ReadVertex(lex);
		vVerts.push_back(v);
	}

	// triangles
	lex->ReadToken();
	assert(lex->TokenValue() == "numtris");
	lex->ReadToken();
	int numTris = lex->TokenValue().ToInteger();
	tris.reserve(numTris * 3);
	for( int j = 0; j < numTris; ++j ) {
		ReadTriangle(lex, tris);
	}

	// weights
	lex->ReadToken();
	assert(lex->TokenValue() == "numweights");
	lex->ReadToken();
	int numWeights = lex->TokenValue().ToInteger();
	std::vector<md5_weight_t> vWeights;
	vWeights.reserve(numWeights);
	for( int k = 0; k < numWeights; ++k ) {
		md5_weight_t w = ReadWeight(lex);
		vWeights.push_back(w);
	}

	// Generate vertex position
	for( int j = 0; j < numVerts; ++j ) {
		md5_vertex_t mv = vVerts[j];
		texels.push_back(mv.texel[0]);
		texels.push_back(mv.texel[1]);
		float x, y, z;
		x = y = z = 0.0f;
		for( int i = mv.start; i < mv.start+mv.num; ++i ) {
			md5_weight_t mw = vWeights[i];
			x += mw.pos[0] * mw.bias;
			y += mw.pos[1] * mw.bias;
			z += mw.pos[2] * mw.bias;
		}
		verts.push_back(x);
		verts.push_back(y);
		verts.push_back(z);
	}

	MingleData(verts, texels, tris);
}

md5_vertex_t Mesh::ReadVertex(LexerFile *lex)
{
	lex->ReadToken();	// 'vert'
	lex->ReadToken();	// index, why does it matter?

	md5_vertex_t vert;
	qStr val;
	lex->ReadToken(); val = lex->TokenValue(); vert.texel[0] = val.ToFloat();
	lex->ReadToken(); val = lex->TokenValue(); vert.texel[1] = val.ToFloat();
	lex->ReadToken(); val = lex->TokenValue(); vert.start = val.ToInteger();
	lex->ReadToken(); val = lex->TokenValue(); vert.num = val.ToInteger();	

	return vert;
}

md5_weight_t Mesh::ReadWeight(LexerFile *lex)
{
	lex->ReadToken();	// 'weight'
	lex->ReadToken();	// index

	md5_weight_t weight;
	qStr val;
	lex->ReadToken(); val = lex->TokenValue(); weight.joint = val.ToInteger();
	lex->ReadToken(); val = lex->TokenValue(); weight.bias = val.ToFloat();
	for( int j = 0; j < 3; ++j ) {
		lex->ReadToken(); val = lex->TokenValue(); 
		weight.pos[j] = val.ToFloat();
	}
	return weight;
}

// Read triangle directly into tris
void Mesh::ReadTriangle(LexerFile *lex, std::vector<unsigned short>& tris)
{
	lex->ReadToken();	// tri
	lex->ReadToken();	// index

	for( int i = 0; i < 3; ++i ) {
		lex->ReadToken();
		qStr val = lex->TokenValue();
		tris.push_back(val.ToInteger());
	}
}

void Mesh::MingleData(std::vector<float>& vVert, std::vector<float>& vText, std::vector<unsigned short>& vTris)
{
	// Make sure we are on the same page
	assert(vVert.size() / 3 == vText.size() / 2);
	if( vertexArray && indexArray )
		return;

	nVert = vVert.size() / 3;
	nIndex = vTris.size();
	vertexArray = (vertex_t *)malloc(nVert * sizeof(vertex_t));
	if( !vertexArray ) {
		common->FatalError("Cannot allocate more memory");
	}

	indexArray = (unsigned short *)malloc(nIndex * sizeof(unsigned short));
	if( !indexArray ) {
		common->FatalError("Cannot allocate more memory");
	}

	int j = 0;
	int k = 0;
	for(int i = 0; i < nVert; ++i ) {
		vertex_t *pVertex = &vertexArray[i];
		pVertex->pos[0] = vVert[j++];
		pVertex->pos[1] = vVert[j++];
		pVertex->pos[2] = vVert[j++];

		pVertex->st[0]  = vText[k++] * 32767;
		pVertex->st[1]  = vText[k++] * 32767; 
	}

	std::copy(vTris.begin(), vTris.end(), indexArray);

	CalcNormal(vertexArray, indexArray, nVert, nIndex);	// Calculate the normal per vertex
}

void Mesh::UploadGPU()
{
	if (isBind) {
		return;
	}

	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, nVert * sizeof(vertex_t), (const GLvoid*)vertexArray, GL_STATIC_DRAW);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "glBufferData() failed. %d\n", err);
		return;
	}
	isBind = true;
}

void Mesh::UnBind()
{
	if( !isBind ) {
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	isBind = false;
}


qArr<edge_t> Mesh::GenEdgeList()
{
    qArr<edge_t> edges;
    int numTri = nIndex / 3;
    for( int i = 0; i < numTri; ++i ) {
        edge_t e1 = { indexArray[i*3], indexArray[i*3+1], i, 0 };
        edge_t e2 = { indexArray[i*3+1], indexArray[i*3+2], i, 0 };
        edge_t e3 = { indexArray[i*3+2], indexArray[i*3], i, 0 };
        edges.Add(3, e1, e2, e3);
    }
    return edges;
}

void Mesh::CalcNormal(vertex_t * varr, const unsigned short * iarr, const int vsize, const int isize)
{
	short iV1, iV2, iV3;
	for( int i = 0; i < vsize; ++i ) {
		vertex_t * pv = varr + i;
		pv->normal.Zero();
	}

	for( int i = 0; i < isize; i += 3) {
		iV1 = *iarr++;
		iV2 = *iarr++;
		iV3 = *iarr++;

		vertex_t * pV1 = varr + iV1;
		vertex_t * pV2 = varr + iV2;
		vertex_t * pV3 = varr + iV3;

		Vec3 n = (pV2->pos - pV1->pos).CrossProduct(pV3->pos - pV1->pos);
		n = n.Normalize();

		pV1->normal = pV1->normal + n;
		pV2->normal = pV2->normal + n;
		pV3->normal = pV3->normal + n;

	}

	for( int i = 0; i < vsize; ++i ) {
		vertex_t * pv = varr + i;
		pv->normal = pv->normal.Normalize();
	}
}


/*=============================================================
 *
 *  Texutre class implementation
 * 
 *============================================================
 */
bool Texture::LoadPNG()
{
	// Currently only support .png format
	qStr ext = texFileName.GetFileExtension();
	if( ext != "png" ) {
		fprintf(stderr, "Only support PNG format\n");
		return false;
	}

	// Does it hurt to call it multiple times
	ilInit();

	ILuint	imgId;
	ilGenImages(1, &imgId);
	ilBindImage(imgId);

	ILenum iErr;

	if( ilLoadImage((ILstring)texFileName.Ptr()) == false ) {
		iErr = ilGetError();
		fprintf(stderr, "Failed loading image %s:%d\n", texFileName.Ptr(), iErr);
		return false;
	} 

	unsigned int bytePerPixel;
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	width =  ilGetInteger(IL_IMAGE_WIDTH);
	bytePerPixel = ilGetInteger(IL_IMAGE_BPP);

	// Determine Format
	if( bytePerPixel == 4 ) {
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		format = TEXTURE_GL_RGBA;
	} else {
		ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		format = TEXTURE_GL_RGB;
	}

	// Allocating memory and copy image data
	apiData = (char*)malloc(height * width  * bytePerPixel);
	if( !apiData ) {
		common->FatalError("Cannot allocate memory. Aborting...");
	}
	memcpy(apiData, ilGetData(), height * width * bytePerPixel);

	ilDeleteImage(imgId);

	if( ilGetError() != IL_NO_ERROR ) {
		fprintf(stderr, "Something wrong with IL library.\n");
	}
	return true;
}


void Texture::UploadGPU()
{
	if( isBind)
		return;

	glGenTextures(1, &apiId);
	glBindTexture(GL_TEXTURE_2D, apiId);

	if( format == TEXTURE_GL_RGB || format == TEXTURE_GL_RGBA ) {
		glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		if( format == TEXTURE_GL_RGBA) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, apiData);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, apiData);
		}
	} else {
        fprintf(stderr, "UploadGPU() unimplemented path !\n");
        return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    

	// Free apiData ?
	free(apiData);
	size = 0;
	isBind = true;
}
