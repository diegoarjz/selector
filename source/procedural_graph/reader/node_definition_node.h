#ifndef SELECTOR_PROCEDURAL_GRAPH_NODE_DEFINITION_NODE_H_
#define SELECTOR_PROCEDURAL_GRAPH_NODE_DEFINITION_NODE_H_

#include "graph_statement_node.h"

namespace selector
{
class NamedArgument;
using NamedArgumentPtr = std::shared_ptr<NamedArgument>;

/**
 * Represents a node definition statement in the graph format.
 */
class NodeDefinitionNode : public GraphStatementNode
{
public:
	/// Storage type for construction arguments
    using ConstructionArgumentContainer_t = std::vector<NamedArgumentPtr>;
    /// Storage type for execution arguments
    using ExecutionArgumentContainer_t = std::vector<NamedArgumentPtr>;

	/**
	 * Constructs a \c NodeDefinitionNode with the \p name, \p nodeType and \p args.
	 */
	NodeDefinitionNode(const std::string &name, const std::string &nodeType, const ConstructionArgumentContainer_t &args);
	
	/**
	 * Constructs a \c NodeDefinitionNode with the \p name, \p nodeType and \p args, passing \p startOffset and \p endOffset to the respective \c AstNode constructor.
	 */
	NodeDefinitionNode(const NodeOffset_t &startOffset, const NodeOffset_t &endOffset, const std::string &name, const std::string &nodeType, const ConstructionArgumentContainer_t &args);
	
	/**
	 * Constructs a \c NodeDefinitionNode with the \p name, \p nodeType and \p args, passing \p nodeRange to the respective \c AstNode constructor.
	 */
	NodeDefinitionNode(const NodeRange_t &nodeRange, const std::string &name, const std::string &nodeType, const ConstructionArgumentContainer_t &args);
    
    virtual ~NodeDefinitionNode();
    
    /**
     * Returns the node name.
     */
    const std::string& GetNodeName() const;
    
    /**
     * Returns the node name.
     */
   	std::string& GetNodeName();
   	
   	/**
   	 * Sets the node name.
   	 */
   	void SetNodeName(const std::string &name);
   	
   	/**
   	 * Returns the construction arguments.
   	 */
    const ConstructionArgumentContainer_t& GetConstructionArguments() const;
    
    /**
     * Sets the construction arguments.
     */
    void SetConstructionArguments(const ConstructionArgumentContainer_t &args);
    
    /**
     * Adds the \p arg to the construction arguments.
     */
    void AddConstructionArgument(const NamedArgumentPtr &arg);
    
    /**
     * Creates and returns a construction argument with the give arguments.
     * \p args should match one of the \c NamedArgument constructors.
     */
    template<typename Args...)
    NamedArgumentPtr CreateConstructionArgument(Args... args)
    {
    	m_constructionArguments.emplace_back(args...);
    	return m_constructionArguments.back();
	}
    
    /**
     * Returns the execution arguments.
     */
    const ExecutionArgumentContainer_t& GetExecutionArguments()  const;
    
    /**
     * Returns the execution arguments.
     */
    void SetExecutionArguments(const ExecutionArgumentContainer_t &args);
    
    /**
     * Adds the \p arg to the execution arguments.
     */
    void AddExecutionArgument(const NamedArgumentPtr &arg);
    
    /**
     * Creates and returns an execution argument with the give arguments.
     * \p args should match one of the \c NamedArgument constructors.
     */
    template<typename Args...>
    NamedArgumentPtr CreateExecutionArgument(Args... args)
    {
    	m_executionArguments.emplace_back(args...);
    	return m_executionArguments.back();
    }
    
private:
    std::string m_nodeName;										///< The graph's \c Node name.
    std::string m_nodeType;										///< The type of node to create.
    ConstructionArgumentContainer_t m_constructionArguments;		///< The construction arguments.
    ExecutionArgumentContainer_t m_executionArguments;			///< The execution arguments.
};

using NodeDefinitionNodePtr = std::shared_ptr<NodeDefinitionNode>;
}

#endif