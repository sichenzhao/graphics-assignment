#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;


#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

enum {
    key_I, // reset Position
    key_O, // reset Orientation
    key_N, // reset Joints

    key_U, // UNDO
    key_R, // REDO

    key_C, // draw Circle for trackball
    key_Z, // Z_buffer
    key_B, // backface culling
    key_F, // frontface culling

    key_P, // Position/Orientation
    key_J, // Joint
    key_M  // Hide all panels
};

enum {
    m_left, m_middle, m_right
};

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{
        memset(keyFlags, 0, sizeof(bool)*12);
        keyFlags[key_P] = true;
        memset(mouseFlags, 0, sizeof(bool)*3);
        m_pickingMode = false;

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
    // theta, aspect, near, far
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
    // eye, center, up
    m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
            //-- Set Perpsective matrix uniform for the scene:
            GLint location = m_shader.getUniformLocation("Perspective");
            glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
            CHECK_GL_ERRORS;


            //-- Set LightSource uniform for the scene:
            {
                location = m_shader.getUniformLocation("light.position");
                glUniform3fv(location, 1, value_ptr(m_light.position));
                location = m_shader.getUniformLocation("light.rgbIntensity");
                glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
                CHECK_GL_ERRORS;
            }

            //-- Set background light ambient intensity
            {
                location = m_shader.getUniformLocation("ambientIntensity");
                vec3 ambientIntensity(0.05f);
                glUniform3fv(location, 1, value_ptr(ambientIntensity));
                CHECK_GL_ERRORS;
            }
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...
    handleKMEvents();

    uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

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
        if( ImGui::BeginMenu("Application") ) {
            if(ImGui::Button("Reset Position")){
                resetPosition();
            }
            if(ImGui::Button("Reset Orientation")){
                resetOrientation();
            }
            if(ImGui::Button("Reset Joints")){
                resetJoint();
            }
            if(ImGui::Button("Reset ALL")){
                resetAll();
            }
            // Create Button, and check if it was clicked:
            if( ImGui::Button( "Quit Application" ) ) {
                glfwSetWindowShouldClose(m_window, GL_TRUE);
            }
            ImGui::EndMenu();
        }
        if( ImGui::BeginMenu("Edit") ) {
            if(ImGui::Button("Undo")){
                keyFlags[key_U] = !keyFlags[key_U];
            }
            if(ImGui::Button("Redo")){
                keyFlags[key_R] = !keyFlags[key_R];
            }
            ImGui::EndMenu();
        }
        if( ImGui::BeginMenu("Options") ) {
            ImGui::Checkbox("Circle", &keyFlags[key_C]);
            ImGui::Checkbox("Z-Buffer", &keyFlags[key_Z]);
            ImGui::Checkbox("Backface Culling", &keyFlags[key_B]);
            ImGui::Checkbox("Frontface Culling", &keyFlags[key_F]);
            ImGui::EndMenu();
        }

        ImGui::PushID(0);
        if (ImGui::RadioButton( "Position/Prientation", keyFlags[key_P] )) {
            keyFlags[key_P] = ! keyFlags[key_P];
            keyFlags[key_J] = false;
            j_clicked = false;
            m_pickedIDs.clear();
        }
        ImGui::PopID();

        ImGui::PushID(1);
        if (ImGui::RadioButton( "Joints", keyFlags[key_J] )) {
            keyFlags[key_J] = ! keyFlags[key_J];
            keyFlags[key_P] = false;
        }
        ImGui::PopID();


		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
void A3::updateShaderUniforms(
		const ShaderProgram & shader,
		GeometryNode & node,
		const glm::mat4 & viewMatrix,
        const glm::mat4 parentM
        //const glm::mat4 virtualM,
        //const glm::mat4 worldTM
) {
    // virtualM for virtual sphere

    bool imPicked = (m_pickedIDs.count(node.m_nodeId)>0);
	shader.enable();
	{
        GLint location;
        //-- Set ModelView matrix:
        location = shader.getUniformLocation("ModelView");
        // last <-- first transformation
        // node.trans has to be right most for scaling first
        mat4 modelView = viewMatrix * w_translate * parentM * m_model * node.trans;
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
        CHECK_GL_ERRORS;

        //-- Set NormMatrix:
        location = shader.getUniformLocation("NormalMatrix");
        mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
        glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
        CHECK_GL_ERRORS;


        if(!m_pickingMode){
            location = shader.getUniformLocation("picking");
            glUniform1f(location, false);
            //-- Set Material values:
            location = shader.getUniformLocation("material.kd");
            vec3 kd = node.material.kd;
            if(imPicked){
                kd = vec3 (0.0f, 1.0f, 1.0f);
            }
            glUniform3fv(location, 1, value_ptr(kd));
            CHECK_GL_ERRORS;
            location = shader.getUniformLocation("material.ks");
            vec3 ks = node.material.ks;
            glUniform3fv(location, 1, value_ptr(ks));
            CHECK_GL_ERRORS;
            location = shader.getUniformLocation("material.shininess");
            glUniform1f(location, node.material.shininess);
            CHECK_GL_ERRORS;
        } else {
            int r = (node.m_nodeId & 0x000000FF) >>  0;
            int g = (node.m_nodeId & 0x0000FF00) >>  8;
            int b = (node.m_nodeId & 0x00FF0000) >> 16;

            //cout << "(r,g,b) = " << r << " " << g << " " << b << endl;

            location = shader.getUniformLocation("picking");
            glUniform1f(location, true);
            location = shader.getUniformLocation("colour");
            glUniform3f(location, r/255.0f, g/255.0f, b/255.0f);
            CHECK_GL_ERRORS;
        }

	}
	shader.disable();
	CHECK_GL_ERRORS;

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

    if (m_pickingMode) {
        pickingHelper();
    }

    if(keyFlags[key_Z]){
        glEnable( GL_DEPTH_TEST );
    }
    if(keyFlags[key_B]){
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }
    if(keyFlags[key_F]){
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    if(keyFlags[key_F] && keyFlags[key_B]){
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT_AND_BACK);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(m_vao_meshData);
	renderSceneGraph(*m_rootNode, m_rootNode->trans);
	glBindVertexArray(0);

    if(keyFlags[key_Z]){
        glDisable( GL_DEPTH_TEST );
    }

    if(keyFlags[key_C] || (keyFlags[key_P] && mouseFlags[m_right])){
        renderArcCircle();
    }

    jointAngle = 0;
    jointAngle_y = 0;
}

void A3::pickingHelper() {
    // clear screen
    //float defaultClear = 0.0f;
    //glClearColor(defaultClear, defaultClear, defaultClear, defaultClear);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glUseProgram(pickingProgramID); // use a different shader

    // Only the positions are needed (not the UVs and normals)
    //glEnableVertexAttribArray(0);

    glEnable( GL_DEPTH_TEST );

    glBindVertexArray(m_vao_meshData);
    renderSceneGraph(*m_rootNode, m_rootNode->trans);
    glBindVertexArray(0);

    glDisable( GL_DEPTH_TEST );

    //cout << "picking" << endl;

    unsigned char data[4];
    //cout << m_xPos << " " << m_windowHeight - m_yPos << endl;
    glReadPixels(m_xPos, m_windowHeight-m_yPos,1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    //cout << (int)data[0] << (int)data[1] << (int)data[2] << endl;  
    int pickedID = data[0] + data[1] * 256 + data[2] * 256*256;
    if(m_pickedIDs.count(pickedID)>0){
        m_pickedIDs.erase(pickedID);
        cout << pickedID << "got unpicked" << endl;
    } else {
        m_pickedIDs.insert(pickedID);
        cout << pickedID << "got picked" << endl;
    }

    m_pickingMode = false;
}

/**
* Get a normalized vector from the center of the virtual ball O to a
* point P on the virtual ball surface, such that P is aligned on
* screen's (X,Y) coordinates.  If (X,Y) is too far away from the
* sphere, return the nearest point on the virtual ball surface.
*/
glm::vec3 A3::get_arcball_vector(int x, int y) {
    glm::vec3 P = glm::vec3(1.0*x/m_windowWidth*2 - 1.0, 1.0*y/m_windowHeight*2 - 1.0, 0);
    P.y = -P.y;
    float OP_squared = P.x * P.x + P.y * P.y;
    if (OP_squared <= 1*1) {
        P.z = sqrt(1*1 - OP_squared);  // Pythagore
    } else {
        P = glm::normalize(P);  // nearest point
    }
    return P;
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(SceneNode & root, glm::mat4 parentM) {
    if(root.children.empty()) {
        return;
    }

    const mat4 rootTransM = root.get_transform();

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	//glBindVertexArray(m_vao_meshData);

	// This is emphatically *NOT* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

    for (SceneNode * node : root.children) {


        if (node->m_nodeType != NodeType::GeometryNode) {
            renderSceneGraph(*node, parentM * node->tr);
            continue;
        }

        GeometryNode * geometryNode = static_cast<GeometryNode *>(node);
        geometryNode->parent = &root;


        if(root.m_nodeType != NodeType::JointNode){
            m_pickedIDs.erase(node->m_nodeId);
        } else {
            // root is joint, rotate
            JointNode* jointNode = (static_cast<JointNode *>(&root));
            if(m_jointNodes.count(jointNode)<=0){
                m_jointNodes.insert(jointNode);
                cout << "insert " <<  jointNode->m_name << endl;
            }
            if(m_pickedIDs.count(geometryNode->m_nodeId)>0){
                float angle_x = jointAngle + jointNode->get_xAngle();
                float angle_y = jointAngle_y + jointNode->get_yAngle();
                jointNode->set_xAngle(angle_x);
                jointNode->set_yAngle(angle_y);
            }
            parentM = rotate(parentM, glm::radians(jointNode->get_xAngle()), glm::vec3(1.0f, 0.0f, 0.0f));
            parentM = rotate(parentM, glm::radians(jointNode->get_yAngle()), glm::vec3(0.0f, 1.0f, 0.0f));
        }

		updateShaderUniforms(m_shader, *geometryNode, m_view, parentM);
        // cout << "prep render " << geometryNode->m_name << "'s mesh " << geometryNode->meshId << endl;

		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
        //cout << batchInfo.startIndex << "~" << batchInfo.numIndices << endl;
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
        //cout << "finish render " << geometryNode->m_name << "'s mesh " << geometryNode->meshId << endl;

        renderSceneGraph(*geometryNode, parentM * node->tr);
	}

	//glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
    if(!m_pickingMode){
        glBindVertexArray(m_vao_arcCircle);

        m_shader_arcCircle.enable();
        GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
        float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
        glm::mat4 M;
        if( aspect > 1.0 ) {
            M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
        } else {
            M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
        }
        glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
        glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
        m_shader_arcCircle.disable();

        glBindVertexArray(0);
        CHECK_GL_ERRORS;
    }
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
    m_xPos = xPos;
    m_yPos = yPos;
    eventHandled = true;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
void A3::handleKMEvents() {
    double delta_x = m_xPos - m_px;
    double delta_y = m_yPos - m_py;

    double delta_xp = delta_x / m_windowWidth;
    double delta_yp = delta_y / m_windowHeight;

    if (mouseFlags[m_left]){
        // translate on x and y axis
        if(keyFlags[key_P]){
            cout << delta_x << " " << delta_y << endl;
            w_translate = translate(w_translate, glm::vec3(delta_xp, -delta_yp, 0.0f));
        }

        // picking
        if(keyFlags[key_J] && !j_clicked){
            j_clicked = true;
            m_pickingMode = true;
        }
    }
    if (mouseFlags[m_middle]){
        // translate on z axis
        if(keyFlags[key_P]){
            w_translate = translate(w_translate, glm::vec3(0.0f, 0.0f, delta_yp));
        }
        if(keyFlags[key_J]){
            jointAngle = delta_yp * 180.0f;
        }
    }
    if (mouseFlags[m_right]){
        // virtual trackball handler
        if(keyFlags[key_P]){
            /**
            vec3 S = get_arcball_vector(m_px, m_py);
            vec3 T = get_arcball_vector(m_xPos, m_yPos);
            float angle = acos(std::min(1.0f, glm::dot(S, T)));
            glm::vec3 axis_in_camera_coord = glm::cross(S, T);
            axis_in_camera_coord = normalize(axis_in_camera_coord);
            vec4 axisInWorldFrame = vec4(axis_in_camera_coord, 0.0f) * inverse(m_view);
            m_rootNode->rotate({axisInWorldFrame.x, axisInWorldFrame.y,  axisInWorldFrame.z}, degrees(angle));
            **/
        }
        if(keyFlags[key_J]){
            jointAngle_y = delta_xp * 180.0f;
        }
    }
    // keeps track of previous x, y positions
    m_px = m_xPos;
    m_py = m_yPos;
    return;
}

bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
                if (button == GLFW_MOUSE_BUTTON_LEFT){
                    if (actions == GLFW_PRESS) {
                            mouseFlags[m_left] = true;
                            //cout << "left mouse pressed" << endl;
                    }

                    if (actions == GLFW_RELEASE) {
                            mouseFlags[m_left] = false;
                            cout << "left mouse released" << endl;
                            j_clicked = false;
                    }
                }
                if (button == GLFW_MOUSE_BUTTON_RIGHT){
                    if (actions == GLFW_PRESS) {
                            mouseFlags[m_right] = true;
                            cout << "right mouse pressed" << endl;
                    }

                    if (actions == GLFW_RELEASE) {
                            mouseFlags[m_right] = false;
                            cout << "right mouse released" << endl;
                    }
                }
                if (button == GLFW_MOUSE_BUTTON_MIDDLE){
                    if (actions == GLFW_PRESS) {
                            mouseFlags[m_middle] = true;
                            cout << "middle mouse pressed" << endl;
                    }

                    if (actions == GLFW_RELEASE) {
                            mouseFlags[m_middle] = false;
                            cout << "middle mouse released" << endl;
                    }
                }
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
            cout << "M key pressed" << endl;
			show_gui = !show_gui;
			eventHandled = true;
        }
        if (key == GLFW_KEY_Q) {
            cout << "Q key pressed" << endl;
            // quit
            glfwSetWindowShouldClose(m_window, GL_TRUE);
            eventHandled = true;
        }
        if (key == GLFW_KEY_P) {
            cout << "P key pressed" << endl;
            // Position/Orientation
            keyFlags[key_P] = true;
            eventHandled = true;
        }
        if (key == GLFW_KEY_J) {
            // Joint
            keyFlags[key_J] = true;
            eventHandled = true;
        }
        if (key == GLFW_KEY_C) {
            // Circle
            keyFlags[key_C] = true;
            eventHandled = true;
        }
        if (key == GLFW_KEY_Z) {
            // Z buffer
            keyFlags[key_Z] = true;
            eventHandled = true;
        }
        if (key == GLFW_KEY_B) {
            // Backface culling
            keyFlags[key_B] = true;
            eventHandled = true;
        }
        if (key == GLFW_KEY_F) {
            // Frontface culling
            keyFlags[key_F] = true;
            eventHandled = true;
        }
        if (key == GLFW_KEY_I) {
            resetPosition();
            eventHandled = true;
        }
        if (key == GLFW_KEY_O) {
            resetOrientation();
            eventHandled = true;
        }
        if (key == GLFW_KEY_N) {
            resetJoint();
            eventHandled = true;
        }
        if (key == GLFW_KEY_A){
            resetAll();
            eventHandled = true;
        }
    }
	// Fill in with event handling code...
    if ( action == GLFW_RELEASE ) {
        if (key == GLFW_KEY_P) {
            cout << "P key released" << endl;
            keyFlags[key_P] = false;
            eventHandled = true;
        }
        if (key == GLFW_KEY_J) {
            // Joint
            keyFlags[key_J] = false;
            eventHandled = true;
            j_clicked = false;
            m_pickedIDs.clear();
        }
        if (key == GLFW_KEY_C) {
            // Circle
            keyFlags[key_C] = false;
            eventHandled = true;
        }
        if (key == GLFW_KEY_Z) {
            // Z buffer
            keyFlags[key_Z] = false;
            eventHandled = true;
        }
        if (key == GLFW_KEY_B) {
            // Backface culling
            keyFlags[key_B] = false;
            eventHandled = true;
        }
        if (key == GLFW_KEY_F) {
            // Frontface culling
            keyFlags[key_F] = false;
            eventHandled = true;
        }
    }

	return eventHandled;
}

void A3::resetAll(){
    resetPosition();
    resetOrientation();
    resetJoint();
    cout << "reset All" << endl;
    return;
}

void A3::resetPosition(){
    w_translate = mat4(1.0f);
}

void A3::resetOrientation(){
}

void A3::resetJoint(){
    for(auto it = m_jointNodes.begin(); it != m_jointNodes.end(); it++){
        (*it)->set_xAngle((*it)->m_joint_x.init);
        (*it)->set_yAngle((*it)->m_joint_y.init);
        cout << "reset " <<  (*it)->m_name << endl;
    }
}
