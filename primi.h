#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include "utils.h"


class Model {
public:
	GLuint vao, buffs[2];
	GLvec vtx_pos;
	GLvec vtx_clrs;

	virtual void init(GLint program)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		const GLchar* attri_name[2] = { "vPosition", "vColor" };
		GLvec* vtx_list[2] = { &vtx_pos, &vtx_clrs };

		glGenBuffers(2, buffs);
		for (int i = 0; i < 2; ++i) {
			glBindBuffer(GL_ARRAY_BUFFER, buffs[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vtx_list[i]->size(), vtx_list[i]->data(), GL_STATIC_DRAW);
			GLint location = glGetAttribLocation(program, attri_name[i]);
			glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(location);
		}
	}

	virtual void draw() {}
	virtual void draw_wire() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		draw();
	}
};

struct CubePrimitive : public Model
{
	glm::vec3 pos;

	CubePrimitive(GLfloat lx, GLfloat ly, GLfloat lz)
	{
		pos = glm::vec3(0.f, 0.f, 0.f);
		get_cube_3d(vtx_pos, lx, ly, lz);
		get_color_3d_by_pos(vtx_clrs, vtx_pos);
	}

	virtual void draw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vtx_pos.size() / 3);
	}
};

struct Grid : public Model
{
	Grid(GLfloat w, GLfloat h, int m, int n) {
		get_grid(vtx_pos, w, h, m, n);

		int N = (int)vtx_pos.size();
		vtx_clrs.resize(N);
		for (int i = 0; i < N; ++i) {
			vtx_clrs[i] = 1.0f;
		}
	}

	virtual void draw()
	{
		glUniform1i(2, 0);
		glBindVertexArray(vao);
		glLineWidth(1.5f);
		glDrawArrays(GL_LINES, 0, vtx_pos.size() / 3);
	}
};

struct StackModel : public Model
{
	Grid* grid;

	StackModel(Grid* grid, CubePrimitive* cube)
	{
		this->grid = grid;
	}

	virtual void init(GLint program)
	{
		// do nothing
	}

	virtual void draw()
	{
		using namespace glm;
		mat4 T(1.0f);
		T = translate(T, vec3(0, -10.5f, 0));
		glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(T));
		grid->draw();

		T = mat4(1.0f);
		T = translate(T, vec3(0, -0.5f, 0));
		glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(T));
		grid->draw();
	}
};


struct CylinderPrimitive : public Model
{
	std::vector<size_t> idx_top;
	std::vector<size_t> idx_side;
	std::vector<size_t> idx_bottom;
	
	CylinderPrimitive(GLfloat radius, GLfloat height, GLint subdivs)
	{
		get_cylinder_3d(vtx_pos, idx_side, idx_top, idx_bottom, radius, height, subdivs);
		get_color_3d_by_pos(vtx_clrs, vtx_pos);
	}
	
	virtual void draw()
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLE_STRIP, idx_side.size(), GL_UNSIGNED_INT, idx_side.data());
		glDrawElements(GL_TRIANGLE_FAN, idx_top.size(), GL_UNSIGNED_INT, idx_top.data());
		glDrawElements(GL_TRIANGLE_FAN, idx_bottom.size(), GL_UNSIGNED_INT, idx_bottom.data());
	}
};


struct Block : public Model
{
	CubePrimitive* cube;

	char type;

	Block(CubePrimitive* cube, const char& type)
	{
		this->cube = cube;

		this->type = type;
	}

	glm::mat4 transf(
		GLfloat tx, GLfloat ty, GLfloat tz,
		glm::mat4* T_pre = NULL,
		glm::mat4* T_post = NULL,
		bool set_uniform = true)
	{
		using namespace glm;
		glm::mat4 T(1.0f);
		T = translate(T, vec3(tx, ty, tz));
		if (T_pre) T = (*T_pre)*T;
		if (T_post) T = T * (*T_post);
		if (set_uniform) {
			glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(T));
		}
		return T;
	}

	virtual void draw()
	{
		switch (type)
		{
		case 'L':
			transf(0.f, 0.f, 0.f);
			cube->draw();
			transf(0.f, 1.f, 0.f);
			cube->draw();
			transf(0.f, 2.f, 0.f);
			cube->draw();
			transf(1.f, 0.0f, 0.f);
			cube->draw();
			break;
		case 'I':
			transf(0.f, 0.f, 0.f);
			cube->draw();
			transf(0.f, -1.f, 0.f);
			cube->draw();
			transf(0.f, 1.f, 0.f);
			cube->draw();
			transf(0.f, 2.0f, 0.f);
			cube->draw();
			break;
		case 'B':
			transf(0.f, 0.f, 0.f);
			cube->draw();
			transf(1.f, 0.f, 0.f);
			cube->draw();
			transf(1.f, 1.f, 0.f);
			cube->draw();
			transf(0.f, 1.f, 0.f);
			cube->draw();
			break;
		case 'N':
			transf(0.f, 0.f, 0.f);
			cube->draw();
			transf(0.f, -1.f, 0.f);
			cube->draw();
			transf(-1.f, 0.f, 0.f);
			cube->draw();
			transf(-1.f, 1.0f, 0.f);
			cube->draw();
			break;
		}
	}
};