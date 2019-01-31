#include <map>
#include <string>
#include <iostream>
#include <Loaders/ObjLoader.h>

using namespace std;

static string VERTEX_LABEL = "v";
static string TEXTURE_COORDINATE_LABEL = "vt";
static string FACE_LABEL = "f";

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
	// TODO
}

VertexData ObjLoader::parseVertexData(string chunk) {
	VertexData vertexData;
	int offset = 0;

	for (int i = 0; i < 3; i++) {
		int p = chunk.find("/", offset);

		if (p - offset == 0 || offset >= chunk.length()) {
			// If the next '/' is immediately after the last,
			// or we've reached the end of the chunk with
			// cycles to spare, this type of vertex data isn't
			// defined.
			vertexData.indexes[i] = 0;
		} else {
			// As long as characters are found in between the
			// previous '/' and the next, or we still have extra
			// characters in the chunk, attempt to parse the index.
			int len = p > -1 ? p : string::npos;

			vertexData.indexes[i] = stoi(chunk.substr(offset, len));
		}

		offset = p + 1;
	}

	return vertexData;
}
