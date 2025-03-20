set_project("bonding")
set_version("0.1.3")
set_xmakever("2.8.1")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

add_requires("plog", "nlohmann_json", "libcap", "libseccomp")

package("libseccomp")
    set_sourcedir(path.join(os.scriptdir(), "3rd/libseccomp"))
    on_install("macosx", "linux", "mingw", function (package)
        local configs = {}
        table.insert(configs, "--enable-shared=" .. (package:config("shared") and "yes" or "no"))
        table.insert(configs, "--enable-static=" .. (package:config("shared") and "no" or "yes"))
        import("package.tools.autoconf").install(package, configs)
    end)

target("bonding")
    set_kind("binary")
    set_languages("c++23")
    set_policy("check.auto_ignore_flags", false)
    set_toolchains("clang")

    add_cxxflags("-stdlib=libc++")
    add_ldflags("-stdlib=libc++")
    add_files("src/*.cpp")
    add_packages("plog", "result", "nlohmann_json", "libcap", "libseccomp")
