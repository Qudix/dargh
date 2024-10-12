-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse")

-- set project
set_project("skse-dargh")
set_version("0.0.0")
set_license("MIT")

-- set defaults
set_languages("c++23")
set_warnings("allextra")
set_defaultmode("releasedbg")

-- add rules
add_rules("mode.releasedbg", "mode.debug")
add_rules("plugin.vsxmake.autoupdate")

-- set policies
set_policy("package.requires_lock", true)

-- enable xbyak
set_config("skse_xbyak", true)

-- setup targets
target("skse-dargh")
    -- add dependencies to target
    add_deps("commonlibsse")

    -- add commonlibsf plugin
    add_rules("commonlibsse.plugin", {
        name = "dargh",
        author = "noxsidereum, qudix",
        description = "An open source version of the Dynamic Animation Replacer for Skyrim SE"
    })

    -- add source files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/PCH.h")

    -- add install files
    add_installfiles("res/*.ini", { prefixdir = "SKSE/Plugins" })
