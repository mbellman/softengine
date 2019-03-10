#include <System/Objects.h>
#include <System/Quaternion.h>
#include <Loaders/ObjLoader.h>
#include <Graphics/TextureBuffer.h>
#include <Helpers.h>
#include <Constants.h>
#include <functional>
#include <algorithm>

/**
 * Object
 * ------
 *
 * A base class used for 3D objects. Subclasses offer more
 * specialized types of Objects with custom geometry.
 */
Object::Object() {
	id = rand() % 30000;
}

Object::~Object() {
	for (auto* lod : lods) {
		delete lod;
	}

	for (auto* polygon : polygons) {
		delete polygon;
	}

	lods.clear();
	polygons.clear();
	vertices.clear();
}

void Object::addLOD(Object* lod) {
	lods.push_back(lod);
}

void Object::addMorphTarget(Object* morphTarget) {
	totalMorphTargets++;

	auto& morphTargetVertices = morphTarget->getVertices();

	for (int i = 0; i < morphTargetVertices.size(); i++) {
		const Vec3& morphTargetVector = morphTargetVertices.at(i).vector;
		Vertex3d& vertex = vertices.at(i);

		vertex.morphTargets.push_back({
			morphTargetVector.x,
			morphTargetVector.y,
			morphTargetVector.z
		});
	}

	// Free the original Object used as the morph target,
	// since its vertices were the only information we needed
	delete morphTarget;
}

void Object::addPolygon(int v1_index, int v2_index, int v3_index) {
	Polygon* polygon = new Polygon();

	polygon->bindVertex(0, &vertices.at(v1_index));
	polygon->bindVertex(1, &vertices.at(v2_index));
	polygon->bindVertex(2, &vertices.at(v3_index));
	polygon->normal = Object::computePolygonNormal(*polygon);
	polygon->sourceObject = this;

	polygons.push_back(polygon);
}

void Object::addVertex(const Vec3& vector) {
	Vertex3d vertex;

	vertex.vector.x = vector.x;
	vertex.vector.y = vector.y;
	vertex.vector.z = vector.z;

	vertices.push_back(vertex);
}

void Object::addVertex(const Vec3& vector, const Color& color) {
	Vertex3d vertex;

	vertex.vector.x = vector.x;
	vertex.vector.y = vector.y;
	vertex.vector.z = vector.z;
	vertex.color = color;

	vertices.push_back(vertex);
}

void Object::addVertex(const Vec3& vector, const Vec2& uv) {
	Vertex3d vertex;

	vertex.vector.x = vector.x;
	vertex.vector.y = vector.y;
	vertex.vector.z = vector.z;
	vertex.uv.x = uv.x;
	vertex.uv.y = uv.y;

	vertices.push_back(vertex);
}

void Object::applyRotationMatrix(const RotationMatrix& matrix) {
	for (auto& vertex : vertices) {
		vertex.vector += transformOrigin;

		for (auto& morphTarget : vertex.morphTargets) {
			morphTarget += transformOrigin;
		}

		vertex.rotate(matrix);

		vertex.vector -= transformOrigin;

		for (auto& morphTarget : vertex.morphTargets) {
			morphTarget -= transformOrigin;
		}
	}

	recomputeSurfaceNormals();

	for (auto* lod : lods) {
		lod->applyRotationMatrix(matrix);
	}
}

Vec3 Object::computePolygonNormal(const Polygon& polygon) {
	const Vec3* v0 = &polygon.vertices[0]->vector;
	const Vec3* v1 = &polygon.vertices[1]->vector;
	const Vec3* v2 = &polygon.vertices[2]->vector;

	return Vec3::crossProduct(*v1 - *v0, *v2 - *v0).unit();
}

void Object::recomputeSurfaceNormals() {
	for (auto* polygon : polygons) {
		polygon->normal = Object::computePolygonNormal(*polygon);
	}

	for (auto& vertex : vertices) {
		vertex.normal = Object::computeVertexNormal(vertex);
	}

	for (auto* lod : lods) {
		lod->recomputeSurfaceNormals();
	}
}

Vec3 Object::computeVertexNormal(const Vertex3d& vertex) {
	Vec3 averageNormal;

	for (auto* polygon : vertex.connectedPolygons) {
		averageNormal += polygon->normal;
	}

	return averageNormal.unit();
}

int Object::getId() const {
	return id;
}

const Object* Object::getLOD(float distance) const {
	if (lods.empty()) {
		return this;
	}

	float distanceRatio = distance / LOD_DISTANCE_THRESHOLD;

	if (distanceRatio < 1) {
		return this;
	}

	int lodIndex = std::min((int)distanceRatio - 1, (int)lods.size() - 1);

	return lods.at(lodIndex);
}

const std::vector<Object*>& Object::getLODs() const {
	return lods;
}

int Object::getPolygonCount() const {
	return polygons.size();
}

const std::vector<Polygon*>& Object::getPolygons() const {
	return polygons;
}

int Object::getVertexCount() const {
	return vertices.size();
}

const std::vector<Vertex3d>& Object::getVertices() const {
	return vertices;
}

bool Object::hasLODs() const {
	return lods.size() > 0;
}

bool Object::isMorphing() const {
	return morph.isActive;
}

void Object::rotate(const Vec3& rotation) {
	RotationMatrix rotationMatrix = RotationMatrix::fromVec3(rotation);

	applyRotationMatrix(rotationMatrix);
}

void Object::rotateDeg(const Vec3& rotation) {
	rotate({
		rotation.x * DEG_TO_RAD,
		rotation.y * DEG_TO_RAD,
		rotation.z * DEG_TO_RAD
	});
}

void Object::rotateOnAxis(float angle, const Vec3& axis) {
	Vec3 normalizedAxis = axis.unit();
	RotationMatrix rotationMatrix = Quaternion::fromAxisAngle(angle * DEG_TO_RAD, normalizedAxis.x, normalizedAxis.y, normalizedAxis.z).toRotationMatrix();

	applyRotationMatrix(rotationMatrix);
}

void Object::scale(float scalar) {
	for (auto& vertex : vertices) {
		vertex.scale(scalar);
	}

	for (auto* lod : lods) {
		lod->scale(scalar);
	}
}

void Object::scale(const Vec3& vector) {
	for (auto& vertex : vertices) {
		vertex.scale(vector);
	}

	for (auto* lod : lods) {
		lod->scale(vector);
	}
}

void Object::setColor(int R, int G, int B) {
	for (int i = 0; i < vertices.size(); i++) {
		vertices.at(i).color = { R, G, B };
	}

	for (auto* lod : lods) {
		lod->setColor(R, G, B);
	}
}

void Object::setColor(const Color& color) {
	setColor(color.R, color.G, color.B);
}

void Object::setMorphTarget(int targetIndex) {
	if (targetIndex >= totalMorphTargets) {
		return;
	}

	for (auto& vertex : vertices) {
		const Vec3& vertexMorphTarget = vertex.morphTargets.at(targetIndex);

		vertex.vector.x = vertexMorphTarget.x;
		vertex.vector.y = vertexMorphTarget.y;
		vertex.vector.z = vertexMorphTarget.z;
	}

	recomputeSurfaceNormals();
}

void Object::setTexture(TextureBuffer* textureBuffer) {
	if (textureBuffer != NULL) {
		texture = textureBuffer;
	}

	setColor(0, 0, 0);

	for (auto* lod : lods) {
		lod->setTexture(textureBuffer);
	}
}

void Object::startMorph(int duration, bool shouldLoop) {
	if (totalMorphTargets == 0) {
		return;
	}

	morph.time = 0;
	morph.duration = duration;
	morph.shouldLoop = shouldLoop;
	morph.isActive = true;
}

void Object::stopMorph() {
	morph.isActive = false;
}

/**
 * Ensures that an Object's LODs all bear the same characteristics
 * of the Object, emphasizing those modified without accessors.
 * LODs are synced when an Object is added to a Scene, and prior
 * to screen projection on each frame.
 */
void Object::syncLODs() {
	for (auto* lod : lods) {
		lod->position = position;
		lod->isStatic = isStatic;
		lod->isFlatShaded = isFlatShaded;
		lod->hasLighting = hasLighting;
		lod->canOccludeSurfaces = canOccludeSurfaces;
		lod->fresnelFactor = fresnelFactor;
		lod->sectorId = sectorId;
		lod->transformOrigin = transformOrigin;
		lod->nearClippingDistance = nearClippingDistance;
	}
}

void Object::update(int dt) {
	updatePosition(dt);

	if (morph.isActive) {
		updateMorph(dt);
	}

	if (onUpdate != nullptr) {
		onUpdate(dt);
	}

	if (lifetime > 0) {
		lifetime = FAST_MAX(lifetime - dt, 0);
	}

	for (auto* lod : lods) {
		lod->update(dt);
	}
}

void Object::updateMorph(int dt) {
	float morphProgress = (float)morph.time / morph.duration;
	float frameProgress = morphProgress * (totalMorphTargets - 1);
	int startFrame = (int)frameProgress;
	int endFrame = startFrame + 1;

	if (frameProgress >= 1.0f) {
		frameProgress -= (int)frameProgress;
	}

	for (auto& vertex : vertices) {
		vertex.morph(startFrame, endFrame, frameProgress);
	}

	morph.time += (morph.isReversed ? -dt : dt);

	bool isMorphComplete = morph.isReversed ? morph.time <= 0 : morph.time >= morph.duration;

	if (isMorphComplete) {
		if (morph.shouldLoop) {
			morph.time = morph.isReversed ? 0 : morph.duration - dt;
			morph.isReversed = !morph.isReversed;
		} else {
			setMorphTarget(0);

			morph.time = 0;
			morph.isActive = false;
		}
	}

	recomputeSurfaceNormals();
}

/**
 * Model
 * -----
 *
 * Creates an object from ObjLoader instance data.
 */
Model::Model(const ObjLoader& obj) {
	bool hasTextureData = obj.textureCoordinates.size() > 0;

	if (hasTextureData) {
		// Since there may be a different number of defined vertex
		// vectors and vertex texture coordinates (owing to the way
		// .obj files store vertex information), we have to examine
		// the vertex index + texture coordinate index tuples defined
		// for each face, map these to a vertex vector and texture
		// coordinate, create and add a single vertex per unique tuple,
		// and finally add our polygons once all vertices are set.

		// Track unique vertex/texture coordinate index pairs and their
		// associated Object vertex index
		std::map<std::pair<int, int>, int> uniqueVertexIndexMap;

		// Track the vertices for each polygon, so we can create them
		// after all vertices are defined
		std::vector<std::tuple<int, int, int>> polygonVertexIndices;

		for (const Face& face : obj.faces) {
			std::pair<int, int> v_vt_pairs[3];
			int vertexIndices[3];

			v_vt_pairs[0] = { face.v1.indexes[0] - 1, face.v1.indexes[1] - 1 };
			v_vt_pairs[1] = { face.v2.indexes[0] - 1, face.v2.indexes[1] - 1 };
			v_vt_pairs[2] = { face.v3.indexes[0] - 1, face.v3.indexes[1] - 1 };

			for (int t = 0; t < 3; t++) {
				std::pair<int, int>& pair = v_vt_pairs[t];
				auto uniqueVertex = uniqueVertexIndexMap.find(pair);

				if (uniqueVertex != uniqueVertexIndexMap.end()) {
					vertexIndices[t] = uniqueVertex->second;
				} else {
					const Vec3& vector = obj.vertices.at(v_vt_pairs[t].first);
					Vec2 uv = obj.textureCoordinates.at(v_vt_pairs[t].second);
					int index = vertices.size();

					vertexIndices[t] = index;
					uv.y = 1 - uv.y;

					addVertex(vector, uv);
					uniqueVertexIndexMap.emplace(pair, index);
				}
			}

			polygonVertexIndices.push_back({ vertexIndices[0], vertexIndices[1], vertexIndices[2] });
		}

		for (const auto& vertexIndices : polygonVertexIndices) {
			addPolygon(std::get<0>(vertexIndices), std::get<1>(vertexIndices), std::get<2>(vertexIndices));
		}
	} else {
		// Since we compute normals independent of those provided
		// by .obj files, a textureless model means vertex indices
		// are our only consideration. We can simply loop over the
		// ObjLoader vertices and set them on the Model, then loop
		// over the faces and create polygons using their defined
		// vertex indices.
		for (int v = 0; v < obj.vertices.size(); v++) {
			Vec3 vector = obj.vertices.at(v);

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
	this->rows = rows;
	this->columns = columns;

	int verticesPerRow = columns + 1;
	int verticesPerColumn = rows + 1;

	for (int z = 0; z < verticesPerColumn; z++) {
		for (int x = 0; x < verticesPerRow; x++) {
			addVertex({ x * tileSize, 0.0f, z * tileSize }, { rand() % 255, rand() % 255, rand() % 255 });
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

void Mesh::setTextureInterval(int rowInterval, int columnInterval) {
	int verticesPerRow = columns + 1;
	int verticesPerColumn = rows + 1;

	for (int i = 0; i < verticesPerColumn; i++) {
		float v = (float)i / columnInterval;

		for (int j = 0; j < verticesPerRow; j++) {
			float u = (float)j / rowInterval;
			int index = i * verticesPerRow + j;

			Vertex3d* vertex = &vertices.at(index);

			vertex->uv.x = u;
			vertex->uv.y = v;
		}
	}
}

void Mesh::setVertexOffsets(std::function<void(int, int, Vec3&)> offsetHandler) {
	int verticesPerRow = columns + 1;
	int verticesPerColumn = rows + 1;

	for (int i = 0; i < verticesPerColumn; i++) {
		for (int j = 0; j < verticesPerRow; j++) {
			int index = i * verticesPerRow + j;

			Vertex3d* vertex = &vertices.at(index);

			offsetHandler(i, j, vertex->vector);
		}
	}

	recomputeSurfaceNormals();
}

/**
 * Cube
 * ----
 *
 * Produces a cube of a given radius.
 */
Cube::Cube(float radius) {
	float diameter = 2 * radius;

	for (int i = 0; i < 24; i++) {
		Vec3 vector;
		Vec3& position = Cube::vertexPositions[i];

		vector.x = position.x * radius;
		vector.y = -position.y * radius;
		vector.z = position.z * radius;

		addVertex(vector, { rand() % 255, rand() % 255, rand() % 255 });
	}

	for (int p = 0; p < 12; p++) {
		const int (*vertices)[3] = &(Cube::polygonVertices[p]);

		addPolygon((*vertices)[0], (*vertices)[1], (*vertices)[2]);
	}
}

/**
 * Defines positions for the 24 vertices constituting
 * a cube. Cubes need 24 vertices in order to allow each
 * face to have uniquely defined texture coordinates at
 * its corners; some vertices thus share positions.
 *
 *        __3__
 *    __--     --__
 *   0__         __2
 *   |  --_____--  |
 *   |      1      |
 *   |             |
 *   |    __7__    |              y+
 *   |__--     --__|              |
 *   4__         __6              |
 *      --__ __--         z+__    |    __x+
 *          5                 --__|__--
 */
Vec3 Cube::vertexPositions[24] = {
	// Side faces
	{ -1.0f, 1.0f, 1.0f },     // 0
	{ -1.0f, 1.0f, -1.0f },    // 1
	{ -1.0f, -1.0f, 1.0f },    // 4
	{ -1.0f, -1.0f, -1.0f },   // 5

	{ -1.0f, 1.0f, -1.0f },    // 1
	{ 1.0f, 1.0f, -1.0f },     // 2
	{ -1.0f, -1.0f, -1.0f },   // 5
	{ 1.0f, -1.0f, -1.0f },    // 6

	{ 1.0f, 1.0f, -1.0f },     // 2
	{ 1.0f, 1.0f, 1.0f },      // 3
	{ 1.0f, -1.0f, -1.0f },    // 6
	{ 1.0f, -1.0f, 1.0f },     // 7

	{ 1.0f, 1.0f, 1.0f },      // 3
	{ -1.0f, 1.0f, 1.0f },     // 0
	{ 1.0f, -1.0f, 1.0f },     // 7
	{ -1.0f, -1.0f, 1.0f },    // 4

	// Top face
	{ -1.0f, 1.0f, 1.0f },     // 0
	{ 1.0f, 1.0f, 1.0f },      // 3
	{ -1.0f, 1.0f, -1.0f },    // 1
	{ 1.0f, 1.0f, -1.0f },     // 2

	// Bottom face
	{ -1.0f, -1.0f, -1.0f },   // 5
	{ 1.0f, -1.0f, -1.0f },    // 6
	{ -1.0f, -1.0f, 1.0f },    // 4
	{ 1.0f, -1.0f, 1.0f },     // 7
};

int Cube::polygonVertices[12][3] = {
	// Side faces
	{ 0, 2, 1 },
	{ 1, 2, 3 },
	{ 4, 6, 5 },
	{ 5, 6, 7 },
	{ 8, 10, 9 },
	{ 9, 10, 11 },
	{ 12, 14, 13 },
	{ 13, 14, 15 },
	// Top face
	{ 16, 19, 17 },
	{ 16, 18, 19 },
	// Bottom face
	{ 22, 21, 20 },
	{ 22, 23, 21 }
};

void Cube::setFaceUVCoordinates(float x1, float y1, float x2, float y2) {
	for (int face = 0; face < 6; face++) {
		int vertexOffset = face * 4;
		Vertex3d* v1 = &vertices.at(vertexOffset);
		Vertex3d* v2 = &vertices.at(vertexOffset + 1);
		Vertex3d* v3 = &vertices.at(vertexOffset + 2);
		Vertex3d* v4 = &vertices.at(vertexOffset + 3);

		v3->uv = { x1, y1 };
		v4->uv = { x2, y1 };
		v1->uv = { x1, y2 };
		v2->uv = { x2, y2 };
	}
}

/**
 * Particle
 * --------
 *
 * Creates a simple flat surface used for particle effects.
 */
Particle::Particle() {
	addVertex({ -1, 1, 0 });
	addVertex({ 1, 1, 0 });
	addVertex({ -1, -1, 0 });
	addVertex({ 1, -1, 0 });

	// 'Front' face
	addPolygon(0, 2, 1);
	addPolygon(1, 2, 3);

	// 'Back' face
	addPolygon(0, 1, 2);
	addPolygon(1, 3, 2);

	isFlatShaded = true;
}

/**
 * Skybox
 * ------
 */
Skybox::Skybox() {
	for (int i = 0; i < 14; i++) {
		addVertex(Skybox::vertexPositions[i], Skybox::uvs[i]);
	}

	addPolygon(0, 1, 2);
	addPolygon(3, 4, 0);
	addPolygon(5, 6, 3);
	addPolygon(7, 8, 5);
	addPolygon(9, 3, 10);
	addPolygon(8, 11, 6);
	addPolygon(0, 4, 1);
	addPolygon(3, 6, 4);
	addPolygon(5, 8, 6);
	addPolygon(7, 12, 8);
	addPolygon(9, 5, 3);
	addPolygon(8, 13, 11);
}

Vec3 Skybox::vertexPositions[14] = {
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f, 1.0f, 1.0f },
	{ -1.0f, -1.0f, 1.0f },
	{ 1.0f, -1.0f, -1.0f },
	{ -1.0f, 1.0f, -1.0f },
	{ 1.0f, -1.0f, 1.0f },
	{ 1.0f, 1.0f, -1.0f },
	{ -1.0f, -1.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f },
	{ -1.0f, -1.0f, 1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f, 1.0f, -1.0f },
	{ -1.0f, 1.0f, 1.0f },
	{ -1.0f, 1.0f, 1.0f }
};

Vec2 Skybox::uvs[14] = {
	{ 0.749978, 0.666474 },
	{ 0.999934, 0.333523 },
	{ 0.999934, 0.666474 },
	{ 0.499000, 0.666474 },
	{ 0.749978, 0.333523 },
	{ 0.250006, 0.666476 },
	{ 0.499000, 0.333525 },
	{ 0.000108, 0.666476 },
	{ 0.250064, 0.333525 },
	{ 0.250064, 0.999427 },
	{ 0.499000, 0.999427 },
	{ 0.499000, 0.000572 },
	{ 0.000108, 0.333525 },
	{ 0.250064, 0.000572 }
};

/**
 * Light
 * -----
 */
const Color& Light::getColor() const {
	return color;
}

const Vec3& Light::getColorRatios() const {
	return cachedColorRatios;
}

void Light::setColor(int R, int G, int B) {
	color.R = R;
	color.G = G;
	color.B = B;

	// Since every vertex of every visible triangle within range
	// of a light needs to check the light's color ratios, we cache
 	// the values for performance.
	cachedColorRatios = color.ratios();
}

void Light::setColor(const Color& color) {
	setColor(color.R, color.G, color.B);
}

/**
 * DirectionalLight
 * ----------------
 */
const Vec3& DirectionalLight::getDirection() const {
	return direction;
}

void DirectionalLight::setDirection(const Vec3& direction) {
	this->direction = direction.unit();
}
