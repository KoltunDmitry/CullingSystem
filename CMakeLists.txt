# Projects Settings
cmake_minimum_required (VERSION 3.1)
project (culling-system)

# The main executable
add_executable(main
	src/camera/camera.cpp
	src/camera/camera.hpp
	src/figures/bounding_figures.cpp
	src/figures/bounding_figures.hpp
	src/figures/cube.cpp
	src/figures/cube.hpp
	src/figures/drawable.cpp
	src/figures/drawable.hpp
	src/bounding_volume_hierarchy.cpp
	src/bounding_volume_hierarchy.hpp
	src/frustum.cpp
	src/frustum.hpp
	src/frustum_manager.cpp
	src/frustum_manager.hpp
	src/main.cpp
	src/shader_configurator.hpp
	src/shader_configurator.cpp
	src/FragmentShader.fragmentshader
	src/VertexShader.vertexshader
	src/imgui/imconfig.h
	src/imgui/imgui.cpp
	src/imgui/imgui.h
	src/imgui/imgui_demo.cpp
	src/imgui/imgui_draw.cpp
	src/imgui/imgui_impl_glfw.cpp
	src/imgui/imgui_impl_glfw.h
	src/imgui/imgui_impl_opengl3.cpp
	src/imgui/imgui_impl_opengl3.h
	src/imgui/imgui_internal.h
	src/imgui/imgui_widgets.cpp
	src/imgui/imstb_rectpack.h
	src/imgui/imstb_textedit.h
	src/imgui/imstb_truetype.h
)
set_property(TARGET main PROPERTY CXX_STANDARD 17)
target_compile_options(main PRIVATE -Wall)

# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(main PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(main PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(main PRIVATE glm)
