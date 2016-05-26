#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

// model coord colour
static vec3 m_x_colour = vec3(0.0f, 0.5f, 0.5f);
static vec3 m_y_colour = vec3(0.5f, 0.5f, 0.0f);
static vec3 m_z_colour = vec3(0.5f, 0.0f, 0.5f);

// world coord colour
static vec3 w_x_colour = vec3(0.0f, 0.0f, 1.0f);
static vec3 w_y_colour = vec3(0.0f, 1.0f, 0.0f);
static vec3 w_z_colour = vec3(1.0f, 0.0f, 0.0f);

// cube line colour
static vec3 cube_colour = vec3(1.0f, 0.7f, 0.8f);

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.reserve(kMaxVertices);
	colours.reserve(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{
        m_3dCube[0] = vec4(0.6f, 0.6f, 0.6f, 0.6f);
        m_3dCube[1] = vec4(0.6f, 0.6f, -0.6f, 0.6f);
        m_3dCube[2] = vec4(-0.6f, 0.6f, -0.6f, 0.6f);
        m_3dCube[3] = vec4(-0.6f, 0.6f, 0.6f, 0.6f);
        m_3dCube[4] = vec4(0.6f, -0.6f, 0.6f, 0.6f);
        m_3dCube[5] = vec4(0.6f, -0.6f, -0.6f, 0.6f);
        m_3dCube[6] = vec4(-0.6f, -0.6f, -0.6f, 0.6f);
        m_3dCube[7] = vec4(-0.6f, -0.6f, 0.6f, 0.6f);

        m_modelCoord[0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        m_modelCoord[1] = vec4(0.5f, 0.0f, 0.0f, 1.0f);
        m_modelCoord[2] = vec4(0.0f, 0.5f, 0.0f, 1.0f);
        m_modelCoord[3] = vec4(0.0f, 0.0f, 0.5f, 1.0f);
        
        m_worldCoord[0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        m_worldCoord[1] = vec4(0.7f, 0.0f, 0.0f, 1.0f);
        m_worldCoord[2] = vec4(0.0f, 0.7f, 0.0f, 1.0f);
        m_worldCoord[3] = vec4(0.0f, 0.0f, 0.7f, 1.0f);
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);


	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & v0,   // Line Start (NDC coordinate)
		const glm::vec2 & v1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = v0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = v1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();

    // Draw cube:
    drawCube();
    drawWorldCoord();

}

void A2::drawCube() {
    // rotate, scale, then transform m_3dCube
    setLineColour(cube_colour);
    for(int i=0; i<4; i++){
        drawLine_world(m_3dCube[i], m_3dCube[i+4]);
        if (i==3){
            drawLine_world(m_3dCube[i], m_3dCube[i-3]);
            drawLine_world(m_3dCube[i+4], m_3dCube[i+4-3]);
        }
        else {
            drawLine_world(m_3dCube[i], m_3dCube[i+1]);
            drawLine_world(m_3dCube[i+4], m_3dCube[i+1+4]);
        }
    }

    // draw model coordinator
    setLineColour(m_x_colour);
    drawLine_world(m_modelCoord[0], m_modelCoord[1]);
    setLineColour(m_y_colour);
    drawLine_world(m_modelCoord[0], m_modelCoord[1]);
    setLineColour(m_z_colour);
    drawLine_world(m_modelCoord[0], m_modelCoord[1]);
}

void A2::drawWorldCoord() {
    setLineColour(w_x_colour);
    drawLine_world(m_worldCoord[0], m_worldCoord[1]);
    setLineColour(w_y_colour);
    drawLine_world(m_worldCoord[0], m_worldCoord[2]);
    setLineColour(w_z_colour);
    drawLine_world(m_worldCoord[0], m_worldCoord[3]);
}

void A2::drawLine_world(glm::vec4 start, glm::vec4 end){
    // for now, no perspective, just draw x y
    // cout << "start " << start.x << start.y << endl;
    // cout << "end " << end.x << end.y << endl;
    drawLine(vec2(start.x, start.y), vec2(end.x, end.y));
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
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
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
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
        }

	return eventHandled;
}

void A2::resetGrid() {
    cout << "Reset Cube" << endl;
    return;
}
