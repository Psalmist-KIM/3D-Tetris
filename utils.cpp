#include "utils.h"
#define _USE_MATH_DEFINES
#include <math.h>

void get_cube_3d(
	std::vector<GLfloat>& p,
	GLfloat lx,
	GLfloat ly,
	GLfloat lz)
{
	static const GLfloat box_vertices[]{
	0.5f, 0.5f,-0.5f, // triangle 1 : begin
	-0.5f,-0.5f,-0.5f,
	-0.5f, 0.5f,-0.5f, // triangle 2 : end
	0.5f, 0.5f,-0.5f,
	0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f,

	-0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f,-0.5f,-0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f,-0.5f,

	0.5f,-0.5f, 0.5f,
	-0.5f,-0.5f,-0.5f,
	0.5f,-0.5f,-0.5f,
	0.5f,-0.5f, 0.5f,
	-0.5f,-0.5f, 0.5f,
	-0.5f,-0.5f,-0.5f,

	-0.5f, 0.5f, 0.5f,
	-0.5f,-0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	0.5f,-0.5f, 0.5f,

	0.5f, 0.5f, 0.5f,
	0.5f,-0.5f,-0.5f,
	0.5f, 0.5f,-0.5f,
	0.5f,-0.5f,-0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f,-0.5f, 0.5f,

	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f,-0.5f,
	-0.5f, 0.5f,-0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f,-0.5f,
	-0.5f, 0.5f, 0.5f
	};

	p.resize(sizeof(box_vertices) / sizeof(GLfloat));
	memcpy(p.data(), box_vertices, sizeof(box_vertices));
	size_t n = p.size() / 3;
	for (int i = 0; i < n; ++i) {
		p[3 * i + 0] *= lx;
		p[3 * i + 1] *= ly;
		p[3 * i + 2] *= lz;
	}
}

void get_cone_3d(
	std::vector<GLfloat>& p,
	std::vector<size_t>& side_idx,
	std::vector<size_t>& bottom_idx,
	GLfloat radius,
	GLfloat height,
	GLint n)
{
	GLfloat half_height = height / 2;
	GLfloat theta, x, z;

	FPUSH_VTX3(p, 0, half_height, 0); // top vertex
	side_idx.push_back(0);
	for (int i = 0; i <= n; ++i) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3(p, x, -half_height, z);
		side_idx.push_back(i + 1);
		bottom_idx.push_back(n + 2 - i);
	}
	FPUSH_VTX3(p, 0, -half_height, 0); // bottom - center vertex
	bottom_idx.push_back(1);
}

void get_cylinder_3d(
	std::vector<GLfloat>& p,
	std::vector<size_t>& side_idx,
	std::vector<size_t>& top_idx,
	std::vector<size_t>& bottom_idx,
	GLfloat radius,
	GLfloat height,
	GLint n)
{
	GLfloat half_height = height / 2;
	GLfloat theta, x, z;
	p.resize(3 * (2 * n + 4));

	FPUSH_VTX3_AT(p, 0, 0, half_height, 0);
	top_idx.push_back(0);
	bottom_idx.push_back(2 * n + 3);
	for (int i = 0; i <= n; ++i) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3_AT(p, 2 * i + 1, x, half_height, z);
		FPUSH_VTX3_AT(p, 2 * i + 2, x, -half_height, z);
		side_idx.push_back(2 * i + 1);
		side_idx.push_back(2 * i + 2);
		top_idx.push_back(2 * i + 1);
		bottom_idx.push_back(2 * n + 2 - 2 * i);
	}
	FPUSH_VTX3_AT(p, 2 * n + 3, 0, -half_height, 0);
}

void get_torus_3d(
	std::vector<GLfloat>& p,
	std::vector<std::vector<size_t>>& side_idx,
	GLfloat r0,
	GLfloat r1,
	GLint longs,
	GLint lats)
{
	side_idx.resize(lats);
	for (int j = lats, k = 0; j >= 0; --j, ++k) {
		GLfloat phi = (GLfloat)(2.0 * M_PI * j / lats);
		GLfloat y = r1 * sin(phi);
		GLfloat l = r1 * cos(phi);
		int a = k * (longs + 1);
		int b = (k + 1)*(longs + 1);
		for (int i = 0; i <= longs; ++i) {
			GLfloat theta = (GLfloat)(2.0 * M_PI * i / longs);
			GLfloat st = sin(theta);

			GLfloat ct = cos(theta);
			GLfloat x0 = (r0 + r1) * st;
			GLfloat z0 = (r0 + r1) * ct;
			GLfloat dx = l * st;
			GLfloat dz = l * ct;

			FPUSH_VTX3(p, x0 + dx, y, z0 + dz);

			if (k < lats) {
				side_idx[k].push_back(a + i);
				side_idx[k].push_back(b + i);
			}
		}
	}
}

void get_sphere_3d(
	std::vector<GLfloat>& p,
	GLfloat r,
	GLint lats,
	GLint longs)
{
	float vx0, vy0, vz0, vx1, vy1, vz1;
	float vx2, vy2, vz2, vx3, vy3, vz3;

	double y0 = 1.0; // y0 = cos(theta0) at theta0 = 0
	double st0 = 0.0; // st0 = sin(theta0) at theta0 = 0

	for (int i = 1; i <= lats; ++i) {
		double theta1 = M_PI * i / lats;
		double y1 = cos(theta1);
		double st1 = sin(theta1);

		// Compute the first two points.
		FSET_VTX3(vx0, vy0, vz0, 0.0f, r*y0, r*st0);
		FSET_VTX3(vx1, vy1, vz1, 0.0f, r*y1, r*st1);

		for (int j = 1; j <= longs; ++j) {
			// Compute the next two points as we move along a horizontal direction.
			double phi = 2 * M_PI * j / longs;
			double cp = cos(phi);
			double sp = sin(phi);
			FSET_VTX3(vx2, vy2, vz2, r*sp*st0, r*y0, r*cp*st0);
			FSET_VTX3(vx3, vy3, vz3, r*sp*st1, r*y1, r*cp*st1);

			if (i < lats) {
				// first triangle(v0 - v1 - v3)
				FPUSH_VTX3(p, vx0, vy0, vz0);
				FPUSH_VTX3(p, vx1, vy1, vz1);
				FPUSH_VTX3(p, vx3, vy3, vz3);
			}
			if (1 < i) {
				// second triangle(v3 - v2 - v0)
				FPUSH_VTX3(p, vx3, vy3, vz3);
				FPUSH_VTX3(p, vx2, vy2, vz2);
				FPUSH_VTX3(p, vx0, vy0, vz0);
			}

			FSET_VTX3(vx0, vy0, vz0, vx2, vy2, vz2);
			FSET_VTX3(vx1, vy1, vz1, vx3, vy3, vz3);
		}
		y0 = y1;
		st0 = st1;
	}
}

void get_grid(
	std::vector<GLfloat>& p,
	GLfloat w,
	GLfloat h,
	int m,
	int n)
{
	GLfloat x0 = -0.5f * w;
	GLfloat x1 = +0.5f * w;
	GLfloat z0 = -0.5f * h;
	GLfloat z1 = +0.5f * h;

	for (int i = 0; i <= m; ++i) {
		GLfloat x = x0 + w * i / m;
		FPUSH_VTX3(p, x, 0, z0);
		FPUSH_VTX3(p, x, 0, z1);
	}
	for (int i = 0; i <= n; ++i) {
		GLfloat z = z0 + h * i / n;
		FPUSH_VTX3(p, x0, 0, z);
		FPUSH_VTX3(p, x1, 0, z);
	}
}

void get_color_3d_by_pos(GLvec& c, GLvec& p, GLfloat offset)
{
	GLfloat max_val[3] = { -INFINITY, -INFINITY, -INFINITY };
	GLfloat min_val[3] = { INFINITY, INFINITY, INFINITY };

	int n = (int)(p.size() / 3);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < 3; ++j) {
			GLfloat val = p[i * 3 + j];
			if (max_val[j] < val) max_val[j] = val;
			else if (min_val[j] > val) min_val[j] = val;
		}
	}

	GLfloat width[3] = {
		max_val[0] - min_val[0],
		max_val[1] - min_val[1],
		max_val[2] - min_val[2]
	};

	c.resize(p.size());
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < 3; ++j) {
			int k = i * 3 + j;
			c[k] = std::fminf((p[k] - min_val[j]) / width[j] + offset, 1.0f);
		}
	}
}

