#include <Objects.h>
#include <Loaders/ObjLoader.h>

/**
 * Object
 * ------
 *
 * A base class used for 3D objects. Subclasses offer more
 * specialized types of Objects with custom geometry.
 */
Object::Object() {}

Object::~Object() {
	polygons.clear();
	vertices.clear();
}

void Object::addPolygon(int v1, int v2, int v3) {
	Polygon polygon;

	polygon.bindVertex(0, &vertices.at(v1));
	polygon.bindVertex(1, &vertices.at(v2));
	polygon.bindVertex(2, &vertices.at(v3));
	polygon.normal = Object::computePolygonNormal(polygon);
	polygon.object = this;

	polygons.push_back(polygon);
}

void Object::addVertex(const Vec3& vector, const Color& color) {
	Vertex3d vertex;

	vertex.vector = vector;
	vertex.color = color;

	vertices.push_back(vertex);
}

Vec3 Object::computePolygonNormal(const Polygon& polygon) {
	const Vec3* v0 = &polygon.vertices[0]->vector;
	const Vec3* v1 = &polygon.vertices[1]->vector;
	const Vec3* v2 = &polygon.vertices[2]->vector;

	return Vec3::crossProduct(*v1 - *v0, *v2 - *v0).unit();
}

void Object::computeSurfaceNormals() {
	for (auto& polygon : polygons) {
		polygon.normal = Object::computePolygonNormal(polygon);
	}
}

int Object::getPolygonCount() {
	return polygons.size();
}

const std::vector<Polygon>& Object::getPolygons() {
	return polygons;
}

void Object::rotate(const Vec3& rotation) {
	RotationMatrix rotationMatrix = RotationMatrix::calculate(rotation);

	for (int i = 0; i < vertices.size(); i++) {
		vertices.at(i).vector.rotate(rotationMatrix);
	}

	computeSurfaceNormals();
}

void Object::scale(float scalar) {
	for (int i = 0; i < vertices.size(); i++) {
		Vec3* vector = &vertices.at(i).vector;

		vector->x *= scalar;
		vector->y *= scalar;
		vector->z *= scalar;
	}
}

void Object::setColor(int R, int G, int B) {
	for (int i = 0; i < vertices.size(); i++) {
		vertices.at(i).color = { R, G, B };
	}
}

void Object::setColor(const Color& color) {
	setColor(color.R, color.G, color.B);
}

/**
 * Model
 * -----
 *
 * Creates an object from ObjLoader instance data.
 */
Model::Model(const ObjLoader& obj) {
	for (int v = 0; v < obj.vertices.size(); v++) {
		Vec3 vector = obj.vertices.at(v);

		// Coordinate system handedness fix (Blender uses a right-handed
		// screen space coordinate system, whereas we use a left-handed
		// coordinate system).
		vector.x *= -1;

		addVertex(vector, { rand() % 255, rand() % 255, rand() % 255 });
	}

	for (int f = 0; f < obj.faces.size(); f++) {
		const Face* face = &obj.faces.at(f);

		int v1 = face->v1.indexes[0] - 1;
		int v2 = face->v2.indexes[0] - 1;
		int v3 = face->v3.indexes[0] - 1;

		addPolygon(v1, v2, v3);
	}
}

/**
 * Mesh
 * ----
 *
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
			addVertex({ x * tileSize, (float)(rand() % 50), z * tileSize }, { rand() % 255, rand() % 255, rand() % 255 });
		}
	}

	int polygonsPerRow = 2 * columns;

	for (int row = 0; row < rows; row++) {
		for (int p = 1; p <= polygonsPerRow; p++) {
			bool isLowerPolygon = p % 2 == 0;
			int firstVertexIndex = row * verticesPerRow + (int)p / 2;
			int vertexBelowFirstIndex = firstVertexIndex + verticesPerRow;
			int v1 = firstVertexIndex;
			int v2 = vertexBelowFirstIndex - (isLowerPolygon ? 1 : 0);
			int v3 = isLowerPolygon ? vertexBelowFirstIndex : firstVertexIndex + 1;

			addPolygon(v1, v2, v3);
		}
	}
}

/**
 * Cube
 * ----
 *
 * Produces a cube of a given radius.
 */
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

		addPolygon((*polygonVertices)[0], (*polygonVertices)[1], (*polygonVertices)[2]);
	}
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

/**
 * Light
 * -----
 */
void Light::flip() {
	power = 0 - (power - 1);
}

bool Light::isLight(Object* object) {
	return dynamic_cast<Light*>(object) != NULL;
}

void Light::on() {
	power = 1;
}

void Light::off() {
	power = 0;
}

void Light::setColor(int R, int G, int B) {
	this->color = { R, G, B };
}

void Light::setColor(const Color& color) {
	setColor(color.R, color.G, color.B);
}
