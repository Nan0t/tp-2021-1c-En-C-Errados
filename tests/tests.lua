project "tests"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{CommonLocations.TargetDir}"
    objdir    "%{CommonLocations.ObjDir}"

    files {
        "%{wks.location}/Discordia/src/console/parser.c",
        "%{wks.location}/Discordia/src/planificador/tripulante.c",
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.c"
    }

    includedirs {
        "%{CommonLocations.Commons.IncludeDir}",
        "%{CommonLocations.Utils.IncludeDir}",
        "%{wks.location}/Discordia/src",
        "%{prj.location}/src"
    }

    links {
        "utils",
        "commons",
        "cunit",
        "pthread",
        "readline"
    }
    
    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"
    
    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"