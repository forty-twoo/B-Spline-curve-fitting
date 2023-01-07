#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "tinynurbs.h"

#include "Shader.hpp"
#include "Curve.hpp"
#include "CurveRender.hpp"
#include "Interpolation.hpp"
#include "Approximation.hpp"

#include <iostream>
#include <vector>
#include <cstdio>


unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void HelpMarker(const char* desc);
static void ShowKnots(const char* prefix, int knot_size, std::vector<double>& knot_vec);
static void ShowControlPts(const char* prefix, int knot_size, std::vector<glm::dvec3>& controlPts);
void displayMousePos(bool* p_open);

void listenToAddPoint(double& lastCursorX, double& lastCursorY, std::vector<glm::dvec3>& points, bool& regenerate);
void listenToPointChange(int& indexToModify, std::vector<glm::dvec3>& points, bool& regenerate);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

bool nearlyEqual(double a, double b, double epsilon);


static int degree_Approx = 2;
static int curDegree_Approx = 2;
static int degree_Interp = 2;
static int curDegree_Interp = 2;
static int cur_conptsSize = 3;
static int conptsSize = 3;
bool regenerate(true);
bool flag(true);
bool hideConstruction[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
bool firstMouse = true;
bool nowgenerate(true);
bool isPointsChange(false);
bool isAddDegree(false);
bool isChangedKnotValue(false);
std::vector<double> knots;
std::vector<glm::dvec3> sceen_points;
std::vector<double> param;
std::vector<glm::dvec3> controlPts;


static Curve curve[8];
static CurveRender crenderer[8];
static PointsRender prenderer = PointsRender();

#define UNIFORM_INTERP 1
#define CHORLD_INTERP 2
#define CENTRIPETAL_INTERP 3
#define UNIVERSAL_INTERP 4

//any cursor movement will update its value
float lastX, lastY;

//cursor position when mouse right clicked 
double cursorXpos, cursorYpos;

bool firstGen = true;
static bool FlagDraw[8] = { 0, 0, 0, 0, 1, 1, 1, 1 };
static float curveColor[8][3] = { { 0, 0.5, 0.6 }, { 0.1, 0.3, 1 }, { 0.7, 0.3, 0.4 }, { 0.7, 0.7, 1 }, { 1, 0.6, 0.5 }, { 1, 0.9, 0.2 }, { 1, 0.7, 1 }, { 0.9, 1, 0.6 } };

void InitSceenPoints() {
	sceen_points.push_back(glm::dvec3(-0.1, -0.6, 0));
	sceen_points.push_back(glm::dvec3(0, 0, 0));
	sceen_points.push_back(glm::dvec3(0.3, 0.4, 0));
	sceen_points.push_back(glm::dvec3(0.5, -0.2, 0));



}


void RenderGUI_new(Curve curve[5], CurveRender crenderer[5], bool& regenerate, bool hideConstruction[5]) {

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0.2, 0.4, 0.1));
	if(ImGui::Begin("B-Spline curve fitting", nullptr, ImGuiWindowFlags_NoMove)) {
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);

		if(ImGui::TreeNode("Fitting Method")) {
			if(ImGui::TreeNode("Interpolation")) {
				ImGui::Spacing();
				ImGui::SetNextItemWidth(150.f);
				ImGui::InputInt("degree", &curDegree_Interp);

				if(curDegree_Interp != degree_Interp) {
					degree_Interp = curDegree_Interp;
					regenerate = true;
				}
				if(ImGui::TreeNode("Uniform")) {
					ImGui::Checkbox("Generate", &FlagDraw[0]);
					ImGui::SameLine();
					ImGui::ColorEdit3("color", curveColor[0], ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("hide control points", &hideConstruction[0]);
					ImGui::TreePop();
				}

				if(ImGui::TreeNode("Chorld")) {
					ImGui::Checkbox("Generate", &FlagDraw[1]);
					ImGui::SameLine();
					ImGui::ColorEdit3("color", curveColor[1], ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("hide control points", &hideConstruction[1]);
					ImGui::TreePop();
				}

				if(ImGui::TreeNode("Centripetal")) {
					ImGui::Checkbox("Generate", &FlagDraw[2]);
					ImGui::SameLine();
					ImGui::ColorEdit3("color", curveColor[2], ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("hide control points", &hideConstruction[2]);
					ImGui::TreePop();
				}

				if(ImGui::TreeNode("Universal")) {
					ImGui::Checkbox("Generate", &FlagDraw[3]);
					ImGui::SameLine();
					ImGui::ColorEdit3("color", curveColor[3], ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("hide control points", &hideConstruction[3]);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("Approximation")) {
				ImGui::Spacing();
				ImGui::SetNextItemWidth(150.f);
				ImGui::InputInt("control_points num", &cur_conptsSize);
				if(cur_conptsSize >= sceen_points.size()) {
					cur_conptsSize = sceen_points.size() - 1;
				}
				if(cur_conptsSize != conptsSize) {
					regenerate = true;
					conptsSize = cur_conptsSize;
				}

				ImGui::Spacing();
				ImGui::SetNextItemWidth(150.f);
				ImGui::InputInt("degree", &curDegree_Approx);
				if(curDegree_Approx >= conptsSize) {
					curDegree_Approx = conptsSize - 1;
				}
				if(curDegree_Approx != degree_Approx) {
					degree_Approx = curDegree_Approx;
					regenerate = true;
				}


				if(ImGui::TreeNode("Uniform")) {
					ImGui::Checkbox("Generate", &FlagDraw[4]);
					ImGui::SameLine();
					ImGui::ColorEdit3("color", curveColor[4], ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("hide control points", &hideConstruction[4]);
					ImGui::TreePop();
				}

				if(ImGui::TreeNode("Chorld")) {
					ImGui::Checkbox("Generate", &FlagDraw[5]);
					ImGui::SameLine();
					ImGui::ColorEdit3("color", curveColor[5], ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("hide control points", &hideConstruction[5]);
					ImGui::TreePop();
				}

				if(ImGui::TreeNode("Centripetal")) {
					ImGui::Checkbox("Generate", &FlagDraw[6]);
					ImGui::SameLine();
					ImGui::ColorEdit3("color", curveColor[6], ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("hide control points", &hideConstruction[6]);
					ImGui::TreePop();
				}

				if(ImGui::TreeNode("Universal")) {
					ImGui::Checkbox("Generate", &FlagDraw[7]);
					ImGui::SameLine();
					ImGui::ColorEdit3("color", curveColor[7], ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("hide control points", &hideConstruction[7]);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();

		}
	}
	ImGui::PopStyleColor();
	ImGui::End();


	if(regenerate) {
		knots.clear(), param.clear(), controlPts.clear();
		UniformInterp(sceen_points, degree_Interp, knots, param);
		GlobalInterp(sceen_points, degree_Interp, knots, param, controlPts);
		curve[0].ConstructCurve(degree_Interp, knots, controlPts);

		crenderer[0].SetUpVBO(curve[0].GetControlPoints(), curve[0].GetCurvePoints());

		knots.clear(), param.clear(), controlPts.clear();
		ChorldInterp(sceen_points, degree_Interp, knots, param);
		GlobalInterp(sceen_points, degree_Interp, knots, param, controlPts);
		curve[1].ConstructCurve(degree_Interp, knots, controlPts);

		crenderer[1].SetUpVBO(curve[1].GetControlPoints(), curve[1].GetCurvePoints());

		knots.clear(), param.clear(), controlPts.clear();
		CentripetalInterp(sceen_points, degree_Interp, knots, param);
		GlobalInterp(sceen_points, degree_Interp, knots, param, controlPts);
		curve[2].ConstructCurve(degree_Interp, knots, controlPts);

		crenderer[2].SetUpVBO(curve[2].GetControlPoints(), curve[2].GetCurvePoints());

		knots.clear(), param.clear(), controlPts.clear();
		UniversalInterp(sceen_points, degree_Interp, knots, param);
		GlobalInterp(sceen_points, degree_Interp, knots, param, controlPts);
		curve[3].ConstructCurve(degree_Interp, knots, controlPts);

		crenderer[3].SetUpVBO(curve[3].GetControlPoints(), curve[3].GetCurvePoints());

		knots.clear(), param.clear(), controlPts.clear();
		Approximation(0, sceen_points, degree_Approx, knots, param, conptsSize - 1, controlPts);
		curve[4].ConstructCurve(degree_Approx, knots, controlPts);

		crenderer[4].SetUpVBO(curve[4].GetControlPoints(), curve[4].GetCurvePoints());

		knots.clear(), param.clear(), controlPts.clear();
		Approximation(1, sceen_points, degree_Approx, knots, param, conptsSize - 1, controlPts);
		curve[5].ConstructCurve(degree_Approx, knots, controlPts);

		crenderer[5].SetUpVBO(curve[5].GetControlPoints(), curve[5].GetCurvePoints());

		knots.clear(), param.clear(), controlPts.clear();
		Approximation(2, sceen_points, degree_Approx, knots, param, conptsSize - 1, controlPts);
		curve[6].ConstructCurve(degree_Approx, knots, controlPts);

		crenderer[6].SetUpVBO(curve[6].GetControlPoints(), curve[6].GetCurvePoints());

		knots.clear(), param.clear(), controlPts.clear();
		Approximation(3, sceen_points, degree_Approx, knots, param, conptsSize - 1, controlPts);
		curve[7].ConstructCurve(degree_Approx, knots, controlPts);

		crenderer[7].SetUpVBO(curve[7].GetControlPoints(), curve[7].GetCurvePoints());

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

	for(int i = 0; i < 8; i++) {
		crenderer[i] = CurveRender();
		crenderer[i].Clear();
		crenderer[i].Init();
	}
	prenderer.Clear();
	prenderer.Init();

	// Our state
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool is_active(true);
	// Main loop

	static int degree, cpointsNum = 0;
	Curve curve5;
	//InitCurve(curve);
	int idxToModify = -1;
	double lastCursorX = 0, lastCursorY = 0;

	// create shaders
	Shader shaderCurve = Shader("../assets/shaders/curve.vs.glsl", "../assets/shaders/curve.fs.glsl");
	Shader shaderCPts = Shader("../assets/shaders/controlPts.vs.glsl", "../assets/shaders/controlPts.fs.glsl");
	Shader shaderPoints = Shader("../assets/shaders/points.vs.glsl", "../assets/shaders/points.fs.glsl");
	InitSceenPoints();


	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glfwSwapBuffers(window);

		glClearColor(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);



		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		listenToAddPoint(lastCursorX, lastCursorY, sceen_points, regenerate);
		listenToPointChange(idxToModify, sceen_points, regenerate);

		displayMousePos(nullptr);

		//RenderGUI(curve, crenderer, regenerate, hideConstruction);
		//crenderer.Draw(shaderCurve, shaderCPts, glm::vec3(curveColor[0], curveColor[1], curveColor[2]), hideConstruction);


		if(isPointsChange || firstGen) {
			prenderer.SetUpVBO(sceen_points);
			isPointsChange = false;
			firstGen = false;
		}
		RenderGUI_new(curve, crenderer, regenerate, hideConstruction);
		prenderer.Draw(shaderPoints);
		for(int i = 0; i < 8; i++) {
			if(FlagDraw[i])
				crenderer[i].Draw(shaderCurve, shaderCPts, glm::vec3(curveColor[i][0], curveColor[i][1], curveColor[i][2]), hideConstruction[i]);

		}



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
void listenToAddPoint(double& lastCursorX, double& lastCursorY, std::vector<glm::dvec3>& points, bool& regenerate) {
	if(lastCursorX != cursorXpos || lastCursorY != cursorYpos) {
		points.emplace_back(glm::dvec3((cursorXpos / SCR_WIDTH) * 2.0 - 1.0, (-cursorYpos / SCR_HEIGHT) * 2.0 + 1.0, 0.0));
		lastCursorX = cursorXpos, lastCursorY = cursorYpos;
		isPointsChange = true;
		regenerate = true;
		//std::cout << "Flad   isAddPoints  :" << isAddPoints << '\n';
		//regenerate = true;
	}

}


void listenToPointChange(int& indexToModify, std::vector<glm::dvec3>& points, bool& regenerate) {
	if(glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
		xpos = (xpos / SCR_WIDTH) * 2.0 - 1.0;
		ypos = (-ypos / SCR_HEIGHT) * 2.0 + 1.0;

		for(unsigned int i = 0; i < points.size(); i++) {
			if(nearlyEqual(xpos, points.at(i).x, 0.05) && nearlyEqual(ypos, points.at(i).y, 0.05)) {
				indexToModify = (int)i;
			}
		}
		if(indexToModify != -1) {
			points.at(indexToModify) = glm::dvec3(xpos, ypos, 0.0);
			isPointsChange = true;
			regenerate = true;
		}

	}
	if(glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) indexToModify = -1;

}
