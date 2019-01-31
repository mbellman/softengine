#include <map>
#include <string>
#include <iostream>
#include <Loaders/ObjLoader.h>

using namespace std;

static string VERTEX_LABEL = "v";
static string TEXTURE_COORDINATE_LABEL = "vt";
static string FACE_LABEL = "f";

/**
 * ObjLoader
 * ---------
 */
ObjLoader::ObjLoader(const char* path) {
	load(path);

	while (isLoading) {
		setChunkDelimiter(" ");

		string label = readNextChunk();

		if (label == VERTEX_LABEL) {
			handleVertex();
		} else if (label == TEXTURE_COORDINATE_LABEL) {
			handleTextureCoordinate();
		} else if (label == FACE_LABEL) {
			handleFace();
		}

		nextLine();
	}
}

ObjLoader::~ObjLoader() {
	vertices.clear();
	faces.clear();
}

void ObjLoader::handleFace() {
	Face face;

	face.v1 = parseVertexData(readNextChunk());
	face.v2 = parseVertexData(readNextChunk());
	face.v3 = parseVertexData(readNextChunk());

	faces.push_back(face);
}

void ObjLoader::handleVertex() {
	float x = stof(readNextChunk());
	float y = stof(readNextChunk());
	float z = stof(readNextChunk());

	vertices.push_back({ x, y, z });
}

void ObjLoader::handleTextureCoordinate() {
	// TODO for texture mapping
}

/**
 * Attempts to parse the primary vertex index, texture coordinate
 * index, and normal index of a polygonal face. A data chunk can
 * be structured in any of the following ways:
 *
 *   v
 *   v/vt
 *   v/vt/vn
 *   v//vn
 *
 * Where v is the primary index, vt the texture coordinate index,
 * and vn the normal index, with respect to previously listed
 * vertex/texture coordinate/normal values.
 */
VertexData ObjLoader::parseVertexData(string chunk) {
	VertexData vertexData;
	int offset = 0;

	for (int i = 0; i < 3; i++) {
		int p = chunk.find("/", offset);
		bool found = p > -1;

		if (p - offset == 0 || offset >= chunk.length()) {
			// If the next '/' is immediately after the last,
			// or we've reached the end of the chunk with
			// cycles to spare, this type of vertex index isn't
			// defined.
			vertexData.indexes[i] = -1;
		} else {
			// As long as characters are found in between the
			// previous '/' and the next, or we still have extra
			// characters in the chunk, attempt to parse the index.
			int len = found ? p : string::npos;

			vertexData.indexes[i] = stoi(chunk.substr(offset, len));
		}

		offset = found ? p + 1 : chunk.length();
	}

	return vertexData;
}
