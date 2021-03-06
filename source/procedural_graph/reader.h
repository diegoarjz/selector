#ifndef PAGODA_PROCEDURAL_GRAPH_READER_H_
#define PAGODA_PROCEDURAL_GRAPH_READER_H_

#include "graph.h"
#include "node.h"
#include "parse_result.h"

#include <string>
#include <unordered_map>

namespace pagoda
{
class NodeFactory;
using NodeFactoryPtr = std::shared_ptr<NodeFactory>;

class GraphReader
{
public:
	GraphReader(NodeFactoryPtr nodeFactory);
	~GraphReader();

	GraphPtr Read(const std::string &str);
	const ParseResult &GetParseResult() const;

private:
	struct Impl;
	std::unique_ptr<Impl> m_implementation;
};
}  // namespace pagoda

#endif
