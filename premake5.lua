newoption {
    trigger = "arch",
    description = "Set the current target architecture",
    default = "x86",
    allowed = {
        { "x86", "32 bit target architecture" },
        { "x86_64", "64 bit target architecture" }
    }
}

workspace "Template"
    configurations { "Debug", "Release" }
    architecture(_OPTIONS["arch"])

    --Common Locations--
    --------------------

    CommonLocations = { }

    CommonLocations["TargetDir"] = "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.buildcfg}"
    CommonLocations["ObjDir"]    = "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.buildcfg}"

    CommonLocations["Commons"] = { }
    CommonLocations["Commons"]["IncludeDir"] = "%{wks.location}/dep/so-commons-library/src"

    CommonLocations["NivelGUI"] = { }
    CommonLocations["NivelGUI"]["IncludeDir"] = "%{wks.location}/dep/so-nivel-gui-library/src"

    CommonLocations["Utils"] = { }
    CommonLocations["Utils"]["IncludeDir"] = "%{wks.location}/utils/include"

    include("dep/so-commons-library/commons.lua")
    include("dep/so-nivel-gui-library/nivel_gui.lua")
    include("tests/tests.lua")
    include("utils/utils.lua")
    include("Discordia/Discordia.lua")
    include("I-Mongo-Store/I-Mongo-Store.lua")
    include("Mi-RAM-HQ/Mi-RAM-HQ.lua")
