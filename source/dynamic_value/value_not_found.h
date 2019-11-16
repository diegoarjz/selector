#pragma once

#include "common/exception.h"

#include <stdexcept>
#include <string>

namespace selector
{
class ValueNotFoundException : public Exception
{
public:
	ValueNotFoundException(const std::string &name);
};
}  // namespace selector
