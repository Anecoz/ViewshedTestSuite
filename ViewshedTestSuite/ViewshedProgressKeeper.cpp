#include "ViewshedProgressKeeper.h"
#include <algorithm>


ViewshedProgressKeeper::ViewshedProgressKeeper()
{

}

GLboolean ViewshedProgressKeeper::isDone() {
	return done;
}

void ViewshedProgressKeeper::start(GLint numObs) {
	done = false;
	remaining = numObs;
}

GLint ViewshedProgressKeeper::getNumberOfCallsForFrame(GLfloat elapsedTime) {
	if (!done) {
		GLfloat availableTime = MAX_FRAME_TIME - elapsedTime;

		if (availableTime <= 0.0) {
			return 1;	// Always at least do 1
			updateRemaining(1);
		}
		else {
			GLint num = std::min(availableTime / TIME_PER_OBS, (GLfloat)remaining);
			updateRemaining(num);
			return num;
		}
	}	
}

void ViewshedProgressKeeper::updateRemaining(GLint dec) {
	remaining -= dec;

	if (remaining == 0) {
		done = true;
	}
}