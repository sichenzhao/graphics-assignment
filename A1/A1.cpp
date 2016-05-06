#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

//static const size_t DIM = 16;
static const size_t DIM = 5;
static int xposition = 0;
static int min_distance = 1;
static bool shift_pressed = false;
static const int flash_frame = 56;
static const float initial_distance = float(DIM)*2.0*M_SQRT1_2;
//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{
        m_eye_distance = initial_distance;;
        height_map = new std::pair<int, glm::vec3>*[DIM];
        for(int i = 0; i<DIM; i++){
            height_map[i] = new std::pair<int, glm::vec3>[DIM];
            for(int j=0; j<DIM; j++){
                height_map[i][j] = make_pair(0, glm::vec3(0.0f, 0.0f, 0.0f));
            }
        }
        flash_helper = 0;
        active_z = active_x = 0;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
    for(int i=0; i<DIM; i++){
        delete [] height_map[i];
    }
    delete[] height_map;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
        // lookAt(eye, center, up)
	view = glm::lookAt( 
		// M_SQRT1_2 = The inverse of the square root of 2.
		glm::vec3( 0.0f, m_eye_distance, m_eye_distance),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );
	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		// x1, y1, z1; x2, y2, z2
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void genVertsArray(float* &vertsCube, float** cubeVertices, int verticesArray[], float x_cord, float y_cord, float z_cord, int arraySize){
	int n = 0;
	for(int i = 0; i<arraySize; i++)
	{
		int index = verticesArray[i];
		vertsCube[n] = cubeVertices[index][0] + x_cord; 
		vertsCube[n+1] = cubeVertices[index][1] + y_cord;
		vertsCube[n+2] = cubeVertices[index][2] + z_cord;
		n = n + 3;
	}
	return;
}

// as first try, draw the first cube on top left corner
void A1::buildCube(float x_cord, float y_cord, float z_cord)
{
	size_t sz = 3 * 3 * 2 * 6;

	float *vertsCube = new float[ sz ];

	int verticesArray[] = 
	{0,1,2, 
		0,2,3, 
		0,3,4, 
		0,4,5, 
		0,1,5, 
		1,5,6, 
		1,2,7, 
		1,7,6, 
		2,3,4, 
		2,4,7, 
		4,6,7, 
		4,5,6};

	float v0[] = {1,1,1};
	float v1[] = {0,1,1};
	float v2[] = {0,0,1};
	float v3[] = {1,0,1};
	float v4[] = {1,0,0};
	float v5[] = {1,1,0};
	float v6[] = {0,1,0};
	float v7[] = {0,0,0};
	float* cubeVertices[] = {v0, v1, v2, v3, v4, v5, v6, v7};
	genVertsArray(vertsCube, cubeVertices, verticesArray, x_cord, y_cord, z_cord, sz/3);

	//GLuint m_cube_vao;
	//GLuint m_cube_vbo;
	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_cube_vao );
	glBindVertexArray( m_cube_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_cube_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		vertsCube, GL_DYNAMIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] vertsCube;

	CHECK_GL_ERRORS;
}

void A1::buildIndicator(int active_x, int active_z)
{
	size_t sz = 4 * 3 * 3;

	float *vertsIndi = new float[ sz ];

	int verticesArray[] = 
	{0,1,4,
		0,4,3,
		0,3,2,
		0,2,1};

	float v0[] = {0.5,0,0.5};
	float v1[] = {1,1,0.5};
	float v2[] = {0.5,1,0};
	float v3[] = {0,1,0.5};
	float v4[] = {0.5,1,1};
	float* cubeVertices[] = {v0, v1, v2, v3, v4};
        int active_height = height_map[active_x][active_z].first;
	genVertsArray(vertsIndi, cubeVertices, verticesArray, active_x, 
                 active_height < 0 ? 0 : active_height + 1, active_z, sz/3);

	//GLuint m_cube_vao;
	//GLuint m_cube_vbo;
	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_indi_vao );
	glBindVertexArray( m_indi_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_indi_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_indi_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		vertsIndi, GL_DYNAMIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] vertsIndi;

	CHECK_GL_ERRORS;

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
        flash_helper!=flash_frame ? flash_helper++ : flash_helper=0;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset Application" ) ) {
                        resetGrid();
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

                for(int i = 0; i<8; i++){
                    ImGui::PushID( i );
                    ImGui::ColorEdit3( "##Colour", colour[i] );
                    ImGui::SameLine();
                    if( ImGui::RadioButton( "##Col", &current_col, i ) ) {
                        // Select this colour.
                        height_map[active_x][active_z].second = glm::vec3(colour[i][0], colour[i][1], colour[i][2]);
                    }
                    ImGui::PopID();
                }

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */

 void A1::drawCube(int x, int y, int z) {
     buildCube(x,y,z);
     glBindVertexArray( m_cube_vao );
     glUniform3f( col_uni, height_map[x][z].second.r, height_map[x][z].second.g, height_map[x][z].second.b);
     glDrawArrays( GL_TRIANGLES, 0,  3 * 3 * 3 * 2 );
 }

void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	view = glm::lookAt( 
		// M_SQRT1_2 = The inverse of the square root of 2.
		glm::vec3( 0.0f, m_eye_distance, m_eye_distance),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( glm::rotate(view, m_view_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
                for(int x=0; x<DIM; x++){
                    // fixed x cord
                    for(int z=0; z<DIM; z++){
                        // fixed z cord
                        if(!(x == active_x && z == active_z && flash_helper>=flash_frame/2)){
                            for(int y = 0; y<height_map[x][z].first; y++){
                                drawCube(x,y,z);
                            }
                            for(int y=-1; y>height_map[x][z].first; y--){
                                drawCube(x,y,z);
                            }

                            // Highlight the active square.
                            if(x==active_x && z==active_z){                        
                            buildIndicator(active_x, active_z);
                            glBindVertexArray( m_indi_vao );
		            glUniform3f( col_uni, 1.0f, 0.0f, 0.0f );
                            glDrawArrays( GL_TRIANGLES, 0, 4 * 3 * 3);
                            }
                        }
                    }
                }

	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
                if(m_mouseButtonActive){
                    m_view_rotation = 6.283f * (xPos-m_mouseStartXPos)/1000;
                } else {
                    m_mouseStartXPos = xPos;
                }
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
                if (button == GLFW_MOUSE_BUTTON_LEFT){
                    if (actions == GLFW_PRESS) {
                            m_mouseButtonActive = true;
                            cout << "left mouse pressed" << endl;
                    }

                    if (actions == GLFW_RELEASE) {
                        m_mouseButtonActive = false;
                            cout << "left mouse released" << endl;
                    }
                }
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.
        m_eye_distance += yOffSet;
        if(m_eye_distance < min_distance) m_eye_distance = min_distance;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
            // Respond to some key events.
            if (key == GLFW_KEY_R) {
                cout << "R key pressed" << endl;

                // reset
                resetGrid();

                eventHandled = true;
            }
            if (key == GLFW_KEY_Q) {
                cout << "Q key pressed" << endl;

                // quit
                glfwSetWindowShouldClose(m_window, GL_TRUE);

                eventHandled = true;
            }
            if (key == GLFW_KEY_BACKSPACE) {
                cout << "backspace" << endl;
                //if(height_map[active_x][active_z].first>0){
                    height_map[active_x][active_z].first--;
                //}
                eventHandled = true;
            }
            if (key == GLFW_KEY_SPACE){
                cout << "space" << endl;
                height_map[active_x][active_z].first++;
                height_map[active_x][active_z].second = glm::vec3(colour[current_col][0],colour[current_col][1],colour[current_col][2]);

                eventHandled = true;
            }
            if (key == GLFW_KEY_LEFT){
                cout << "left, select[" << active_x << ", " << active_z<<"]" << endl;
                if(active_x>0) {
                    // Assume only copy if the new position is empty
                    if(shift_pressed && height_map[active_x-1][active_z].first==0) {
                        height_map[active_x-1][active_z] = height_map[active_x][active_z];
                    } else {
                        height_map[active_x-1][active_z] = height_map[active_x][active_z];
                        height_map[active_x][active_z].first = 0;
                    }
                        active_x--;
                }
                eventHandled = true;
            }
            if (key == GLFW_KEY_RIGHT){
                cout << "right, select[" << active_x << ", " << active_z<<"]" << endl;
                if(active_x<DIM-1) {
                    // Assume only copy if the new position is empty
                    if(shift_pressed && height_map[active_x+1][active_z].first==0) {
                        height_map[active_x+1][active_z] = height_map[active_x][active_z];
                    } else {
                        height_map[active_x+1][active_z] = height_map[active_x][active_z];
                        height_map[active_x][active_z].first = 0;
                    }
                        active_x++;
                }
                eventHandled = true;
            }
            if (key == GLFW_KEY_UP){
                cout << "up, select[" << active_x << ", " << active_z<<"]" << endl;
                if(active_z>0) {
                    // Assume only copy if the new position is empty
                    if(shift_pressed && height_map[active_x][active_z-1].first==0) {
                        height_map[active_x][active_z-1] = height_map[active_x][active_z];
                    } else {
                        height_map[active_x][active_z-1] = height_map[active_x][active_z];
                        height_map[active_x][active_z].first = 0;
                    }
                        active_z--;
                }
                eventHandled = true;
            }
            if (key == GLFW_KEY_DOWN){
                cout << "down, select[" << active_x << ", " << active_z<<"]" << endl;
                if(active_z<DIM-1) {
                    // Assume only copy if the new position is empty
                    if(shift_pressed && height_map[active_x][active_z+1].first==0) {
                        height_map[active_x][active_z+1] = height_map[active_x][active_z];
                    } else {
                        height_map[active_x][active_z+1] = height_map[active_x][active_z];
                        height_map[active_x][active_z].first = 0;
                    }
                    active_z++;
                }
                eventHandled = true;
            }
            if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
                cout << "shift" <<endl;
                shift_pressed = true;
                eventHandled = true;
            }
	}

        if( action== GLFW_RELEASE) {
            if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
                cout << "shift release" <<endl;
                shift_pressed = false;
                eventHandled = true;
            }
        
        }

	return eventHandled;
}

void A1::resetGrid(){
    for(int i=0; i<DIM; i++){
        for(int j=0; j<DIM; j++){
            height_map[i][j].first = 0;
        }
    }
    active_z = active_x = 0;
    m_view_rotation = 0;
    m_eye_distance = initial_distance;
}
