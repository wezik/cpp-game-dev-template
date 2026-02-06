from conan import ConanFile

class CppGame(ConanFile):
    generators = ("MesonToolchain", "PkgConfigDeps")
    settings = ("os", "compiler", "build_type", "arch")

    def requirements(self):
        self.requires("raylib/5.5")
        self.requires("glfw/3.4")
        self.requires("flecs/4.1.1")
        self.requires("joltphysics/5.2.0")

    def build_requirements(self):
        self.build_requires("meson/[>=1.10.0]")
        self.build_requires("ninja/[>=1.13.2]")
