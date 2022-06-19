/*
	ImGuiBuild.cpp
	Here we link to the approprate ImGui backend implementaions.
	If we want cases other than OpenGL, use switches on Metal, 
	DirectX, etc.
*/
#include "backends/imgui_impl_opengl3.cpp"
#include "backends/imgui_impl_glfw.cpp"
