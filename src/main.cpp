#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <iostream>

#include <vector>
#include <cstdio>


unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void HelpMarker(const char* desc);
static void ShowPlaceholderObject(const char* prefix, int knot_size, std::vector<float>& knot_vec);
static void ShowCanvas(bool* p_open);
static void ShowExampleAppLayout(bool* p_open);

int main(int, char**)
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow((int)SCR_WIDTH, (int)SCR_HEIGHT, "B-Spline-curve-fitting", NULL, NULL);
	if(window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	// setup callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// glad: load all OpenGL function pointers
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	// setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	//Setup Dear ImGui style
	//GreenTheme();
	ImGui::StyleColorsClassic();
	ImGui::GetIO().FontGlobalScale = 1.0;
	ImGui::GetIO().Fonts->AddFontFromFileTTF("../assets/fonts/NotoSans-Regular.ttf", 28.0f);

	bool flag = true;

	// Our state
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool is_active(true);
	// Main loop

	static int degree, cpointsNum = 0;

	static std::vector<float> knot_vec;
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);

		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
		if(ImGui::Begin("B-Spline curve fitting", nullptr, flags))
		{
			//left
			{
				ImGui::BeginChild("left pane", ImVec2(350, 0), true);
				ImGui::Text("Right click to add control point");
				if(ImGui::Button("add a control point"))
				{
					cpointsNum++;
				}
				ImGui::SameLine();
				ImGui::Text("control points num = %d", cpointsNum);
				ImGui::InputInt("degree", &degree);
				if(ImGui::Button("clear"))
				{
					cpointsNum = 0;
					degree = 0;
				}

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
				if(ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
				{
					ShowPlaceholderObject("Knot Vector", cpointsNum + degree + 1, knot_vec);
					ImGui::EndTable();
				}
				ImGui::PopStyleVar();
				ImGui::EndChild();

			}

			ImGui::SameLine();

			//right
			{
				ImGui::BeginGroup();


				if(ImGui::BeginChild("Fitting Method", ImVec2(0, 80), false))
				{
					static ImVector<ImVec2> points;
					static ImVec2 scrolling(0.0f, 0.0f);
					static bool opt_enable_grid = true;
					static bool opt_enable_context_menu = true;
					static bool adding_line = false;
					if(ImGui::BeginChild("fitting choise", ImVec2(0, 0), false))
					{
						if(ImGui::BeginTabBar("##TabBar"))
						{
							if(ImGui::BeginTabItem("Interpolation"))
							{
								ImGui::SetNextItemWidth(200);
								const char* interp_method[] = { "Uniform", "Chord Length", "Centripetal", "Universal" };
								static int interp_choice = 0;
								ImGui::Combo("Method", &interp_choice, interp_method, IM_ARRAYSIZE(interp_method));

								//ImGui::Text("Interpolation test");
								ImGui::EndTabItem();
							}

							if(ImGui::BeginTabItem("Approximation"))
							{
								ImGui::Text("Approximation test");
								ImGui::EndTabItem();
							}
							ImGui::EndTabBar();
						}

					}
					ImGui::EndChild();

					ImGui::SameLine();

					ImGui::SetNextItemWidth(900);
					if(ImGui::BeginChild("checking boxx", ImVec2(100, 0), false))
					{
						ImGui::Checkbox("Enable grid", &opt_enable_grid);
						ImGui::Checkbox("Enable context menu", &opt_enable_context_menu);
						ImGui::Text("Mouse Left: drag to move control point,\nMouse Right: drag to scroll, click for add control point.");
						ImGui::EndChild();
					}
					ImGui::EndChild();

				}
				ImGui::EndChild();
				//ImGui::Separator();

				//ImGui::BeginGroup();
				if(ImGui::BeginChild("canvas", ImVec2(0, 0), true))
				{

					//ImGui::Checkbox("check canvas", &checkf);
					ShowCanvas(&flag);
				}
				ImGui::EndChild();


				ImGui::EndGroup();
			}


		}
		ImGui::End();

		//Render
		ImGui::Render();

		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

	}
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
	//ImGui::GetStyle().WindowMinSize = ImVec2((float)SCR_WIDTH * 0.2f, (float)SCR_HEIGHT);
}

static void ShowCanvas(bool* p_open)
{

	static ImVector<ImVec2> points;
	static ImVec2 scrolling(0.0f, 0.0f);
	static bool opt_enable_grid = true;
	static bool opt_enable_context_menu = true;
	static bool adding_line = false;



	ImGui::Checkbox("Enable grid", &opt_enable_grid);
	ImGui::Text("Mouse Left: drag to move control point,\nMouse Right: drag to scroll, click for add control point.");

	// Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping region + own scrolling.
	// Here we demonstrate that this can be replaced by simple offsetting + custom drawing + PushClipRect/PopClipRect() calls.
	// To use a child window instead we could use, e.g:
	//      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
	//      ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
	//      ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoMove);
	//      ImGui::PopStyleColor();
	//      ImGui::PopStyleVar();
	//      [...]
	//      ImGui::EndChild();

	// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
	if(canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
	if(canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
	ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
	//std::cout << canvas_p1.x << " " << canvas_p1.y << std::endl;

	// Draw border and background color
	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
	draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

	// This will catch our interactions
	ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool is_hovered = ImGui::IsItemHovered(); // Hovered
	const bool is_active = ImGui::IsItemActive();   // Held
	const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
	const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
	std::cout << io.MousePos.x << " " << io.MousePos.y << '\n';

	// Add first and second point
	if(is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		points.push_back(mouse_pos_in_canvas);
		points.push_back(mouse_pos_in_canvas);
		adding_line = true;
	}
	if(adding_line)
	{
		points.back() = mouse_pos_in_canvas;
		if(!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			adding_line = false;
	}

	// Pan (we use a zero mouse threshold when there's no context menu)
	// You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
	const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
	if(is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
	{
		scrolling.x += io.MouseDelta.x;
		scrolling.y += io.MouseDelta.y;
	}

	// Context menu (under default mouse threshold)
	ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
	if(opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
		ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
	if(ImGui::BeginPopup("context"))
	{
		if(adding_line)
			points.resize(points.size() - 2);
		adding_line = false;
		if(ImGui::MenuItem("Remove one", NULL, false, points.Size > 0))
		{
			points.resize(points.size() - 2);
		}
		if(ImGui::MenuItem("Remove all", NULL, false, points.Size > 0))
		{
			points.clear();
		}
		ImGui::EndPopup();
	}

	// Draw grid + all lines in the canvas
	draw_list->PushClipRect(canvas_p0, canvas_p1, true);
	if(opt_enable_grid)
	{
		const float GRID_STEP = 64.0f;
		for(float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
			draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
		for(float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
			draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
	}
	for(int n = 0; n < points.Size; n += 2)
		draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
	draw_list->PopClipRect();

}

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if(ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}


static void ShowPlaceholderObject(const char* prefix, int knot_size, std::vector<float>& knot_vec)
{

	// Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();
	bool node_open = ImGui::TreeNode("Knot", "%s", prefix);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("Value");

	if(node_open)
	{
		knot_vec.resize(knot_size);
		for(int i = 0; i < knot_size; i++)
		{
			ImGui::PushID(i); // Use field index as identifier.
				// Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
			ImGui::TreeNodeEx("Field", flags, "Knot_%d", i);

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputFloat("##value", &knot_vec[i]);
			//ImGui::DragFloat("##value", &placeholder_members[i], 0.01f);
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
}

// Demonstrate create a window with multiple child windows.
static void ShowExampleAppLayout(bool* p_open)
{
	ImGui::SetNextWindowSize(ImVec2(800, 440), ImGuiCond_FirstUseEver);
	if(ImGui::Begin("Example: Simple layout", p_open, ImGuiWindowFlags_MenuBar))
	{
		if(ImGui::BeginMenuBar())
		{
			if(ImGui::BeginMenu("File"))
			{
				if(ImGui::MenuItem("Close", "Ctrl+W"))
				{
					*p_open = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if(ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_W))
			*p_open = false;

		// Left
		static int selected = 0;
		{
			ImGui::BeginChild("left pane", ImVec2(150, 0), true);
			for(int i = 0; i < 100; i++)
			{
				// FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
				char label[128];
				sprintf(label, "MyObject %d", i);
				if(ImGui::Selectable(label, selected == i))
					selected = i;
			}
			ImGui::EndChild();
		}
		ImGui::SameLine();

		// Right
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("MyObject: %d", selected);
			ImGui::Separator();
			if(ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
			{
				if(ImGui::BeginTabItem("Description"))
				{
					ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
					ImGui::EndTabItem();
				}
				if(ImGui::BeginTabItem("Details"))
				{
					ImGui::Text("ID: 0123456789");
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::EndChild();
			if(ImGui::Button("Revert"))
			{
			}
			ImGui::SameLine();
			if(ImGui::Button("Save"))
			{
			}
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

