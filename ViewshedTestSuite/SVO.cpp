#include "SVO.h"

SVO::SVO(GLint startSize, glm::vec3 startPos)
{
	// Make one node the size of startSize
	topNode = SVONode(startSize, startPos);

	// Split the topnode immediately, since we will always do that anyway
	topNode.split();
}

SVONode* SVO::getTopNode() {
	return &topNode;
}

GLint SVO::insert(glm::vec3& voxelPos) {
	GLuint splitCounter = 0;
	// Go down the tree until the target size is reached and insert leaf node there
	if (topNode.isInside(voxelPos)) {
		GLuint octant = topNode.getOctant(voxelPos);		

		// Traverse down until done
		GLboolean done = false;
		SVONode* currNode = topNode.children.at(octant - 1);
		while (!done) {
			if (!currNode->isLeaf) {
				octant = currNode->getOctant(voxelPos);
				currNode = currNode->children.at(octant - 1);
			}
			else if (currNode->size == TARGET_SIZE) {
				currNode->containsVoxel = true;
				done = true;
			}
			else {
				currNode->split();
				splitCounter += 1;
			}
		}
	}

	return splitCounter;
}