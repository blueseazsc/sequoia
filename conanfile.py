from conan import ConanFile

class Project(ConanFile):

    # all project are the same:
    python_requires = "project_base/1.0"
    python_requires_extend = "project_base.ProjectBase"

    def init(self):
        base = self.python_requires["project_base"].module.ProjectBase
        self.settings = base.settings
        self.options.update(base.options, base.default_options)
        self.revision_mode = base.revision_mode

    # difference between project:
    def requirements(self):
        self.requires("spdlog/1.14.1")
        self.requires("doctest/2.4.11")

    def build_requirements(self):
        super().build_requirements()
    
    # def package_info(self):
