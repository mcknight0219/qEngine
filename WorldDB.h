/**
 * World database which manage all entities in the rendering
 * scenario.
 */

#ifndef _WORLDDB_H
#define _WORLDDB_H

#include "Mesh.h"
#include "Entity.h"
#include "String.h"
#include "File.h"
#include "Math.h"
#include "qEngine.h"

// Key-value storage
#include <list>
#include <vector>
#include <assert.h>


class WorldDB
{
public:
    // Minimum implementation of factory pattern
    static WorldDB *getInstance()
    {
        if( !self ) {
            self = new WorldDB();
        }
        return self;
    }

    bool    LoadMap(const char *map);
    int     Count() const;    
    void    Reset();
    void    AddEntity(Mat4 pos, qStr fmt, qStr path);
    // For iteration
    Entity * operator[](int n) const;

private:
    WorldDB();
    ~WorldDB();

    Mat4    ReadMat4(LexerFile * lex);

private:
    static WorldDB *        self;
   
    // Population of the world
    int                     numEnt;
    // Entities 
    std::vector<Entity*>    entities;
    std::vector<Mesh*>      meshes;
    // map file name
    char *                  mapName;
    // If the world is loaded
    bool                    loaded;
	// Nr of background entities
	int						numBgEntities;
	// Disable copy and assign ctor
	WorldDB(const WorldDB&) {}
	WorldDB& operator=(const WorldDB&) { return *this; /* silence compiler */}
};


inline void WorldDB::Reset()
{
    if( !loaded ) {
        return;
    }

    if( mapName ) {
        free(mapName);
        mapName = 0;
    } 
     
    loaded = false;
}


inline int WorldDB::Count() const
{
    return numEnt;
}
#endif /* !_WORLDDB_H */
