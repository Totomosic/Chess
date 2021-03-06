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
include (BoxfishDir .. "Boxfish-Test")
include (BoxfishDir .. "Boxfish-Book")

group ("Chess")
include ("Chess")