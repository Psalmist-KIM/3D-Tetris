#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <vector>

#define FPUSH_VTX3(p, vx, vy, vz)	\
	do { \
		p.push_back(vx); \
		p.push_back(vy); \
		p.push_back(vz); \
	} while(0)

#define FPUSH_VTX3_AT(p, i, vx, vy, vz)	\
	do { \
		size_t i3 = 3*(i); \
		p[i3+0] = (float)(vx); \
		p[i3+1] = (float)(vy); \
		p[i3+2] = (float)(vz); \
	} while(0)

#define FSET_VTX3(vx, vy, vz, valx, valy, valz) \
	do { \
		vx=(float)(valx); \
		vy=(float)(valy); \
		vz=(float)(valz); \
	} while(0)


typedef std::vector<GLfloat> GLvec;

void get_cube_3d(
	std::vector<GLfloat>& p,
	GLfloat lx,
	GLfloat ly,
	GLfloat lz);

void get_cone_3d(
	std::vector<GLfloat>& p,
	std::vector<size_t>& side_idx,
	std::vector<size_t>& bottom_idx,
	GLfloat radius,
	GLfloat height,
	GLint subdivs);

void get_cylinder_3d(
	std::vector<GLfloat>& p,
	std::vector<size_t>& side_idx,
	std::vector<size_t>& top_idx,
	std::vector<size_t>& bottom_idx,
	GLfloat radius,
	GLfloat height,
	GLint subdivs);

void get_torus_3d(
	std::vector<GLfloat>& p,
	std::vector<std::vector<size_t>>& side_idx,
	GLfloat radius0,
	GLfloat radiusl,
	GLint longs,
	GLint lats);

void get_sphere_3d(
	std::vector<GLfloat>& p,
	GLfloat r,
	GLint lats,
	GLint longs);

void get_grid(
	std::vector<GLfloat>& p,
	GLfloat w,
	GLfloat h,
	int m,
	int n);

void get_color_3d_by_pos(
	GLvec& c,
	GLvec& p,
	GLfloat offset = 0);
