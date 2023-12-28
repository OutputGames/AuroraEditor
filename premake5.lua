
project "AuroraEditor"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   location "generated\\"
   compileas "C++"
   targetdir "bin/%{cfg.buildcfg}"

   defines {"AURORA_DLLBUILD"}

   defines {"_CRT_SECURE_NO_WARNINGS"}

   files {
       "src/**",
        "include/**",
         ".editorconfig",
         "%{_OPTIONS['utilslocation']}/include/**" ,
         "%{_OPTIONS['utilslocation']}/vendor/glm/glm/**" ,
         "resources/**",
         "%{_OPTIONS['utilslocation']}".."/vendor/sdl2/include/**" , 
      }
   includedirs {
       "include/", 
       "%{_OPTIONS['utilslocation']}".."/include", 
       "%{_OPTIONS['utilslocation']}".."/vendor/glm",
       "%{_OPTIONS['sunriselocation']}".."/vendor/", 
       "%{_OPTIONS['sunriselocation']}".."/vendor/alsoft/include", 
       "%{_OPTIONS['sunriselocation']}".."/include/", 
       "%{_OPTIONS['agllocation']}".."/include/",
       "%{_OPTIONS['agllocation']}".."/vendor/glfw/include",
       "%{_OPTIONS['utilslocation']}".."/vendor/glm", 
       "%{_OPTIONS['utilslocation']}".."/vendor/sdl2/include/",
       "%{_OPTIONS['corelocation']}/vendor/jolt/include/",
       "%{_OPTIONS['corelocation']}/include/"
      }
    libdirs {
      "%{_OPTIONS['agllocation']}/vendor/glfw/lib-vc2022/",
      "%{_OPTIONS['agllocation']}/vendor/assimp/lib/Release/", 
      "%{_OPTIONS['utilslocation']}".."/vendor/sdl2/lib",
      "%{_OPTIONS['corelocation']}/vendor/jolt/lib/",
      "%{_OPTIONS['corelocation']}/bin/%{cfg.buildcfg}/"
    }


    links {"vulkan-1.lib"}
    libdirs {"C:/VulkanSDK/1.3.250.0/Lib"}
    includedirs {"C:/VulkanSDK/1.3.250.0/Include"}
    --files {"%{_OPTIONS['agllocation']}/vendor/imgui/backends/imgui_impl_vulkan.*"}

    files {"%{_OPTIONS['agllocation']}/vendor/imgui/backends/imgui_impl_vulkan.*"}
    files {"%{_OPTIONS['agllocation']}/vendor/imgui/backends/imgui_impl_sdl2.*", "%{_OPTIONS['agllocation']}/vendor/imgui/*"}
    files {"%{_OPTIONS['agllocation']}/vendor/imgui/misc/debuggers/**", "%{_OPTIONS['agllocation']}/vendor/imgui/misc/cpp/**"}
    includedirs {"%{_OPTIONS['agllocation']}/vendor/imgui/"}

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      debugdir "./"
      runtime "Debug"
      optimize "Off"
      links {"SDL2d", "SDL2maind"}

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      runtime "Release"
      links {"SDL2", "SDL2main"}

   filter "action:vs*" 
      defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
      dependson {"AuroraAudioLibrary", "AuroraGraphicsLibrary", "AuroraCore"}
      links {"AuroraAudioLibrary", "AuroraGraphicsLibrary", "AuroraCore"}
      characterset ("MBCS")

   filter "system:windows"
      defines {"_WIN32"}
      libdirs {"bin/%{cfg.buildcfg}/"}
      links {"winmm", "kernel32"}
      postbuildcommands { "xcopy \"%{_OPTIONS['corelocation']}\\bin\\%{cfg.buildcfg}\\*.dll\" \"..\\bin\\%{cfg.buildcfg}\\\" /y /F", "xcopy \"%{_OPTIONS['corelocation']}\\bin\\%{cfg.buildcfg}\\*.pdb\" \"..\\bin\\%{cfg.buildcfg}\\\" /y /F" }