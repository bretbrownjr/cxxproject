from pathlib import Path

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.cps import CPSDeps

required_conan_version = ">=2.32"


class CxxprojectConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    requires = "nlohmann_json/[>=3.11]", "valijson/[>=1.0]"
    tool_requires = "cmake/[>=4.4]", "ninja/[>=1.12]"

    def build_requirements(self):
        if not self.conf.get("tools.build:skip_test", default=False):
            self.test_requires("catch2/[>=3.0]")

    def _cmake_generator(self):
        return self.conf.get("tools.cmake.cmaketoolchain:generator", default="Ninja")

    def layout(self):
        cmake_layout(self, generator=self._cmake_generator())

    def _cps_config_name(self):
        values = (
            self.settings.compiler,
            self.settings.compiler.version,
            self.settings.arch,
            self.settings.compiler.cppstd,
            self.settings.build_type,
        )
        return "-".join(str(value).lower() for value in values if value)

    def _cps_search_path(self):
        return Path(self.folders.base_build) / "build" / "cps" / self._cps_config_name()

    def generate(self):
        CPSDeps(self).generate()
        deps = CMakeDeps(self)
        for name in ("nlohmann_json", "valijson"):
            deps.set_property(name, "cmake_find_mode", "none")
        deps.generate()

        tc = CMakeToolchain(self, generator=self._cmake_generator())
        tc.cache_variables["CMAKE_PREFIX_PATH"] = str(self._cps_search_path())
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if not self.conf.get("tools.build:skip_test", default=False):
            cmake.ctest(cli_args=["--output-on-failure", "--no-tests=error"])
