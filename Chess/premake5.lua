do
    local ProjectName = "Chess"
    local BuildDir = "../bin/" .. outputdir .. "/" .. ProjectName
    local ObjDir = "../bin-int/" .. outputdir .. "/" .. ProjectName

    -- Path to Bolt install dir from project dir
    local BoltInstallDir = "../Bolt/"
    local BoxfishInstallDir = "../Boxfish/"

    project (ProjectName)
        location ""
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"

        targetdir (BuildDir)
        objdir (ObjDir)
        
        files
        {
            "src/**.h",
            "src/**.hpp",
            "src/**.cpp"
        }
        
        includedirs
        {
            BoltInstallDir .. "%{IncludeDirs.GLFW}",
            BoltInstallDir .. "%{IncludeDirs.Glad}",
            BoltInstallDir .. "%{IncludeDirs.ImGui}",
            BoltInstallDir .. "%{IncludeDirs.spdlog}",
            BoltInstallDir .. "%{IncludeDirs.FreeTypeGL}",
            BoltInstallDir .. "%{IncludeDirs.FreeType}",
            BoltInstallDir .. "%{IncludeDirs.Lua}",
            BoltInstallDir .. "%{IncludeDirs.Dukglue}",
            BoltInstallDir .. "%{IncludeDirs.AssetsLib}",
            BoltInstallDir .. "%{IncludeDirs.BoltLib}",
            BoltInstallDir .. "%{IncludeDirs.Bolt}",
            BoxfishInstallDir .. "%{BoxfishIncludeDirs.Boxfish}",
            "src"
        }

        links
        {
            "Bolt-Core",
            "Boxfish-Lib"
        }

        filter "system:windows"
            systemversion "latest"

            defines
            {
                "BLT_PLATFORM_WINDOWS",
                "BLT_BUILD_STATIC",
                "BOX_PLATFORM_WINDOWS",
                "BOX_BUILD_STATIC",
                "_CRT_SECURE_NO_WARNINGS",
            }

        filter "system:linux"
            systemversion "latest"

            defines
            {
                "BLT_PLATFORM_LINUX",
                "BLT_BUILD_STATIC",
                "BOX_PLATFORM_LINUX",
                "BOX_BUILD_STATIC",
            }

            links
            {
                "AssetsLib",
                "BoltLib",
                "GLFW",
                "Glad",
                "ImGui",
                "Lua",
                "Dukglue",
                "FreeType-GL",
                "FreeType",
                "stdc++fs",
                "dl",
                "X11",
                "pthread"
            }

        filter "system:macosx"
            systemversion "latest"

            defines
            {
                "BLT_PLATFORM_MAC",
                "BLT_BUILD_STATIC",
                "BOX_PLATFORM_MAC",
                "BOX_BUILD_STATIC",
            }

            links
            {
                "AssetsLib",
                "BoltLib",
                "GLFW",
                "Glad",
                "ImGui",
                "Lua",
                "Dukglue",
                "FreeType-GL",
                "FreeType",
                "stdc++fs",
                "dl",
                "X11",
                "pthread"
            }

        filter "configurations:Debug"
            defines
            {
                "BLT_DEBUG",
                "BOX_DEBUG"
            }
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            defines
            {
                "BLT_RELEASE",
                "BOX_RELEASE"
            }
            runtime "Release"
            optimize "on"

        filter "configurations:Dist"
            defines
            {
                "BLT_DIST",
                "BOX_DIST"
            }
            runtime "Release"
            optimize "on"
end