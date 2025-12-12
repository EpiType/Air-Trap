from conan import ConanFile

class RTypeConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    
    def requirements(self):
        self.requires("asio/1.28.0")
        self.requires("gtest/1.14.0")
        
        # Skip SFML on macOS (will be built from source with GCC)
        if self.settings.os != "Macos":
            self.requires("sfml/3.0.2")
    
    def configure(self):
        # Only configure SFML options if not on macOS
        if self.settings.os != "Macos":
            self.options["sfml"].shared = True
            self.options["sfml"].graphics = True
            self.options["sfml"].window = True
            self.options["sfml"].audio = True
            self.options["sfml"].network = False

