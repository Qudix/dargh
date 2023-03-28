-- project
set_project("dargh")
set_version("0.0.0")
set_license("MIT")
set_languages("c++20")
set_optimize("faster")
set_warnings("allextra", "error")

-- rules
add_rules("mode.releasedbg", "mode.debug")

-- policies
set_policy("build.ccache", false)
set_policy("package.requires_lock", true)

-- packages
add_requires("xbyak")
add_requires("commonlibsse-ng", { configs = { skyrim_vr = false, skse_xbyak = true } })

-- includes
includes("res/package.lua")

-- targets
target("dargh")
    add_packages("fmt", "spdlog", "commonlibsse-ng")

    add_rules("@commonlibsse-ng/plugin", {
        name = "dargh",
        author = "noxsidereum",
        description = "An open source version of the Dynamic Animation Replacer for Skyrim SE"
    })

    add_files("src/**.cpp")
    add_headerfiles("include/**.h")
    add_includedirs("include")
    set_pcxxheader("include/PCH.h")

    add_rules("package", {
        ["@{target}"] = {
            archive = "@{target}-@{target_ver}.zip",
            deploy = { "$(env SKYRIM_MODS_PATH)", "@{target}" },
            files = {
                { "@{target_dir}", "@{target}.dll", "@(Data)/SKSE/Plugins" },
                { "@{target_dir}", "@{target}.pdb", "@(Data)/SKSE/Plugins" },
                { "@{project_dir}/res", "*.ini", "@(Data)/SKSE/Plugins" },
            }
        }
    })
