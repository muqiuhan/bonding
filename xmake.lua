set_project("bonding")
set_version("0.1.3")
set_xmakever("2.8.1")

includes("src/**/xmake.lua")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

add_requires("nlohmann_json", "libcap", "libseccomp", "plog")

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
    set_warnings("all")
    add_cxxflags("-stdlib=libc++", "-fPIE")
    add_ldflags("-stdlib=libc++")
    add_files("src/*.cpp")
    add_packages("nlohmann_json", "libcap", "libseccomp", "plog")
    add_deps("logging")
    