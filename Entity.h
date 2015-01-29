#ifndef __ENTITY_H
#define __ENTITY_H

#include "Mesh.h"
#include "String.h"
#include "Math.h"
#include "Geometry.h"

/*
===================================================

An object that engine will render

===================================================
*/
class Entity
{
public:
				Entity();
				~Entity() {}

	int			GetId() const;
	void 		AttachMesh(Mesh *model);
	void 		AttachTexture(Texture *tex);
	Mesh *		GetModel() const;
	Texture *	GetTexture() const;
	// translation
	void  		MoveTo(const Vec3 pos);
	void   		Scale (const Vec3 factor);
	void 		Rotate(const Vec3 eulerAngle);
	Mat4		GetModelToWorldMat() const { return modelToWorldMat; }
    void        SetModelToWorldMat(const Mat4 mat);

    BBox		Bound();
    Vec3		GetPosition() ;

private:
	int				id;
	Mesh *			model; 	// Entity doesn't own model
	Texture *		tex;	// Texture belonging to entity
	unsigned int 	vboId;

	// orientation
	float	xAxis;
	float	yAxis;
	float 	zAxis;

	Mat4			modelToWorldMat;
};

inline void Entity::AttachMesh(Mesh *m)
{
	model = m;
	
}

inline void Entity::AttachTexture(Texture *t)
{
	tex = t;
}

inline int Entity::GetId() const
{
	return id;
}

inline Mesh * Entity::GetModel() const
{
	return model;
}

inline Texture * Entity::GetTexture() const
{
	return tex;
}

inline void Entity::SetModelToWorldMat(const Mat4 m)
{
    modelToWorldMat = m;
}


#endif
