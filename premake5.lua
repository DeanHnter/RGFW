location ("build")

workspace "RGFW"
    configurations { "Debug", "Release" }
    if os.host() == "macosx" then
        startproject "ios_metal_triangle"
    else
        startproject "callbacks"
    end

    -- platforms: only add Windows cross variants on Windows hosts
    if os.host() == "windows" then
        platforms({ "Native", "Win32", "Win64" })
        defaultplatform("Native")

        filter("platforms:Win32")
            system("windows")
            architecture("x86")
            gccprefix("i686-w64-mingw32-")

        filter("platforms:Win64")
            system("windows")
            architecture("x86_64")
            gccprefix("x86_64-w64-mingw32-")
    else
        platforms({ "Native" })
        defaultplatform("Native")
    end

    filter({})

    -- clean command
    newaction({
        trigger = "clean",
        description = "Clean the files",
        execute = function()
            os.rmdir("bin")
            os.rmdir("bin-int")
            os.rmdir("build")
        end
    })

    -- Custom Options
    newoption {
        trigger = "wayland",
        description = "Enable Wayland support"
    }
    newoption {
        trigger = "no-vulkan",
        description = "Disable Vulkan example"
    }
    newoption {
        trigger = "no-gles",
        description = "Disable GLES example"
    }
    newoption {
        trigger = "no-osmesa",
        description = "Disable OSMesa example"
    }
    newoption {
        trigger = "devteam",
        value = "TEAMID",
        description = "Apple Development Team ID for iOS signing (e.g. premake5 --devteam=ABCDE12345)"
    }

    -- Variables
    local isWindows = os.host() == "windows"
    local isLinux   = os.host() == "linux"
    local isMac     = os.host() == "macosx"

    local no_vulkan = _OPTIONS["no-vulkan"]
    local no_gles   = _OPTIONS["no-gles"] or not isLinux
    local no_osmesa = _OPTIONS["no-osmesa"] or not isLinux
    local use_wayland = _OPTIONS["wayland"]

group "examples"
    -- Helper: check if a simple example exists (by discovering a main file)
    local function simple_example_exists(name)
        return os.isfile("examples/" .. name .. "/" .. name .. ".c")
            or os.isfile("examples/" .. name .. "/" .. name .. ".m")
            or os.isfile("examples/" .. name .. "/" .. name .. ".mm")
    end

    -- List of simple examples (will be filtered by presence on disk)
    local exampleOutputs = {
        "basic",
        "buffer",
        "events",
        "callbacks",
        "flags",
        "monitor",
        "gl33_ctx",
        "smooth-resize",
        "multi-window"
    }

    for _, example in ipairs(exampleOutputs) do
        if simple_example_exists(example) then
            project(example)
                kind "ConsoleApp"
                language "C"
                targetdir "bin/%{cfg.buildcfg}"
                objdir "bin-int/%{cfg.buildcfg}"

                files { 
                    "examples/" .. example .. "/" .. example .. ".*",
                    "RGFW.h"
                }
                includedirs { "." }

                filter "system:windows"
                    links { "gdi32", "opengl32" }

                filter "system:linux"
                    if use_wayland then
                        links { "EGL", "GL" }
                    else
                        links { "GL", "X11", "Xrandr", "dl", "pthread" }
                    end

                filter "system:macosx"
                    links { "Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework" }
        else
            print("Skipping missing example: " .. example)
        end
    end

    -- List of special examples (conditionally compiled)
    local exampleCustomOutputs = {
        { name = "icons/icons" },
        { name = "gamepad/gamepad" },
        { name = "silk/silk" },
        { name = "first-person-camera/camera", projectname = "camera" },
        { name = "microui_demo/microui_demo" },
        { name = "gl33/gl33" },
        { name = "portableGL/pgl", condition = not (os.target() == "emscripten") },
        { name = "gles2/gles2", condition = not no_gles },
        { name = "dx11/dx11", system = "windows", condition = isWindows },
        { name = "metal/metal", system = "macosx", condition = isMac },
        { name = "ios_metal_triangle", system = "ios", condition = true },
        { name = "ios_rgfw_simple", system = "ios", condition = true },
        { name = "webgpu/webgpu", system = "emscripten", condition = (os.target() == "emscripten") },
        { name = "minimal_links/minimal_links" },
        { name = "osmesa_demo/osmesa_demo", condition = not no_osmesa },
        { name = "vk10/vk10", condition = not no_vulkan },
    }

    for _, e in ipairs(exampleCustomOutputs) do
        if e.condition == nil or e.condition then
            -- verify directory or main file exists before adding the project
            local dir
            if string.find(e.name, "/") then
                dir = "examples/" .. path.getdirectory(e.name)
            else
                dir = "examples/" .. e.name
            end
            local exists = os.isdir(dir)
            if not exists then
                -- fall back to specific main filename probing (legacy layout)
                local main_c = os.isfile("examples/" .. e.name .. ".c")
                local main_m = os.isfile("examples/" .. e.name .. ".m")
                local main_mm = os.isfile("examples/" .. e.name .. ".mm")
                exists = main_c or main_m or main_mm
            end
            -- whitelist iOS multi-file examples
            if e.name == "ios_metal_triangle" or e.name == "ios_rgfw_simple" then
                exists = true
            end
            if not exists then
                print("Skipping missing example: " .. e.name)
            else
                project(e.projectname or path.getname(e.name))
                kind "ConsoleApp"
                language "C"
                targetdir "bin/%{cfg.buildcfg}"
                objdir "bin-int/%{cfg.buildcfg}"

                if e.name == "ios_metal_triangle" or e.name == "ios_rgfw_simple" then
                    -- Configure as iOS app target
                    system "ios"
                    kind "WindowedApp"
                    defines { "RGFW_IMPORT" }
                    if e.name == "ios_metal_triangle" then
                        files {
                            "examples/ios_metal_triangle/*.m",
                            "examples/ios_metal_triangle/*.mm",
                            "examples/ios_metal_triangle/*.metal",
                            "examples/ios_metal_triangle/*.h",
                            "examples/ios_metal_triangle/rgfw_impl.c",
                            "RGFW.h"
                        }
                    else
                        files {
                            "examples/ios_rgfw_simple/*.m",
                            "examples/ios_rgfw_simple/*.mm",
                            "examples/ios_rgfw_simple/*.metal",
                            "examples/ios_rgfw_simple/*.h",
                            "examples/ios_rgfw_simple/rgfw_impl.c",
                            "RGFW.h"
                        }
                    end
                    -- Include common iOS entry shim for the simple track
                    if e.name == "ios_rgfw_simple" then
                        files { "examples/common/ios/rgfw_ios_entry.m", "examples/common/ios/rgfw_ios_entry.h" }
                        includedirs { "examples/common/ios" }
                    end
                    filter { "files:examples/ios_metal_triangle/rgfw_impl.c" }
                        undefines { "RGFW_IMPORT" }
                        defines { "RGFW_EXPORT", "RGFW_IMPLEMENTATION" }
                    filter {}
                    filter { "files:examples/ios_rgfw_simple/rgfw_impl.c" }
                        undefines { "RGFW_IMPORT" }
                        defines { "RGFW_EXPORT", "RGFW_IMPLEMENTATION" }
                    filter {}
                    buildoptions { "-fobjc-arc" }
                    local plistPath = "$(SRCROOT)/../examples/ios_metal_triangle/Info.plist"
                    if e.name == "ios_rgfw_simple" then
                        plistPath = "$(SRCROOT)/../examples/ios_rgfw_simple/Info.plist"
                    end
                    xcodebuildsettings {
                        ["INFOPLIST_FILE"] = plistPath,
                        ["SDKROOT"] = "iphoneos",
                        ["TARGETED_DEVICE_FAMILY"] = "1,2",
                        ["IPHONEOS_DEPLOYMENT_TARGET"] = "13.0",
                        ["SUPPORTED_PLATFORMS"] = "iphoneos iphonesimulator",
                        ["SUPPORTS_MACCATALYST"] = "NO",
                        ["ENABLE_BITCODE"] = "NO",
                        ["CODE_SIGN_STYLE"] = "Automatic",
                        ["ARCHS"] = "$(ARCHS_STANDARD)",
                        ["PRODUCT_BUNDLE_IDENTIFIER"] = "com.rgfw.example.iosmetal"
                    }
                    -- Prefer simulator in Debug to avoid signing requirements
                    filter { "system:ios", "configurations:Debug" }
                        xcodebuildsettings {
                            ["SDKROOT"] = "iphonesimulator",
                            ["SUPPORTED_PLATFORMS"] = "iphonesimulator",
                            ["ARCHS"] = "$(ARCHS_STANDARD)",
                            ["EXCLUDED_ARCHS"] = "arm64e",
                            ["ONLY_ACTIVE_ARCH"] = "YES",
                            ["CODE_SIGNING_ALLOWED"] = "NO",
                            ["CODE_SIGNING_REQUIRED"] = "NO"
                        }
                    filter {}
                    if _OPTIONS["devteam"] ~= nil then
                        xcodebuildsettings { ["DEVELOPMENT_TEAM"] = _OPTIONS["devteam"] }
                    end
                    links { 
                        "UIKit.framework", "Foundation.framework", "QuartzCore.framework",
                        "Metal.framework", "MetalKit.framework", "CoreGraphics.framework"
                    }
                else
                    -- include the main source with any extension (.c/.m/.mm) that actually exists
                    files {
                        "examples/" .. e.name .. ".*",
                        -- include any Metal shader sources in the example's folder (if present)
                        "examples/" .. path.getdirectory(e.name) .. "/*.metal",
                        "RGFW.h"
                    }
                end
                includedirs { "." }

                if e.name == "gles2/gles2" then
                    links {"EGL"}
                end
                
                if e.name == "osmesa_demo/osmesa_demo" then
                    links { "OSMesa" }
                end
                
                if e.name == "microui_demo/microui_demo" then
                    files { "examples/microui_demo/microui.c" }
                end
 

                filter "system:windows"
                    if e.name == "dx11/dx11" then
                        links { "d3d11", "dxgi", "d3dcompiler", "uuid", "gdi32" }
                    else
                        links { "gdi32", "opengl32" }
                    end

                filter "system:linux"
                    if use_wayland then
                        links { "EGL", "GL" }
                    else    
                        links { "GL", "X11", "Xrandr", "dl", "pthread", "m" }
                    end
                filter "system:macosx"
                    -- Set a conservative macOS deployment target for all macOS examples
                    xcodebuildsettings { ["MACOSX_DEPLOYMENT_TARGET"] = "12.0" }
                    if e.name == "metal/metal" then
                        -- Force macOS SDK for the metal example
                        system "macosx"
                        xcodebuildsettings {
                            ["SDKROOT"] = "macosx",
                            ["SUPPORTED_PLATFORMS"] = "macosx",
                            ["MACOSX_DEPLOYMENT_TARGET"] = "12.0",
                        }
                        links { "Metal.framework", "QuartzCore.framework", "Cocoa.framework", "IOKit.framework", "CoreVideo.framework" }
                    else
                        links { "Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework" }
                    end

                -- end per-system filters
            end
        end
    end
