#include "Entity.h"
#include <cfloat>

static int entity_id = 0;

Entity::Entity() : id(entity_id++), model(NULL)
{
	// make identity default
	modelToWorldMat.Ident();
}

void Entity::MoveTo(const Vec3 pos) 
{
	modelToWorldMat[3][0] = pos[0];
	modelToWorldMat[3][1] = pos[1];
	modelToWorldMat[3][2] = pos[2];
}

void Entity::Scale(const Vec3 factor)
{
	modelToWorldMat[0][0] *= factor[0];
	modelToWorldMat[1][1] *= factor[1];
	modelToWorldMat[2][2] *= factor[2];
}

// Rotate about x, y, and z axis one after another
void Entity::Rotate(const Vec3 eulerAngle)
{
    Mat4 rot;
    // rorate about x
    if( eulerAngle[0] != 0 ) {
        float theta = eulerAngle[0];
        rot.Ident();
        rot[1][1] = cos(theta);
        rot[1][2] = sin(theta);
        rot[2][1] = -sin(theta);
        rot[2][2] = cos(theta);

        modelToWorldMat = modelToWorldMat.RightMul(rot);
    } 

     // rorate about y
    if( eulerAngle[1] != 0 ) {
        float theta = eulerAngle[1];
        rot.Ident();
        rot[0][0] = cos(theta);
        rot[0][2] = -sin(theta);
        rot[2][0] = sin(theta);
        rot[2][2] = cos(theta);

        modelToWorldMat = modelToWorldMat.RightMul(rot);
    } 

    // rorate about z
    if( eulerAngle[2] != 0 ) {
        float theta = eulerAngle[2];
        rot.Ident();
        rot[0][0] = cos(theta);
        rot[0][1] = sin(theta);
        rot[1][0] = -sin(theta);
        rot[1][1] = cos(theta);

        modelToWorldMat = modelToWorldMat.RightMul(rot);
    } 
}

// Bounding the entity
BBox Entity::Bound()
{
	Vec3 vmin(FLT_MAX);
    Vec3 vmax(FLT_MIN);

	vertex_t * vert = model->GetVertexArray(); 
	for( int i = 0; i < model->GetNumVert(); ++i ) {
        vertex_t v = vert[i];
        for( int j = 0; j < 3; ++j ) {
			if( v.pos[j] < vmin[j] )
                vmin[j] = v.pos[j];
            if( v.pos[j] > vmax[j] )
                vmax[j] = v.pos[j];
        }    
    }

    return BBox(vmin, vmax);
}

/* Use the center of bounding box as the position
 * of the entity. */
Vec3 Entity::GetPosition() 
{
    BBox box = Bound();
    Vec3 center = (box.GetMin() + box.GetMax()).Scale(0.5);

    return center;
}
