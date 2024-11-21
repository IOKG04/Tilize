-- premake configuration
workspace "Tilize"
    configurations { "Debug", "Release" }
    platforms { "NoSDL", "SDL" }

project "Tilize"
    kind "ConsoleApp"
    language "C"
    cdialect "C11"
    targetdir "bin/%{cfg.buildcfg}_%{cfg.platform}"

    files { "src/**.h", "src/**.c" }
    links { "m" }

    strictaliasing "Off"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter "platforms:NoSDL"
        defines {"GUI_SUPPORTED=0"}

    filter "platforms:SDL"
        defines {"GUI_SUPPORTED=1"}
        includedirs { os.findheader("SDL2.h") }
        libdirs { os.findlib("SDL2") }
        links { "SDL2" }
