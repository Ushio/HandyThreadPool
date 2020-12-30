
workspace "HandyThreadPool"
    location "build"
    configurations { "Debug", "Release" }
    startproject "main"

architecture "x86_64"

project "main"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/"
    systemversion "latest"
    flags { "MultiProcessorCompile", "NoPCH" }

    -- Src
    files { "*.cpp", "*.hpp" }
    includedirs { "." }

    symbols "On"

    filter {"Debug"}
        runtime "Debug"
        targetname ("Main_Debug")
        optimize "Off"
    filter {"Release"}
        runtime "Release"
        targetname ("Main")
        optimize "Full"
    filter{}
