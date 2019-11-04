#include "input_interface_node.h"

#include "graph.h"
#include "node.h"
#include "node_set_visitor.h"
#include "output_interface_node.h"
#include "parameter/parameter.h"

#include <procedural_objects/procedural_operation.h>

namespace selector
{
const char* InputInterfaceNode::name = "InputInterface";

InputInterfaceNode::InputInterfaceNode() : m_interfaceName("", 0) {}
InputInterfaceNode::~InputInterfaceNode() {}

void InputInterfaceNode::SetConstructionArguments(
    const std::unordered_map<std::string, DynamicValueBasePtr>& constructionArgs)
{
	auto interfaceNameIter = constructionArgs.find("interface");
	if (interfaceNameIter == std::end(constructionArgs))
	{
		// TODO: throw
	}
	auto offsetIter = constructionArgs.find("offset");

	auto interfaceName = get_parameter_as<std::string>(interfaceNameIter->second);
	uint16_t offset = 0;
	if (offsetIter != std::end(constructionArgs))
	{
		offset = static_cast<uint16_t>(get_parameter_as<float>(offsetIter->second));
	}
	SetInterfaceName(InterfaceName(interfaceName, offset));
}

void InputInterfaceNode::SetInterfaceName(const InterfaceName& interfaceName) { m_interfaceName = interfaceName; }
const InterfaceName& InputInterfaceNode::GetInterfaceName() const { return m_interfaceName; }

void InputInterfaceNode::Execute(const NodeSet<Node>& inNodes, const NodeSet<Node>& outNodes)
{
	START_PROFILE;

	m_proceduralObjects.clear();
	NodeSet<OutputInterfaceNode> outputInterfaceNodes;
	node_type_filter<OutputInterfaceNode>(inNodes, outputInterfaceNodes);

	for (auto outNode : outputInterfaceNodes)
	{
		auto outNodeObjects = outNode->GetProceduralObjects();
		m_proceduralObjects.insert(std::end(m_proceduralObjects), std::begin(outNodeObjects), std::end(outNodeObjects));
	}
}
}  // namespace selector
