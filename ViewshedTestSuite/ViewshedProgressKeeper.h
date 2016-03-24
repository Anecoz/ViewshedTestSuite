#pragma once

#include <GL\glew.h>
#include <GL\freeglut.h>

/*
Responsible for keeping track of how many observer depth maps there are to do.
Calculates, based on elapsed frame time, how many calculations we will be able to do in 1 frame
*/
class ViewshedProgressKeeper
{
public:
	ViewshedProgressKeeper();
	~ViewshedProgressKeeper() = default;

	GLboolean isDone();
	void start(GLint numObs);
	
	GLint getNumberOfCallsForFrame(GLfloat elapsedTime);

private:
	GLboolean done = true;
	GLint remaining;

	const GLfloat TIME_PER_OBS = 2.0;	// How many milliseconds does one observer render take? use a gross overstatement
	const GLfloat MAX_FRAME_TIME = (1.0 / 30.0)*1000.0;

	void updateRemaining(GLint);
};

