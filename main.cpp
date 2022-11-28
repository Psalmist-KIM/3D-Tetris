#include <stdio.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LoadShaders.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "primi.h"
#include "camera.h"
#include "tetrix.h"

CubePrimitive cube(1.0f, 1.0f, 1.0f);
CylinderPrimitive cylinder(0.5f, 1.0f, 30);
Grid grid(5, 5, 5, 5);
StackModel stack_grid(&grid, &cube);
Block block(&cube, 'L');
std::vector<Model*> models;

StackManager stack_manager(&cube);
BlockManager block_manager(&block, &stack_manager);
GameManager game_manager(&stack_manager, &block_manager);

GLuint program;
int button_pressed[3] = { GLUT_UP, GLUT_UP, GLUT_UP };
int mouse_pos[2] = { 0, 0 };

bool show_axis = false;

bool show_wireframe = true;

Camera camera;

void set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f)
{
	using namespace glm;
	glUniform1i(2, 1);
	glUniform4fv(3, 1, value_ptr(vec4(r, g, b, a)));
}

void cb_main_menu(int value)
{
	printf("cb_main_menu >> value: %d\n", value);
	switch (value)
	{
	case 0:
		camera.projection_mode = Camera::ORTHOGRAPHIC;
		break;

	case 1:
		camera.projection_mode = Camera::PERSPECTIVE;
		break;
	}
	glutPostRedisplay();
}

void init() {
	int main_menu_id = glutCreateMenu(cb_main_menu);
	glutAddMenuEntry("Orthographic projection", 0);
	glutAddMenuEntry("Perspective projection", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	models.push_back(&cube);
	models.push_back(&cylinder);
	models.push_back(&block);
	models.push_back(&grid);
	models.push_back(&stack_grid);

	ShaderInfo shaders[]{
		{ GL_VERTEX_SHADER, "term.vert" },
		{ GL_FRAGMENT_SHADER, "term.frag" },
		{ GL_NONE, NULL }
	};

	program = LoadShaders(shaders);
	glUseProgram(program);

	int num_of_models = (int)models.size();
	for (int i = 0; i < num_of_models; ++i) {
		models[i]->init(program);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void draw_cylinder(glm::mat4& T_in, GLfloat radius, GLfloat height, GLfloat dx = 0, GLfloat dy = 0, GLfloat dz = 0, glm::mat4* R0 = NULL)
{
	using namespace glm;
	mat4 T = T_in;
	T = translate(T, vec3(dx, dy, dz));
	if (R0) T *= (*R0);
	T = scale(T, vec3(2 * radius, height, 2 * radius));
	GLint location = glGetUniformLocation(program, "M");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(T));
	glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(mat4(1.0f)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	cylinder.draw();
}

void draw_coordinate_system()
{
	using namespace glm;
	mat4 T(1.0f);
	T = translate(T, vec3(0, -10.5f, 0));
	set_color(0, 1, 0);
	draw_cylinder(T, 0.05f, 1.0f, 0, 0.5f, 0);

	set_color(0, 0, 1);
	draw_cylinder(T, 0.05f, 1.0f, 0, 0, 0.5f, &rotate(mat4(1.0f), radians(90.f), vec3(1, 0, 0)));

	set_color(1, 0, 0);
	draw_cylinder(T, 0.05f, 1.0f, 0.5f, 0, 0, &rotate(mat4(1.0f), radians(90.f), vec3(0, 0, 1)));
}

void display()
{
	using namespace glm;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1, 1);
	glUniform1i(2, 0);
	
	GLint location;	
	// modeling transformation matrix
	mat4 T;
	glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(T));

	// modeling transformation matrix
	mat4 M(1.f);
	M = block_manager.get_T() * block_manager.get_R();
	location = glGetUniformLocation(program, "M");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(M));

	// viewing transformation matrix
	mat4 V = camera.get_viewing();
	location = glGetUniformLocation(program, "V");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(V));

	// projection transformation
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	float aspect = 1.0f * width / height;

	mat4 P = camera.get_projection(aspect);

	location = glGetUniformLocation(program, "P");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(P));

	// Draw the selected model.
	block.draw();
	
	glDisable(GL_POLYGON_OFFSET_FILL);

	if (show_wireframe)
	{
		set_color(0, 0, 0);
		glLineWidth(1.0f);
		glUniform1i(2, 1);
		block.draw_wire();
	}

	stack_manager.draw();

	if (show_wireframe)
	{
		stack_manager.draw_wire();
	}

	// Draw Stack
	glUniformMatrix4fv(4, 1, GL_FALSE, value_ptr(glm::mat4(1.0f)));
	stack_grid.draw();
	
	if (show_axis)
	{
		draw_coordinate_system();
	}

	glFlush();
}
void idle()
{
	static clock_t prev_time = clock();
	clock_t curr_time = clock();

	if (1.0 * (curr_time - prev_time) / CLOCKS_PER_SEC > 1.0) {
		/* Update the block to make it go down */
		switch (block_manager.move(0, -1, 0))
		{
		case 0:
		case 1:
			game_manager.calScore();
			if(!game_manager.gameOver())
				block_manager.newBlock();
			break;
		default: break;
		}
		prev_time = curr_time;
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		switch (block_manager.move(0, -1, 0))
		{
		case 0:
		case 1:
			game_manager.calScore();
			if (!game_manager.gameOver())
				block_manager.newBlock();

		default: break;
		}
		break;
	case 'a':
		block_manager.rotate_x();
		break;
	case 's':
		block_manager.rotate_y();
		break;
	case 'd':
		block_manager.rotate_z();
		break;
	case 'c':
		show_axis = !show_axis;
		glutPostRedisplay();
		break;
	case 'w':
		show_wireframe = !show_wireframe;
		glutPostRedisplay();
		break;
	case 'e':
		glutLeaveMainLoop();
		break;
	}
}


void specialkey(int key, int x, int y)
{
	using namespace glm;
	mat4 VT = transpose(camera.get_viewing());
	vec3 x_axis(VT[0]);

	x_axis[1] = 0;
	x_axis = glm::normalize(x_axis);
	float angle = (float)(180.0f * acosf(glm::dot(x_axis, vec3(1, 0, 0))) / M_PI);

	if (angle < 45)
	{
		switch (key)
		{
		case GLUT_KEY_LEFT: block_manager.move(-1, 0, 0); break;
		case GLUT_KEY_RIGHT: block_manager.move(+1, 0, 0); break;
		case GLUT_KEY_UP: block_manager.move(0, 0, -1); break;
		case GLUT_KEY_DOWN: block_manager.move(0, 0, +1); break;
		}
	}
	else if (angle < 135)
	{
		if (x_axis[2] > 0)
		{
			switch (key)
			{
			case GLUT_KEY_LEFT: block_manager.move(0, 0, -1); break;
			case GLUT_KEY_RIGHT: block_manager.move(0, 0, +1); break;
			case GLUT_KEY_UP: block_manager.move(+1, 0, 0); break;
			case GLUT_KEY_DOWN: block_manager.move(-1, 0, 0); break;
			}
		}
		else
		{
			switch (key)
			{
			case GLUT_KEY_LEFT: block_manager.move(0, 0, +1); break;
			case GLUT_KEY_RIGHT: block_manager.move(0, 0, -1); break;
			case GLUT_KEY_UP: block_manager.move(-1, 0, 0); break;
			case GLUT_KEY_DOWN: block_manager.move(+1, 0, 0); break;
			}
		}
	}
	else
	{
		switch (key)
		{
		case GLUT_KEY_LEFT: block_manager.move(+1, 0, 0); break;
		case GLUT_KEY_RIGHT: block_manager.move(-1, 0, 0); break;
		case GLUT_KEY_UP: block_manager.move(0, 0, +1); break;
		case GLUT_KEY_DOWN: block_manager.move(0, 0, -1); break;
		}
	}
}

void mouse(int button, int state, int x, int y)
{
	button_pressed[button] = state;
	mouse_pos[0] = x;
	mouse_pos[1] = y;
}
void motion(int x, int y)
{
	using namespace glm;

	int modifiers = glutGetModifiers();
	int is_alt_active = modifiers & GLUT_ACTIVE_ALT;

	// Calculate the normalized displacement value according to the window size.
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	GLfloat dx = 1.f * (x - mouse_pos[0]) / w;
	GLfloat dy = -1.f * (y - mouse_pos[1]) / h;

	bool redraw = false;

	// Tumble Tool
	if (button_pressed[GLUT_LEFT_BUTTON] == GLUT_DOWN)
	{
		if (is_alt_active)
		{
			// current -z axis of eye coordinate
			vec4 disp(camera.eye - camera.center, 1);

			// coefficient of displacement(magnitude)
			GLfloat alpha = 2.0f;
			// current viewing transformation matrix
			mat4 V = camera.get_viewing();
			// rotate amount of alpha * dy around x-axis of eye coorinate in world coordinate 
			mat4 Rx = rotate(mat4(1.0f), alpha*dy, vec3(transpose(V)[0]));
			// rotate amount of -alpha * dx around y-axis of world coordinate
			mat4 Ry = rotate(mat4(1.0f), -alpha * dx, vec3(0, 1, 0));
			mat4 R = Ry * Rx;
			// calculate new eye, up for New Viewing transformation matrix
			camera.eye = camera.center + vec3(R*disp); // disp: -z axis of eye coordinate, Rx: rotate matrix around x-axis of eye coorinate in world coordinate 
			camera.up = mat3(R)*camera.up;
			redraw = true;
		}
	}

	// Track Tool
	if (button_pressed[GLUT_MIDDLE_BUTTON] == GLUT_DOWN)
	{
		if (is_alt_active) {
			// caculate new eye, center according to eye coordinate seen on world coordinate
			mat4 VT = transpose(camera.get_viewing());
			camera.eye += vec3(-dx * VT[0] + -dy * VT[1]);
			camera.center += vec3(-dx * VT[0] + -dy * VT[1]);
			redraw = true;
		}
	}

	mouse_pos[0] = x;
	mouse_pos[1] = y;

	if (redraw) {
		glutPostRedisplay();
	}
}
void mouse_wheel(int wheel, int dir, int x, int y)
{
	int is_alt_active = glutGetModifiers() & GLUT_ACTIVE_ALT;

	if (is_alt_active) {
		// adjust camera position
		glm::vec3 disp = camera.eye - camera.center;
		if (dir > 0)
			camera.eye = camera.center + 0.95f*disp;
		else
			camera.eye = camera.center + 1.05f*disp;
	}
	else
	{
		// adjust lens angle
		if (dir > 0)
			camera.zoom_factor *= 0.95f;
		else
			camera.zoom_factor *= 1.05f;
	}

	glutPostRedisplay();
}


void main(int argc, char** argv)
{
	// initialize GLUT library(
	glutInit(&argc, argv);
	// set a display mode
	glutInitDisplayMode(GLUT_RGBA);
	// set a window size
	glutInitWindowSize(512, 512);
	// create new window named argv[0]
	glutCreateWindow(argv[0]);
	// initialize the extension entry points.
	GLenum err = glewInit();
	// If glewInit() doesn't return GLEW_OK the initialization failed 
	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	init();

	// register called back functions and enter the GLUT event processing loop
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkey);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMouseWheelFunc(mouse_wheel);
	glutMainLoop();
}
