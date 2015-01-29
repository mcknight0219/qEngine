#ifndef _CAMERAPATH_H
#define _CAMERAPATH_H

#include <vector>

#include "File.h"
#include "Math.h"
#include "Quaternion.h"

struct camera_frame_t
{
	unsigned short	id;
	unsigned int	time;
	Vec3			position;
	Quaternion		orientation;
	camera_frame_t *next;
};

class CameraPath
{
public:
					        CameraPath(const char * filename);	// Load camera path from a file
	void			        LoadPath(const char * filename);
    void                    ExpandPath();
    int                     GetNumFrames() const;
    const camera_frame_t *  GetRawPtr() const;
    camera_frame_t *        GetPlayingFrame() const;

    // Camera frame movement
    void                    Advance();
    //void                    Rewind();

private:
	camera_frame_t* startFrame;
    camera_frame_t* endFrame;  // for convenience of interpolation
    camera_frame_t* playingFrame;
	unsigned short	numKeyFrames;
    unsigned short  numFrames;

private:
	// Disable default and assignment ctor
	CameraPath() {}
	CameraPath(const CameraPath&) {}
};


#endif /* !_CAMERAPATH_H_ */
