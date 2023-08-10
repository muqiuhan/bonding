add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
add_rules("plugin.vsxmake.autoupdate", {outputdir = "."})

add_requires("libcap")
add_requires("libcgroup")
add_requires("libseccomp")

package("spdlog")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "lib/spdlog"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

package("structopt")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "lib/structopt"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

package("result")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "lib/result"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

target("bonding")
    set_kind("binary")
    set_languages("c++20")

    add_files("src/*.cpp")
    add_includedirs("lib/spdlog/include", 
                    "lib/structopt/include", 
                    "lib/result/include")
    add_packages("spdlog", "structopt", "result")
    add_links("seccomp", "cap", "cgroup")

    after_build(function (target)
        import("core.project.project")
        import("core.base.task")
        
        task.run("project", {kind = "makefile", outputdir = "."})
        task.run("project", {kind = "ninja", outputdir = "."})
    end)