#include "pagoda.h"

#include "common/factory.h"
#include "common/file_util.h"
#include "common/logger.h"

#include <procedural_graph/graph.h>
#include <procedural_graph/input_interface_node.h>
#include <procedural_graph/operation_node.h>
#include <procedural_graph/output_interface_node.h>
#include <procedural_graph/parameter_node.h>
#include <procedural_graph/reader.h>
#include <procedural_graph/router_node.h>

#include <procedural_objects/clip_geometry.h>
#include <procedural_objects/create_box.h>
#include <procedural_objects/create_rect.h>
#include <procedural_objects/create_sphere.h>
#include <procedural_objects/export_geometry.h>
#include <procedural_objects/extract_faces.h>
#include <procedural_objects/extract_scope.h>
#include <procedural_objects/extrude_geometry.h>
#include <procedural_objects/face_offset.h>
#include <procedural_objects/geometry_system.h>
#include <procedural_objects/hierarchical_system.h>
#include <procedural_objects/repeat_split.h>
#include <procedural_objects/rotate.h>
#include <procedural_objects/scale.h>
#include <procedural_objects/split.h>
#include <procedural_objects/translate.h>
#include <procedural_objects/triangulate_geometry.h>

#include <procedural_objects/procedural_object_predicate_registry.h>
#include <procedural_objects/scope_axis_direction_predicate.h>

namespace pagoda
{
class Pagoda::Impl
{
public:
	Impl()
	{
		LOG_TRACE(Core, "Initializing Pagoda");

		m_proceduralObjectSystem = std::make_shared<ProceduralObjectSystem>();
		m_proceduralObjectSystem->RegisterProceduralComponentSystem(std::make_shared<GeometrySystem>());
		m_proceduralObjectSystem->RegisterProceduralComponentSystem(std::make_shared<HierarchicalSystem>());

		m_operationFactory = std::make_shared<OperationFactory>();
		m_predicateRegistry = std::make_shared<ProceduralObjectPredicateRegistry>();

		// Register Nodes
		{
			m_nodeFactory = std::make_shared<NodeFactory>();
			m_nodeFactory->Register("InputInterface", []() { return std::make_shared<InputInterfaceNode>(); });
			m_nodeFactory->Register("OutputInterface", []() { return std::make_shared<OutputInterfaceNode>(); });
			m_nodeFactory->Register("Parameter", []() { return std::make_shared<ParameterNode>(); });
			m_nodeFactory->Register("Operation",
			                        [this]() { return std::make_shared<OperationNode>(m_operationFactory); });
			m_nodeFactory->Register("Router", [this]() { return std::make_shared<RouterNode>(m_predicateRegistry); });
		}

		// Register Operations
		{
			m_operationFactory->Register("CreateRectGeometry", [this]() {
				return std::make_shared<CreateRectGeometry>(m_proceduralObjectSystem);
			});
			m_operationFactory->Register("CreateSphereGeometry", [this]() {
				return std::make_shared<CreateSphereGeometry>(m_proceduralObjectSystem);
			});
			m_operationFactory->Register("CreateBoxGeometry", [this]() {
				return std::make_shared<CreateBoxGeometry>(m_proceduralObjectSystem);
			});
			m_operationFactory->Register(
			    "ExportGeometry", [this]() { return std::make_shared<ExportGeometry>(m_proceduralObjectSystem); });
			m_operationFactory->Register("ExtractScope",
			                             [this]() { return std::make_shared<ExtractScope>(m_proceduralObjectSystem); });
			m_operationFactory->Register(
			    "ExtrudeGeometry", [this]() { return std::make_shared<ExtrudeGeometry>(m_proceduralObjectSystem); });
			m_operationFactory->Register(
			    "FaceOffset", [this]() { return std::make_shared<FaceOffsetOperation>(m_proceduralObjectSystem); });
			m_operationFactory->Register("TriangulateGeometry", [this]() {
				return std::make_shared<TriangulateGeometry>(m_proceduralObjectSystem);
			});
			m_operationFactory->Register("ClipGeometry",
			                             [this]() { return std::make_shared<ClipGeometry>(m_proceduralObjectSystem); });
			m_operationFactory->Register("RepeatSplit",
			                             [this]() { return std::make_shared<RepeatSplit>(m_proceduralObjectSystem); });
			m_operationFactory->Register("ExtractFaces",
			                             [this]() { return std::make_shared<ExtractFaces>(m_proceduralObjectSystem); });
			m_operationFactory->Register("Translate",
			                             [this]() { return std::make_shared<Translate>(m_proceduralObjectSystem); });
			m_operationFactory->Register("Split",
			                             [this]() { return std::make_shared<Split>(m_proceduralObjectSystem); });
			m_operationFactory->Register("Scale",
			                             [this]() { return std::make_shared<Scale>(m_proceduralObjectSystem); });
			m_operationFactory->Register("Rotate",
			                             [this]() { return std::make_shared<Rotate>(m_proceduralObjectSystem); });
		}

		// Register Predicates
		{
			m_predicateRegistry->Register(
			    "front", std::make_shared<ScopeAxisDirectionPredicate>(m_proceduralObjectSystem, 'z', Vec3F{1, 0, 0}));
			m_predicateRegistry->Register(
			    "back", std::make_shared<ScopeAxisDirectionPredicate>(m_proceduralObjectSystem, 'z', Vec3F{-1, 0, 0}));

			m_predicateRegistry->Register(
			    "up", std::make_shared<ScopeAxisDirectionPredicate>(m_proceduralObjectSystem, 'z', Vec3F{0, 0, 1}));
			m_predicateRegistry->Register(
			    "down", std::make_shared<ScopeAxisDirectionPredicate>(m_proceduralObjectSystem, 'z', Vec3F{0, 0, -1}));

			m_predicateRegistry->Register(
			    "left", std::make_shared<ScopeAxisDirectionPredicate>(m_proceduralObjectSystem, 'z', Vec3F{0, 1, 0}));
			m_predicateRegistry->Register(
			    "right", std::make_shared<ScopeAxisDirectionPredicate>(m_proceduralObjectSystem, 'z', Vec3F{0, -1, 0}));
			m_predicateRegistry->Register(
			    "side", std::make_shared<ScopeAxisDirectionPredicate>(m_proceduralObjectSystem, 'y', Vec3F{0, 0, 1}));
		}
	}

	~Impl()
	{
		LOG_TRACE(Core, "Shutting down Pagoda");
		Logger::Shutdown();
	}

	ProceduralObjectSystemPtr GetProceduralObjectSystem() { return m_proceduralObjectSystem; }

	OperationFactoryPtr GetOperationFactory() { return m_operationFactory; }

	NodeFactoryPtr GetNodeFactory() { return m_nodeFactory; }

	GraphPtr CreateGraph() { return std::make_shared<Graph>(GetNodeFactory()); }

	GraphPtr CreateGraphFromFile(const std::string &filePath)
	{
		LOG_TRACE(Core, "Creating Graph From File: " << filePath.c_str());
		GraphReader reader(GetNodeFactory());
		GraphPtr graph = reader.Read(file_util::LoadFileToString(filePath));
		return graph;
	}

private:
	ProceduralObjectSystemPtr m_proceduralObjectSystem;
	OperationFactoryPtr m_operationFactory;
	NodeFactoryPtr m_nodeFactory;
	ProceduralObjectPredicateRegistryPtr m_predicateRegistry;
};

Pagoda::Pagoda() : m_implementation(std::make_unique<Pagoda::Impl>()) {}
Pagoda::~Pagoda() {}

ProceduralObjectSystemPtr Pagoda::GetProceduralObjectSystem()
{
	return m_implementation->GetProceduralObjectSystem();
}

OperationFactoryPtr Pagoda::GetOperationFactory() { return m_implementation->GetOperationFactory(); }

NodeFactoryPtr Pagoda::GetNodeFactory() { return m_implementation->GetNodeFactory(); }

GraphPtr Pagoda::CreateGraph() { return m_implementation->CreateGraph(); }

GraphPtr Pagoda::CreateGraphFromFile(const std::string &filePath)
{
	return m_implementation->CreateGraphFromFile(filePath);
}
}  // namespace pagoda
