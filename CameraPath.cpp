#include "CameraPath.h"


CameraPath::CameraPath(const char *filename) : startFrame(0), endFrame(0), playingFrame(0), numKeyFrames(0), numFrames(0)
{
	LoadPath(filename);
}

void CameraPath::LoadPath(const char *filename)
{
	if (!filename || filename[0] == '\0') {
		//engine->GetLogger()->LogWarning("Bad filename is given as camera path file");
		return;
	}

	qStr path(filename);
	if (path.GetFileExtension() != "cp") {
		//engine->GetLogger()->LogWarning("Wrong extension for cp file");
		return;
	}

	LexerFile lex(path.Ptr());
	// Parse camera path file
	lex.ReadToken();
	if (lex.TokenValue() != "cp1") {
		//engine->GetLogger()->LogWarning("Bad magic header for camera path file");
		return;
	}

	lex.ReadToken();
	lex.ReadToken();
	numKeyFrames = lex.TokenValue().ToInteger();
	if (numKeyFrames <= 0) {
		return;
	}

	
	camera_frame_t * lastFrame = NULL, *head;
	Vec3 lookAt, up;
	for (int i = 0; i < numKeyFrames; ++i) {
		camera_frame_t * cf = (camera_frame_t*)malloc(sizeof(camera_frame_t));
		memset(cf, 0, sizeof(*cf));

		// first frame
		if (!lastFrame)	
			cf->id = 0;
		else
			cf->id = lastFrame->id + 1;
		// 'time' label
		lex.ReadToken();	
		lex.ReadToken();
		qStr time = lex.TokenValue();
		if (time == "00000")
			cf->time = 0;
		else
			cf->time = time.ToInteger();

		// position label
		lex.ReadToken();
		for (int k = 0; k < 3; ++k) {
			lex.ReadToken();
			cf->position[k] = lex.TokenValue().ToFloat();
		}

		// lookat label
		lex.ReadToken();
		for (int k = 0; k < 3; ++k) {
			lex.ReadToken();
			lookAt[k] = lex.TokenValue().ToFloat();
		}

		// up label
		lex.ReadToken();
		for (int k = 0; k < 3; ++k) {
			lex.ReadToken();
			up[k] = lex.TokenValue().ToFloat();
		}

		// Calculate quaternion
		Vec3 zAxis = cf->position -	lookAt;
		zAxis = zAxis.Normalize();
		up = up.Normalize();	
        
        Vec3 xAxis = up.CrossProduct(zAxis);
        Mat3 rotMat;
        rotMat[0][0] = xAxis[0]; rotMat[1][0] = up[0]; rotMat[2][0] = zAxis[0];
        rotMat[0][1] = xAxis[1]; rotMat[1][1] = up[1]; rotMat[2][1] = zAxis[1];
        rotMat[0][2] = xAxis[2]; rotMat[1][2] = up[2]; rotMat[2][2] = zAxis[2];

        cf->orientation = Quaternion::FromMatrix(rotMat);
        if( lastFrame ) {
        	lastFrame->next = cf;
        } else {
        	head = cf;
        }
        lastFrame = cf;
	}

	startFrame = head;
    endFrame = lastFrame;
	playingFrame = startFrame;
}

void CameraPath::ExpandPath()
{
    if( numKeyFrames < 2 )
        return;

    int diff = endFrame->time - startFrame->time;
    int timeStep = 0;
    float extraAccuracyTimeStep = 0;

    camera_frame_t * currentFrame = startFrame, * newFrame;

    while( currentFrame->time < endFrame->time ) {
        newFrame = (camera_frame_t*)calloc(1, sizeof(camera_frame_t));
        newFrame->time = currentFrame->time + timeStep;         
        newFrame->next = endFrame;
        currentFrame->next = newFrame;

        float factor = (newFrame->time - startFrame->time) / (float)diff;
        // interpolating position
        currentFrame->position = startFrame->position.Scale(factor) + endFrame->position.Scale(1-factor);
        // interpolating orientation
        currentFrame->orientation = (startFrame->orientation).Slerp(endFrame->orientation, factor);

        currentFrame = newFrame;
        // What's the trick here? 
        extraAccuracyTimeStep += 0.6666667f;
        timeStep = 16 + (int)extraAccuracyTimeStep;
        extraAccuracyTimeStep -= (int)extraAccuracyTimeStep;
        numFrames++;
    }  

} 


const camera_frame_t*  CameraPath::GetRawPtr() const
{
    return startFrame;
}

int CameraPath::GetNumFrames() const
{
    return numFrames;
}

void CameraPath::Advance()
{
    if( playingFrame != NULL ) {
        playingFrame = playingFrame->next;
    }
}

camera_frame_t * CameraPath::GetPlayingFrame() const
{
    return playingFrame;
}




