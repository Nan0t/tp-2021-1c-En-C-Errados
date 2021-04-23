project "utils"
    kind "SharedLib"
    language "C"
    cdialect "GNU11"

    targetdir "%{CommonLocations.TargetDir}"
    objdir    "%{CommonLocations.ObjDir}"

    includedirs {
        "%{CommonLocations.Commons.IncludeDir}",
        "%{CommonLocations.Utils.IncludeDir}",
    }

    files {
        "%{prj.location}/include/**.h",
        "%{prj.location}/src/**.c"
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"
    
    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"