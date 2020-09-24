#ifndef PAGODA_PROCEDURAL_GRAPH_GRAPH_FORMAT_GRAMMAR_H_
#define PAGODA_PROCEDURAL_GRAPH_GRAPH_FORMAT_GRAMMAR_H_

#include "graph_definition_node.h"
#include "graph_reader_grammar_helper.h"

#include <boost/phoenix/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

#include <vector>

namespace pagoda::graph::io
{
template<class Iterator>
struct GraphReaderGrammar
  : boost::spirit::qi::grammar<Iterator, GraphDefinitionNodePtr(), boost::spirit::qi::space_type>
{
	template<typename... Args>
	using Rule_t = boost::spirit::qi::rule<Args..., boost::spirit::qi::space_type>;

	GraphReaderGrammar() : GraphReaderGrammar::base_type(graph_definition)
	{
		/*
		 * graph_definition -> (node_definition | node_links)*
		 * node_definition -> identifier = identifier
		 *                      "(" construction_args ")"
		 *                      ("{" execution_args "}")?
		 * operation_definition -> identifier = identifier "{" execution_args "}"
		 * identifier -> ("_" | alpha) (alnum | "_")*
		 * literal -> quoted_string | float
		 * expression -> "$<" expression_body ">$"
		 * expression_body -> .* \ ">$"
		 * construction_args -> (named_simple_arg ("," named_simple_arg)*)?
		 * named_simple_arg -> identifier ":" literal
		 * execution_args -> (named_expression_arg ("," named_expression_arg)*)?
		 * named_expression_arg -> identifier ":" ( expression | literal )
		 * link_definition -> (identifier:)?identifier(:identifier)?
		 * node_links -> link_definition "->" link_definition ("->" link_definition)*
		 */

		using namespace boost::spirit;
		using namespace boost::spirit::qi;
		using namespace grammar_helpers;
		using boost::phoenix::bind;

		// clang-format off
		/*
		 * identifier -> ("_" | alpha) (alnum | "_")*
		 */
		identifier %= (char_('_') | alpha) >> *(alnum | char_('_'));
		quoted_string %= ('"' > lexeme[*(char_ - char_('"'))] >> '"');

		/*
		 * literal -> quoted_string | float
		 */
		literal = quoted_string | float_;

		/*
		 * expression -> "$<" expression_body ">$"
		 */
		expression = "$<" >> expression_body >> ">$";

		/*
		 * expression_body = .* \ ">$"
		 */
		expression_body = lexeme[*(char_ - ">$")];

		/*
		 * named_simple_arg -> identifier ":" literal
		 */
		named_simple_arg = (identifier >> ':' >> quoted_string) [_val = bind(CreateStringNamedArgument, boost::spirit::_1, boost::spirit::_2)] |
                           (identifier >> ':' >> float_) [_val = bind(CreateFloatNamedArgument, boost::spirit::_1, boost::spirit::_2)] |
                           (identifier >> ':' >> int_ >> !float_) [_val = bind(CreateIntegerNamedArgument, boost::spirit::_1, boost::spirit::_2)];

		/*
		 * construction_args -> (named_simple_arg ("," named_simple_arg)*)?
		 */
		construction_args = -(named_simple_arg % ',');

		/*
		 * named_expression_arg -> identifier ":" ( expression | literal )
		 */
		named_expression_arg = named_simple_arg [_val = boost::spirit::_1 ]|
                               (identifier >> ':' >> expression) [_val = bind(CreateExpressionNamedArgument, boost::spirit::_1, boost::spirit::_2)];

		/*
		 * execution_args -> (named_expression_arg ("," named_expression_arg)*)?
		 */
		execution_args = -(named_expression_arg % ',');

		/*
		 * node_definition -> identifier = identifier
		 *                      "(" construction_args ")"
		 *                      ("{" execution_args "}")?
		 */
		node_definition = (
                identifier >> '=' >> identifier >>
                '(' >> construction_args >> ')') [ _val = bind(CreateNodeDefinition, boost::spirit::_1, boost::spirit::_2, boost::spirit::_3) ] >>
                -(('{' >> execution_args >> '}') [ _val = bind(SetExecutionArguments, _val, boost::spirit::_1) ]);

    /*
		 *operation_definition -> identifier = identifier "{" execution_args "}"
     */
    operation_definition = (identifier >> '=' >> identifier >> "{" >> execution_args >> "}")
                           [
                            _val = bind(CreateOperationDefinition, boost::spirit::_1, boost::spirit::_2, boost::spirit::_3)
                           ];

    /*
     * link_definition -> (identifier:)?identifier(:identifier)?
     */
    link_definition = eps[_val = bind(CreateLinkDefinition)] >>
                      (-(identifier >> '<') [bind(SetInputInterface, _val, boost::spirit::_1)] >>
                      identifier [bind(SetNodeName, _val, boost::spirit::_1)] >>
                      -('>' >> identifier) [bind(SetOutputInterface, _val, boost::spirit::_1)]);

		/*
		 * node_links -> link_definition "->" link_definition ("->" link_definition)*
		 */
		node_links = eps[ _val = bind(CreateNodeLink) ] >>
                     link_definition[ bind(AddLinkedNode, _val, boost::spirit::_1) ] >> "->" >>
                     (link_definition[ bind(AddLinkedNode, _val, boost::spirit::_1) ] % "->") > ';';

		/*
		 * graph_definition -> (node_definition | node_links)*
		 */
		graph_definition = eps[ _val = bind(CreateGraphDefinition) ] >>
                           *(
                            node_definition[ bind(AddNodeDefinition, _val, boost::spirit::_1) ] |
                            operation_definition[ bind(AddNodeDefinition, _val, boost::spirit::_1)] |
                            node_links[ bind(AddNodeLinks, _val, boost::spirit::_1) ]
                            );

		// clang-format on
	}

	Rule_t<Iterator, std::string()> identifier;
	Rule_t<Iterator, std::string()> quoted_string;
	Rule_t<Iterator, boost::variant<std::string, float>()> literal;
	Rule_t<Iterator, std::string()> expression;
	Rule_t<Iterator, std::string()> expression_body;
	Rule_t<Iterator, NamedArgumentPtr()> named_simple_arg;
	Rule_t<Iterator, std::vector<NamedArgumentPtr>()> construction_args;
	Rule_t<Iterator, NamedArgumentPtr()> named_expression_arg;
	Rule_t<Iterator, std::vector<NamedArgumentPtr>()> execution_args;
	Rule_t<Iterator, NodeDefinitionNodePtr()> node_definition;
	Rule_t<Iterator, NodeDefinitionNodePtr()> operation_definition;
	Rule_t<Iterator, NodeLinkDefinitionPtr()> link_definition;
	Rule_t<Iterator, NodeLinkNodePtr()> node_links;
	Rule_t<Iterator, GraphDefinitionNodePtr()> graph_definition;
};

}  // namespace pagoda::graph::io

#endif