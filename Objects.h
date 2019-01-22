#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <Rasterizer.h>

struct Vertex3D {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vertex3D() {}

	Vertex3D(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vertex3D operator +(const Vertex3D& v) {
		Vertex3D vt;

		vt.x = x + v.x;
		vt.y = y + v.y;
		vt.z = z + v.z;

		return vt;
	}
};

struct Polygon3D {
	Vertex3D* vertices[3];
	Polygon2D projection;

	void setVertex(int index, Vertex3D* v) {
		vertices[index] = v;
	}
};

struct Object {
	public:
		Object() {}

		~Object() {
			polygons.clear();
			vertices.clear();
		}

		void setPosition(float x, float y, float z) {
			position.x = x;
			position.y = y;
			position.z = z;
		}

		void forEachPolygon(std::function<void(const Vertex3D&, const Vertex3D&, const Vertex3D&)> handle) {
			for (int i = 0; i < polygons.size(); i++) {
				Polygon3D* p = &polygons.at(i);

				handle(*p->vertices[0] + position, *p->vertices[1] + position, *p->vertices[2] + position);
			}
		}

		int getPolygonCount() {
			return polygons.size();
		}

	protected:
		std::vector<Vertex3D> vertices;

		void addPolygon(Vertex3D* v1, Vertex3D* v2, Vertex3D* v3) {
			Polygon3D p;

			p.setVertex(0, v1);
			p.setVertex(1, v2);
			p.setVertex(2, v3);

			polygons.push_back(p);
		}

		void addVertex(float x, float y, float z) {
			Vertex3D* v = new Vertex3D(x, y, z);

			vertices.push_back(*v);
		}

	private:
		std::vector<Polygon3D> polygons;
		Vertex3D position;
};

struct Mesh : Object {
	public:
		Mesh(int rows, int columns, float tileSize) {
			// Vertex creation
			int verticesPerRow = columns + 1;
			int verticesPerColumn = rows + 1;

			for (int z = 0; z < verticesPerColumn; z++) {
				for (int x = 0; x < verticesPerRow; x++) {
					addVertex(x * tileSize, 0, z * tileSize);
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
					int first = row * verticesPerRow + (int)p / 2;

					addPolygon(
						&vertices.at(first),
						&vertices.at(isLowerPolygon ? first + verticesPerRow - 1 : first + 1),
						&vertices.at(first + verticesPerRow)
					);
				}
			}
		}
};

struct Cube : Object {

};