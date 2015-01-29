#include "qEngine.h"
#include "Geometry.h"
#include "Timer.h"

// Global indicating if engine is on or off
extern bool engineOn;
extern Common * common;
extern Timer * timer;

Mat4 textureMatrix = Mat4( 1.0f/32767,0,0,0,0,1.0f/32767,0,0,0,0,1,0,0,0,0,1 );

bool qEngine::Init()
{

#ifdef _WIN32
		wchar_t sBuf[256];
		GetCurrentDirectory(256, sBuf);
		char * dirName = qStr::GetCStr(sBuf);
#else
	char sBuf[256];
	getcwd(sBuf, 256);
	char * dirName = sBuf;
#endif
	qStr sCurrentDir(dirName);
	sCurrentDir.ConcatSelf("/data");
	dataDir = sCurrentDir;
	struct stat st;
	if( stat(dataDir.Ptr(), &st) || !st.st_mode & S_IFDIR ) {
		logger->LogWarning("Cannot find resource folder");
		return false;
	}

    /*TODO Maybe it's not a good idea to pre-load all models and
     * textures. Move them to a separate class to load resources
     * upon first use
     */
	InitModelCache();
	InitTextureCache();
    // Camera path is a relatively light-weight resources so we
    // load them all at the beginning.
    PreloadCP();

	Set3D();
    SetLighting();

	world = NULL;
	engineOn = true;
	debugOn = true;
	logger = new Log();
	// In development, set maximum logging 
	logger->SetLevel(L_NORMAL);

	//free(dirName);
    
	return true;
}

/*
================================================

Load all models and cache them directly in engine.
This is bad design. Will implement full-blown cache
system later

================================================
*/
bool qEngine::InitModelCache()
{
	qStr modelDir = dataDir.Concat("/model");
	struct stat st;
	if( stat(modelDir.Ptr(), &st) || !st.st_mode & S_IFDIR ) {
		logger->LogFatal("Cannot find model resource folder");
		return false;	// Just return, don't panic
	}

	// Load all models in modelDir
	std::vector<qStr> modFiles = common->ListFiles(modelDir.Ptr());
	if( modFiles.empty() ) {
		logger->LogFatal("Cannot find model in resource folder");
		return false;
	}

	for( std::vector<qStr>::iterator it = modFiles.begin(); it != modFiles.end(); ++it ) {
		Mesh *mobj = new Mesh(*it);
		if( !mobj->LoadMD5() ) {
			logger->LogWarning("Cannot load model %s", (*it).Ptr());
			continue;
		}
		meshCache.push_back(mobj);
	}

	return true;
}

bool qEngine::InitTextureCache()
{
	qStr texDir = dataDir.Concat("/texture");
	struct stat st;
	if( stat(texDir.Ptr(), &st) || !st.st_mode & S_IFDIR ) {
		logger->LogWarning("Cannot find texure folder");
		return false;
	}

	std::vector<qStr> texFiles = common->ListFiles(texDir.Ptr());
	if( texFiles.empty() ) {
		logger->LogWarning("Cannot find texure in folder");
		return false;
	}

	for( std::vector<qStr>::iterator it = texFiles.begin(); it != texFiles.end(); ++it ) {
		Texture *tobj = new Texture(*it);
		tobj->LoadPNG();
		texCache.push_back(tobj);
	}

	return true;
}

bool qEngine::PreloadCP()
{
    qStr cpDir = dataDir.Concat("/cp");
    struct stat st;
    if( stat(cpDir.Ptr(), &st) || !st.st_mode & S_IFDIR ) {
        logger->LogWarning("Cannot find camerapth folder");
        return false;
    }

    std::vector<qStr> cpFiles = common->ListFiles(cpDir.Ptr());

    for( std::vector<qStr>::iterator it = cpFiles.begin(); it != cpFiles.end(); ++it ) {
        LoadCameraPath((*it).Ptr());
    }

    return true;
}


void qEngine::Shutdown()
{
	// Release mesh object
	for( std::vector<Mesh*>::iterator it = meshCache.begin(); it != meshCache.end(); ++it) {
		//delete (*it);
	}
	// Release texure object
	for( std::vector<Texture*>::iterator it = texCache.begin(); it != texCache.end(); ++it) {
		//delete (*it);
	}

	if( logger ) {
		delete logger;
	}

	engineOn = false;
}

// Initialize 3D settings
void qEngine::Set3D()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Unbind everything
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glColor4f(1, 1, 1, 1);

	// Setup initial camera parameter
	SetupCamera(70.0f, (float)windowWidth / (float)windowHeight, 0.2f, 50.0f); 
	MoveCamera(Vec3(0.0f, 0.0f, 10.0f), Vec3(0, 1, 0));
	LookAt(Vec3(0.0f, 0.0f, 0.0f));

}

void qEngine::SetLighting()
{
    if( !numLights ) {
        return;
    }

    light_t * l = lights;
    for( ; l != NULL; l = l->next ) {
        if( !l->enabled ) {
            continue;
        }
        glLightfv(GL_LIGHT0, GL_POSITION, (float*)&l->pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  (float*)&l->ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  (float*)&l->diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, (float*)&l->specular);

        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, l->constantAttenuation);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, l->linearAttenuation);
    } 

}

void qEngine::SetProjectionMat()
{
	float f = (float)(1 / tan(camera.fov * DEG_TO_RAD / 2));
	float zNear = camera.zNear;
	float zFar  = camera.zFar;	
	projectionMat[0][0] = f / camera.aspect;
	projectionMat[1][1] = f;
	projectionMat[2][2] = (zFar + zNear) / (zNear - zFar);
	projectionMat[2][3] = -1;
	projectionMat[3][2] = 2 * (zFar * zNear) / (zNear - zFar);
}

void qEngine::SetViewMat()
{
	Vec3 vN = camera.pos - camera.lookAt;

	Vec3 vU = camera.up.CrossProduct(vN);

	vN = vN.Normalize();
	vU = vU.Normalize();
	Vec3 vV = vN.CrossProduct(vU);

	modelViewMat[0][0] = vU[0];		modelViewMat[1][0] = vU[1];		modelViewMat[2][0] = vU[2]; modelViewMat[3][0] = 0.0f;
	modelViewMat[0][1] = vV[0];		modelViewMat[1][1] = vV[1];		modelViewMat[2][1] = vV[2]; modelViewMat[3][1] = 0.0f;
	modelViewMat[0][2] = vN[0];		modelViewMat[1][2] = vN[1];		modelViewMat[2][2] = vN[2]; modelViewMat[3][2] = 0.0f;
	modelViewMat[0][3] = 0.0f;		modelViewMat[1][3] = 0.0f;		modelViewMat[2][3] = 0.0f;	modelViewMat[3][3] = 1.0f;
	// Optimize a bit so don't need to do matrix concatenation of Rotation matrix and Translation matrix
	modelViewMat[3][0] = -camera.pos.DotProduct(vU);
	modelViewMat[3][1] = -camera.pos.DotProduct(vV);
	modelViewMat[3][2] = -camera.pos.DotProduct(vN);	
}

Mesh * qEngine::GetModel(const char * name) const
{
	if( !name || strlen(name) == 0) {
		return NULL;
	}
	for( std::vector<Mesh*>::const_iterator it = meshCache.begin(); it != meshCache.end(); ++it ) {
		if( (*it)->GetName() == name ) {
			return *it;
		}
	}
	return NULL;
}

Texture * qEngine::GetTexture(const char * name) const
{
	if( !name || strlen(name) == 0 ) {
		return NULL;
	}
	for( std::vector<Texture*>::const_iterator it = texCache.begin(); it != texCache.end(); ++it ) {
		if( (*it)->GetName() == name ) {
			return *it;
		}
	}
	return NULL;
}


void qEngine::LoadMap(const char * map)
{
	if( !world ) {
		world = WorldDB::getInstance();
	}
	// init the world database
	if( !world->LoadMap(map) ) {
		common->FatalError("Aborting...");
	}
}


/* Load camera path from disk and expand the path way. In
 * a FPS game, camera should be attached to the view, therefore
 * will be controlled by the player. This feature allow developer
 * to spin camera around the world and examine rendering results.
 * Also it's an exercise of quaternion manipulation as well.
 */
void qEngine::LoadCameraPath(const char * pathFile)
{      
    qStr fn = qStr(pathFile);
    if( fn.Empty() ) {
        logger->LogWarning("Cannot load files outside engine directory");
        return;
    }    

    int avail = 0;
    for( ; avail < MAX_CAMERAPATH && cameraPath[avail] != NULL; avail++);
    if( avail == MAX_CAMERAPATH ) {
        logger->LogWarning("Reached maximum number of camera path.");
        return;
    }

    CameraPath * cp = new CameraPath(fn.Ptr());
    cp->ExpandPath();
    if( cp->GetNumFrames() == 0 ) {
        logger->LogWarning("Zero camera frame is loaded !");
        return;
    }    
   
    cameraPath[avail] = cp;
}

void qEngine::SetCurrentCameraPath(int id) 
{
    if( id < 0 || id >= MAX_CAMERAPATH ) {
        logger->LogWarning("CameraPath %d doesn't exist !", id);
        return;
    }

    currentCameraPath = cameraPath[id];
}

CameraPath* qEngine::GetCurrentCameraPath() const
{
    return currentCameraPath;
}

void qEngine::SetupCamera(float fov_y, float aspect, float zNear, float zFar)
{
    camera.fov = fov_y;
    camera.aspect = aspect;
    camera.zNear = zNear;
    camera.zFar = zFar;
}

// Attach the camera to the entity so camera is moving
// along with this entity.
// TODO Move to a separate class
void qEngine::AttachCamera(const Entity * entity)
{
	logger->LogNormal("Camera is attached to entity: %d", entity->GetId());
	attachedEntity = const_cast<Entity*>(entity);
}

void qEngine::DetachCamera()
{
	if( attachedEntity ) {
		logger->LogNormal("Camera is detached from entity: %d", attachedEntity->GetId());
		attachedEntity = NULL;
	}
}

void qEngine::UpdateWorld()
{
	timer->Tick();
    CameraPath * curCp = GetCurrentCameraPath();
    if( !curCp )
        return;

    camera_frame_t *cf = curCp->GetPlayingFrame();
	if( !cf ) {
		frameCount++;
        return;
	}
    Quaternion quat = cf->orientation;
    Mat3 axis = quat.ToMatrix();
	
	Vec3 forward = axis[2].Scale(-1.0f);
    Vec3 up = axis[1];
	Vec3 lookat = cf->position + forward;

    MoveCamera(cf->position, up);
    LookAt(lookat);
	
	// Proceed one camera frame
	curCp->Advance();
    logger->LogNormal("Frame: %d", frameCount);
    frameCount++;
}

// Heavy lifting
void qEngine::RenderFrame()
{
    
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	
    
	Entity *ent;

	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf(textureMatrix.GetRawPtr());

	glMatrixMode(GL_PROJECTION);
	SetProjectionMat();
	glLoadMatrixf(projectionMat.GetRawPtr());

	glMatrixMode(GL_MODELVIEW);
	SetViewMat();
	glLoadMatrixf(modelViewMat.GetRawPtr());

	glViewport(0, 0, windowWidth, windowHeight);


	glEnable(GL_CULL_FACE);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	// Why set color here?
    
    
	for( int i = 0; i < world->Count(); ++ i ) {
		ent = (*world)[i];
		RenderEntity(ent);
	}

}

// Draw normals vectors on the surface of entity
void qEngine::RenderNormal(Entity * entity)
{
	if( !entity ) {
		return;
	}

	vertex_t * verts = 	entity->GetModel()->GetVertexArray();
	unsigned int sz = 	entity->GetModel()->GetNumVert();
	float * buf = (float*)malloc( sz * 3 * 2 * sizeof(float));		// For vertex and normal
	float * start = buf;
	float scale = 0.5;
	for( size_t i = 0; i < sz; ++i ) {
		vertex_t * pv = verts + i;
		
		start[0] = pv->pos[0];
		start[1] = pv->pos[1];
		start[2] = pv->pos[2];

		start[3] = pv->pos[0] + pv->normal[0] * scale;
		start[4] = pv->pos[1] + pv->normal[1] * scale;
		start[5] = pv->pos[2] + pv->normal[2] * scale;

		start += 6;
	}

	entity->GetModel()->UnBind();
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glColor4f(1, 0, 0, 1);
	glVertexPointer(3, GL_FLOAT, 0, buf);
	glDrawArrays(GL_LINES, 0, sz);

	// Restore
	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	free(buf);
}


// For debugging purpose. By drawing a bounding box, it's easier
// to visualize the drawing and cullng process.
void qEngine::RenderBBox(Entity * entity)
{
	if( !entity )
		return;

	BBox box = entity->Bound();
	std::vector<Vec3> verts = box.GetVertex();
	if( verts.size() == 0 ) {
		logger->LogNormal("Entity %d has invalid bounding box", entity->GetId());
		return;
	}

	float * buf = (float*)malloc( verts.size() * 3 * sizeof(float));
	float * start = buf;
	for( size_t i = 0; i < verts.size(); ++i ) {
		*start++ = verts[i][0];
		*start++ = verts[i][1];
		*start++ = verts[i][2];
	}

	// Unbind the GL_ARRAY_BUFFER, so we can use it to draw
	// basic geometry
	entity->GetModel()->UnBind();

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glColor4f(0, 1, 0, 1);
	glVertexPointer(3, GL_FLOAT, 0, buf);
	glDrawArrays(GL_LINES, 0, verts.size());

	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	free(buf);
}

void qEngine::RenderSilhouette(Entity * entity, light_t * l)
{
    if( l == NULL || entity == NULL ) {
        return;
    }

	GetSilhouette(entity, l);

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    // Is this a cool color for silhouette
    glColor4f(0.5, 0.5, 0.5, 0.8);
   
    
    glColor4f(1, 1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

}


void qEngine::RenderEntity(Entity * entity)
{
	// We are in GL_MODELVIEW mode
	glPushMatrix();

	Mat4 matrix = entity->GetModelToWorldMat();
	glMultMatrixf(matrix.GetRawPtr());

	if (!entity->GetModel()->IsUploaded()) {
		entity->GetModel()->UploadGPU();
	}


	qStr texName = entity->GetModel()->GetTexName();
	if( !texName.Empty() ) {
		Texture * tex = GetTexture(texName.Ptr());
		if( !tex ) {
			logger->LogWarning("Cannot find texture for entity");
			//fprintf(stderr, "Cannot find texture for entity\n");
		} else {
			if( !tex->IsUploaded() ) {
				tex->UploadGPU();
			}
			entity->AttachTexture(tex);
		}
	}
	float specularColor[3] = {1, 1, 1};
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

	glVertexPointer(3, GL_FLOAT, sizeof(vertex_t), 0 /*(char*)((char*)&entity->GetModel()->GetVertexArray()[0].pos - (char*)&entity->GetModel()->GetVertexArray()[0].pos)*/);
	glTexCoordPointer(2, GL_SHORT, sizeof(vertex_t), (char*)((char*)&entity->GetModel()->GetVertexArray()[0].st[0] - (char*)&entity->GetModel()->GetVertexArray()[0].pos));
	glNormalPointer(     GL_FLOAT, sizeof(vertex_t), (char*)((char*)&entity->GetModel()->GetVertexArray()[0].normal[0] - (char*)&entity->GetModel()->GetVertexArray()[0].pos));
	glDrawElements(GL_TRIANGLES, entity->GetModel()->GetNumIndex(), GL_UNSIGNED_SHORT, entity->GetModel()->GetIndexArray());

	if( GetFrameCount() == 100 ) {
		Snapshot();
	}

    GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
        logger->LogWarning("GLerror: %u", err);
	}

	
	//RenderBBox(entity);
	//RenderNormal(entity);
    RenderSilhouette(entity, GetDefaultLight());

	glPopMatrix();
}

void qEngine::GetColorBuffer(unsigned char * buf)
{
	if( !buf ) {
		return;
	}
	glReadPixels(0, 0, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf);
}

// By taking a screenshot developer can discern the subtleties
// before and after activation of some features, such as
void qEngine::Snapshot()
{
	// Save screenshot in local folder 'screenshot'
	if( !File::DirExist("screenshot") ) {
		File::CreateDir("screenshot");
	}

	qStr name("screenshot/");
	name.ConcatSelf( common->GetTime() );
    name.AppendExtension("tga");
    // RGBA
    unsigned char * data = (unsigned char*)calloc(windowWidth * windowHeight, 4 * sizeof(unsigned char));
    if( !data ) {
        common->FatalError("qEngine Snapshot: Cannot locate more memory");
    }
    GetColorBuffer(data);

    unsigned char header[18];
    memset(header, 0, 18 * sizeof(unsigned char));
    header[2] = 2;
    header[12] = ( windowWidth & 0x00ff );
    header[13] = ( windowWidth & 0xff00 ) / 256;
    header[14] = ( windowHeight & 0x00ff);
    header[15] = ( windowHeight & 0xff00) / 256;
    header[16] = 32;

    File fh(name.Ptr(), "wb");
    fh.Write(header, 18, sizeof(unsigned char));

    // Convert RGB to BGR
    unsigned char * pixel = data;
    for( unsigned int i = 0; i < windowHeight * windowWidth; ++i ) {
    	unsigned char tmp = pixel[0];	// R;
    	pixel[0] = pixel[2];
    	pixel[2] = tmp;
    	pixel += 4;
    }
    fh.Write(data, windowHeight * windowWidth, 4 * sizeof(unsigned char));
    //fh.Close();
}

void qEngine::AddLight(light_t * l)
{
    numLights++;
    if( !lights ) {
        lights = l;
        lights->next = NULL;
        return;
    }

    light_t * first = l->next;
    lights->next = l;
    l->next = first;
}

light_t* qEngine::GetDefaultLight()
{
    for( light_t* l = lights; l != NULL; l = l->next ) {
        if( l->id == 0 ) {
            return l;
        }
    }
    return NULL;
}


/*=====================================================
 *
 * Silhouette edge determination algorithm
 *
 *======================================================
 */

static void R_SilCullFacing(unsigned short* tri, int numIndex, vertex_t* vert, light_t* light, silhouette_t& sil)
{
    if( numIndex <= 0) {
        return;
    }
    int numTri = numIndex / 3;
    sil.facing = (byte*)malloc(numTri * sizeof(byte));
    for( int i = 0; i < numTri; ++i ) {
        Vec3 v1 = vert[tri[i*3]].pos;
        Vec3 v2 = vert[tri[i*3+1]].pos;
        Vec3 v3 = vert[tri[i*3+2]].pos;

        Plane pl(v1, v2, v3);
		Vec3 pos(light->pos[0], light->pos[1], light->pos[2]);
        sil.facing[i] = pl.Side(pos) == PS_OUT ? 1 : 0;
    }
} 

#if 1
static void R_DebugEdgeArr(const qArr<edge_t>& edges)
{
    printf("================ Sorted Edge ==================\n");
    for( int i = 0; i < edges.Size(); ++i ) {
        edge_t e = edges[i];
        printf("%d: %d -> %d\n", i, e.p1, e.p2);
    }
    printf("================ End ==========================\n");
}
#endif

static int edge_cmp_f(const void * a, const void * b) 
{
    edge_t * ea = (edge_t*) a;
    edge_t * eb = (edge_t*) b;
    if( ea->p1 == eb->p1 ) {
        if( ea->p2 == eb->p2 ) {
            return 0;
        } 
        else if( ea->p2 < eb->p2 ) {
            return -1;
        } else {
            return 1;
        }
    } 
    else if( ea->p1 < eb->p1 ) {
        return -1;
    } else {
        return 1;
    }
}

// For faster accessing a sorted edge_t array. We use a
// index array to provide random access on p1
struct siledge_index_t
{
    siledge_index_t()
    {
        index = NULL;
    }

    siledge_index_t(size_t capacity)
    {
        index = (int*)malloc(capacity * sizeof(int));
    }

    int*    index;
};

static void R_SilIndexing(qArr<edge_t>& edges, siledge_index_t** index)
{
    if( *index )
        return;

    int maxIndex = edges.Last().p1;
    *index = new siledge_index_t(maxIndex);
    siledge_index_t * si = *index;
    memset(si->index, 0, sizeof(int) * maxIndex);

    for( int i = 0; i < edges.Size(); ++i ) {
        int j = edges[i].p1;
        if( !si->index[j] ) {
            si->index[j] = i;
        }
    }
}


// Assuming edges are sorted, elimenate dangling edges. Hopefully there
// aren't many of them
static void R_SilPrune(qArr<edge_t>& edges, siledge_index_t** index)
{
    if( !*index ) {
        R_SilIndexing(edges, index);
    }
    siledge_index_t * idx = *index;

    qArr<edge_t> processed(edges.Size());
    for( int i = 0; i < edges.Size(); ++i ) {
        int p1 = edges[i].p1;
        int p2 = edges[i].p2;
        if( !idx->index[p2] ) {
            continue;
        }
        bool found = false;
        for( int j = idx->index[p2]; j < idx->index[p2+1]; ++j ) {
            if( edges[j].p2 == p1 )
                found = true;
        }
        if( !found ) {
            continue;
        }
        processed.Add(edges[i]);
    }
    edges.Replace(processed);
}

static int R_SilFindOtherTri(qArr<edge_t>& edges, edge_t ea, siledge_index_t* si)
{
    int k = si->index[ea.p2];
    for( int j = k; j < si->index[ea.p2+1]; ++j ) {
        edge_t eb = edges[j];
        if( eb.p2 == ea.p1 ) {
            return eb.tri;
        }
    }
    // Should never happen
    return -1;
}


// Mess around with edges. Inefficient algorithm could be nightmare here
static void R_SilFillEdge(Mesh* model, light_t* light, silhouette_t& sil)
{   
    qArr<edge_t> es = model->GenEdgeList();
    qsort(es.Ptr(), es.Size(), sizeof(edge_t), edge_cmp_f);
    
    siledge_index_t * si = NULL;

    R_SilPrune(es, &si);

    for( int i = 0; i < es.Size(); ++i ) {
        edge_t k = es[i];
        if( k.marked )
            continue;
    
        siledge_t* se = &sil.sil[sil.numSilEdges++];
        se->v1 = k.p1;
        se->v2 = k.p2;

        se->p1 = k.tri;
        // find another triangle share this edge
        se->p2 = R_SilFindOtherTri(es, k, si);
		k.marked = 1;
    }
}

// Get silhoutte from current light
silhouette_t* qEngine::GetSilhouette(const Entity * entity, light_t * light)
{
    if( !entity || !light ) {
        return NULL;
    }
    silhouette_t* sil = (silhouette_t*)calloc(1, sizeof(*sil));
    sil->sil = (siledge_t*)calloc(1, sizeof(siledge_t) * entity->GetModel()->GetNumIndex());

    Mesh* model = entity->GetModel();
    R_SilFillEdge  (model, light, *sil);
    R_SilCullFacing(model->GetIndexArray(), model->GetNumIndex(), model->GetVertexArray(), light, *sil);

    if( !sil->facing ) {
        logger->LogWarning("Failed generating facing info for entity");
        return NULL;
    }

	return sil;
}



