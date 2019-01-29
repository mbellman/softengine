#include <Objects.h>

Object::Object() {}

Object::~Object() {
	polygons.clear();
	vertices.clear();
}

void Object::computeSurfaceNormals() {
	for (int i = 0; i < polygons.size(); i++) {
		Polygon* polygon = &polygons.at(i);
		Vec3* v0 = &polygon->vertices[0]->vector;
		Vec3* v1 = &polygon->vertices[1]->vector;
		Vec3* v2 = &polygon->vertices[2]->vector;

		polygon->normal = Vec3::crossProduct(*v1 - *v0, *v2 - *v0);
	}
}

void Object::forEachPolygon(std::function<void(const Polygon&)> handle) {
	for (int i = 0; i < polygons.size(); i++) {
		handle(polygons.at(i));
	}
}

int Object::getPolygonCount() {
	return polygons.size();
}

void Object::rotate(const Vec3& rotation) {
	RotationMatrix rotationMatrix = RotationMatrix::calculate(rotation);

	for (int i = 0; i < vertices.size(); i++) {
		vertices.at(i).vector.rotate(rotationMatrix);
	}

	computeSurfaceNormals();
}

void Object::addPolygon(Vertex3d* v1, Vertex3d* v2, Vertex3d* v3) {
	Polygon polygon;

	polygon.bindVertex(0, v1);
	polygon.bindVertex(1, v2);
	polygon.bindVertex(2, v3);

	polygons.push_back(polygon);
}

void Object::addVertex(const Vec3& vector, const Color& color) {
	Vertex3d vertex;

	vertex.vector = vector;
	vertex.color = color;

	vertices.push_back(vertex);
}

/**
 * Creates a flat polygon mesh of given rows and columns using a fixed
 * tile size. The total polygon count corresponds to 2x the number of
 * tiles (2 x rows x columns). We add polygons in the following manner:
 *
 *   ----------------
 *   |1 / |3 / |5 / |
 *   | / 2| / 4| / 6| . . .
 *   ----------------
 *
 * The vertices of a polygon can be computed from its index in a particular
 * row. 'Lower' polygons are defined as those on the bottom right of any
 * given tile (i.e., evenly-numbered polygons). Once we reach the end of
 * a row, we move to the next until we run out of rows.
 *
 * Vertices are ordered in the following manner for each respective
 * type of polygon (counter-clockwise rotation is important for proper
 * surface normal determination):
 *
 *  0--2     0
 *  | /     /|
 *  |/     / |
 *  1     1--2
 */
Mesh::Mesh(int rows, int columns, float tileSize) {
	int verticesPerRow = columns + 1;
	int verticesPerColumn = rows + 1;

	for (int z = 0; z < verticesPerColumn; z++) {
		for (int x = 0; x < verticesPerRow; x++) {
			addVertex({ x * tileSize, (float)(rand() % 50), z * tileSize }, { 255, 255, 255 });
		}
	}

	int polygonsPerRow = 2 * columns;

	for (int row = 0; row < rows; row++) {
		for (int p = 1; p <= polygonsPerRow; p++) {
			bool isLowerPolygon = p % 2 == 0;
			int firstVertexIndex = row * verticesPerRow + (int)p / 2;
			int vertexBelowFirstIndex = firstVertexIndex + verticesPerRow;
			int i1 = firstVertexIndex;
			int i2 = vertexBelowFirstIndex - (isLowerPolygon ? 1 : 0);
			int i3 = isLowerPolygon ? vertexBelowFirstIndex : firstVertexIndex + 1;

			addPolygon(
				&vertices.at(i1),
				&vertices.at(i2),
				&vertices.at(i3)
			);
		}
	}

	computeSurfaceNormals();
}

void Mesh::setColor(int R, int G, int B) {
	for (int i = 0; i < vertices.size(); i++) {
		// vertices.at(i).color = { R, G, B };
		vertices.at(i).color = { rand() % 255, rand() % 255, rand() % 255 };
	}
}

void Mesh::setColor(const Color& color) {
	setColor(color.R, color.G, color.B);
}

int Cube::polygonVertices[12][3] = {
	// Side faces
	{ 0, 4, 1 },
	{ 1, 4, 5 },
	{ 1, 5, 2 },
	{ 2, 5, 6 },
	{ 2, 6, 3 },
	{ 3, 6, 7 },
	{ 3, 7, 0 },
	{ 0, 7, 4 },
	// Top face
	{ 0, 2, 3 },
	{ 0, 1, 2 },
	// Bottom face
	{ 4, 6, 5 },
	{ 4, 7, 6 }
};

Cube::Cube(float radius) {
	float diameter = 2 * radius;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			Vec3 vector;

			vector.x = (j == 2 || j == 3 ? diameter : 0) - radius;
			vector.y = (i * diameter) - radius;
			vector.z = (j == 1 || j == 2 ? -diameter : 0) + radius;

			addVertex(vector, { rand() % 255, rand() % 255, rand() % 255 });
		}
	}

	for (int p = 0; p < 12; p++) {
		const int (*polygonVertices)[3] = &(Cube::polygonVertices[p]);

		addPolygon(
			&vertices.at((*polygonVertices)[0]),
			&vertices.at((*polygonVertices)[1]),
			&vertices.at((*polygonVertices)[2])
		);
	}

	computeSurfaceNormals();
}