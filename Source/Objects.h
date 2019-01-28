#pragma once
#include <functional>
#include <vector>
#include <algorithm>
#include <Types.h>

struct Object {
	public:
		Vec3 position;

		Object();
		~Object();
		
		void forEachPolygon(std::function<void(const Polygon&)> handle);
		int getPolygonCount();
		void rotate(const Vec3& rotation);

	protected:
		std::vector<Vertex3d> vertices;

		void addPolygon(Vertex3d* v1, Vertex3d* v2, Vertex3d* v3);
		void addVertex(const Vec3& vector, const Color& color);

	private:
		std::vector<Polygon> polygons;
};

struct Mesh : Object {
	Mesh(int rows, int columns, float tileSize);

	void setColor(int R, int G, int B);
	void setColor(const Color& color);
};

namespace CubeVertices {
	const static int vertexMap[12][3] = {
		{ 0, 1, 4 },
		{ 1, 4, 5 },
		{ 1, 2, 5 },
		{ 2, 5, 6 },
		{ 2, 3, 6 },
		{ 3, 6, 7 },
		{ 3, 0, 7 },
		{ 0, 4, 7 },
		{ 0, 2, 3 },
		{ 0, 1, 2 },
		{ 4, 5, 6 },
		{ 4, 6, 7 }
	};
};


struct Cube : Object {
	public:
		Cube(float radius);
};