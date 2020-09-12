#include <pagoda/dynamic/boolean_value.h>
#include <pagoda/dynamic/free_function_callable_body.h>
#include <pagoda/dynamic/type_info.h>

#include <gtest/gtest.h>

#include "../test_utils.h"

using namespace pagoda;
using namespace pagoda::dynamic;

static bool test_function1Called = false;
void test_function1() { test_function1Called = true; }

static BooleanPtr test_function2(BooleanPtr b) { return b; }

TEST(FreeFunctionCallableBody, test_no_args_void_return)
{
	FreeFunctionCallableBody<std::function<void(void)>> f(test_function1);
	f.Call({});
	ASSERT_TRUE(test_function1Called);
}

TEST(FreeFunctionCallableBody, test_args_return)
{
	FreeFunctionCallableBody<std::function<BooleanPtr(BooleanPtr)>> f(test_function2);
	try {
		std::vector<DynamicValueBasePtr> args;
		args.push_back(std::make_shared<Boolean>(true));
		f.Call(args);
	} catch (DynamicValueBasePtr &returnValue) {
		auto b = std::dynamic_pointer_cast<Boolean>(returnValue);
		ASSERT_NE(b, nullptr);
		EXPECT_TRUE(static_cast<bool>(b));
		return;
	}
	FAIL() << "Should have thrown";
}
