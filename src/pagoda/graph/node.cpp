#include "node.h"

#include <pagoda/common/debug/assertions.h>
#include <pagoda/dynamic/expression.h>
#include <pagoda/dynamic/type_info.h>
#include <pagoda/dynamic/value_not_found.h>

using namespace pagoda::dynamic;

namespace pagoda::graph
{
const TypeInfoPtr Node::s_typeInfo = std::make_shared<TypeInfo>("Node");

Node::Node() : BuiltinClass(s_typeInfo), m_nodeName(""), m_nodeId(0) {}
Node::~Node() {}

uint32_t Node::GetId() const { return m_nodeId; }
void Node::SetId(uint32_t nodeId) { m_nodeId = nodeId; }

void Node::SetName(const std::string &name) { m_nodeName = name; }
const std::string &Node::GetName() const { return m_nodeName; }

void Node::SetExecutionArguments(const std::unordered_map<std::string, DynamicValueBasePtr> &arguments)
{
	for (const auto &arg : arguments) {
		RegisterMember(arg.first, arg.second);
	}
}

std::string Node::ToString() const { return "<Node>"; }

void Node::AcceptVisitor(ValueVisitorBase &visitor) { throw common::exception::Exception("Unimplemented"); }

}  // namespace pagoda::graph