workspace "D3D12Study"
    architecture "x64"
    startproject "D3D12Study"

    configurations { "Debug", "Release", "Dist" }

    characterset("ASCII")

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "D3D12Study"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    files { "%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.hlsl",
        "%{prj.name}/src/**.rc",
		"%{prj.name}/vendor/**.h",
	 	"%{prj.name}/vendor/**.cpp",
        "%{prj.name}/vendor/**.hlsl",
        "%{prj.name}/vendor/**.rc",
        "%{prj.name}/Resources/**.cur",
        "%{prj.name}/Resources/**.ico",
        "%{prj.name}/Shaders/**.cso"
    }

   filter { "files:**Pixel.hlsl" }
    buildmessage "Compiling Pixel Shader %{file.name}"
    buildcommands {
        "fxc /T ps_5_0 /E main /Fo %{cfg.objdir}/%{file.basename}.cso %{file.relpath}"
    }
    buildoutputs {
        "%{cfg.objdir}/%{file.basename}.cso"
    }
     
    filter { "files:**Vertex.hlsl" }
    buildmessage "Compiling Vertex Shader %{file.name}"
    buildcommands {
        "fxc /T vs_5_0 /E main /Fo %{cfg.objdir}/%{file.basename}.cso %{file.relpath}"
    }
    buildoutputs {
        "%{cfg.objdir}/%{file.basename}.cso"
    }

    filter {}

    links
	{
		"dxgi.lib",
        "d3d12.lib",
        "d3dcompiler.lib",
        "winmm.lib"
	}
    
    filter "system:windows"
	systemversion "latest"
    
    defines{

        "_DEBUG",
        "_WINDOWS"
    }

    filter "configurations:Debug"
	defines "D3D12Study_DEBUG"
	runtime "Debug"
	symbols "on"
    optimize "Off"

	filter "configurations:Release"
	defines "D3D12Study_RELEASE"
	runtime "Release"
	optimize "Speed"

	filter "configurations:Dist"
	defines "D3D12Study_DIST"
	runtime "Release"
	symbols "Off"
	optimize "Full"

   