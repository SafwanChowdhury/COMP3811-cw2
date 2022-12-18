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

		struct CamCtrl_
		{
			bool cameraActive;
			bool actionZoomIn, actionZoomOut, actionMoveL, actionMoveR, actionMoveU, actionMoveD;

			float phi, theta;
			float radius;

			float lastX, lastY;
		} camControl;
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

int main() try
{
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
	std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
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
	state.prog = &prog;
	state.camControl.radius = 10.f;

	

	// Animation state
	auto last = Clock::now();

	float angle = 0.f;
	OGL_CHECKPOINT_ALWAYS();
	

	// TODO: 
	auto xcyl = make_cylinder(true, 16, { 0.f, 1.f, 0.f },
		make_scaling(5.f, 0.1f, 0.1f) // scale X by 5, Y and Z by 0.1
	);

	auto xcone = make_cone(true, 16, { 0.f, 0.f, 0.f },
		make_scaling(1.f, 0.3f, 0.3f) *
		make_translation({ 5.f, 0.f, 0.f })
	);
	auto xarrow = concatenate(std::move(xcyl), xcone);

	auto ycyl = make_cylinder(true, 16, { 1.f, 0.f, 0.f },
		make_rotation_y(3.141592f / -2.f) *
		make_scaling(5.f, 0.1f, 0.1f) // scale X by 5, Y and Z by 0.1
	);

	auto ycone = make_cone(true, 16, { 0.f, 0.f, 0.f },
		make_rotation_y(3.141592f / -2.f) *
		make_scaling(1.f, 0.3f, 0.3f) *
		make_translation({ 5.f, 0.f, 0.f })
	);
	auto yarrow = concatenate(std::move(ycyl), ycone);

	auto zcyl = make_cylinder(true, 16, { 0.f, 0.f, 1.f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(5.f, 0.1f, 0.1f) // scale X by 5, Y and Z by 0.1
	);

	auto zcone = make_cone(true, 16, { 0.f, 0.f, 0.f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(1.f, 0.3f, 0.3f) *
		make_translation({ 5.f, 0.f, 0.f })
	);
	auto zarrow = concatenate(std::move(zcyl), zcone);

	auto xy = concatenate(xarrow, yarrow);
	auto xyz = concatenate(xy, zarrow);
	GLuint vao3 = create_vao(xyz);
	std::size_t vertexCount3 = xyz.positions.size();
	
	auto testCylinder = make_cylinder(true, 128, { 0.4f, 0.4f, 0.4f },
		make_rotation_z(3.141592f / 2.f)
		* make_scaling(8.f, 2.f, 2.f)
	);
	GLuint vao = create_vao(testCylinder);
	std::size_t vertexCount = testCylinder.positions.size(); 

	auto cone = make_cone(true, 16, { 0.f, 0.f, 0.f },
		make_translation({ 10.f, 6.f, 6.f })
	);
	GLuint vao2 = create_vao(cone);
	std::size_t vertexCount2 = cone.positions.size();


	OGL_CHECKPOINT_ALWAYS();

	// Main loop
	float y = 0;
	float x = 0;
	while( !glfwWindowShouldClose( window ) )
	{
		// Let GLFW process events
		glfwPollEvents();
		
		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			glViewport( 0, 0, iwidth, iheight );
		}

		// Update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;


		//angle += dt * kPi_ * 0.3f;
		//if (angle >= 2.f * kPi_)
		//	angle -= 2.f * kPi_;

		// Update camera state
		if (state.camControl.actionZoomIn) {
			y += sin(state.camControl.theta) * kMovementPerSecond_ * dt;
			x -= cos(state.camControl.theta) * sin(state.camControl.phi) * kMovementPerSecond_ * dt;
			state.camControl.radius -= cos(state.camControl.theta) * cos(state.camControl.phi) * kMovementPerSecond_ * dt;
		}
		else if (state.camControl.actionZoomOut) {
			y -= sin(state.camControl.theta) * kMovementPerSecond_ * dt;
			x += cos(state.camControl.theta) * sin(state.camControl.phi) * kMovementPerSecond_ * dt;
			state.camControl.radius += cos(state.camControl.theta) * cos(state.camControl.phi) * kMovementPerSecond_ * dt;

		}
		if (state.camControl.actionMoveL) {
			x += cos(state.camControl.phi) * kMovementPerSecond_ * dt;
			state.camControl.radius -= sin(state.camControl.phi) * kMovementPerSecond_ * dt;
		}
		else if (state.camControl.actionMoveR) {
			x -= cos(state.camControl.phi) * kMovementPerSecond_ * dt;
			state.camControl.radius += sin(state.camControl.phi) * kMovementPerSecond_ * dt;

		}
		if (state.camControl.actionMoveU) {
			y -= kMovementPerSecond_ * dt;
		}
		else if (state.camControl.actionMoveD) {
			y += kMovementPerSecond_ * dt;

		}
		//Compute matricies
		Mat44f Rx = make_rotation_x(state.camControl.theta);
		Mat44f Ry = make_rotation_y(state.camControl.phi);
		Mat44f T = make_translation({ 0.f, 0.f, -state.camControl.radius });
		Vec3f T2 = { x, y, 0.f };
		Mat44f model2world = make_rotation_y(0);
		Mat44f world2camera = (make_translation({ 0.f, 0.f, 0.f }) * Rx) * Ry * T;
		world2camera = world2camera * make_translation(T2);
		Mat44f projection = make_perspective_projection(
			60.f * 3.1415926f / 180.f,
			fbwidth / float(fbheight),
			0.1f, 100.0f
		);
		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		Mat44f projCameraWorld = projection * world2camera * model2world;
		// Draw scene
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		
		// Clear color buffer to specified clear color (glClearColor())
		// We want to draw with our program..

		glUseProgram(prog.programId());
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix4fv(5, 1, GL_TRUE, model2world.v);
		glUniformMatrix4fv(6, 1, GL_TRUE, world2camera.v);
		float shininess = 20.f;
		Vec3f lightPos = { 10.f, 6.f, 6.f }; //light position
		glUniform3f(glGetUniformLocation(prog.programId(), "uLightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform1f(glGetUniformLocation(prog.programId(), "shininess"), shininess);
		glUniform3f(3, 0.9f, 0.9f, 0.6f); //lightcolor
		glUniform3f(4, 0.05f, 0.05f, 0.05f); //ambient light

		OGL_CHECKPOINT_DEBUG();
		//TODO: draw frame
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glBindVertexArray(vao2);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount2);
		glBindVertexArray(vao3);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount3);
		

		OGL_CHECKPOINT_DEBUG();

		glBindVertexArray(0);

		OGL_CHECKPOINT_DEBUG();

		// Display results
		glfwSwapBuffers( window );
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

