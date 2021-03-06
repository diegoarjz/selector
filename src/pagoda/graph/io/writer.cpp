#include "writer.h"

#include "graph_reader_grammar.h"

#include "graph_definition_node.h"

#include <pagoda/graph/input_interface_node.h>
#include <pagoda/graph/node.h>
#include <pagoda/graph/node_visitor.h>
#include <pagoda/graph/operation_node.h>
#include <pagoda/graph/output_interface_node.h>
#include <pagoda/graph/parameter_node.h>
#include <pagoda/graph/traversal/forward.h>

#include <pagoda/graph/graph.h>

#include <pagoda/dynamic/value_visitor.h>

#include <boost/spirit/include/karma.hpp>

using namespace pagoda::graph::traversal;
using namespace pagoda::dynamic;

namespace pagoda::graph::io
{
namespace
{
class NodeTypeVisitor : public pagoda::graph::NodeVisitor
{
	public:
	NodeTypeVisitor(std::ostream& outStream) : m_outStream{outStream} {}
	void Visit(std::shared_ptr<OperationNode> n) override { m_outStream << n->GetOperation()->GetOperationName(); }
	void Visit(std::shared_ptr<InputInterfaceNode> n) override
	{
		m_outStream << "InputInterface(interface: \"" << n->GetInterfaceName() << "\")";
	}
	void Visit(std::shared_ptr<OutputInterfaceNode> n) override
	{
		m_outStream << "OutputInterface(interface: \"" << n->GetInterfaceName() << "\")";
	}
	void Visit(std::shared_ptr<ParameterNode> n) override { m_outStream << "Parameter()"; }
	std::ostream& m_outStream;
};
}  // namespace

class ParameterVisitor : public ValueVisitorBase
{
	public:
	ParameterVisitor(const std::string& name, std::vector<std::string>& parameters)
	  : m_parameterName{name}, m_parameters{parameters}
	{
	}

	void Visit(Boolean& v) override {}
	void Visit(FloatValue& v) override { write(v.ToString()); }
	void Visit(Integer& v) override { write(v.ToString()); }
	void Visit(String& v) override { write("\"" + v.ToString() + "\""); }
	void Visit(NullObject& v) override {}
	void Visit(TypeInfo& v) override {}
	void Visit(Vector3& v) override
	{
		std::stringstream ss;
		ss << "$< Vector3(" << v.GetX() << ", " << v.GetY() << ", " << v.GetZ() << "); $>";
		write(ss.str());
	}
	void Visit(DynamicPlane& v) override
	{
		std::stringstream ss;
		auto p = v.GetPoint();
		auto n = v.GetNormal();
		ss << "$< Plane(Vector3(" << p->GetX() << ", " << p->GetY() << ", " << p->GetZ() << "), Vector3(" << n->GetX()
		   << ", " << n->GetY() << ", " << n->GetZ() << ")); >$";
		write(ss.str());
	}
	void Visit(Function& v) override {}
	void Visit(DynamicClass& v) override {}
	void Visit(DynamicInstance& v) override {}
	void Visit(Expression& v) override { write("$< " + v.GetExpressionString() + ">$"); }
	void Visit(objects::ProceduralOperation& v) override {}

	private:
	void write(const std::string& val) { m_parameters.push_back(m_parameterName + " : " + val); }

	const std::string& m_parameterName;
	std::vector<std::string>& m_parameters;
};

struct GraphWriter::Impl
{
	Impl(GraphPtr graph) : m_graph{graph} {}

	void Write(std::ostream& out)
	{
		using boost::spirit::karma::generate;
		using boost::spirit::karma::int_;

		auto graphDef = std::make_shared<GraphDefinitionNode>();

		NodeTypeVisitor typeVisitor{out};
		Forward traversal(*m_graph);
		std::vector<std::string> links;

		std::unordered_set<NodePtr> visitedNodes;

		while (traversal.HasNext()) {
			auto node = traversal.Get();
			if (visitedNodes.find(node) == visitedNodes.end()) {
				visitedNodes.insert(node);

				out << node->GetName() << " = ";
				node->AcceptNodeVisitor(&typeVisitor);

				auto membersEnd = node->GetMembersEnd();
				auto memberCount = node->GetMemberCount();
				std::vector<std::string> parameters;
				parameters.reserve(memberCount);

				for (auto p = node->GetMembersBegin(); p != membersEnd; ++p) {
					ParameterVisitor parameterVisitor{p->first, parameters};
					p->second.m_value->AcceptVisitor(parameterVisitor);
				}

				out << "{\n";
				if (!parameters.empty()) {
					for (auto i = 0u; i < parameters.size(); ++i) {
						out << "  " << parameters[i];
						if (i + 1 != parameters.size()) {
							out << ",";
						}
						out << "\n";
					}
				}
				out << "}\n";

				auto inputNodes = m_graph->GetNodeInputNodes(node->GetName());
				for (auto& in : inputNodes) {
					links.push_back(in->GetName() + " -> " + node->GetName() + ";");
				}
			}

			traversal.Advance();
		}

		for (const auto& l : links) {
			out << l << std::endl;
		}
	}

	GraphPtr m_graph;
};

GraphWriter::GraphWriter(GraphPtr graph) : m_implementation{std::make_unique<GraphWriter::Impl>(graph)} {}
GraphWriter::~GraphWriter() {}

void GraphWriter::Write(std::ostream& outStream) { m_implementation->Write(outStream); }
}  // namespace pagoda::graph::io
