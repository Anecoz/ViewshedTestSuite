#include "SVO.h"

SVO::SVO(GLint startSize, glm::vec3 startPos)
{
	// Make one node the size of startSize
	topNode = SVONode(startSize, startPos);
}

void SVO::insert(glm::vec3& voxelPos) {
	// Go down the tree until the target size is reached and insert leaf node there
	if (topNode.isInside(voxelPos)) {
		if (!topNode.isLeaf) {
			GLuint octant = topNode.getOctant(voxelPos);

			GLuint splitCounter = 0;

			// Traverse down until done
			GLboolean done = false;
			SVONode* currNode = topNode.children.at(octant - 1);
			while (!done) {
				if (!currNode->isLeaf) {
					GLuint octant = currNode->getOctant(voxelPos);
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

			printf("splitCounter is: %d\n", splitCounter);
		}
		else {
			topNode.split();
			insert(voxelPos);	// Recursive... no idea what happens to memory, might be fine
			return;
		}
	}
}