#include "WorldDB.h"
/**
 * Implementation of WorldDB class
 */

extern qEngine * engine;
extern Common * common;

WorldDB* WorldDB::self = NULL;

WorldDB::WorldDB()
{
    numEnt = 0;
	mapName = NULL;
}

WorldDB::~WorldDB()
{
}

Vec4 ReadVec4(LexerFile * lex)
{
    Vec4 ret;
    for( int i = 0; i < 4; ++i ) {
        lex->ReadToken();
        ret[i] = lex->TokenValue().ToFloat();
    }
    return ret;
}

Vec3 ReadVec3(LexerFile * lex)
{
    Vec3 ret;
    for( int i = 0; i < 3; ++i ) {
        lex->ReadToken();
        ret[i] = lex->TokenValue().ToFloat();
    }

	return ret;
}


bool WorldDB::LoadMap(const char *map)
{
    if( !map || map[0] == 0 ) {
        fprintf(stderr, "Incorrect map file name.");
        return false;
    }

	qStr dataDir = engine->GetGameDir();
    qStr fn(map);
	fn.Insert("/map/");
	fn.Insert(dataDir.Ptr());

    if( fn.GetFileExtension() != "map" ) {
        fprintf(stderr, "Incorrect map file extension.");
        return false;
    }

    if( loaded ) {
        Reset();
    }

    LexerFile lex(fn);
    while( lex.MoreToken() ) {
        lex.ReadToken();
        qStr val = lex.TokenValue();
        // Background entities
        if( val == "numBackgroundEntities" ) {
            lex.ReadToken();
			numBgEntities = lex.TokenValue().ToInteger();
			continue;
        } 

        // Parse a whole entity
		if( val == "matrix" ) {
            lex.ReadToken();    // '{'    
            Mat4 pos = ReadMat4(&lex);
            lex.ReadToken();    // '}'
            
            lex.ReadToken();
            if( lex.TokenValue() != "entities" ) {
                fprintf(stderr, "Bad map file format.");
                return false;
            }

            lex.ReadToken();
            qStr fmt = lex.TokenValue();

            lex.ReadToken();    // '{'
            lex.ReadToken();    // 'model'
            lex.ReadToken();   
            qStr path = lex.TokenValue();

            AddEntity(pos, fmt, path);
		} else if( val == "light" ) {
            static int lightId = 0;
            light_t * l = (light_t*)malloc(sizeof(*l));
            l->id = lightId++;
			lex.ReadToken();
			assert( lex.TokenValue() == "{" );

            lex.ReadToken();    // 'enabled' label
            lex.ReadToken();
			l->enabled = (lex.TokenValue().ToInteger() == 1);

            lex.ReadToken();    // 'position' label 
            l->pos = ReadVec4(&lex);

            lex.ReadToken();    // 'ambientColor' label
            l->ambient = ReadVec3(&lex);

            lex.ReadToken();    // 'diffuseColor' label
            l->diffuse = ReadVec3(&lex);

            lex.ReadToken();    // 'specularColor' label
            l->specular = ReadVec3(&lex);

            lex.ReadToken();
            l->constantAttenuation = lex.TokenValue().ToFloat();

            lex.ReadToken();
            l->linearAttenuation = lex.TokenValue().ToFloat();
            
            l->directional = (l->pos[3] == 0);

			lex.ReadToken();	// '}'
            engine->AddLight(l);
        }
    }    

    loaded = true;
	return true;
}

Mat4 WorldDB::ReadMat4(LexerFile * lex)
{
    Mat4 res;
    for( int i = 0; i < 4; ++i ) {
        for( int j = 0; j < 4; ++j ) {
            lex->ReadToken();
            res[j][i] = lex->TokenValue().ToFloat();
        }
    }
    return res;
}

void WorldDB::AddEntity(Mat4 pos, qStr fmt, qStr path)
{
    // file name is enough to identifier a mesh instance
    qStr meshName = path.GetFileName();

    // Mesh is already loaded by engine
    Mesh * mesh = engine->GetModel(meshName.Ptr());
    
    if( !mesh ) {
        common->FatalError("Mesh doesn't exist.\n");
    } 

    Entity * ent = new Entity();   
    ent->AttachMesh(mesh);
    ent->SetModelToWorldMat(pos);

    entities.push_back(ent);
    numEnt++;
}



Entity * WorldDB::operator[](int n) const
{
    assert( n >= 0 && n < numEnt );
    return entities[n];
}














