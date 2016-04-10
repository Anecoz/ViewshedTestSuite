#include "SVONode.h"

SVONode::SVONode() {

}

SVONode::SVONode(GLint initSize, glm::vec3 initPos)
{
	this->size = initSize;
	this->pos = initPos;

	minX = pos.x - (GLfloat)size / 2.0;
	minY = pos.y - (GLfloat)size / 2.0;
	minZ = pos.z - (GLfloat)size / 2.0;

	maxX = pos.x + (GLfloat)size / 2.0;
	maxY = pos.y + (GLfloat)size / 2.0;
	maxZ = pos.z + (GLfloat)size / 2.0;
}

SVONode::~SVONode() {
	for (SVONode *child : children) {
		delete child;
		// Rest will be garbage collected
	}
}

void SVONode::split() {
	// Split into 8 octants and put into children
	isLeaf = false;
	GLint childSize = this->size / 2;

	for (int i = 0; i < 8; i++) {
		glm::vec3 currChildPos = calcChildPos(i + 1);
		children.push_back(new SVONode(childSize, currChildPos));
	}
}

glm::vec3 SVONode::calcChildPos(GLint octant) {
	// Some helpers	
	GLfloat xHelp = (pos.x - (GLfloat)minX) / 2.0;
	GLfloat yHelp = (pos.y - (GLfloat)minY) / 2.0;
	GLfloat zHelp = (pos.z - (GLfloat)minZ) / 2.0;

	switch (octant) {
	case 1:
		return glm::vec3((GLfloat)minX + xHelp, (GLfloat)minY + yHelp, pos.z + zHelp);
		break;
	case 2:
		return glm::vec3(pos.x + xHelp, (GLfloat)minY + yHelp, pos.z + zHelp);
		break;
	case 3:
		return glm::vec3((GLfloat)minX + xHelp, (GLfloat)minY + yHelp, (GLfloat)minZ + zHelp);
		break;
	case 4:
		return glm::vec3(pos.x + xHelp, (GLfloat)minY + yHelp, (GLfloat)minZ + zHelp);
		break;
	case 5:
		return glm::vec3((GLfloat)minX + xHelp, pos.y + yHelp, pos.z + zHelp);
		break;
	case 6:
		return glm::vec3(pos.x + xHelp, pos.y + yHelp, pos.z + zHelp);
		break;
	case 7:
		return glm::vec3((GLfloat)minX + xHelp, pos.y + yHelp, (GLfloat)minZ + zHelp);
		break;
	case 8:
		return glm::vec3(pos.x + xHelp, pos.y + yHelp, (GLfloat)minZ + zHelp);
		break;
	default:
		break;
	}
}

GLboolean SVONode::isInside(glm::vec3& inPos) {
	if (inPos.x >= minX && inPos.x <= maxX &&
		inPos.y >= minY && inPos.y <= maxY &&
		inPos.z >= minZ && inPos.z <= maxZ) {
		return true;
	}
	else {
		return false;
	}
}

GLuint SVONode::getOctant(glm::vec3& inPos) {
	// Already checked if inPos is inside the node volume
	// Big effin if
	if (inPos.x <= pos.x && inPos.y <= pos.y && inPos.z >= pos.z) {
		return 1;
	} 
	else if (inPos.x >= pos.x && inPos.y <= pos.y && inPos.z >= pos.z) {
		return 2;
	}
	else if (inPos.x <= pos.x && inPos.y <= pos.y && inPos.z <= pos.z) {
		return 3;
	}
	else if (inPos.x >= pos.x && inPos.y <= pos.y && inPos.z <= pos.z) {
		return 4;
	}
	else if (inPos.x <= pos.x && inPos.y >= pos.y && inPos.z >= pos.z) {
		return 5;
	}
	else if (inPos.x >= pos.x && inPos.y >= pos.y && inPos.z >= pos.z) {
		return 6;
	}
	else if (inPos.x <= pos.x && inPos.y >= pos.y && inPos.z <= pos.z) {
		return 7;
	}
	else {
		return 8;
	}
}