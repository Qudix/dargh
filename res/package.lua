--[[
rule("package")
    on_package(function(a_target)
        import("core.base.semver")
        import("core.project.config")
        import("core.project.project")
        import("utils.archive")

        local function _parse_str(a_target, a_str)
            local map = {
                project =           function() return project.name() or "" end,
                project_ver =       function() return project.version() or "0.0.0" end,
                project_ver_major = function() return semver.new(map.project_ver()):major() end,
                project_ver_minor = function() return semver.new(map.project_ver()):minor() end,
                project_ver_patch = function() return semver.new(map.project_ver()):patch() end,
                project_dir =       function() return os.projectdir() end,
                target =            function() return a_target:name() or "" end,
                target_ver =        function() return a_target:version() or "0.0.0" end,
                target_ver_major =  function() return semver.new(map.target_ver()):major() end,
                target_ver_minor =  function() return semver.new(map.target_ver()):minor() end,
                target_ver_patch =  function() return semver.new(map.target_ver()):patch() end,
                target_dir =        function() return path.absolute(a_target:targetdir()) end,
            }

            a_str = a_str:gsub("(%@{([^\n]-)})", function(_, a_var)
                a_var = a_var:trim()

                local result = map[a_var]
                if type(result) == "function" then
                    result = result()
                end

                assert(result ~= nil, "cannot get variable(%s)", a_var)
                return result
            end)

            return a_str
        end

        local packages_dir = path.join(path.absolute(config.buildir()), "packages")
        os.mkdir(packages_dir)

        local packages = a_target:extraconf("rules", "package") or {}
        for name, files in pairs(packages) do
            if name and files then
                local package_name = _parse_str(a_target, name)
                local package_dir = path.join(packages_dir, package_name:match("(.+)%..+$"))
                os.mkdir(package_dir)

                for _, file in pairs(files) do
                    local root = path.absolute(_parse_str(a_target, file[1]))
                    local src = path.join(root, _parse_str(a_target, file[2]))
                    local dest = path.join(package_dir, _parse_str(a_target, file[3] or ""))

                    if not os.exists(dest) then
                        os.mkdir(dest)
                    end

                    os.trycp(src, dest, { rootdir = root })
                end

                archive.archive(path.join("../", package_name), ".", { curdir = package_dir })
            end
        end
    end)

    after_clean(function()
        local packages_dir = path.join(path.absolute(config.buildir()), "packages")
        os.tryrm(packages_dir)
    end)]]

local function _parse_str(a_target, a_str, a_exclude)
    local map = {
        project =           function() return project.name() or "" end,
        project_ver =       function() return project.version() or "0.0.0" end,
        project_ver_major = function() return semver.new(map.project_ver()):major() end,
        project_ver_minor = function() return semver.new(map.project_ver()):minor() end,
        project_ver_patch = function() return semver.new(map.project_ver()):patch() end,
        project_dir =       function() return os.projectdir() end,
        target =            function() return a_target:name() or "" end,
        target_ver =        function() return a_target:version() or "0.0.0" end,
        target_ver_major =  function() return semver.new(map.target_ver()):major() end,
        target_ver_minor =  function() return semver.new(map.target_ver()):minor() end,
        target_ver_patch =  function() return semver.new(map.target_ver()):patch() end,
        target_dir =        function() return path.absolute(a_target:targetdir()) end,
    }

    if a_exclude then
        a_str = a_str:gsub("(%@%(([^\n]-)%))", "")
    end

    a_str = a_str:gsub("(%@{([^\n]-)})", function(_, a_var)
        a_var = a_var:trim()

        local result = map[a_var]
        if type(result) == "function" then
            result = result()
        end

        --assert(result ~= nil, "cannot get variable(%s)", a_var)
        return result
    end)

    return a_str
end

rule("package")
    on_package(function(a_target)

    end)

    after_build(function(a_target)
        local defs = a_target:extraconf("rules", "package") or {}
        for name, cdef in pairs(defs) do
            if name and cdef then
                if cdef.deploy then
                    local cdef_deploy_root = _parse_str(a_target, cdef.deploy[1], true)
                    if os.exists(cdef_deploy_root) then
                        local cdef_deploy_dest = path.join(cdef_deploy_root, _parse_str(a_target, cdef.deploy[2] or "", true))
                        os.mkdir(cdef_deploy_dest)
                        for _, fdef in pairs(cdef.files) do
                            local root = path.absolute(_parse_str(a_target, fdef[1], true))
                            local src = path.join(root, _parse_str(a_target, fdef[2], true))
                            local dest = path.join(cdef_deploy_dest, _parse_str(a_target, fdef[3] or "", true))
                            os.mkdir(dest)
                            os.trycp(src, dest, { rootdir = root })
                        end
                    end
                end
            end
        end
    end)
