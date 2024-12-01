-- download dependencies
function file_exist(file) -- source: https://stackoverflow.com/a/4991602/19679136
    local f = io.open(file, "r")
    if f ~= nil then
        io.close(f)
        return true
    else
        return false
    end
end
function download_file(file, url)
    if not file_exist(file) then
        io.write("Downloading " .. file)
        local str, code = http.download(url, file)
        if code ~= 200 then
            io.write(" // Failed with http code " .. code .. " - " .. str .. "\n")
            error()
        else
            io.write(" // Done\n")
        end
    end
end
download_file("src/cJSON.c", "https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.c")
download_file("src/cJSON.h", "https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.h")
download_file("src/tinycthread.c", "https://raw.githubusercontent.com/tinycthread/tinycthread/refs/heads/master/source/tinycthread.c")
download_file("src/tinycthread.h", "https://raw.githubusercontent.com/tinycthread/tinycthread/refs/heads/master/source/tinycthread.h")
download_file("src/stb_image.h", "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h")
download_file("src/stb_image_write.h", "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image_write.h")

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

    strictaliasing "Off"
    warnings "Extra"

    postbuildcommands {
        "{COPYDIR} resources/ %{cfg.buildtarget.directory}/"
    }

    filter "configurations:Debug"
        defines { "DEBUG=1" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG=1" }
        optimize "On"

    filter "platforms:NoSDL"
        defines { "GUI_SUPPORTED=0" }

    filter "platforms:SDL"
        defines { "GUI_SUPPORTED=1" }
        includedirs { os.findheader("SDL2.h") }
        libdirs { os.findlib("SDL2") }
        links { "SDL2" }

    filter "toolset:gcc or toolset:clang"
        buildoptions { "-Wpedantic", "-Werror" }

    filter "system:not windows"
        links { "m" }
