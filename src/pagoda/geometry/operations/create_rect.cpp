#include "create_rect.h"

#include <pagoda/geometry/algorithms/create_rect.h>
#include <pagoda/geometry/geometry_component.h>
#include <pagoda/geometry/geometry_system.h>

#include <pagoda/objects/hierarchical_component.h>
#include <pagoda/objects/hierarchical_system.h>
#include <pagoda/objects/procedural_component.h>
#include <pagoda/objects/procedural_object_system.h>

#include <pagoda/dynamic/float_value.h>
#include <pagoda/dynamic/get_value_as.h>

#include <boost/qvm/map_vec_mat.hpp>

using namespace pagoda::objects;

namespace pagoda::geometry::operations
{
using namespace geometry::core;
using namespace geometry::algorithms;
using namespace dynamic;

const std::string CreateRectGeometry::output_geometry("out");
const char* CreateRectGeometry::name = "CreateRectGeometry";

CreateRectGeometry::CreateRectGeometry(ProceduralObjectSystemPtr objectSystem) : ProceduralOperation(objectSystem)
{
	START_PROFILE;

	RegisterValues({{"width", std::make_shared<FloatValue>(0.0f)},
	                {"height", std::make_shared<FloatValue>(0.0f)},
	                {"plane", std::make_shared<String>("z")}});

	CreateOutputInterface(output_geometry);
}

CreateRectGeometry::~CreateRectGeometry() {}

void CreateRectGeometry::DoWork()
{
	START_PROFILE;

	float width = get_value_as<float>(*GetValue("width"));
	float height = get_value_as<float>(*GetValue("height"));
	std::string planeName = get_value_as<std::string>(*GetValue("plane"));
	boost::qvm::vec<float, 3> rectXAxis;
	boost::qvm::vec<float, 3> rectYAxis;

	switch (planeName[0])
	{
		case 'x':
			rectXAxis = boost::qvm::vec<float, 3>{0, 1, 0};
			rectYAxis = boost::qvm::vec<float, 3>{0, 0, 1};
			break;
		case 'y':
			rectXAxis = boost::qvm::vec<float, 3>{1, 0, 0};
			rectYAxis = boost::qvm::vec<float, 3>{0, 0, -1};
			break;
		case 'z':
			rectXAxis = boost::qvm::vec<float, 3>{1, 0, 0};
			rectYAxis = boost::qvm::vec<float, 3>{0, 1, 0};
			break;
		default:
			throw common::exception::Exception(
			    "The 'plane' parameter in create rect must be one of 'x', 'y', or 'z'. It was '" + planeName + "'");
	}

	auto geometrySystem = m_proceduralObjectSystem->GetComponentSystem<GeometrySystem>();
	auto hierarchicalSystem = m_proceduralObjectSystem->GetComponentSystem<HierarchicalSystem>();

	CreateRect<Geometry> create_rect(width, height, rectXAxis, rectYAxis);
	auto geometry = std::make_shared<Geometry>();
	create_rect.Execute(geometry);

	ProceduralObjectPtr object = CreateOutputProceduralObject(output_geometry);
	std::shared_ptr<GeometryComponent> geometry_component =
	    geometrySystem->CreateComponentAs<GeometryComponent>(object);

	geometry_component->SetGeometry(geometry);
	geometry_component->SetScope(Scope::FromGeometryAndConstrainedRotation(
	    geometry, boost::qvm::mat<float, 3, 3>(boost::qvm::diag_mat(boost::qvm::vec<float, 3>{1.0f, 1.0f, 1.0f}))));

	std::shared_ptr<HierarchicalComponent> hierarchical_component =
	    hierarchicalSystem->CreateComponentAs<HierarchicalComponent>(object);
}
}  // namespace pagoda::geometry::operations
