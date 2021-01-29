project "client"
        language "C++"
        cppdialect "C++17"
        architecture "x86_64"

        kind "ConsoleApp"
        files { "**.hpp", "**.cpp", "../common/**.hpp", "../common/**.cpp" }
        includedirs { }
        links {
                "sfml-system",
                "sfml-window",
                "sfml-graphics",
        }
