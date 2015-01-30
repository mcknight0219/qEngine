#ifndef __QENGINE_H
#define __QENGINE_H

#include <GLES/egl.h>
#include <GLES/gl.h>
#include <vector>

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "String.h"
#include "Entity.h"
#include "Math.h"
#include "File.h"
#include "Common.h"
#include "WorldDB.h"
#include "InputEvent.h"
#include "Log.h"
#include "CameraPath.h"
#include "qArr.h"

#define QENGINE_VERSION	"0.1"
#define MAX_ENTITY_NUMBER	256
#define MAX_CAMERAPATH 15

#define DISALLOW_DEFAULT_AND_COPY_CTOR(NAME) \
	private: \
		NAME() {} \
		NAME(const NAME& other) {}

typedef unsigned char   byte;


typedef struct {
	Vec3	pos;
	Vec3	lookAt;
	Vec3	up;
	float 	aspect;

	float fov;
	float zNear;
	float zFar;
} camera_t;

struct light_t {
    unsigned short id;
    bool    enabled;
    // false if it's point light
    bool    directional;
    Vec4    pos;
    Vec3    ambient;
    Vec3    diffuse;
    Vec3    specular;
    float   constantAttenuation;
    float   linearAttenuation;

    light_t * next;
};

/* Winding is very important here. For light facing triangle,
 v2-v1 is the ccw, and on the other side, v1-v2 is ccw */
struct siledge_t {
    int     p1, p2; // two triangle sharing the edge
    int     v1, v2;
    byte    flag;   // marks if it's a silhoutee edge
};


struct silhouette_t {
    Entity*     entity;
    int         numSilEdges;
    siledge_t*  sil;   
    // one byte for each edge. 0 not facing, 1 facing 
    byte*       facing; 
};


class Texture;
class WorldDB;
/*
======================================================

Use fixed function render. It allows me to experience
render pipeline more directly

======================================================
*/
class qEngine
{
public:
			    qEngine(unsigned int width, unsigned int height);
			    ~qEngine();

	bool 	    Init();
	void	    Stop();
	void	    Shutdown();
	bool	    IsOn();
	// The root game directory
	qStr	    GetGameDir() const;

	void	    RenderFrame();
	void	    RenderEntity(Entity * entity);
	void	    RenderBBox(Entity * entity);
	void	    RenderNormal(Entity * entity);
	void	    SetProjectionMat();
	void	    SetViewMat();
    void        SetLighting();
	void	    UpdateWorld();

	// Load entities into world
	void	    LoadMap(const char * map);

	// Camera setting
	void	    SetupCamera(float fov_y, float aspect, float zNear, float zFar);
	void	    LookAt(const Vec3 vLookat);
	void	    MoveCamera(const Vec3 vPos, const Vec3 vUp);
	void	    AttachCamera(const Entity * entity);
	void	    DetachCamera();
	void	    LoadCameraPath(const char * pathFile);
    void        SetCurrentCameraPath(int id);
    CameraPath* GetCurrentCameraPath() const;

    // Light configuration
    void        AddLight(light_t *l);
    light_t *   GetDefaultLight();

	Mesh *	    GetModel(const char *name) const;
	Log *	    GetLogger() const;

	int		    GetFrameCount() const { return frameCount; }
    void        Snapshot();

private:
	void	    Set3D();
	bool	    InitModelCache();
	bool	    InitTextureCache();
    bool        PreloadCP();
	Texture*    GetTexture(const char *name) const;
	void	    AddEntity(qStr modelName, Vec3 modelPos);
	void	    GetColorBuffer(unsigned char *);
    silhouette_t*     GetSilhouette(const Entity * entity, light_t * l);
    void        R_SilDebugDraw(silhouette_t *);

private:
	// place to find all resources
	qStr					dataDir;
	std::vector<Mesh*>		meshCache;
	std::vector<Texture*>	texCache;
	std::vector<Entity*>	entityCache;
	// View space to projection space
	Mat4					projectionMat;
	// World sapce to view space
	Mat4					modelViewMat;
	camera_t				camera;
    light_t *               lights;
    int                     numLights;
	CameraPath*     		cameraPath[MAX_CAMERAPATH];
    CameraPath*             currentCameraPath;
	Entity *				attachedEntity;
	WorldDB*				world;

	bool					engineOn;
	bool					debugOn;
	unsigned int			windowWidth;
	unsigned int			windowHeight;
    int                     frameCount;

	Log	*					logger;

	DISALLOW_DEFAULT_AND_COPY_CTOR(qEngine)
};

inline qEngine::qEngine(unsigned int width, unsigned int height) : lights(0), numLights(0), currentCameraPath(0), attachedEntity(0), engineOn(false), debugOn(true), windowWidth(width), windowHeight(height), frameCount(0)
{
    memset(cameraPath, 0, sizeof(CameraPath*) * MAX_CAMERAPATH);
	Init();
}

inline qEngine::~qEngine()
{
	Shutdown();
}

inline void qEngine::LookAt(const Vec3 vLookat)
{
	camera.lookAt = vLookat;
}

inline void qEngine::MoveCamera(const Vec3 vPos, const Vec3 vUp)
{
	camera.pos = vPos;
	camera.up = vUp;
}


inline bool qEngine::IsOn()
{
	return engineOn;
}

inline void qEngine::Stop()
{
	engineOn = false;
}

inline qStr qEngine::GetGameDir() const
{
	return dataDir;
}

inline Log * qEngine::GetLogger() const
{
	return logger;
}
 
#endif /* !__QENGINE_H */
