workspace "authoritative_multiplayer"
        targetdir "bin/%{cfg.longname}/"
        objdir "obj/"

        includedirs { "external", "external/msgpack-c/include", "external/entt/single_include" }

        configurations { "Debug", "Release" }

        filter { "configurations:Debug" }
                symbols "On"

        filter { "configurations:Release" }
                optimize "On"
                defines { "NDEBUG" }

        filter { }

        include "client"
        include "server"

newaction {
        trigger = "clean",
        description = "Clean the workspace",
        execute = function()
                os.rmdir "./bin"
                os.rmdir "./obj"
                os.remove "./Makefile"
                os.remove "**/Makefile"
                os.remove "**.make"
                print "Finished."
        end
}
