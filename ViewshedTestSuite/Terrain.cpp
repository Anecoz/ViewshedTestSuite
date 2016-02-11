#include "Terrain.h"
#include "FileHandler.h"
#include <vector>

using namespace std;


Terrain::Terrain()
{
	
}

Terrain::~Terrain()
{
	// Take care of all VBOs in here, freeing them
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &indexVBO);
	glDeleteBuffers(1, &normalVBO);

	free(vertexArray);
	free(indexArray);
}

GLuint* Terrain::getVertexVBO() {
	return &vertexVBO;
}

GLuint* Terrain::getIndexVBO() {
	return &indexVBO;
}

const GLuint Terrain::getTriangleCount() {
	return TRIANGLE_COUNT;
}

void Terrain::init() {
	shader = Shader("terrain.vert", "terrain.frag");
}

void Terrain::render(glm::mat4 camMatrix, glm::mat4 projMatrix, glm::mat4 lightSpaceMatrix, GLuint& depthMap) {
	shader.activate();
	glBindVertexArray(vao);

	// Bind texture
	activateDepthTexture();
	glBindTexture(GL_TEXTURE_2D, depthMap);
	shader.uploadTexture(0, "depthMap");

	// matrix uploads
	shader.uploadMatrix(camMatrix, "camMatrix");
	shader.uploadMatrix(projMatrix, "projMatrix");
	shader.uploadMatrix(lightSpaceMatrix, "lightSpaceMatrix");
	glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT*3, GL_UNSIGNED_INT, 0L);

	shader.deactivate();
}

void Terrain::activateDepthTexture() {
	glActiveTexture(GL_TEXTURE0);
}

void Terrain::generate() {
	// Check first if terrain already exists on file
	/*if (FileHandler::checkIfExists(VERTEX_FILE_NAME)) {
		printf("Terrain is on file, loading...\n");

		// Load vertices, indices and normals from file
		vertexArray = FileHandler::loadFloatArrFromFile(VERTEX_FILE_NAME);
		printf("vertexArray loaded\n");
		normalArray = FileHandler::loadFloatArrFromFile(NORMAL_FILE_NAME);
		printf("normalArray loaded\n");
		indexArray = FileHandler::loadUintArrFromFile(INDEX_FILE_NAME);
		printf("indexArray loaded\n");

		if (vertexArray != nullptr && normalArray != nullptr && indexArray != nullptr)
			printf("Loaded terrain from file!\n");
		else {
			printf("Could not load terrain from file, exiting.\n");
			exit(4);
		}	
	}
	else {*/
		printf("Terrain is not on file, generating on CPU\n");

		// Start with setting some constants
		GLint width = TILE_SIZE;
		GLint height = TILE_SIZE;

		// Vectors to hold vertex data as well as indices
		indexArray = (GLuint *)malloc(sizeof(GLuint) * TRIANGLE_COUNT * 3);
		vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * VERTEX_COUNT * 3);
		normalArray = (GLfloat *)malloc(sizeof(GLfloat) * VERTEX_COUNT * 3);

		// Now loop over both x and z and set the vertices +  normals
		for (int x = 0; x < width; x++)
			for (int z = 0; z < height; z++) {
				GLfloat height = generator.getHeight((GLfloat)x, (GLfloat)z);
				vertexArray[(x + z * width) * 3 + 0] = (GLfloat)x;
				vertexArray[(x + z * width) * 3 + 1] = height; // Height (this is 2.5D)
				vertexArray[(x + z * width) * 3 + 2] = (GLfloat)z;

				glm::vec3 normal = calcNormal((GLfloat)x, height, (GLfloat)z);
				// Set this in the array
				normalArray[(x + z * width) * 3 + 0] = normal.x;
				normalArray[(x + z * width) * 3 + 1] = normal.y;
				normalArray[(x + z * width) * 3 + 2] = normal.z;
			}

		// Same for indices
		for (int x = 0; x < width - 1; x++)
			for (int z = 0; z < height - 1; z++) {
				// Triangle 1
				indexArray[(x + z * (width - 1)) * 6 + 0] = x + z * width;
				indexArray[(x + z * (width - 1)) * 6 + 1] = x + (z + 1) * width;
				indexArray[(x + z * (width - 1)) * 6 + 2] = x + 1 + z * width;
				// Triangle 2
				indexArray[(x + z * (width - 1)) * 6 + 3] = x + 1 + z * width;
				indexArray[(x + z * (width - 1)) * 6 + 4] = x + (z + 1) * width;
				indexArray[(x + z * (width - 1)) * 6 + 5] = x + 1 + (z + 1) * width;
			}

		//printf("Generation done! Saving to file... \n");

		/*bool vertSaveRes = FileHandler::saveToFile(VERTEX_FILE_NAME, vertexArray, VERTEX_COUNT * 3);
		bool normSaveRes = FileHandler::saveToFile(NORMAL_FILE_NAME, normalArray, VERTEX_COUNT * 3);
		bool indSaveRes = FileHandler::saveToFile(INDEX_FILE_NAME, indexArray, TRIANGLE_COUNT * 3);

		if (vertSaveRes && normSaveRes && indSaveRes) {
			printf("Terrain saved to file.\n");
		}
		else {
			printf("Could not save terrain to file, exiting\n");
			exit(3);
		}*/
	//}

	// Now construct a VAO from this data.
	setupVAO();
}

glm::vec3 Terrain::calcNormal(GLfloat x, GLfloat y, GLfloat z) {
	GLfloat offset = 0.01;

	glm::vec3 normal = { 0.0, y, 0.0 };
	glm::vec3 v1 = normal - glm::vec3(offset, generator.getHeight(x + offset, z), 0.0);
	glm::vec3 v2 = normal - glm::vec3(0.0, generator.getHeight(x, z - offset), -offset);
	normal = glm::cross(v1, v2);
	normal = glm::normalize(normal);

	return normal;
}

void Terrain::setupVAO() {
	// Generate a new VAO as well as VBOs, and construct it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &normalVBO);
	glGenBuffers(1, &indexVBO);
	
	shader.activate();

	// Vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * 3 * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);
	shader.setAndEnableVertexAttrib(VERTEX_IN_NAME);

	// Normal VBO
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * 3 * sizeof(GLfloat), normalArray, GL_STATIC_DRAW);
	shader.setAndEnableNormalAttrib(NORMAL_IN_NAME);	

	// Index VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLE_COUNT*sizeof(GLuint)*3, indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
	shader.deactivate();
	printf("VAO setup done! vao is %d\n", vao);
}
