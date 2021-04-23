project "commons"
    kind "SharedLib"
    language "C"
    cdialect "GNU11"

    targetdir "%{CommonLocations.TargetDir}"
    objdir    "%{CommonLocations.ObjDir}"

    files {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.c"
    }

    filter "configurations:Debug"
        symbols "on"
        buildoptions "-g3"
    
    filter "configurations:Release"
        optimize "speed"