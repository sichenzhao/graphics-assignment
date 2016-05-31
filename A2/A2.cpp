#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

static double eps = 1e-4;

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

enum {
    key_O, // rotate view
    key_N, // translate view
    key_P, // perspective
    key_R, // rotate model
    key_T, // translate model
    key_S, // scale model
    key_V  // viewport
};

enum {
    m_left, m_middle, m_right
};

enum {
    x, y, z, w
};

static double p_theta = 30*M_PI/180;
static double p_z = 1;


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{
        memset(keyFlags, 0, sizeof(bool)*7);
        memset(mouseFlags, 0, sizeof(bool)*3);
        memset(mousePosStarts, 0, sizeof(mousePosStarts[0][0])*(3*6+1)*2);

        m_width = 768;
        m_height = 674 ;

        m_near = 1;
        m_far = 5;

        m_viewport[0] = -0.9; 
        m_viewport[1] = 0.9; 
        m_viewport[2] = -0.9; 
        m_viewport[3] = 0.9; 

        m_MMatrix = mat4(1.0f);
        m_SMatrix = mat4(1.0f);
        m_MCoordMatrix = mat4(1.0f);

        m_VMatrix = mat4(1.0f);
        m_VMatrix[3].z = 2; // eye distance
        m_VMatrix[2].z = -1;
        m_VMatrix = inverse(m_VMatrix);

        m_pTheta = 30 * M_PI / 180;
        m_PMatrix = mat4(1.0f);
        m_PMatrix[0].x = 1 / tan(m_pTheta/2);
        m_PMatrix[1].y = 1 / tan(m_pTheta/2);
        m_PMatrix[2].z = (m_far + m_near) / (m_far - m_near);
        m_PMatrix[3].z = -2*m_far*m_near / (m_far - m_near);
        m_PMatrix[2].w = 1;

        m_3dCube[0] = vec4(0.6f, 0.6f, 0.6f, 1.0f);
        m_3dCube[1] = vec4(0.6f, 0.6f, -0.6f, 1.0f);
        m_3dCube[2] = vec4(-0.6f, 0.6f, -0.6f, 1.0f);
        m_3dCube[3] = vec4(-0.6f, 0.6f, 0.6f, 1.0f);
        m_3dCube[4] = vec4(0.6f, -0.6f, 0.6f, 1.0f);
        m_3dCube[5] = vec4(0.6f, -0.6f, -0.6f, 1.0f);
        m_3dCube[6] = vec4(-0.6f, -0.6f, -0.6f, 1.0f);
        m_3dCube[7] = vec4(-0.6f, -0.6f, 0.6f, 1.0f);

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
        updateMMatrix();

        if(keyFlags[key_V] && mouseFlags[0]){
            double* V_startX = & mousePosStarts[18][0];
            double* V_startY = & mousePosStarts[18][1];
            double rel_xPos = (m_xPos/m_width)*2 -1;
            double rel_yPos = 1-(m_yPos/m_height)*2;
            if(*V_startX==0 && *V_startY==0){
                cout << "V00" << endl;
                m_viewport[0] = *V_startX = rel_xPos;
                m_viewport[3] = *V_startY = rel_yPos;
            } else {
                m_viewport[0] = std::min(*V_startX, rel_xPos);
                m_viewport[2] = std::min(*V_startY, rel_yPos);
                m_viewport[1] = std::max(rel_xPos, *V_startX);
                m_viewport[3] = std::max(rel_yPos, *V_startY);
            }
        }

	// Call at the beginning of frame, before drawing lines:
	initLineData();

    // Draw cube:
    drawCube();
    drawWorldCoord();

    // Draw viewport
    drawLine(vec2(m_viewport[0],m_viewport[2]), vec2(m_viewport[0],m_viewport[3]));
    drawLine(vec2(m_viewport[0],m_viewport[2]), vec2(m_viewport[1],m_viewport[2]));
    drawLine(vec2(m_viewport[1],m_viewport[2]), vec2(m_viewport[1],m_viewport[3]));
    drawLine(vec2(m_viewport[0],m_viewport[3]), vec2(m_viewport[1],m_viewport[3]));
}

void A2::drawCube() {
    // rotate, scale, then transform m_3dCube
    vec4 tmp_3dCube[8];
    for (int i=0; i<8; i++){
        tmp_3dCube[i] = m_VMatrix * m_MMatrix * m_SMatrix * m_3dCube[i];
    }
    setLineColour(cube_colour);
    cubeClip(tmp_3dCube);
    for(int i=0; i<4; i++){
        drawLine_world(tmp_3dCube[i], tmp_3dCube[i+4]);
        if (i==3){
            drawLine_world(tmp_3dCube[i], tmp_3dCube[i-3]);
            drawLine_world(tmp_3dCube[i+4], tmp_3dCube[i+4-3]);
        }
        else {
            drawLine_world(tmp_3dCube[i], tmp_3dCube[i+1]);
            drawLine_world(tmp_3dCube[i+4], tmp_3dCube[i+1+4]);
        }
    }

    vec4 tmp_modelCoord[4];
    for (int i=0; i<4; i++){
        tmp_modelCoord[i] = m_VMatrix * m_MCoordMatrix * m_modelCoord[i];
    }
    // draw model coordinator
    setLineColour(m_x_colour);
    drawLine_world(tmp_modelCoord[0], tmp_modelCoord[1]);
    setLineColour(m_y_colour);
    drawLine_world(tmp_modelCoord[0], tmp_modelCoord[2]);
    setLineColour(m_z_colour);
    drawLine_world(tmp_modelCoord[0], tmp_modelCoord[3]);
}

void A2::cubeClip(vec4 *cube){
    vec4 eye1 = -cube[0] + vec4(0,0,0,1);
    vec4 eye2 = -cube[6] + vec4(0,0,0,1);

    //cout << cube[0].x << " " << cube[0].y << " " << cube[0].z << " " << endl;

    bool planeShow[6];
    planeShow[0] = (dot(eye1,cube[0]-cube[4])>0);
    planeShow[1] = (dot(eye1,cube[0]-cube[3])>0);
    planeShow[2] = (dot(eye1,cube[0]-cube[1])>0);
    planeShow[3] = (dot(eye2,cube[6]-cube[7])>0);
    planeShow[4] = (dot(eye2,cube[6]-cube[5])>0);
    planeShow[5] = (dot(eye2,cube[6]-cube[2])>0);

    return;
}

void A2::drawWorldCoord() {
    vec4 tmp_worldCoord[4];
    for (int i=0; i<4; i++){
        tmp_worldCoord[i] = m_VMatrix * m_worldCoord[i];
    }
    setLineColour(w_x_colour);
    drawLine_world(tmp_worldCoord[0], tmp_worldCoord[1]);
    setLineColour(w_y_colour);
    drawLine_world(tmp_worldCoord[0], tmp_worldCoord[2]);
    setLineColour(w_z_colour);
    drawLine_world(tmp_worldCoord[0], tmp_worldCoord[3]);
}

bool A2::viewportClipping(float &sx, float &sy, float &ex, float &ey){
    int oA = 0;
    int oB = 0;

    double xl = m_viewport[0];
    double xr = m_viewport[1];
    double yt = m_viewport[2];
    double yb = m_viewport[3];
    // r of r
    // l of l
    // b of b
    // above of top
    oA |= (sx>xr)?0:1;
    oA |= (sx<xl)?0:(1<<1);
    oA |= (sy<yt)?0:1<<2;
    oA |= (sy>yb)?0:1<<3;

    oB |= (ex>xr)?0:1;
    oB |= (ex<xl)?0:(1<<1);
    oB |= (ey<yt)?0:1<<2;
    oB |= (ey>yb)?0:1<<3;

    if((oA|oB)!=15) return false;
    if((oA&oB)==15) return true;
    else {
        // cout << oA << " " << oB << endl;
        oA = ~oA;
        oB = ~oB;
        double t = 0;
        if(oA & 1){
            //cout << sx << "sichen "<< sy << endl;
            if(ex==sx) return false;
            t = (xr - sx)/(ex - sx);
            sy = sy + t*(ey - sy);
            sx = xr-eps; 
        }
        else if(oA & 1<<1){
            //cout << sx << "sichen "<< sy << endl;
            if(ex==sx) return false;
            t = (xl - sx)/(ex - sx);
            sy = sy + t*(ey - sy);
            sx = xl+eps; 
        }
        else if(oA & 1<<2){
            if(ey==sy) return false;
            t = (yt - sy)/(ey - sy);
            sx = sx + t*(ex - sx);
            sy = yt+eps;
        }
        else if(oA & 1<<3){
            if(ey==sy) return false;
            t = (yb - sy)/(ey - sy);
            sx = sx + t*(ex - sx);
            sy = yb-eps;
        }
        else if(oB & 1){
            if(ex==sx) return false;
            t = (xr - ex)/(sx - ex);
            ey = ey + t*(sy - ey);
            ex = xr-eps; 
        }
        else if(oB & 1<<1){
            if(ex==sx) return false;
            t = (xl - ex)/(sx - ex);
            ey = ey + t*(sy - ey);
            ex = xl+eps; 
        }
        else if(oB & 1<<2){
            if(ey==sy) return false;
            t = (yt - ey)/(sy - ey);
            ex = ex + t*(sx - ex);
            ey = yt+eps;
        }
        else if(oB & 1<<3){
            if(ey==sy) return false;
            t = (yb - ey)/(sy - ey);
            ex = ex + t*(sx - ex);
            ey = yb-eps;
        }
        return viewportClipping(sx,sy,ex,ey);
    }
}

void A2::mapViewport(float &x, float &y){
    double xl,xr,yt,yb;
    xl = m_viewport[0];
    xr = m_viewport[1];
    yt = m_viewport[2];
    yb = m_viewport[3];
    x = (x+1)/2*(xr-xl) + xl;
    y = (y+1)/2*(yb-yt) + yt;
    return;
}

void A2::drawLine_world(glm::vec4 start, glm::vec4 end){
    // cout << "start " << start.x << start.y << endl;
    // cout << "end " << end.x << end.y << endl;
    // 3d clipping
    if(start.w == -1 || end.w == -1) return;

    // map viewport
    mapViewport(start.x, start.y);
    mapViewport(end.x, end.y);

    // TODO: perspective
    float sx = start.x / start.z * m_near;
    float sy = start.y / start.z * m_near;
    float ex = end.x / end.z * m_near;
    float ey = end.y / end.z * m_near;

    if(viewportClipping(sx, sy, ex, ey)){
        if(nfClipping(sx, sy, start.z, ex, ey, end.z)){
            drawLine(vec2(sx, sy), vec2(ex, ey));
        }
    }

    /**
    // TODO: perspective
    start = m_PMatrix*start;
    end = m_PMatrix*end;
    if(viewportClipping(start.x, start.y, end.x, end.y)){
        if(nfClipping(start.x, start.y, start.z, end.x, end.y, end.z)){
            drawLine(vec2(start.x, start.y), vec2(end.x, end.y));
        }
    }
    **/
}

bool A2::nfClipping(float &sx, float &sy, float &sz, float &ex, float &ey, float &ez){
    if(sz<m_near && ez<m_near) return false;
    if(sz>m_far && ez>m_far) return false;
    double t = 0;
    if(sz<m_near){
        t = (m_near - sz)/(ez - sz);
        sy = sy + t*(ey - sy);
        sx = sx + t*(ex - sx);
        sz = m_near+eps;
        return nfClipping(sx,sy,sz,ex,ey,ez);
    } else if(sz>m_far){
        t = (m_far - sz)/(ez - sz);
        sy = sy + t*(ey - sy);
        sx = sx + t*(ex - sx);
        sz = m_far-eps;
    } else if(ez<m_near){
        t = (m_near - ez)/(sz - ez);
        ey = ey + t*(sy - ey);
        ex = ex + t*(sx - ex);
        ez = m_near + eps;
    } else if(ez>m_far){
        t = (m_far - ez)/(sz - ez);
        ey = ey + t*(sy - ey);
        ex = ex + t*(sx - ex);
        ez = m_far - eps;
    }
    return true;
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


        // TODO: Add more gui elements here here ...



        if( ImGui::Button( "Reset" ) ) {
            resetGrid();
        }

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
 * Called once per frame, update MVP matrices respectively.
 */
// index of key mouse for mousePosStarts
int iKM(const int key, const int mouse) {
    return (key==6) ? 18 : key*3 + mouse;
}

void A2::resetK(const int key) {
    for(int i=0; i<3; i++){
        mousePosStarts[iKM(key, i)][0] = 0;
        mousePosStarts[iKM(key, i)][1] = 0;
    }
}

void A2::resetM(const int mouse) {
    for(int i=0; i<7; i++){
        mousePosStarts[iKM(i, mouse)][0] = 0;
        mousePosStarts[iKM(i, mouse)][1] = 0;
    }
}

// construct Matrix for model given axis (x,y,z) and action (R,T,S)
mat4 A2::consM (int axis, int action, double val){
    mat4 ret( 1.0f ); // identity matrix
    val = val / 1000;
    int transAlter = 2;
    if(axis == x){
        if (action==key_R || action==key_O){
            ret[1].y = cos(val);
            ret[1].z = sin(val);
            ret[2].y = -sin(val);
            ret[2].z = cos(val);
            return ret;
        } else if (action==key_T || action==key_N){
            val = val*transAlter;
            ret[3].x = val;
            return ret;
        } else if (action==key_S){
            ret[0].x = (val+1==0?1:val+1);
            return ret;
        }
    } else if (axis == y) {
        if (action==key_R || action==key_O){
            ret[0].x = cos(val);
            ret[0].z = -sin(val);
            ret[2].x = sin(val);
            ret[2].z = cos(val);
            return ret;
        } else if (action==key_T || action==key_N){
            val = val*transAlter;
            ret[3].y = val;
            return ret;
        } else if (action==key_S){
            ret[1].y = (val==-1?1:val+1);
            return ret;
        }
    } else if (axis == z) {
        if (action==key_R || action==key_O){
            ret[0].x = cos(val);
            ret[0].y = sin(val);
            ret[1].x = -sin(val);
            ret[1].y = cos(val);
            return ret;
        } else if (action==key_T || action==key_N){
            val = val*transAlter;
            ret[3].z = val;
            return ret;
        } else if (action==key_S){
            ret[2].z = (val+1==0?1:val+1);
            return ret;
        }

    } else {
        cout << "error consM getting axis " << axis << endl;
    }
    return ret;
}

void A2::updateMMatrixHelper(int key, int mouse) {
    //cout << "updateMMatrixHelper key, mouse" << key << ", " << mouse << endl;
    double* startsX = & mousePosStarts[iKM(key, mouse)][0];
    double* startsY = & mousePosStarts[iKM(key, mouse)][1];
    if(*startsX==0 && *startsY==0){
        *startsX = m_xPos;
        *startsY = m_yPos;
    } else {
        if(key==key_S){
            double theta = (m_xPos - *startsX);
            mat4 transformM = consM(mouse, key, theta);
            m_SMatrix = m_SMatrix*transformM;
        }else if(key==key_S||key==key_T||key==key_R){
            double theta = (m_xPos - *startsX);
            mat4 transformM = consM(mouse, key, theta);
            m_MMatrix = m_MMatrix*transformM;
            m_MCoordMatrix = (key==key_S) ? m_MCoordMatrix : m_MCoordMatrix*transformM;
        } else if(key==key_O || key==key_N) {
            double theta = (m_xPos - *startsX);
            mat4 transformM = consM(mouse, key, theta);
            //m_VMatrix = m_VMatrix*transformM;
            m_VMatrix = inverse(transformM)*m_VMatrix;
        }
        *startsX = m_xPos;
    }
}

void A2::updateMMatrix() {
    // x axis
    if (mouseFlags[m_left]) {
        // model rotate
        if(keyFlags[key_R]){
            updateMMatrixHelper(key_R, m_left);
        }
        // model translate
        if(keyFlags[key_T]){
            updateMMatrixHelper(key_T, m_left);
        }
        // model scale
        if(keyFlags[key_S]){
            updateMMatrixHelper(key_S, m_left);
        }
        if(keyFlags[key_N]){
            updateMMatrixHelper(key_N, m_left);
        }
        if(keyFlags[key_O]){
            updateMMatrixHelper(key_O, m_left);
        }
    }
    // y axis
    if (mouseFlags[m_middle]) {
        if(keyFlags[key_R]){
            updateMMatrixHelper(key_R, m_middle);
        }
        // model translate
        if(keyFlags[key_T]){
            updateMMatrixHelper(key_T, m_middle);
        }
        // model scale
        if(keyFlags[key_S]){
            updateMMatrixHelper(key_S, m_middle);
        }
        if(keyFlags[key_N]){
            updateMMatrixHelper(key_N, m_middle);
        }
        if(keyFlags[key_O]){
            updateMMatrixHelper(key_O, m_middle);
        }
    }
    // z axis
    if (mouseFlags[m_right]) {
        if(keyFlags[key_R]){
            updateMMatrixHelper(key_R, m_right);
        }
        // model translate
        if(keyFlags[key_T]){
            updateMMatrixHelper(key_T, m_right);
        }
        // model scale
        if(keyFlags[key_S]){
            updateMMatrixHelper(key_S, m_right);
        }
        if(keyFlags[key_N]){
            updateMMatrixHelper(key_N, m_right);
        }
        if(keyFlags[key_O]){
            updateMMatrixHelper(key_O, m_right);
        }
    }
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
	if (!ImGui::IsMouseHoveringAnyWindow()) {
            m_xPos = xPos;
            m_yPos = yPos;
            cout << "x y is " << xPos << " " << yPos << endl;
        }

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
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
                if (button == GLFW_MOUSE_BUTTON_LEFT){
                    if (actions == GLFW_PRESS) {
                            mouseFlags[m_left] = true;
                            cout << "left mouse pressed" << endl;
                    }

                    if (actions == GLFW_RELEASE) {
                            mouseFlags[m_left] = false;
                            resetM(m_left);
                            cout << "left mouse released" << endl;
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
                            resetM(m_right);
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
                            resetM(m_middle);
                    }
                }
	}

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
        cout << height << "<-h w->" << width << endl;

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

                // model rotate
                keyFlags[key_R] = true;

                eventHandled = true;
            }
            if (key == GLFW_KEY_T) {
                cout << "T key pressed" << endl;

                // model translate
                keyFlags[key_T] = true;

                eventHandled = true;
            }
            if (key == GLFW_KEY_S) {
                cout << "S key pressed" << endl;

                // model scale
                keyFlags[key_S] = true;

                eventHandled = true;
            }
            if (key == GLFW_KEY_V) {
                cout << "V key pressed" << endl;

                // viewport
                keyFlags[key_V] = true;

                eventHandled = true;
            }
            if (key == GLFW_KEY_O) {
                cout << "O key pressed" << endl;

                // viewport
                keyFlags[key_O] = true;

                eventHandled = true;
            }
            if (key == GLFW_KEY_N) {
                cout << "N key pressed" << endl;

                // viewport
                keyFlags[key_N] = true;

                eventHandled = true;
            }
            if (key == GLFW_KEY_P) {
                cout << "P key pressed" << endl;

                // viewport
                keyFlags[key_P] = true;

                eventHandled = true;
            }
            if (key == GLFW_KEY_Q) {
                cout << "Q key pressed" << endl;

                // quit
                glfwSetWindowShouldClose(m_window, GL_TRUE);

                eventHandled = true;
            }
            if (key == GLFW_KEY_A) {
                cout << "A key pressed" << endl;

                // reset
                resetGrid();

                eventHandled = true;
            }
        }
	if( action == GLFW_RELEASE ) {
            if (key == GLFW_KEY_R) {
                keyFlags[key_R] = false;
                cout << "R key released" << endl;
                resetK(key_R);
                eventHandled = true;
            }
            if (key == GLFW_KEY_T) {
                keyFlags[key_T] = false;
                cout << "T key released" << endl;
                resetK(key_T);
                eventHandled = true;
            }
            if (key == GLFW_KEY_S) {
                keyFlags[key_S] = false;
                cout << "S key released" << endl;
                resetK(key_S);
                eventHandled = true;
            }
            if (key == GLFW_KEY_V) {
                keyFlags[key_V] = false;
                cout << "V key released" << endl;
                resetK(key_V);
                eventHandled = true;
            }
            if (key == GLFW_KEY_O) {
                keyFlags[key_O] = false;
                cout << "O key released" << endl;
                resetK(key_O);
                eventHandled = true;
            }
            if (key == GLFW_KEY_N) {
                keyFlags[key_N] = false;
                cout << "N key released" << endl;
                resetK(key_N);
                eventHandled = true;
            }
            if (key == GLFW_KEY_P) {
                keyFlags[key_P] = false;
                cout << "P key released" << endl;
                resetK(key_P);
                eventHandled = true;
            }
        }

	return eventHandled;
}

void A2::resetGrid() {
    cout << "Reset Cube" << endl;

    memset(keyFlags, 0, sizeof(bool)*7);
    memset(mouseFlags, 0, sizeof(bool)*3);
    memset(mousePosStarts, 0, sizeof(mousePosStarts[0][0])*(3*6+1)*2);

    m_width = 768;
    m_height = 674 ;

    m_near = 1;
    m_far = 5;

    m_viewport[0] = -0.9; 
    m_viewport[1] = 0.9; 
    m_viewport[2] = -0.9; 
    m_viewport[3] = 0.9; 

    m_MMatrix = mat4(1.0f);
    m_SMatrix = mat4(1.0f);
    m_MCoordMatrix = mat4(1.0f);

    m_VMatrix = mat4(1.0f);
    m_VMatrix[3].z = 2; // eye distance
    m_VMatrix[2].z = -1;
    m_VMatrix = inverse(m_VMatrix);

    return;
}
