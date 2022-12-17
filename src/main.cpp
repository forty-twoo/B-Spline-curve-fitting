#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "tinynurbs.h"

#include "Shader.hpp"
#include "Curve.hpp"
#include "CurveRender.hpp"

#include <iostream>
#include <vector>
#include <cstdio>


unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void HelpMarker(const char* desc);
static void ShowKnots(const char* prefix, int knot_size, std::vector<double>& knot_vec);
static void ShowControlPts(const char* prefix, int knot_size, std::vector<glm::dvec3>& controlPts);
static void ShowCanvas(bool* p_open);
static void ShowExampleAppLayout(bool* p_open);
void displayMousePos(bool* p_open);

void listenToAddPoint(double& lastCursorX, double& lastCursorY, Curve& curve, bool& regenerate);
void listenToPointChange(int& indexToModify, Curve& curve, bool& regenerate);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

bool nearlyEqual(double a, double b, double epsilon);
void setUpRender(Curve& curve, CurveRender& renderer);


static int degree = 2;
bool regenerate(false);
std::vector<double> knots;
bool flag(true);
bool hideConstruction(false);
bool firstMouse = true;
bool nowgenerate(true);
bool isAddPoints(false);
bool isAddDegree(false);
bool isChangedKnotValue(false);

//any cursor movement will update its value
float lastX, lastY;

//cursor position when mouse right clicked 
double cursorXpos, cursorYpos;

void InitCurve(Curve& curve) {
	curve.clear();
	curve.GetControlPoints().push_back(glm::dvec3(-0.8, -0.4, 0.0));
	curve.GetControlPoints().push_back(glm::dvec3(-0.5, -0.3, 0.0));
	curve.GetControlPoints().push_back(glm::dvec3(-0.1, -0.7, 0.0));
	curve.GetControlPoints().push_back(glm::dvec3(0.3, 0.1, 0.0));
	curve.GetControlPoints().push_back(glm::dvec3(0.5, -0.5, 0.0));
	curve.GetControlPoints().push_back(glm::dvec3(0.8, 0.4, 0.0));
	knots = { 0, 0, 0, 0.3, 0.4, 0.5, 0.6, 0.7, 1 };
	curve.Setknots(knots);
	curve.SetDegree(2);


}
bool firstGen = true;
static bool InterpUniform(false);
static bool InterpChord(false);
static bool InterpCentripetal(false);
static bool InterpUniversal(false);
static bool ApproGlobal(false);

static float colUnifrom[3];
static float colChord[3];
static float colCentripetal[3];
static float colUniversal[3];
static float colAppro[3];
static float curveColor[3] = { 1.0, 0, 0 };

void RenderGUI(Curve& curve, CurveRender& crenderer, bool& regenerate, bool& hideConstruction) {

	//static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	if(ImGui::Begin("B-Spline curve fitting", nullptr, ImGuiWindowFlags_NoMove)) {
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);

		//std::cout << "isAddPoints  :" << isAddPoints << '\n';
		//left
		//ImGui::SameLine();
		//ImGui::Text("control points num = %d", controlPts.size());
		//ImGui::Dummy(ImVec2(0.0f, 6.0f));
		if(ImGui::Button("clear")) {
			curve.clear();
			knots.clear();
			crenderer.Clear();
			degree = 0;
			regenerate = true;
		}
		//ImGui::Dummy(ImVec2(0.f, 0.0f));

		ImGui::SameLine();
		if(ImGui::Button("Generate Curve")) {
			curve.Setknots(knots);
			regenerate = true;
		}
		ImGui::ColorEdit3("curveColor", curveColor);
		std::cout << curveColor[1] << '\n';

		ImGui::Dummy(ImVec2(0.0f, 6.0f));
		ImGui::Checkbox(" Hide Control Points", &hideConstruction);

		ImGui::Dummy(ImVec2(0.0f, 6.0f));
		ImGui::Text("Right click to add control point");

		ImGui::SetNextItemWidth(150.f);
		ImGui::InputInt("degree", &degree);
		if(degree != curve.GetDegree()) {
			curve.SetDegree(degree);
			isAddDegree = true;
			//regenerate = true;
		}


		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		if(ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable)) {
			//std::cout << knots.size() << "  VS  " << curve.ControlPtsSize() + degree + 1 << "\n";
			int knotSz = curve.ControlPtsSize() + degree + 1;
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			bool node_open = ImGui::TreeNode("Knot_Vector", "Knot Vector");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Value: [0, 1]");

			//std::cout << "knotSize :" << knotSz << " VS " << knot_vec.size() << "\n";
			//std::cout << "now Add point Flag:" << isAddPoints << '\n';

			if(knots.size() < knotSz && (isAddPoints || isAddDegree)) {
				knots.resize(knotSz);
				double num = knotSz - 2 * degree - 1;
				double delta = (double)1.0 / num;
				for(int i = 0; i <= degree; i++) {
					knots[i] = 0;
				}
				for(int i = degree + 1; i < degree + num + 1; i++) {
					knots[i] = knots[i - 1] + delta;
				}
				for(int i = degree + num + 1; i < knotSz; i++) {
					knots[i] = 1;
				}
				regenerate = true;
				isAddPoints = false;
			}
			knots.resize(knotSz);

			for(int i = 0; i < knotSz; i++) {
				ImGui::PushID(i); // Use field index as identifier.
					// Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
				ImGui::TreeNodeEx("Field", flags, "Knot_%d", i);

				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-FLT_MIN);
				float lastKnotValue = knots[i];
				ImGui::InputDouble("value", &knots[i]);
				if(knots[i] != lastKnotValue) {
					isChangedKnotValue = true;
					regenerate = true;
				}
				//ImGui::DragFloat("##value", &placeholder_members[i], 0.01f);
				ImGui::NextColumn();
				ImGui::PopID();
			}
			ImGui::TreePop();
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			bool controlP_open = ImGui::TreeNode("Control Points", "Control Points");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Coordinate: [-1, 1]");


			if(controlP_open) {
				for(int i = 0; i < curve.ControlPtsSize(); i++) {
					ImGui::PushID(i); // Use field index as identifier.
						// Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::AlignTextToFramePadding();
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
					ImGui::TreeNodeEx("Field", flags, "point_%d", i);
					ImGui::TableSetColumnIndex(1);
					ImGui::SetNextItemWidth(-FLT_MIN);
					ImGui::Text("(%.5lf, %.5lf)", curve.GetControlPoints()[i].x, curve.GetControlPoints()[i].y);
					ImGui::NextColumn();
					ImGui::PopID();
				}
				ImGui::TreePop();
			}



			ImGui::EndTable();
		}

		ImGui::PopStyleVar();



		ImGui::Spacing();
		if(ImGui::TreeNode("Fitting Method")) {
			if(ImGui::TreeNode("Interpolation")) {
				ImGui::Checkbox("     Uniform", &InterpUniform);
				ImGui::SameLine();
				ImGui::ColorEdit3("color", colUnifrom, ImGuiColorEditFlags_NoInputs);

				ImGui::Checkbox("        Chorld", &InterpChord);
				ImGui::SameLine();
				ImGui::ColorEdit3("color", colChord, ImGuiColorEditFlags_NoInputs);

				ImGui::Checkbox("Centripetal", &InterpCentripetal);
				ImGui::SameLine();
				ImGui::ColorEdit3("color", colCentripetal, ImGuiColorEditFlags_NoInputs);

				ImGui::Checkbox("  Universal ", &InterpUniversal);
				ImGui::SameLine();
				ImGui::ColorEdit3("color", colUniversal, ImGuiColorEditFlags_NoInputs);

				ImGui::TreePop();
			}
			if(ImGui::TreeNode("Approximation")) {
				ImGui::Checkbox("Global", &ApproGlobal);
				ImGui::SameLine();
				ImGui::ColorEdit3("color", colAppro, ImGuiColorEditFlags_NoInputs);

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

	}
	ImGui::End();
	if(firstGen) {
		setUpRender(curve, crenderer);
		firstGen = false;
	}

	if((regenerate || isAddPoints || isChangedKnotValue) && !curve.GetControlPoints().empty()) {
		if(isAddPoints) {
			curve.Setknots(knots);
			isAddPoints = false;
		} else if(isChangedKnotValue) {
			curve.Setknots(knots);
			isChangedKnotValue = false;
		}
		setUpRender(curve, crenderer);
		regenerate = false;
	}


	//Render
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

int main(int, char**) {
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow((int)SCR_WIDTH, (int)SCR_HEIGHT, "B-Spline-curve-fitting", NULL, NULL);
	if(window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	// setup callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);



	// glad: load all OpenGL function pointers
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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
	ImGui_ImplOpenGL3_Init("#version 330");

	//Setup Dear ImGui style
	//GreenTheme();
	ImGui::StyleColorsClassic();
	ImGui::GetIO().FontGlobalScale = 1.0;
	ImGui::GetIO().Fonts->AddFontFromFileTTF("../assets/fonts/NotoSans-Regular.ttf", 28.0f);

	CurveRender crenderer = CurveRender();
	crenderer.Clear();
	crenderer.Init();

	// Our state
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool is_active(true);
	// Main loop

	static int degree, cpointsNum = 0;
	Curve curve;
	InitCurve(curve);
	int idxToModify = -1;
	double lastCursorX = 0, lastCursorY = 0;

	// create shaders
	Shader shaderCurve = Shader("../assets/shaders/curve.vs", "../assets/shaders/curve.fs");
	Shader shaderCPts = Shader("../assets/shaders/controlPts.vs", "../assets/shaders/controlPts.fs");


	static std::vector<double> knot_vec;
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glfwSwapBuffers(window);

		glClearColor(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);



		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		listenToAddPoint(lastCursorX, lastCursorY, curve, regenerate);
		listenToPointChange(idxToModify, curve, regenerate);

		displayMousePos(nullptr);

		RenderGUI(curve, crenderer, regenerate, hideConstruction);
		crenderer.Draw(shaderCurve, shaderCPts, glm::vec3(curveColor[0], curveColor[1], curveColor[2]), hideConstruction);


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
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
	//ImGui::GetStyle().WindowMinSize = ImVec2((float)SCR_WIDTH * 0.2f, (float)SCR_HEIGHT);
}

static void ShowCanvas(bool* p_open) {

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
	//std::cout << canvas_p0.x << "," << canvas_p0.y << std::endl;
	//std::cout << io.MousePos.x - origin.x << " " << io.MousePos.y - origin.y << '\n';

	// Add first and second point
	if(is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {

		points.push_back(mouse_pos_in_canvas);
		//std::cout << "1 :(" << mouse_pos_in_canvas.x << "," << mouse_pos_in_canvas.y << std::endl;
		points.push_back(mouse_pos_in_canvas);
		//std::cout << "2 :(" << mouse_pos_in_canvas.x << "," << mouse_pos_in_canvas.y << std::endl;
		adding_line = true;
	}
	if(adding_line) {
		points.back() = mouse_pos_in_canvas;
		//std::cout << "3 :(" << mouse_pos_in_canvas.x << "," << mouse_pos_in_canvas.y << std::endl;
		if(!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			adding_line = false;
	}

	//Right Click to add point







	// Pan (we use a zero mouse threshold when there's no context menu)
	// You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
	const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
	if(is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan)) {
		scrolling.x += io.MouseDelta.x;
		scrolling.y += io.MouseDelta.y;
	}

	// Context menu (under default mouse threshold)
	ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right, mouse_threshold_for_pan);
	if(opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
		ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
	if(ImGui::BeginPopup("context")) {
		if(adding_line)
			points.resize(points.size() - 2);
		adding_line = false;
		if(ImGui::MenuItem("Remove one", NULL, false, points.Size > 0)) {
			points.resize(points.size() - 2);
		}
		if(ImGui::MenuItem("Remove all", NULL, false, points.Size > 0)) {
			points.clear();
		}
		ImGui::EndPopup();
	}

	// Draw grid + all lines in the canvas
	draw_list->PushClipRect(canvas_p0, canvas_p1, true);
	if(opt_enable_grid) {
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
static void HelpMarker(const char* desc) {
	ImGui::TextDisabled("(?)");
	if(ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool nearlyEqual(double a, double b, double epsilon) {
	// if the distance between a and b is less than epsilon, then a and b are "close enough"
	return std::abs(a - b) <= epsilon;
}


static void ShowControlPts(const char* prefix, int cPts_size, std::vector<glm::dvec3>& controlPts) {
	// Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
}

static void ShowKnots(const char* prefix, int knotSz, std::vector<double>& knot_vec) {
	// Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
}

// Demonstrate create a window with multiple child windows.
static void ShowExampleAppLayout(bool* p_open) {
	ImGui::SetNextWindowSize(ImVec2(800, 440), ImGuiCond_FirstUseEver);
	if(ImGui::Begin("Example: Simple layout", p_open, ImGuiWindowFlags_MenuBar)) {
		if(ImGui::BeginMenuBar()) {
			if(ImGui::BeginMenu("File")) {
				if(ImGui::MenuItem("Close", "Ctrl+W")) {
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
			for(int i = 0; i < 100; i++) {
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
			if(ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
				if(ImGui::BeginTabItem("Description")) {
					ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
					ImGui::EndTabItem();
				}
				if(ImGui::BeginTabItem("Details")) {
					ImGui::Text("ID: 0123456789");
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::EndChild();
			if(ImGui::Button("Revert")) {
			}
			ImGui::SameLine();
			if(ImGui::Button("Save")) {
			}
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if(firstMouse) {
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	lastX = (float)xpos;
	lastY = (float)ypos;
}



// glfw: mouse button callback
// ---------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		//getting cursor position
		glfwGetCursorPos(window, &cursorXpos, &cursorYpos);
	}
}

void displayMousePos(bool* p_open) {
	const float DISTANCE = 10.0f;
	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if(corner != -1) {
		window_flags |= ImGuiWindowFlags_NoMove;
		ImVec2 window_pos = ImVec2(io.DisplaySize.x - DISTANCE, DISTANCE);
		ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	if(ImGui::Begin("Example: Simple overlay", p_open, window_flags | ImGuiWindowFlags_NoBackground)) {
		double xpos, ypos;
		glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
		xpos = (xpos / SCR_WIDTH) * 2.0 - 1.0;
		ypos = (-ypos / SCR_HEIGHT) * 2.0 + 1.0;
		ImGui::Text("Mouse Position: (%.3f,%.3f)", xpos, ypos);
	}
	ImGui::End();
}

//When mouse right click, the cursorXpos/cursorYpos changes, the function will update the lastCursor value
//Then a new control point can be added to the curve
void listenToAddPoint(double& lastCursorX, double& lastCursorY, Curve& curve, bool& regenerate) {
	if(lastCursorX != cursorXpos || lastCursorY != cursorYpos) {
		curve.GetControlPoints().emplace_back(glm::dvec3((cursorXpos / SCR_WIDTH) * 2.0 - 1.0, (-cursorYpos / SCR_HEIGHT) * 2.0 + 1.0, 0.0));
		lastCursorX = cursorXpos, lastCursorY = cursorYpos;
		isAddPoints = true;
		std::cout << "Flad   isAddPoints  :" << isAddPoints << '\n';
		//regenerate = true;
	}

}


void listenToPointChange(int& indexToModify, Curve& curve, bool& regenerate) {
	if(glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
		xpos = (xpos / SCR_WIDTH) * 2.0 - 1.0;
		ypos = (-ypos / SCR_HEIGHT) * 2.0 + 1.0;

		for(unsigned int i = 0; i < curve.ControlPtsSize(); i++) {
			if(nearlyEqual(xpos, curve.GetControlPoints().at(i).x, 0.05) && nearlyEqual(ypos, curve.GetControlPoints().at(i).y, 0.05)) {
				indexToModify = (int)i;
			}
		}
		if(indexToModify != -1) {
			curve.GetControlPoints().at(indexToModify) = glm::dvec3(xpos, ypos, 0.0);
			regenerate = true;
		}

	}
	if(glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) indexToModify = -1;

}

void setUpRender(Curve& curve, CurveRender& renderer) {
	renderer.SetUpVBO(curve.GetControlPoints(), curve.ConstructCurve(degree, knots, curve.GetControlPoints()));
}
