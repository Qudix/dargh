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

-- targets
target("dargh")
    add_packages("fmt", "spdlog", "commonlibsse-ng")

    add_rules("@commonlibsse-ng/plugin", {
        name = "dargh",
        author = "noxsidereum, Qudix",
        description = "An open source version of the Dynamic Animation Replacer for Skyrim SE"
    })

    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/PCH.h")

    after_build(function(target)
        local copy = function(env, ext)
            for _, env in pairs(env:split(";")) do
                if os.exists(env) then
                    local plugins = path.join(env, ext, "SKSE/Plugins")
                    os.mkdir(plugins)
                    os.trycp(target:targetfile(), plugins)
                    os.trycp(target:symbolfile(), plugins)
                    os.trycp("$(projectdir)/res/*.ini", plugins)
                end
            end
        end
        if os.getenv("SKYRIM_MODS_PATH") then
            copy(os.getenv("SKYRIM_MODS_PATH"), target:name())
        elseif os.getenv("SKYRIM_PATH") then
            copy(os.getenv("SKYRIM_PATH"), "Data")
        end
    end)
