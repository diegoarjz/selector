#include <gtest/gtest.h>

#include <common/file_util.h>
#include <common/logger.h>
#include <common/profiler.h>
#include <procedural_graph/graph.h>
#include <procedural_graph/reader.h>
#include <procedural_graph/default_scheduler.h>
#include <procedural_objects/geometry_system.h>
#include <procedural_objects/hierarchical_system.h>
#include <procedural_objects/procedural_object.h>

#include <selector.h>

#include <boost/filesystem/path.hpp>

using namespace selector;

class RegressionTest
{
public:
	static void SetExecutablePath(const std::string& path)
	{
		s_testFilesDirectory = boost::filesystem::path(path).remove_filename();
	}

	static boost::filesystem::path GetTestFilesDirectory() { return s_testFilesDirectory; }

	RegressionTest(const std::string& name, const std::vector<std::string>& filesToMatch)
	    : m_regressionTestName(name), m_filesToMatch(filesToMatch)
	{
		ReadGraphFromFile(GetInputGraph().string());
		ExecuteGraph();
		MatchFiles();
	}

	boost::filesystem::path GetInputGraph()
	{
		auto pathToGraphFile = GetTestFilesDirectory();
		pathToGraphFile /= "test_files";
		pathToGraphFile /= m_regressionTestName;
		pathToGraphFile /= m_regressionTestName + ".sel";
		return pathToGraphFile;
	}

	boost::filesystem::path GetExpectedResultFile(const std::string& expectedFile)
	{
		auto pathToExpectedFile = GetTestFilesDirectory();
		pathToExpectedFile /= "test_files";
		pathToExpectedFile /= m_regressionTestName;
		pathToExpectedFile /= expectedFile;
		return pathToExpectedFile;
	}

	std::shared_ptr<Graph> ReadGraphFromFile(const std::string& filePath)
	{
		m_graph = m_selector.CreateGraphFromFile(filePath);
        return m_graph;
	}

	void ExecuteGraph()
	{
        m_graph->Execute();
	}

	void MatchFiles()
	{
		for (const auto& f : m_filesToMatch)
		{
			std::string expectedFile = FileUtil::LoadFileToString(GetExpectedResultFile(f).string());
			std::string resultFile = FileUtil::LoadFileToString(f);
			EXPECT_EQ(expectedFile, resultFile);
		}
	}

private:
	std::string m_regressionTestName;
	std::vector<std::string> m_filesToMatch;

	GraphPtr m_graph;
    Selector m_selector;

	static boost::filesystem::path s_testFilesDirectory;
};
boost::filesystem::path RegressionTest::s_testFilesDirectory = "";

#define REGRESSION_TEST(NAME, FILES_TO_MATCH) \
	TEST(RegressionTestCase, NAME) { RegressionTest(#NAME, FILES_TO_MATCH); }

REGRESSION_TEST(create_rect, {"geometry.obj"});
REGRESSION_TEST(export_geometry, {"geometry.obj"});
REGRESSION_TEST(expression, {"geometry.obj"});
REGRESSION_TEST(extrusion, {"geometry.obj"});
REGRESSION_TEST(parameter_definition, {"geometry.obj"});
REGRESSION_TEST(parameter_overwrite, {"geometry.obj"});
REGRESSION_TEST(parameter_renaming, {"geometry.obj"});

int main(int argc, char* argv[])
{
	RegressionTest::SetExecutablePath(argv[0]);
	::testing::InitGoogleTest(&argc, argv);

	auto returnVal = RUN_ALL_TESTS();

	ConsoleProfilerLogger consoleLogger(ProfilerManager::Instance());
	consoleLogger.Log(20);

	return returnVal;
}