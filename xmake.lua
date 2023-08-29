set_project("bonding")
set_version("0.1.3")
set_xmakever("2.8.1")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

add_requires("libcap", "libseccomp", "spdlog", "nlohmann_json")

target("bonding")
    set_kind("binary")
    set_languages("c++17")

    add_files("src/*.cpp")
    add_packages("spdlog", "result", "nlohmann_json")
    add_links("seccomp", "cap")

    after_build(function (target)
        import("core.project.project")
        import("core.base.task")
        
        task.run("project", {kind = "cmake", outputdir = "."})
        task.run("project", {kind = "ninja", outputdir = "."})
    end)
