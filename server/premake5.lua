project "server"
        language "C++"
        cppdialect "C++17"
        architecture "x86_64"

        kind "ConsoleApp"
        files { "**.hpp", "**.cpp" }
        includedirs { }
        links {
                "sfml-system",
        }
