#include "VoxelTester.h"
#include "Voxelizer.h"
#include "SVONode.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

VoxelTester::VoxelTester()
{

}

VoxelTester::~VoxelTester()
{
	delete voxelModel;
	voxelModel = nullptr;
}

void VoxelTester::init() {
	shader = Shader("observer.vert", "observer.frag", "observer.geom");
	setupModel();
}

void VoxelTester::createVoxelsFromContainer(VoxelContainer &voxels) {
	int WIDTH = voxels.getDim();
	int HEIGHT = voxels.getHeight();
	int DEPTH = voxels.getDim();

	for (int x = 0; x < WIDTH; x++)
	for (int y = 0; y < HEIGHT; y++)
	for (int z = 0; z < DEPTH; z++) {
		GLfloat val = voxels.getValue(x, y, z);
		if (val > 0.0) {
			voxelList.push_back(Voxel(glm::vec3(x, y, z), 1, false));
		}
	}
}

void VoxelTester::updateSVOChildren(SVONode* node) {
	// Loop through all children, check if they are leaves/empty and do same thing for their children
	for (int i = 0; i < 8; i++) {
		SVONode* currChildNode = node->children.at(i);

		if (currChildNode->isLeaf && currChildNode->containsVoxel) {
			voxelList.push_back(Voxel(currChildNode->getPos(), currChildNode->size, false));
		}
		else if (currChildNode->isLeaf) {
			voxelList.push_back(Voxel(currChildNode->getPos(), currChildNode->size, true));
		}
		else {
			updateSVOChildren(currChildNode);
		}
	}
}

void VoxelTester::createVoxelsFromSVO(SVO* svo) {
	SVONode* topNode = svo->getTopNode();
	updateSVOChildren(topNode);
}

void VoxelTester::createVoxelsFromTexture(GLuint& voxTex, GLint voxTexDim) {
	// Loop through the texture and set voxels in the list to appropriate positions
	int WIDTH = voxTexDim;
	int HEIGHT = voxTexDim;
	int DEPTH = 8;

	GLuint *testArr = new GLuint[WIDTH*HEIGHT*DEPTH*4];
	glGetTextureImage(voxTex, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, 4*WIDTH*HEIGHT*DEPTH*sizeof(GLuint), testArr);

	for (int x = 0; x < WIDTH; x++)
	for (int y = 0; y < HEIGHT; y++)
	for (int z = 0; z < DEPTH; z++) {
		//GLuint childX = testArr[(HEIGHT*WIDTH*z + WIDTH*y + x) * 4 + 0];
		//GLuint childY = testArr[(HEIGHT*WIDTH*z + WIDTH*y + x) * 4 + 1];
		GLuint size = testArr[(HEIGHT*WIDTH*z + WIDTH*y + x) * 4 + 2];
		GLuint leafStatus = testArr[(HEIGHT*WIDTH*z + WIDTH*y + x) * 4 + 3];
		
		glm::vec3 nodePos;

		if (leafStatus > 0) {
			// Means we have something here
			if (leafStatus == 1) {
				voxelList.push_back(Voxel(glm::vec3(x, y, z), size, true));	// "empty" voxel
			}
			else {
				voxelList.push_back(Voxel(glm::vec3(x, y, z), size, false));	// not "empty"
			}			
		}
	}

	printf("Size of the voxellist is after creation: %d\n", voxelList.size());

	// Free resources
	delete[](testArr);
}

void VoxelTester::render(glm::mat4& projMatrix, glm::mat4& camMatrix) {
	// Loop through all voxel objects and draw em
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	shader.activate();
	glm::vec3 white = {1.0, 1.0, 1.0};
	glm::vec3 red = { 0.4, 0.0, 0.0 };

	for (Voxel &voxel : voxelList) {
		GLint size = voxel.getSize();

		if (voxel.getEmpty())// || size <= 2)
			continue;

		voxelModel->prepare();		

		// Uploads
		shader.uploadMatrix(projMatrix, "projMatrix");
		shader.uploadMatrix(camMatrix, "camMatrix");
		if (voxel.getEmpty()) {
			//shader.uploadVec(glm::vec3((GLfloat) size/128.0), "color");
			shader.uploadVec(white, "color");
		}
		else {
			shader.uploadVec(white, "color");
		}
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), voxel.getPos());
		shader.uploadInt(size, "scale");
		shader.uploadMatrix(translationMatrix, "modelMatrix");

		voxelModel->render();
	}

	shader.deactivate();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void VoxelTester::setupModel() {
	GLfloat *vertexArray = (GLfloat*)malloc(8 * 3 * sizeof(GLfloat));
	GLuint *indexArray = (GLuint*)malloc(12 * 3 * sizeof(GLuint));

	// 8 vertices
	vertexArray[0] = -.5;
	vertexArray[1] = .5;
	vertexArray[2] = -.5;

	vertexArray[3] = -.5;
	vertexArray[4] = -.5;
	vertexArray[5] = -.5;

	vertexArray[6] = .5;
	vertexArray[7] = .5;
	vertexArray[8] = -.5;

	vertexArray[9] = .5;
	vertexArray[10] = -.5;
	vertexArray[11] = -.5;

	vertexArray[12] = -.5;
	vertexArray[13] = .5;
	vertexArray[14] = .5;

	vertexArray[15] = -.5;
	vertexArray[16] = -.5;
	vertexArray[17] = .5;

	vertexArray[18] = .5;
	vertexArray[19] = .5;
	vertexArray[20] = .5;

	vertexArray[21] = .5;
	vertexArray[22] = -.5;
	vertexArray[23] = .5;

	// Indices, 12 triangles
	indexArray[0] = 0;
	indexArray[1] = 1;
	indexArray[2] = 2;

	indexArray[3] = 2;
	indexArray[4] = 1;
	indexArray[5] = 3;

	indexArray[6] = 2;
	indexArray[7] = 3;
	indexArray[8] = 6;

	indexArray[9] = 6;
	indexArray[10] = 3;
	indexArray[11] = 7;

	indexArray[12] = 4;
	indexArray[13] = 5;
	indexArray[14] = 0;

	indexArray[15] = 0;
	indexArray[16] = 5;
	indexArray[17] = 1;

	indexArray[18] = 6;
	indexArray[19] = 7;
	indexArray[20] = 4;

	indexArray[21] = 4;
	indexArray[22] = 7;
	indexArray[23] = 5;

	indexArray[24] = 4;
	indexArray[25] = 0;
	indexArray[26] = 6;

	indexArray[27] = 6;
	indexArray[28] = 0;
	indexArray[29] = 2;

	indexArray[30] = 1;
	indexArray[31] = 5;
	indexArray[32] = 3;

	indexArray[33] = 3;
	indexArray[34] = 5;
	indexArray[35] = 7;

	// init the model
	voxelModel = new DrawableModel(vertexArray, nullptr, indexArray);
	ShaderList shaders;
	shaders.push_back(shader);
	voxelModel->init(shaders, 8, 12);
}