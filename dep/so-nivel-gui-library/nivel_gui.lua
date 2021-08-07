project "nivel_gui"
    kind "StaticLib"
    language "C"
    cdialect "GNU11"

    targetdir "%{CommonLocations.TargetDir}"
    objdir    "%{CommonLocations.ObjDir}"

    includedirs {
        "%{CommonLocations.Commons.IncludeDir}",
        "%{CommonLocations.NivelGUI.IncludeDir}"
    }
    
    files {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.c"
    }

    filter "configurations:Debug"
        symbols "on"
        buildoptions "-g3"
    
    filter "configurations:Release"
        optimize "speed"