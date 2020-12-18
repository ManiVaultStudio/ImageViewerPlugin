from conans import ConanFile, CMake, tools
import os
import shutil
import json
import pathlib

class ImageViewerPluginConan(ConanFile):
    #TODO wrap with Conan build tools to extract version from source
    name = "ImageViewerPlugin"
    version = "latest"
    license = "MIT"
    author = "B. van Lew b.van_lew@lumc.nl"
    # The url for the conan recipe
    url = "https://github.com/bldrvnlw/conan-ImageViewerPlugin"
    description = "A plugin for viewing image data in the high-dimensional plugin system (HDPS)."
    topics = ("hdps", "plugin", "image data", "loading")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    #exports_sources = ["CMakeLists.txt", "build_trigger.json"]
    generators = ("cmake")

    # Custom attributes for Bincrafters recipe conventions
    _source_subfolder = name
    _build_subfolder = "build_subfolder"
    install_dir = None

    requires = (
        "qt/5.15.1@lkeb/stable",
        "hdps-core/latest@lkeb/stable"
    )

    scm = {
        "type": "git",
        "subfolder": "hdps/ImageViewerPlugin",
        "url": "auto",
        "revision": "auto"
    }

    def _configure_cmake(self, build_type):
        # locate Qt root to allow find_package to work
        qtpath = pathlib.Path(self.deps_cpp_info["qt"].rootpath)
        qt_root = str(list(qtpath.glob('**/Qt5Config.cmake'))[0].parents[3])
        print("Qt root ", qt_root)

        cmake = CMake(self, build_type=build_type)
        if self.settings.os == "Windows" and self.options.shared:
            cmake.definitions["CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS"] = True
        cmake.definitions["CMAKE_PREFIX_PATH"] = qt_root
        cmake.configure(source_folder="hdps/ImageViewerPlugin")  # needed for scm
        cmake.verbose = True
        return cmake

    def build(self):
        print('Build OS is : ', self.settings.os)
        # If the user has no preference in HDPS_INSTALL_DIR simply set the install dir
        if not os.environ.get('HDPS_INSTALL_DIR', None):
            os.environ['HDPS_INSTALL_DIR'] = os.path.join(self.build_folder, "install")
        print('HDPS_INSTALL_DIR: ', os.environ['HDPS_INSTALL_DIR'])
        self.install_dir = os.environ['HDPS_INSTALL_DIR']

        # The ImageViewerPlugin build expects the HDPS package to be in this install dir
        hdps_pkg_root= self.deps_cpp_info["hdps-core"].rootpath
        print("Install dir type: ", os.path.join(self.install_dir, self.settings.get_safe("build_type")))
        shutil.copytree(hdps_pkg_root, os.path.join(self.install_dir, self.settings.get_safe("build_type")))

        cmake_debug = self._configure_cmake('Debug')
        cmake_debug.build()

        cmake_release = self._configure_cmake('Release')
        cmake_release.build()


    def package(self):
        print('Packaging install dir: ', self.install_dir)
        self.copy(pattern="*", src=self.install_dir)

    def package_info(self):
        self.cpp_info.debug.libdirs = ['Debug/lib']
        self.cpp_info.debug.bindirs = ['Debug/Plugins', 'Debug']
        self.cpp_info.debug.includedirs = ['Debug/include', 'Debug']
        self.cpp_info.release.libdirs = ['Release/lib']
        self.cpp_info.release.bindirs = ['Release/Plugins', 'Release']
        self.cpp_info.release.includedirs = ['Release/include', 'Release']

