#pragma once

#include <pagoda/graph/node_set.h>

#include <functional>
#include <memory>

namespace pagoda::graph
{
class Graph;
class Node;
using NodePtr = std::shared_ptr<Node>;
}  // namespace pagoda::graph

namespace pagoda::graph::query
{
/**
 * Base class to perform queries on a \c Graph.
 *
 * \code{.cpp}
 * NodeSet<Node> result;
 * Query q(result);
 * graph->ExecuteQuery(q);
 * \endcode
 *
 * or
 *
 * \code{.cpp}
 * std::vector<NodePtr> result;
 * Query q([&result](NodePtr n) { result.push_back(n); });
 * graph->ExecuteQuery(q);
 * \endcode
 */
class Query
{
public:
    /// Function type to handle each \c Node that matches a \c Query.
	using QueryHandle_t = std::function<void(NodePtr)>;

    /**
     * Constructor with a custom \c QueryHandle_t.
     */
	Query(QueryHandle_t queryHandle);
    /**
     * Specialized constructor that adds results in the \p nodeSet \c NodeSet.
     */
	Query(NodeSet<Node>& nodeSet);

	virtual bool Matches(NodePtr n);

	const std::string& GetQueryHash() const;

protected:
	Graph* m_graph;
	QueryHandle_t m_queryHandle;

private:
	void Start(Graph* graph);
	void AddNode(NodePtr n);

    friend class pagoda::graph::Graph;
};
}  // namespace pagoda::graph::query
