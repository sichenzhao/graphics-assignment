#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "grid.hpp"
#include <vector>

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:
	void initGrid();
        void resetGrid();
    void buildCube(float x, float y, float z);
    void buildIndicator(int x_active, int z_active);
    void drawCube(int x, int y, int z);


	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to cube geometry.
        GLuint m_cube_vao;
        GLuint m_cube_vbo;
        std::pair<int, glm::vec3> **height_map;
        int flash_helper;
        int active_z;
        int active_x;
        
        // Fields for indicator
        GLuint m_indi_vao;
        GLuint m_indi_vbo;

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float colour[8][3];
	int current_col;

        // mouse button listener
        bool m_mouseButtonActive;
        double m_mouseStartXPos;
        float m_view_rotation;
        float m_eye_distance;
};
