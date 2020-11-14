from conans import tools, ConanFile, CMake

import os

class Pagoda(ConanFile):
    name = "pagoda"
    description = "Graph-Based Procedural Modelling Framework"
    version = "0.1"
    license = "MIT"
    author = "Diego Jesus <diego.a.r.jz@gmail.com>"
    url = "https://github.com/diegoarjz/pagoda"
    description = "<Description of Hello here>"
    topics = ("procedural modelling", "geometry", "content generation")

    settings = "os", "compiler", "arch", "build_type"
    default_settings = (
        "build_type=Release"
    )
    options = {
        "shared" : [True, False]
    }
    default_options = (
        "shared=False"
    )
    generators = "cmake"
    exports_sources = "*"

    requires = [
        ("boost/1.73.0"),
        ("gtest/1.10.0"),
        ("libpng/1.6.37"),
        ("nlohmann_json/3.9.1")
    ]


    def build(self):
        cmake = CMake(self, build_type=self.settings.build_type)

        # CI Definitions
        cmake.definitions['PAGODA_VERSION'] = self.version
        if 'GITHUB_RUN_NUMBER' in os.environ:
            cmake.definitions['PAGODA_BUILD_NUMBER'] = os.environ['GITHUB_RUN_NUMBER']

        cmake.configure(defs=cmake.definitions)
        cmake.build()


    def package(self):
        self.copy("bin/pagoda", "bin", keep_path=False)
        self.copy("bin/pagoda.exe", "bin", keep_path=False)
        self.copy("lib/*", "lib", keep_path=False)


    def package_info(self):
        self.cpp_info.libs = ["pagoda"]
