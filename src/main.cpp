#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <iostream>

char buf[10];
int x = 0;
float f = 0;
int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow((int)1200, (int)1200, "Bezier Curve Editor", nullptr, nullptr);
	if(window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();


	bool my_tool_active(true);

	while(!glfwWindowShouldClose(window))
	{

		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Create a window called "My First Tool", with a menu bar.
		ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
		if(ImGui::BeginMenuBar())
		{
			if(ImGui::BeginMenu("File"))
			{
				if(ImGui::MenuItem("Open..", "Ctrl+O"))
				{ /* Do stuff */
				}
				if(ImGui::MenuItem("Save", "Ctrl+S"))
				{ /* Do stuff */
				}
				if(ImGui::MenuItem("Close", "Ctrl+W"))
				{
					my_tool_active = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		float col[4] = { 0.1, 0.1, 0.1, 1 };
		// Edit a color stored as 4 floats
		ImGui::ColorEdit4("Color", col);

		// Generate samples and plot them
		float samples[100];
		for(int n = 0; n < 100; n++)
			samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
		ImGui::PlotLines("Samples", samples, 100);

		// Display contents in a scrolling region
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
		ImGui::BeginChild("Scrolling");
		for(int n = 0; n < 50; n++)
			ImGui::Text("%04d: Some text", n);
		ImGui::EndChild();
		ImGui::End();
	}

	return 0;
}