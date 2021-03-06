#pragma once

#include <common/assertions.h>
#include <common/profiler.h>
#include <geometry_core/geometry.h>
#include <geometry_core/geometry_builder.h>

#include <math_lib/bissectrix.h>

namespace pagoda
{
template<class G>
class FaceOffset
{
private:
	using Geometry = G;
	using GeometryPtr = std::shared_ptr<Geometry>;
	using Index_t = typename Geometry::Index_t;

public:
	explicit FaceOffset(const float& amount) : m_amount(amount)
	{
		DBG_ASSERT_MSG(amount != 0, "Can't offset by zero.");
	}

	template<class T>
	void Execute(GeometryPtr geometryIn, T innerGeometries, T outerGeometries)
	{
		START_PROFILE;
		LOG_TRACE(GeometryOperations, "FaceOffset. Amount: " << m_amount);

		for (auto fIter = geometryIn->FacesBegin(); fIter != geometryIn->FacesEnd(); ++fIter)
		{
			std::vector<Vec3F> outerPoints;
			std::vector<Vec3F> innerPoints;
			for (auto fvIter = geometryIn->FaceSplitPointCirculatorBegin(*fIter); fvIter; ++fvIter)
			{
				auto curr = geometryIn->GetPosition(geometryIn->GetPoint(*fvIter));
				auto prev = geometryIn->GetPosition(geometryIn->GetPoint(geometryIn->GetPrevSplitPoint(*fvIter)));
				auto next = geometryIn->GetPosition(geometryIn->GetPoint(geometryIn->GetNextSplitPoint(*fvIter)));

				auto currBiss = m_amount * bissectrix((prev - curr), (next - curr));

				outerPoints.push_back(curr);
				innerPoints.push_back(curr + currBiss);
			}

			// inner geometry
			auto inner = std::make_shared<Geometry>();
			GeometryBuilderT innerBuilder(inner);
			auto face = innerBuilder.StartFace(innerPoints.size());
			for (const auto& i : innerPoints)
			{
				face.AddIndex(innerBuilder.AddPoint(i));
			}
			face.CloseFace();
			innerGeometries = inner;

			// outer geometries
			for (auto i = 0u; i < outerPoints.size(); ++i)
			{
				auto outer = std::make_shared<Geometry>();
				GeometryBuilderT outerBuilder(outer);

				face = outerBuilder.StartFace(4);
				face.AddIndex(outerBuilder.AddPoint(outerPoints[i]));
				face.AddIndex(outerBuilder.AddPoint(outerPoints[(i + 1) % outerPoints.size()]));
				face.AddIndex(outerBuilder.AddPoint(innerPoints[(i + 1) % innerPoints.size()]));
				face.AddIndex(outerBuilder.AddPoint(innerPoints[i]));
				face.CloseFace();

				outerGeometries = outer;
			}
		}
	}

private:
	float m_amount;
};
}  // namespace pagoda
