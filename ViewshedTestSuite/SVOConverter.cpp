#include "SVOConverter.h"

extern "C" {
#include "GL_utilities.h"
}

SVOConverter::SVOConverter()
{
	// Meh
}

void SVOConverter::insertChildrenInTexture(GLuint *data, SVONode* node, glm::vec2 nodePos) {
	// NOTE: nodePos is in x,y texture coordinates (not world coordinates or anything like that)
	glm::vec2 childPos;

	for (int i = 0; i < NUM_CHILDREN_PER_NODE; i++) {
		// First see if the child node is a leaf or not
		SVONode* childNode = node->children.at(i);
		GLboolean isLeaf = childNode->isLeaf;
		GLboolean containsVoxel = childNode->containsVoxel;

		// If it is a leaf and also contains a voxel, write a 2 to the A channel
		if (isLeaf && containsVoxel) {
			insertInArr(data, 2, nodePos.x, nodePos.y, i, 3);
			insertInArr(data, childNode->size, nodePos.x, nodePos.y, i, 2);
		}
		// If it's only a leaf but doesn't contain a voxel, write a 1 to the A channel
		else if (isLeaf) {
			insertInArr(data, 1, nodePos.x, nodePos.y, i, 3);
			insertInArr(data, childNode->size, nodePos.x, nodePos.y, i, 2);
		}
		// if it is not, we write the position of that child node to the texture
		else {
			childPos = getNextAvailableCoord();
			insertInArr(data, childPos.x, nodePos.x, nodePos.y, i, 0);
			insertInArr(data, childPos.y, nodePos.x, nodePos.y, i, 1);
			insertInArr(data, childNode->size, nodePos.x, nodePos.y, i, 2);

			// Also recursively do the same thing for all children of this child (scary)
			insertChildrenInTexture(data, childNode, childPos);
		}
	}
}

GLuint& SVOConverter::buildTexture(SVO* svo, GLint numNodes) {
	// Calculate width and height
	GLint width = ceil(sqrt(numNodes));
	this->dim = width;
	printf("SVOConverter (tex setup): Height and width became: %d, numNodes was %d\n", width, numNodes);

	// Build an array on CPU that we copy in to the texture once it's done
	GLuint* data = new GLuint[width*width*NUM_CHILDREN_PER_NODE * 4];	// 4 channels
	std::fill(data, data + width*width*NUM_CHILDREN_PER_NODE * 4, 0);

	SVONode* topNode = svo->getTopNode();

	// Loop through the children and set corresponding RGB-values in texture
	insertChildrenInTexture(data, topNode, glm::vec2(0, 0));
	// This is a recursive function, so all subsequent children will also be added

	// DEBUG
	/*for (int z = 0; z < NUM_CHILDREN_PER_NODE; z++)
		for (int y = 0; y < 86; y++)
			for (int x = 0; x < 333; x++) {
				GLubyte val = data[(width*width*z + width*y + x) * 4 + 0];
				printf("x-coord written to at %d %d %d is %d\n", x, y, z, val);
			}*/
	//printf("x-coord at width, width, 7: %d\n", data[(width*width * 7 + width * (width-1) + width-1) * 4]);

	// Now setup the texture
	glGenTextures(1, &texId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, texId);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16UI, width, width, NUM_CHILDREN_PER_NODE, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, data);
	printError("after filling texture");

	glBindTexture(GL_TEXTURE_3D, 0);

	delete[](data);
	return texId;
}

void SVOConverter::insertInArr(GLuint* data, GLuint val, GLint x, GLint y, GLint z, GLint channel) {
	data[(dim*dim*z + dim*y + x) * 4 + channel] = val;
}

glm::vec2 SVOConverter::getNextAvailableCoord() {
	if (currentTexturePos.x + 1 != dim) {
		if (currentTexturePos.y != dim) {
			currentTexturePos.x = currentTexturePos.x + 1;
			return currentTexturePos;
		}
	}
	else {
		currentTexturePos.x = 0;
		currentTexturePos.y = currentTexturePos.y + 1;
		return currentTexturePos;
	}
}
