#pragma once

#include <map>
#include <vector>
#include <string>
#include <System/Math.h>
#include <Loaders/Loader.h>

using namespace std;

/**
 * VertexData
 * ----------
 *
 * A small data container for a particular vertex, representing its
 * primary vertex index, texture coordinate index, and vertex normal
 * index among the lists defined in an .obj file.
 */
struct VertexData {
	/**
	 * [0] -> vertex index
	 * [1] -> texture coordinate index
	 * [2] -> vertex normal index
	 */
	int indexes[3];
};

/**
 * Face
 * ----
 *
 * Contains vertex data for polygonal faces.
 */
struct Face {
	VertexData v1;
	VertexData v2;
	VertexData v3;
};

/**
 * ObjLoader
 * ---------
 *
 * Opens and parses .obj files into an intermediate representation
 * for conversion into Model instances.
 *
 * Usage:
 *
 *   ObjLoader modelObj("path/to/file.obj");
 *   Model model(modelObj);
 */
class ObjLoader : Loader {
public:
	vector<Vec3> vertices;
	vector<Vec2> textureCoordinates;
	vector<Face> faces;

	ObjLoader(const char* path);
	~ObjLoader();

private:
	void handleFace();
	void handleVertex();
	void handleTextureCoordinate();
	VertexData parseVertexData(string data);
};
