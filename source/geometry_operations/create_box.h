#ifndef SELECTOR_GEOMETRY_OPERATIONS_CREATE_BOX_H_
#define SELECTOR_GEOMETRY_OPERATIONS_CREATE_BOX_H_

#include <common/logger.h>
#include <geometry_core/geometry.h>
#include <geometry_core/geometry_builder.h>

namespace selector
{
template<class G>
class CreateBox
{
private:
	using Geometry = G;
	using GeometryPtr = std::shared_ptr<Geometry>;

public:
	CreateBox(const float &xSize, const float &ySize, const float &zSize)
	{
		auto halfXSize = 0.5 * xSize;
		auto halfYSize = 0.5 * ySize;
		auto halfZSize = 0.5 * zSize;

		m_points[0] = Vec3F(-halfXSize, -halfYSize, -halfZSize);
		m_points[1] = Vec3F(halfXSize, -halfYSize, -halfZSize);
		m_points[2] = Vec3F(-halfXSize, halfYSize, -halfZSize);
		m_points[3] = Vec3F(halfXSize, halfYSize, -halfZSize);

		m_points[4] = Vec3F(-halfXSize, -halfYSize, halfZSize);
		m_points[5] = Vec3F(halfXSize, -halfYSize, halfZSize);
		m_points[6] = Vec3F(-halfXSize, halfYSize, halfZSize);
		m_points[7] = Vec3F(halfXSize, halfYSize, halfZSize);
	}

	CreateBox(const std::array<Vec3F, 8> &points) : m_points(points) {}

	CreateBox(const Scope &s) : m_points(s.GetWorldPoints()) {}

	void Execute(GeometryPtr geometryOut)
	{
		START_PROFILE;
		LOG_TRACE(GeometryOperations, "CreateBox.");
#ifdef DEBUG
		for (auto i = 0u; i < 8; ++i)
		{
			LOG_TRACE(GeometryOperations, " Point: %s", to_string(m_points[i]).c_str());
		}
#endif

		GeometryBuilderT<Geometry> builder(geometryOut);
		std::array<typename Geometry::Index_t, 8> pointIndices = {
		    builder.AddPoint(m_points[0]), builder.AddPoint(m_points[1]), builder.AddPoint(m_points[2]),
		    builder.AddPoint(m_points[3]), builder.AddPoint(m_points[4]), builder.AddPoint(m_points[5]),
		    builder.AddPoint(m_points[6]), builder.AddPoint(m_points[7]),
		};

		// Bottom face
		auto face = builder.StartFace(4);
		face.AddIndex(pointIndices[0]);
		face.AddIndex(pointIndices[2]);
		face.AddIndex(pointIndices[3]);
		face.AddIndex(pointIndices[1]);
		face.CloseFace();

		// Top Face
		face = builder.StartFace(4);
		face.AddIndex(pointIndices[4]);
		face.AddIndex(pointIndices[5]);
		face.AddIndex(pointIndices[7]);
		face.AddIndex(pointIndices[6]);
		face.CloseFace();

		// Front Face
		face = builder.StartFace(4);
		face.AddIndex(pointIndices[0]);
		face.AddIndex(pointIndices[1]);
		face.AddIndex(pointIndices[5]);
		face.AddIndex(pointIndices[4]);
		face.CloseFace();

		// Left Face
		face = builder.StartFace(4);
		face.AddIndex(pointIndices[0]);
		face.AddIndex(pointIndices[4]);
		face.AddIndex(pointIndices[6]);
		face.AddIndex(pointIndices[2]);
		face.CloseFace();

		// Right Face
		face = builder.StartFace(4);
		face.AddIndex(pointIndices[1]);
		face.AddIndex(pointIndices[3]);
		face.AddIndex(pointIndices[7]);
		face.AddIndex(pointIndices[5]);
		face.CloseFace();

		// Back Face
		face = builder.StartFace(4);
		face.AddIndex(pointIndices[2]);
		face.AddIndex(pointIndices[6]);
		face.AddIndex(pointIndices[7]);
		face.AddIndex(pointIndices[3]);
		face.CloseFace();
	}

private:
	std::array<Vec3F, 8> m_points;
};

}  // namespace selector

#endif