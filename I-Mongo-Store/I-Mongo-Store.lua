project "I-Mongo-Store"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{CommonLocations.TargetDir}"
    objdir    "%{CommonLocations.ObjDir}"

    files {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.c"
    }

    includedirs {
        "%{CommonLocations.Commons.IncludeDir}",
        "%{CommonLocations.Utils.IncludeDir}",
        "%{prj.location}/src"
    }

    links {
        "utils",
        "commons",
        "cunit",
        "pthread",
        "readline",
        "m"
    }
    
    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"
    
    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"
