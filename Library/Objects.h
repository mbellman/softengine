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
		void computeSurfaceNormals();

	private:
		std::vector<Polygon> polygons;
};

struct Mesh : Object {
	Mesh(int rows, int columns, float tileSize);

	void setColor(int R, int G, int B);
	void setColor(const Color& color);
};

struct Cube : Object {
	public:
		Cube(float radius);

	private:
		static int polygonVertices[12][3];
};
