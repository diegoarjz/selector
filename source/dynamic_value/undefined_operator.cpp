#include "undefined_operator.h"

#include "type_info.h"

namespace pagoda
{
UndefinedBinaryOperatorException::UndefinedBinaryOperatorException(const std::string &op, const TypeInfoPtr &lhsType,
                                                                   const TypeInfoPtr &rhsType)
    : Exception("Undefined Binary Operator."),  // + op + " between " + lhsType->Name() + " and " + rhsType->Name()),
      m_operatorName(op),
      m_lhsType(lhsType),
      m_rhsType(rhsType)
{
}

UndefinedBinaryOperatorException::~UndefinedBinaryOperatorException() {}

UndefinedUnaryOperatorException::UndefinedUnaryOperatorException(const std::string &op, const TypeInfoPtr &operand)
    : Exception("Undefined Unary Operator."),  // + op + " for " + operand->Name()),
      m_operatorName(op),
      m_operandType(operand)
{
}

UndefinedUnaryOperatorException::~UndefinedUnaryOperatorException() {}
}  // namespace pagoda
