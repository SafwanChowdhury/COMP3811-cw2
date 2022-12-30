#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"

#include "defaults.hpp"
#include "cube.hpp"
#include "frustum.hpp"
#include "cone.hpp"
#include "cylinder.hpp"
#include "loadobj.hpp"
#include "screenshot.hpp"
#include "skybox.hpp"
using namespace std;

namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - Coursework 2";

	//Camera Initialisers from Ex3
	constexpr float kPi_ = 3.1415926f;
	constexpr float kMovementPerSecond_ = 5.f; // units per second
	constexpr float kMouseSensitivity_ = 0.01f; // radians per pixel

	struct State_
	{
		ShaderProgram* prog;
		ShaderProgram* skybox;

		struct CamCtrl_
		{
			bool cameraActive;
			bool actionZoomIn, actionZoomOut, actionMoveL, actionMoveR, actionMoveU, actionMoveD;

			float phi, theta;
			float radius;
			bool screenshot;
			float lastX, lastY, x, y, mod;
		} camControl;

		struct AnimCtrl_
		{
			bool animation = false;
			bool animPlay, animFWD, animRWD;

			float mod;

		} animControl;

		struct ObjCtrl_
		{
			bool displayCoords = false;
			bool objFWD, objBKD, objLFT, objRGT, objUP, objDWN;

			float x ,y ,z, x1, y1, z1;
			bool pressed = false;
			Mat44f storePos{};

		} objControl;

	};
	//end

	void glfw_callback_error_( int, char const* );

	void glfw_callback_key_( GLFWwindow*, int, int, int, int );
	void glfw_callback_motion_(GLFWwindow*, double, double);
	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};
}

int main() try{
	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '%s' (%d)", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_DEPTH_BITS, 24 );

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG
	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);
	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '%s' (%d)", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	State_ state{};

	glfwSetWindowUserPointer(window, &state);

	glfwSetKeyCallback(window, &glfw_callback_key_);
	glfwSetCursorPosCallback(window, &glfw_callback_motion_);

	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoaDGLLoader() failed - cannot load GL API!" );

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	//std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// TODO: global GL setup goes here
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );

	glViewport( 0, 0, iwidth, iheight );

	// Other initialization & loading
	// Load shader program
	ShaderProgram prog({
		{ GL_VERTEX_SHADER, "assets/default.vert" },
		{ GL_FRAGMENT_SHADER, "assets/default.frag" }
		});
	ShaderProgram skybox({
		{ GL_VERTEX_SHADER, "assets/skybox.vert" },
		{ GL_FRAGMENT_SHADER, "assets/skybox.frag" }
		});

	state.prog = &prog;
	state.skybox = &skybox;
	state.camControl.radius = 10.f;



	// Animation state
	auto last = Clock::now();

	float angle = 0.f;
	float rktHeight = 0.f;
	OGL_CHECKPOINT_ALWAYS();


	// TODO:

	auto baseCyl = make_cylinder(true, 16, { 0.05f, 0.05f, 0.05f }, {0.1f, 0.1f, 0.1f }, {0.2f,0.2f,0.2f }, 12.8f, 1.f,
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.1f, 0.02f, 0.02f) *
		make_translation({ 0.f, 0.f, 0.f })
	);


	auto cylR = make_cylinder(true, 16, { 0.2f, 0.2f, 0.2f }, { 0.2f, 0.2f, 0.2f }, { 0.4f,0.4f,0.4f }, 12.8f, 1.f,
		make_rotation_z(45.f / (180.f / 3.141592f)) *
		make_scaling(0.115f, 0.02f, 0.02f) *
		make_translation({ 0.3f, 1.7f, 0.f })

	);


	auto cylL = make_cylinder(true, 16, { 0.2f, 0.2f, 0.2f }, { 0.2f, 0.2f, 0.2f }, { 0.4f,0.4f,0.4f }, 12.8f, 1.f,
		make_rotation_z(135.f/(180.f/3.141592f))*
		make_scaling(0.115f, 0.02f, 0.02f)*
		make_translation({ 0.3f, -1.7f, 0.f })
	);



	auto cylR2 = make_cylinder(true, 16, { 0.2f, 0.2f, 0.2f }, { 0.2f, 0.2f, 0.2f }, { 0.4f,0.4f,0.4f }, 12.8f, 1.f,
		make_rotation_y(270.f / (180.f / 3.141592f)) *
		make_scaling(0.06f, 0.02f, 0.02f) *
		make_translation({ 0.f, 5.7f, 3.3f })
	);



	auto cylL2 = make_cylinder(true, 16, { 0.2f, 0.2f, 0.2f }, { 0.2f, 0.2f, 0.2f }, { 0.4f,0.4f,0.4f }, 12.8f, 1.f,
		make_rotation_y(270.f / (180.f / 3.141592f))*
		make_scaling(0.06f, 0.02f, 0.02f)*
		make_translation({ 0.f, 5.7f, -3.3f })
	);

	auto cube = make_cube(1, { 0.f, 0.f, 0.f }, { 0.01f, 0.01f, 0.01f }, { 0.5f,0.5f,0.5f }, 50.f, 1.f,
		make_scaling(0.1f, 0.07f, 0.02f)*
		make_translation({ -1.2f, 1.7f, 4.f })
	);

	auto cube2 = make_cube(1, { 0.f, 0.f, 0.f }, { 0.01f, 0.01f, 0.01f }, { 0.5f,0.5f,0.5f }, 50.f, 1.f,
		make_scaling(0.1f, 0.07f, 0.02f)*
		make_translation({ 1.2f, 1.7f, 4.f })
	);




	auto RightArm = concatenate(baseCyl, cylR);
	auto MonitorArms = concatenate(RightArm, cylL);
	auto MonitorArms1 = concatenate(MonitorArms, cylR2);
	auto MonitorArms2 = concatenate(MonitorArms1, cylL2);
	auto MonitorScreen1 = concatenate(MonitorArms2, cube);
	auto Monitors = concatenate(MonitorScreen1, cube2);
	GLuint MonitorsVao = create_vao(Monitors);
	std::size_t MonitorsVert = Monitors.positions.size();


	state.objControl.x = 0.f;
	state.objControl.y = 0.f;
	state.objControl.z = 0.f;
	state.objControl.x1 = 18.8f;
	state.objControl.y1 = 8.42f;
	state.objControl.z1 = 9.2f;
	state.camControl.x = -5.48f;
	state.camControl.y = -0.55f;
	state.camControl.radius = 2.05f;
	state.camControl.mod = 1.f;
	state.animControl.mod = 1.f;
	state.animControl.animation = false;


	auto redCone = make_cone(true, 16, { 1.f, 0.f, 0.f }, { 1.0f, 0.f, 0.f }, { 0.5f,0.f,0.f }, 32.f, 1.f,
		make_scaling(0.2f, 0.1f, 0.1f) *
		make_translation({ 13.1f, 98.6f, 15.1f }) *
		make_rotation_z(3.141592f * 0.8)
	);

	GLuint floodLight1Vao = create_vao(redCone);
	std::size_t coneVertex = redCone.positions.size();

	auto blueCone = make_cone(true, 16, { 0.f, 0.f, 1.f }, { 0.f, 0.f, 1.f }, { 0.f,0.f,0.5f }, 32.f, 1.f,
		make_scaling(0.2f, 0.1f, 0.1f) *
		make_translation({ 13.1f, 98.6f, 24.7f }) *
		make_rotation_z(3.141592f * 0.8)
	);

	GLuint floodLight2Vao = create_vao(blueCone);
	std::size_t coneVertex2 = blueCone.positions.size();



	auto rocket = load_wavefront_obj("external/Rocket/rocket.obj",
		make_scaling(0.005f, 0.005f, 0.005f) *
		make_rotation_x(3.141592f / -2.f) *
		make_translation({ 750.f, -400.f, 600.f })
	);


	GLuint rocketVAO = create_vao(rocket);

	GLuint textureObjectId = load_texture_2d("external/Rocket/rocket.jpg");
	std::size_t rocketVertex = rocket.positions.size();


	auto launch = load_wavefront_obj("external/Scene/scene.obj", make_scaling(0.49f, 0.49f, 0.49f));
	for (int i = 0; i < launch.positions.size(); i++) {
		launch.positions[i] = launch.positions[i] + Vec3f{ 2.f, 0.f, 2.f };
	}
	GLuint launchVAO = create_vao(launch);
	std::size_t launchVertex = launch.positions.size();


	auto cube3 = make_cube(1, { 0.5f, 0.87f, 1.f }, { 0.5f, 0.87f, 1.f }, { 0.5f,0.5f,0.5f }, 32.f, 0.1f,
		make_scaling(2.45f, 0.6f, 0.1f) *
		make_translation({ -0.19f, 0.58f, -2.56f })
	);

	GLuint windowGlass = create_vao(cube3);
	std::size_t windowVertex = cube3.positions.size();

	auto cube4 = make_cube(1, { 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f }, { 1.f,1.f,1.f }, 2.f, 1.f,
		make_scaling(0.1f, 0.1f, 0.1f) *
		make_translation({ 18.8f, 8.42f, 9.2f })
	);

	GLuint lightBox1 = create_vao(cube4);
	std::size_t lightBoxVertex1 = cube4.positions.size();

	auto cube5 = make_cube(1, { 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f }, { 1.f,1.f,1.f }, 2.f, 1.f,
		make_scaling(0.1f, 0.1f, 0.1f) *
		make_translation({ -28.17f, 8.42f, 9.2f })
	);

	GLuint lightBox2 = create_vao(cube5);
	std::size_t lightBoxVertex2 = cube5.positions.size();

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces =
	{
		"external/skybox/right.png",
		"external/skybox/left.png",
		"external/skybox/top.png",
		"external/skybox/bottom.png",
		"external/skybox/front.png",
		"external/skybox/back.png"
	};

	GLuint cubemapTexture = load_cubemap(faces);

	OGL_CHECKPOINT_ALWAYS();

	//imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");






	// Main loop
	float rktLast = 1.f;
	int tog = 0;
	int tog2 = 0;
	while (!glfwWindowShouldClose(window))
	{
		// Let GLFW process events
		glfwPollEvents();

		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize(window, &nwidth, &nheight);

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if (0 == nwidth || 0 == nheight)
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize(window, &nwidth, &nheight);
				} while (0 == nwidth || 0 == nheight);
			}

			glViewport(0, 0, fbwidth, fbheight);
		}

		// Update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;

		angle += dt * kPi_ * 0.3f;
		if (angle >= 2.f * kPi_)
			angle -= 2.f * kPi_;

		float x = 1.1f;
		if (state.animControl.animation) {
			if (rktHeight < 10) {
				x += 0.01f;
			}
			rktHeight += (rktLast / x) * (0.015f * state.animControl.mod);
			rktLast = rktHeight;
		}

		if (state.objControl.displayCoords == 1 && tog == 0) {
			printf("T = %f %f %f\n", state.objControl.x, state.objControl.y, state.objControl.z);
			printf("S = %f %f %f\n", state.objControl.x1, state.objControl.y1, state.objControl.z1);
			printf("x: %f, y: %f, z: %f\n", state.camControl.x, state.camControl.y, state.camControl.radius);
			tog = 1;
		}
		if (state.objControl.displayCoords == 0)
		{
			tog = 0;
		}



		Vec3f pointLightPositions[] = {
			Vec3f{ 2.7f, 10.f, 1.51f },
			Vec3f{2.7f, 10.f, 2.5f},
			Vec3f{19.7f, 17.7f, 23.8f},
			Vec3f{1.7f, 1.63f, 22.21f},
			Vec3f{6.1f, 1.63f, 22.21f}
		};

		// Update camera state
		if (state.camControl.actionZoomIn) {
			state.camControl.y += sin(state.camControl.theta) * kMovementPerSecond_ * dt * state.camControl.mod;
			state.camControl.x -= cos(state.camControl.theta) * sin(state.camControl.phi) * kMovementPerSecond_ * dt * state.camControl.mod;
			state.camControl.radius -= cos(state.camControl.theta) * cos(state.camControl.phi) * kMovementPerSecond_ * dt * state.camControl.mod;
		}
		else if (state.camControl.actionZoomOut) {
			state.camControl.y -= sin(state.camControl.theta) * kMovementPerSecond_ * dt * state.camControl.mod;
			state.camControl.x += cos(state.camControl.theta) * sin(state.camControl.phi) * kMovementPerSecond_ * dt * state.camControl.mod;
			state.camControl.radius += cos(state.camControl.theta) * cos(state.camControl.phi) * kMovementPerSecond_ * dt * state.camControl.mod;

		}
		if (state.camControl.actionMoveL) {
			state.camControl.x += cos(state.camControl.phi) * kMovementPerSecond_ * dt * state.camControl.mod;
			state.camControl.radius -= sin(state.camControl.phi) * kMovementPerSecond_ * dt * state.camControl.mod;
		}
		else if (state.camControl.actionMoveR) {
			state.camControl.x -= cos(state.camControl.phi) * kMovementPerSecond_ * dt * state.camControl.mod;
			state.camControl.radius += sin(state.camControl.phi) * kMovementPerSecond_ * dt * state.camControl.mod;

		}
		if (state.camControl.actionMoveU) {
			state.camControl.y -= kMovementPerSecond_ * dt * state.camControl.mod;
		}
		else if (state.camControl.actionMoveD) {
			state.camControl.y += kMovementPerSecond_ * dt * state.camControl.mod;

		}
		//Compute matricies-
		Mat44f Rx = make_rotation_x(state.camControl.theta);
		Mat44f Ry = make_rotation_y(state.camControl.phi);
		Mat44f T = make_translation({ state.camControl.x, state.camControl.y, -state.camControl.radius });
		Mat44f model2world = make_rotation_y(0);
		Mat44f world2camera = Rx * Ry * T;
		Mat44f projection = make_perspective_projection(
			60.f * 3.1415926f / 180.f,
			fbwidth / float(fbheight),
			0.1f, 100.0f
		);
		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		// Draw scene
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// Clear color buffer to specified clear color (glClearColor())
		// We want to draw with our program..

		glUseProgram(prog.programId());
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(0, 1, GL_TRUE, projection.v);
		glUniformMatrix4fv(4, 1, GL_TRUE, T.v);
		glUniformMatrix4fv(5, 1, GL_TRUE, model2world.v);
		glUniformMatrix4fv(6, 1, GL_TRUE, world2camera.v);


		Vec3f lightColor = { 1.f, 0.f, 0.f };
		Vec3f diffuseColor = lightColor * 0.3f;
		Vec3f ambientColor = diffuseColor * 0.01f;

		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[0].ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[0].diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[0].specular"), 0.25f, 0.f, 0.f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[0].quadratic"), 0.032f);

		lightColor = { 0.f, 0.f, 1.f };
		diffuseColor = lightColor * 0.3f;
		ambientColor = diffuseColor * 0.01f;

		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[1].ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[1].diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[1].specular"), 0.f, 0.f, 0.25f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[1].quadratic"), 0.032f);

		lightColor = { 1.f, 1.f, 1.f };
		diffuseColor = lightColor * 1.f;
		ambientColor = diffuseColor * 0.01f;

		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[2].ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[2].diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[2].specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[2].quadratic"), 0.032f);

		lightColor = { 1.f, 1.f, 1.f };
		diffuseColor = lightColor * 0.5f;
		ambientColor = diffuseColor * 0.01f;

		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[3].ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[3].diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[3].specular"), 0.f, 0.f, 0.f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[3].quadratic"), 0.032f);

		lightColor = { 1.f, 1.f, 1.f };
		diffuseColor = lightColor * 0.5f;
		ambientColor = diffuseColor * 0.01f;

		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[4].position"), pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[4].ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[4].diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(prog.programId(), "pointLights[4].specular"), 0.f, 0.f, 0.f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[4].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[4].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(prog.programId(), "pointLights[4].quadratic"), 0.032f);

		OGL_CHECKPOINT_DEBUG();
		//TODO: draw frame
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(launchVAO);
		glDrawArrays(GL_TRIANGLES, 0, launchVertex);
		glBindVertexArray(floodLight1Vao);

		//exterior lights
		glUniform1f(8, 1.f);
		glUniform3f(glGetUniformLocation(prog.programId(), "emissive"), 1.f, 0.f, 0.f);
		glDrawArrays(GL_TRIANGLES, 0, coneVertex);
		glUniform3f(glGetUniformLocation(prog.programId(), "emissive"), 0.f, 0.f, 1.f);
		glBindVertexArray(floodLight2Vao);
		glDrawArrays(GL_TRIANGLES, 0, coneVertex2);
		glUniform1f(8, 0.f);
		glUniform3f(glGetUniformLocation(prog.programId(), "emissive"), 0.f, 0.f, 0.f);

		//rocket
		model2world = make_translation({ 0.f, rktHeight, 0.f });
		glUniformMatrix4fv(5, 1, GL_TRUE, model2world.v);
		normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniform1f(7, 1.f);
		glBindVertexArray(rocketVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,textureObjectId);
		glDrawArrays(GL_TRIANGLES, 0, rocketVertex);
		model2world = make_rotation_x(0.f);
		OGL_CHECKPOINT_DEBUG();
		glUniform1f(7, 0.f);
		glBindTexture(GL_TEXTURE_2D,0);

		//monitors
		model2world = make_translation({ 4.4f, 0.86f, 21.45f });
		glUniformMatrix4fv(5, 1, GL_TRUE, model2world.v);
		normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glBindVertexArray(MonitorsVao);
		glDrawArrays(GL_TRIANGLES, 0, MonitorsVert);

		//interior lights
		glUniform1f(8, 1.f);
		glUniform3f(glGetUniformLocation(prog.programId(), "emissive"), 1.f, 1.f, 1.f);
		glBindVertexArray(lightBox1);
		glDrawArrays(GL_TRIANGLES, 0, lightBoxVertex1);
		glBindVertexArray(lightBox2);
		glDrawArrays(GL_TRIANGLES, 0, lightBoxVertex2);
		glUniform3f(glGetUniformLocation(prog.programId(), "emissive"), 0.f, 0.f, 0.f);
		glUniform1f(8, 0.f);

		


		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		glUseProgram(skybox.programId());
		model2world = make_scaling( 100.f, 100.f, 100.f );
		glUniformMatrix4fv(1, 1, GL_TRUE, world2camera.v);
		glUniformMatrix4fv(0, 1, GL_TRUE, projection.v);
		glUniformMatrix4fv(2, 1, GL_TRUE, model2world.v);
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default


		OGL_CHECKPOINT_DEBUG();

		//window
		glUseProgram(prog.programId());
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glBindVertexArray(windowGlass);
		glDrawArrays(GL_TRIANGLES, 0, windowVertex);
		glDisable(GL_BLEND);

		glBindVertexArray(0);

		// Display results
		glfwSwapBuffers(window);
	}

	// Cleanup.
	//TODO: additional cleanup

	return 0;
}
catch( std::exception const& eErr )
{
	std::fprintf( stderr, "Top-level Exception (%s):\n", typeid(eErr).name() );
	std::fprintf( stderr, "%s\n", eErr.what() );
	std::fprintf( stderr, "Bye.\n" );
	return 1;
}


namespace
{
	void glfw_callback_error_( int aErrNum, char const* aErrDesc )
	{
		std::fprintf( stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum );
	}
	int mod = 0;
	float dc = 0.f;
	void glfw_callback_key_( GLFWwindow* aWindow, int aKey, int, int aAction, int )
	{
		if( GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction )
		{
			glfwSetWindowShouldClose( aWindow, GLFW_TRUE );
			return;
		}

		//Camera Controls from Ex3...
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{

			if (mod == 0) {
				dc = 1.f;
			}
			else if (mod < 0) {
				dc = 0.1f;
				mod = -1;
			}
			else if (mod > 0) {
				dc = 10.f;
				mod = 1;
			}

			if (GLFW_KEY_KP_ADD == aKey && GLFW_PRESS == aAction)
			{
				mod+= 1;
			}
			if (GLFW_KEY_KP_SUBTRACT == aKey && GLFW_PRESS == aAction)
			{
				mod -= 1;
			}
			// R-key reloads shaders.
			if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				if (state->prog)
				{
					try
					{
						state->prog->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}
			}

			// Space toggles camera
			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.cameraActive = !state->camControl.cameraActive;

				if (state->camControl.cameraActive)
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				else
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			//screenshot
			if (GLFW_KEY_F12 == aKey && GLFW_PRESS == aAction)
			{
				if (GLFW_PRESS == aAction)
					saveScreenshot();
			}
			//animation controls
			//slow down
			if (GLFW_KEY_1 == aKey || GLFW_KEY_LEFT == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->animControl.mod = 0.5f;
				}
			}
			//play pause
			if ((GLFW_KEY_2 == aKey || GLFW_KEY_UP == aKey) && GLFW_PRESS == aAction)
			{
				state->animControl.animPlay = !state->animControl.animPlay;
				if (state->animControl.animPlay) {
					printf("off\n");
					state->animControl.animation = 0.f;
				}
				else {
					printf("on\n");
					state->animControl.animation = 1.f;
				}
			}
			//reset speed modifier
			if ((GLFW_KEY_3 == aKey || GLFW_KEY_DOWN == aKey) && GLFW_PRESS == aAction)
			{
				state->animControl.mod = 1.f;
			}
			//speed up
			if (GLFW_KEY_4 == aKey || GLFW_KEY_RIGHT == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->animControl.mod = 2.f;
				}
			}



			// Camera controls if camera is active
			if (state->camControl.cameraActive)
			{
				if (GLFW_KEY_W == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionZoomIn = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionZoomIn = false;
				}
				else if (GLFW_KEY_S == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionZoomOut = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionZoomOut = false;
				}
				if (GLFW_KEY_A == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveL = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveL = false;
				}
				else if (GLFW_KEY_D == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveR = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveR = false;
				}
				if (GLFW_KEY_Q == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveU = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveU = false;
				}
				else if (GLFW_KEY_E == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveD = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveD = false;
				}
				if (GLFW_KEY_LEFT_SHIFT == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.mod = 2;
					else if (GLFW_RELEASE == aAction)
						state->camControl.mod = 1;
				}
				else if (GLFW_KEY_LEFT_CONTROL == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.mod = 0.5;
					else if (GLFW_RELEASE == aAction)
						state->camControl.mod = 1;
				}
			}


			//object controls, for development only
			if (GLFW_KEY_KP_4 == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.x -= dc;
					state->objControl.pressed = 1;
				}
				else if (GLFW_RELEASE == aAction) {
					state->objControl.x += 0;
					state->objControl.pressed = 0;
				}
			}
			else if (GLFW_KEY_KP_6 == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.x += dc;
					state->objControl.pressed = 1;
				}
				else if (GLFW_RELEASE == aAction) {
					state->objControl.x += 0;
					state->objControl.pressed = 0;
				}
			}
			if (GLFW_KEY_KP_2 == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.y -= dc;
					state->objControl.pressed = 1;
				}
				else if (GLFW_RELEASE == aAction) {
					state->objControl.y += 0;
					state->objControl.pressed = 0;
				}
			}
			else if (GLFW_KEY_KP_8 == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.y += dc;
					state->objControl.pressed = 1;
				}
				else if (GLFW_RELEASE == aAction) {
					state->objControl.y += 0;
					state->objControl.pressed = 0;
				}
			}
			if (GLFW_KEY_KP_7 == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.z -= dc;
					state->objControl.pressed = 1;
				}
				else if (GLFW_RELEASE == aAction) {
					state->objControl.z += 0;
					state->objControl.pressed = 0;
				}
			}
			else if (GLFW_KEY_KP_9 == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.z += dc;
					state->objControl.pressed = 1;
				}
				else if (GLFW_RELEASE == aAction) {
					state->objControl.z += 0;
					state->objControl.pressed = 0;
				}
			}
			if (GLFW_KEY_5 == aKey || GLFW_KEY_KP_5 == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.displayCoords = 1;
				}
				else if (GLFW_RELEASE == aAction)
					state->objControl.displayCoords = 0;
			}

			if (GLFW_KEY_U== aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.x1 -= (dc/10);
				}
				else if (GLFW_RELEASE == aAction)
					state->objControl.x1 += 0;
			}
			else if (GLFW_KEY_I == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.x1 += (dc/10);
				}
				else if (GLFW_RELEASE == aAction)
					state->objControl.x1 += 0;
			}
			if (GLFW_KEY_J == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.y1 -= (dc/10);
				}
				else if (GLFW_RELEASE == aAction)
					state->objControl.y1 += 0;
			}
			else if (GLFW_KEY_K == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.y1 += (dc/10);
				}
				else if (GLFW_RELEASE == aAction)
					state->objControl.y1 += 0;
			}
			if (GLFW_KEY_N == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.z1 -= (dc/10);
				}
				else if (GLFW_RELEASE == aAction)
					state->objControl.z1 += 0;
			}
			else if (GLFW_KEY_M == aKey)
			{
				if (GLFW_PRESS == aAction) {
					state->objControl.z1 += (dc/10);
				}
				else if (GLFW_RELEASE == aAction)
					state->objControl.z1 += 0;
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl.cameraActive)
			{
				auto const dx = aX - state->camControl.lastX;
				auto const dy = aY - state->camControl.lastY;

				state->camControl.phi += dx * kMouseSensitivity_;

				state->camControl.theta += dy * kMouseSensitivity_;
				if (state->camControl.theta > kPi_ / 2.f)
					state->camControl.theta = kPi_ / 2.f;
				else if (state->camControl.theta < -kPi_ / 2.f)
					state->camControl.theta = -kPi_ / 2.f;
			}

			state->camControl.lastX = aX;
			state->camControl.lastY = aY;
		}
	}
	//...End
}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if( window )
			glfwDestroyWindow( window );
	}
}


