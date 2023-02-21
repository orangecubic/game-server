add_rules("mode.debug", "mode.release")

add_requires("conan::gflags/2.2.2", {alias = "gflags"})
add_requires("conan::flatbuffers/22.12.06", {alias = "flatbuffers"})
add_requires("conan::box2d/2.4.1", {alias = "box2d"})

set_languages("c++20")
package("photon")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "library/photon"))
    set_policy("package.install_always", true)
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_OSX_ARCHITECTURES=" .. ("arm64"))
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("photon")

target("game-server")
    set_kind("binary")
    add_includedirs("library")
    add_files("game-server/**.cpp")
    add_packages("photon", "gflags", "box2d", "flatbuffers")
