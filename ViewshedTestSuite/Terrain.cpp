#include "Terrain.h"
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

	free(vertexArray);
	free(indexArray);
}

void Terrain::init() {
	shader = Shader("terrain.vert", "terrain.frag");
}

void Terrain::render(glm::mat4 camMatrix, glm::mat4 projMatrix) {
	shader.activate();

	glBindVertexArray(vao);

	// matrix uploads
	shader.uploadMatrix(camMatrix, "camMatrix");
	shader.uploadMatrix(projMatrix, "projMatrix");
	glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT*3, GL_UNSIGNED_INT, 0L);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0L);

	shader.deactivate();
}

void Terrain::generate() {
	printf("Generating terrain...\n");
	// Start with setting some constants
	GLint width = TILE_SIZE;
	GLint height = TILE_SIZE;

	// Vectors to hold vertex data as well as indices
	indexArray = (GLuint *)malloc(sizeof(GLuint) * TRIANGLE_COUNT * 3);
	vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * VERTEX_COUNT * 3);
	//indexArray = (GLuint *)malloc(sizeof(GLuint) * 6);
	//vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * 12);

	// TESTING
	/*vertexArray[0] = -0.5f;
	vertexArray[1] = 0.5f;
	vertexArray[2] = 0.0f;

	vertexArray[3] = -0.5f;
	vertexArray[4] = -0.5f;
	vertexArray[5] = 0.0f;

	vertexArray[6] = 0.5f;
	vertexArray[7] = -0.5f;
	vertexArray[8] = 0.0f;

	vertexArray[9] = 0.5f;
	vertexArray[10] = 0.5f;
	vertexArray[11] = 0.0f;

	indexArray[0] = 0;
	indexArray[1] = 1;
	indexArray[2] = 3;
	indexArray[3] = 3;
	indexArray[4] = 1;
	indexArray[5] = 2;*/

	// Now loop over both x and z and set the vertices
	for (int x = 0; x < width; x++)
		for (int z = 0; z < height; z++) {
			vertexArray[(x + z * width) * 3 + 0] = (GLfloat)x;
			vertexArray[(x + z * width) * 3 + 1] = (GLfloat)0.0; // Height (this is 2.5D)
			vertexArray[(x + z * width) * 3 + 2] = (GLfloat)z;
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

	printf("Generation done! setting up vao\n");	
	// Now construct a VAO from this data.
	setupVAO();
}

void Terrain::setupVAO() {
	// Generate a new VAO as well as VBOs, and construct it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &indexVBO);
	
	shader.activate();

	// Vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * 3 * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);

	// Also set the attriblocations
	shader.setAndEnableVertexAttrib(VERTEX_IN_NAME);

	// Index VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLE_COUNT*sizeof(GLuint)*3, indexArray, GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLuint), indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
	shader.deactivate();
	printf("VAO setup done! vao is %d\n", vao);
}
