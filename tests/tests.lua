project "tests"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{CommonLocations.TargetDir}"
    objdir    "%{CommonLocations.ObjDir}"

    files {
        "%{wks.location}/Discordia/src/console/parser.c",
        "%{wks.location}/I-Mongo-Store/src/fs/blocks/blocks_manager.c",
        "%{wks.location}/I-Mongo-Store/src/fs/blocks/block.c",
        "%{wks.location}/I-Mongo-Store/src/fs/disk/disk.c",
        "%{wks.location}/I-Mongo-Store/src/fs/file/file.c",
        "%{wks.location}/I-Mongo-Store/src/fs/blocks/block.c",
        "%{wks.location}/I-Mongo-Store/src/fs/task_parser/task_parser.c",
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.c"
    }

    includedirs {
        "%{CommonLocations.Commons.IncludeDir}",
        "%{CommonLocations.Utils.IncludeDir}",
        "%{wks.location}/Discordia/src",
        "%{wks.location}/I-Mongo-Store/src",
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