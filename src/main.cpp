//#include <stdio.h>
//#include <stdlib.h>
#include <string>
#include <iostream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <vector>
#include <memory>

#include <chrono>
#include <ctime>
#include <random>

#include "shader_configurator.hpp"
#include "figures/cube.hpp"
#include "camera/camera.hpp"
#include "frustum_manager.hpp"

static const char* glsl_version = "#version 130";
GLFWwindow* window;

void drawLine()
{

}

int main(void) {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
		GL_TRUE);  // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(860, 640, "Frustum culling", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr,
			"Failed to open GLFW window. If you have an Intel GPU, they are "
			"not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Dark blue background
	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders(
		"..\\src\\VertexShader."
		"vertexshader",
		"..\\src\\FragmentShader."
		"fragmentshader");

	// Get a handle for our "MVP" uniform
	//GLuint MatrixID = glGetUniformLocation(programID, "MVP");
//ImGui variable --------------------
	bool isAABBOptimization = false;
	bool isAABBOptimizationV2 = false;
	bool isSphereOptimization = false;
	bool useCaching = false;
	bool isNoMovementOptimisation = false;
	bool isRotationOpmization = false;
	bool isTranslationOptimisation = false;
	bool useBVH = false;
	int optimization = 0;
//ImGui variable --------------------
	Camera camera(window);
    Camera viewFrustum(window);

	std::vector<std::unique_ptr<Drawable>> cubes;
	int countOfCube = 500;
	cubes.reserve(countOfCube);

	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution dis(-100.0, 100.0);
	for(int i = 0; i < countOfCube; i++)
	{
		cubes.push_back(std::make_unique<Cube>(viewFrustum.getViewProjection(), programID));
		cubes.back()->setWorldTransform(glm::translate(glm::mat4(), glm::vec3(dis(gen), 0.0f, dis(gen))));
	}

	std::vector<Drawable*> objectsToDraw;
	objectsToDraw.reserve(countOfCube);

	for(auto& ptr : cubes)
	{
		objectsToDraw.push_back(ptr.get());
	}

	FrustumManager frustumManager(objectsToDraw, optimization);

	float deltaTime = 0.f;
	auto start = std::chrono::system_clock::now();
	auto finish = std::chrono::system_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()/1000.f;
	float sec = 0.f;
    bool isFirst = true;
	do {
		int countIteration = 0;
		optimization = frustumManager.getOptimizations();
		start = std::chrono::system_clock::now();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//tick
        if (isFirst)
		{
			camera.tick(dt);
			isFirst = false;
		} 
		else
		{
			/*camera.fakeTick(dt);*/
		}
		//camera.tick(dt);
		viewFrustum.tick(dt);
		// what is it, direction is int???
		int direction = camera.getDirection();
		frustumManager.recalculateFrustumView(camera, direction);
		//draw
		bool needUpdateTypeOfCulling = false;
		//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		if(ImGui::Checkbox("AABB Culling(v1): ", &isAABBOptimization))
		{
			if(isAABBOptimization)
			{
				optimization |= Optimization::AABB_SIMPLE_CULLING;
			}
			else
			{
				optimization &= ~Optimization::AABB_SIMPLE_CULLING;
			}
		}
		if(ImGui::Checkbox("AABB Culling(v2): ", &isAABBOptimizationV2))
		{
			if(isAABBOptimizationV2)
			{
				optimization |= Optimization::AABB_POSITIVE_CULLING;
			}
			else
			{
				optimization &= ~Optimization::AABB_POSITIVE_CULLING;
			}
		}
		if (ImGui::Checkbox("Sphere Culling: ", &isSphereOptimization))
		{
			if (isSphereOptimization)
			{
				optimization |= Optimization::SPHERE_CULLING;
			}
			else
			{
				optimization &= ~Optimization::SPHERE_CULLING;
			}
		}

		if(ImGui::Checkbox("Use caching: ", &useCaching))
		{
			if (useCaching)
			{
				optimization |= Optimization::PLANE_COHERENCY;
			}
			else
			{
				optimization &= ~Optimization::PLANE_COHERENCY;
			}
		}
		
		if(ImGui::Checkbox("Rotation coherency: ", &isRotationOpmization))
		{
			if (isRotationOpmization)
			{
				optimization |= Optimization::ROTATION_COHERENCY;
			}
			else
			{
				optimization &= ~Optimization::ROTATION_COHERENCY;
			}
		}
		if(ImGui::Checkbox("Translation coherency: ", &isTranslationOptimisation))
		{
			if (isTranslationOptimisation)
			{
				optimization |= Optimization::TRANSLATION_COHERENCY;
			}
			else
			{
				optimization &= ~Optimization::TRANSLATION_COHERENCY;
			}
		}
		if (ImGui::Checkbox("Use BVH: ", &useBVH))
		{
			if (useBVH)
			{
				optimization |= Optimization::BVH;
			}
			else
			{
				optimization &= ~Optimization::BVH;
			}
		}
		frustumManager.updateOptimizations(optimization);
		frustumManager.tick(deltaTime);

		ImGui::Text("application average %.3f ms/frame (%.1f fps)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Count drawed item:%d", frustumManager.getCountDrawed());
		ImGui::SliderFloat("Camera speed", &camera.speedCamera(), 0, 1);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		finish = std::chrono::system_clock::now();
		//std::cout << countIteration << std::endl;
		dt = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() / 1000.f;
	}  // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
