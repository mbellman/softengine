#include <Objects.h>

Object::Object() {}

Object::~Object() {
    polygons.clear();
    vertices.clear();    
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

Mesh::Mesh(int rows, int columns, float tileSize) {
    // Vertex creation
    int verticesPerRow = columns + 1;
    int verticesPerColumn = rows + 1;

    for (int z = 0; z < verticesPerColumn; z++) {
        for (int x = 0; x < verticesPerRow; x++) {
            addVertex({ x * tileSize, (float)(rand() % 50), z * tileSize }, { 255, 255, 255 });
        }
    }

    // Polygon creation, using the previously allotted vertices. The total
    // polygon count corresponds to 2x the number of tiles, e.g. 2*w*l. We
    // add polygons in the following manner:
    //
    //   ----------------
    //   |1 / |3 / |5 / |
    //   | / 2| / 4| / 6| . . .
    //   ----------------
    //
    // The vertices of a polygon can be computed from its index in a particular
    // row. 'Lower' polygons are defined as those on the bottom right of any
    // given tile (i.e., evenly-numbered polygons). Once we reach the end of
    // a row, we move to the next until we run out of rows.
    int polygonsPerRow = 2 * columns;

    for (int row = 0; row < rows; row++) {
        for (int p = 1; p <= polygonsPerRow; p++) {
            bool isLowerPolygon = p % 2 == 0;
            int firstVertexIndex = row * verticesPerRow + (int)p / 2;

            addPolygon(
                &vertices.at(firstVertexIndex),
                &vertices.at(isLowerPolygon ? firstVertexIndex + verticesPerRow - 1 : firstVertexIndex + 1),
                &vertices.at(firstVertexIndex + verticesPerRow)
            );
        }
    }
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

Cube::Cube(float radius) {
    float diameter = 2 * radius;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            Vec3 vector;

            vector.x = (j == 2 || j == 3 ? diameter : 0) - radius;
            vector.y = (i * diameter) - radius;
            vector.z = (j == 1 || j == 2 ? diameter : 0) - radius;

            addVertex(vector, { rand() % 255, rand() % 255, rand() % 255 });
        }
    }

    for (int p = 0; p < 12; p++) {
        const int (*polygonVertices)[3] = &(CubeVertices::vertexMap[p]);

        addPolygon(
        	&vertices.at((*polygonVertices)[0]),
        	&vertices.at((*polygonVertices)[1]),
        	&vertices.at((*polygonVertices)[2])
        );
    }
}