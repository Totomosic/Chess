workspace "Chess"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

-- Path to directory containing Bolt-Core
BoltDir = "Bolt/"
BoxfishDir = "Boxfish/"

include (BoltDir .. "BoltInclude.lua")
group("Boxfish")
include (BoxfishDir .. "Include.lua")
include (BoxfishDir .. "Boxfish-Cli")

group ("Chess")
include ("Chess")