#pragma once

#define _USE_MATH_DEFINES
#include <iostream>
#include <algorithm>
#include "primi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct StackManager
{
	bool occupied[350] = { false };
	CubePrimitive* cube;

	StackManager(CubePrimitive* cube)
	{
		this->cube = cube;
	}

	glm::vec3 index2pos(int i)
	{
		int x = i % 5 - 2;
		i = i - (x + 2);

		int z = -((i % 25) / 5 - 2);
		i = i - (-z + 2) * 5;

		int y = i / 25 - 10;

		return glm::vec3((float)x, (float)y, (float)z);
	}

	int pos2index(const glm::vec3& pos)
	{
		int idx = (((int)pos.y + 10) * 25 + ((int)-pos.z + 2) * 5 + ((int)pos.x + 2) * 1);
		return idx;
	}

	void push_cube(glm::vec3& pos)
	{
		occupied[pos2index(pos)] = true;
	}

	bool isOccupied(glm::vec3& pos)
	{
		if (pos.x > 2 || pos.x < -2 || pos.z > 2 || pos.z < -2 || pos.y < -10) return true;
		return occupied[pos2index(pos)];
	}

	void draw()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1, 1);
		glUniform1i(2, 0);

		for (int i = 0; i < 350; ++i)
		{
			if (occupied[i])
			{
				glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(glm::mat4(1.f)));

				glm::mat4 T(1.0f);
				T = glm::translate(T, index2pos(i));
				glUniformMatrix4fv(4, 1, GL_FALSE, value_ptr(T));
				cube->draw();
			}
		}
	}

	void draw_wire()
	{
		glUniform4fv(3, 1, value_ptr(glm::vec4(0, 0, 0, 1)));

		glLineWidth(1.0f);
		glUniform1i(2, 1);

		for (int i = 0; i < 350; ++i)
		{
			if (occupied[i])
			{
				glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(glm::mat4(1.f)));

				glm::mat4 T(1.0f);
				T = glm::translate(T, index2pos(i));
				glUniformMatrix4fv(4, 1, GL_FALSE, value_ptr(T));
				cube->draw_wire();
			}
		}
	}
};

struct BlockManager
{
	glm::mat4 R;
	glm::mat4 T;

	Block* block;
	StackManager* stack;

	struct BlockState
	{
		glm::vec3 pos;

		glm::vec3 cube1;
		glm::vec3 cube2;
		glm::vec3 cube3;
		glm::vec3 cube4;
	}
	block_state;

	BlockManager(Block* block, StackManager* stack)
	{
		this->block = block;
		this->stack = stack;

		block_state.pos = glm::vec3(0, 1, 0);

		R = glm::mat4(1.0f);
		T = glm::mat4(1.0f);

		switch (block->type)
		{
		case 'L':
			block_state.cube1 = block_state.pos;
			block_state.cube2 = block_state.pos + glm::vec3(0.f, 1.0f, 0.f);
			block_state.cube3 = block_state.pos + glm::vec3(0.f, 2.0f, 0.f);
			block_state.cube4 = block_state.pos + glm::vec3(1.f, 0.f, 0.f);
			break;
		case 'I':
			block_state.cube1 = this->block_state.pos;
			block_state.cube2 = this->block_state.pos + glm::vec3(0.f, -1.0f, 0.f);
			block_state.cube3 = this->block_state.pos + glm::vec3(0.f, 1.0f, 0.f);
			block_state.cube4 = this->block_state.pos + glm::vec3(0.f, 2.0f, 0.f);
			break;
		case 'B':
			block_state.cube1 = this->block_state.pos;
			block_state.cube2 = this->block_state.pos + glm::vec3(1.f, 0.0f, 0.f);
			block_state.cube3 = this->block_state.pos + glm::vec3(1.f, 1.0f, 0.f);
			block_state.cube4 = this->block_state.pos + glm::vec3(0.f, 1.0f, 0.f);
			break;
		case 'N':
			block_state.cube1 = this->block_state.pos;
			block_state.cube2 = this->block_state.pos + glm::vec3(0.f, -1.0f, 0.f);
			block_state.cube3 = this->block_state.pos + glm::vec3(-1.f, 0.0f, 0.f);
			block_state.cube4 = this->block_state.pos + glm::vec3(-1.f, 1.0f, 0.f);
			break;
		}
	}

	glm::mat4 get_R()
	{
		return R;
	}

	glm::mat4 get_T()
	{
		this->T = glm::translate(glm::mat4(1.f), block_state.pos);
		return T;
	}

	int move(int dx, int dy, int dz)
	{
		using namespace glm;
		vec3 pos1 = block_state.cube1 + vec3(dx, dy, dz);
		vec3 pos2 = block_state.cube2 + vec3(dx, dy, dz);
		vec3 pos3 = block_state.cube3 + vec3(dx, dy, dz);
		vec3 pos4 = block_state.cube4 + vec3(dx, dy, dz);

		bool occupied = (stack->isOccupied(pos1) || stack->isOccupied(pos2) || stack->isOccupied(pos3) || stack->isOccupied(pos4));

		if (dy == -1)
		{
			if (occupied)
			{
				pos1 = pos1 + vec3(0, 1.f, 0);
				pos2 = pos2 + vec3(0, 1.f, 0);
				pos3 = pos3 + vec3(0, 1.f, 0);
				pos4 = pos4 + vec3(0, 1.f, 0);

				stack->push_cube(pos1);
				stack->push_cube(pos2);
				stack->push_cube(pos3);
				stack->push_cube(pos4);

				// game over condition
				if (pos1.y >= 0 || pos2.y >= 0 || pos3.y >= 0 || pos4.y >= 0)
				{
					return 0;
				}
				// stacked condition
				else
				{
					return 1;
				}
			}
			// move condition
			else
			{
				block_state.pos = pos1;
				block_state.cube1 = pos1;
				block_state.cube2 = pos2;
				block_state.cube3 = pos3;
				block_state.cube4 = pos4;

				return 2;
			}
		}
		else if (!occupied)
		{
			block_state.pos = pos1;
			block_state.cube1 = pos1;
			block_state.cube2 = pos2;
			block_state.cube3 = pos3;
			block_state.cube4 = pos4;

			return 2;
		}
		return 3;
	}

	void rotate_x()
	{
		using namespace glm;

		mat4 T(1.0f);
		T = translate(T, block_state.pos);
		T = rotate(T, (float)M_PI / 2, vec3(1.f, 0.f, 0.f));
		T = translate(T, -block_state.pos);


		vec3 pos1 = vec3(T*vec4(block_state.cube1, 1.f));
		vec3 pos2 = vec3(T*vec4(block_state.cube2, 1.f));
		vec3 pos3 = vec3(T*vec4(block_state.cube3, 1.f));
		vec3 pos4 = vec3(T*vec4(block_state.cube4, 1.f));

		bool occupied = (stack->isOccupied(pos1) || stack->isOccupied(pos2) || stack->isOccupied(pos3) || stack->isOccupied(pos4));

		if (!occupied)
		{
			block_state.cube1 = floor(pos1 + vec3(0.5));
			block_state.cube2 = floor(pos2 + vec3(0.5));
			block_state.cube3 = floor(pos3 + vec3(0.5));
			block_state.cube4 = floor(pos4 + vec3(0.5));

			T = rotate(mat4(1.f), (float)M_PI / 2, vec3(1.f, 0.f, 0.f));
			R = T * R;
		}
	}

	void rotate_y()
	{
		using namespace glm;

		mat4 T(1.0f);
		T = translate(T, block_state.pos);
		T = rotate(T, (float)M_PI / 2, vec3(0.f, 1.f, 0.f));
		T = translate(T, -block_state.pos);

		vec3 pos1 = vec3(T*vec4(block_state.cube1, 1.f));
		vec3 pos2 = vec3(T*vec4(block_state.cube2, 1.f));
		vec3 pos3 = vec3(T*vec4(block_state.cube3, 1.f));
		vec3 pos4 = vec3(T*vec4(block_state.cube4, 1.f));

		bool occupied = (stack->isOccupied(pos1) || stack->isOccupied(pos2) || stack->isOccupied(pos3) || stack->isOccupied(pos4));

		if (!occupied)
		{
			block_state.cube1 = floor(pos1 + vec3(0.5));
			block_state.cube2 = floor(pos2 + vec3(0.5));
			block_state.cube3 = floor(pos3 + vec3(0.5));
			block_state.cube4 = floor(pos4 + vec3(0.5));

			T = rotate(mat4(1.f), (float)M_PI / 2, vec3(0.f, 1.f, 0.f));
			R = T * R;
		}
	}

	void rotate_z()
	{
		using namespace glm;

		mat4 T(1.0f);
		T = translate(T, block_state.pos);
		T = rotate(T, (float)M_PI / 2, vec3(0.f, 0.f, 1.f));
		T = translate(T, -block_state.pos);

		vec3 pos1 = vec3(T*vec4(block_state.cube1, 1.f));
		vec3 pos2 = vec3(T*vec4(block_state.cube2, 1.f));
		vec3 pos3 = vec3(T*vec4(block_state.cube3, 1.f));
		vec3 pos4 = vec3(T*vec4(block_state.cube4, 1.f));

		bool occupied = (stack->isOccupied(pos1) || stack->isOccupied(pos2) || stack->isOccupied(pos3) || stack->isOccupied(pos4));

		if (!occupied)
		{
			block_state.cube1 = floor(pos1 + vec3(0.5));
			block_state.cube2 = floor(pos2 + vec3(0.5));
			block_state.cube3 = floor(pos3 + vec3(0.5));
			block_state.cube4 = floor(pos4 + vec3(0.5));

			T = rotate(mat4(1.f), (float)M_PI / 2, vec3(0.f, 0.f, 1.f));
			R = T * R;
		}
	}

	char rand_type()
	{
		srand(time(NULL));
		int i = rand() % 4;

		switch (i)
		{
		case 0: return 'L';
		case 1: return 'I';
		case 2: return 'B';
		case 3: return 'N';
		}
	}

	void newBlock()
	{
		block->type = rand_type();

		R = glm::mat4(1.0f);
		T = glm::mat4(1.0f);

		block_state.pos = glm::vec3(0, 1, 0);

		switch (block->type)
		{
		case 'L':
			block_state.cube1 = block_state.pos;
			block_state.cube2 = block_state.pos + glm::vec3(0.f, 1.0f, 0.f);
			block_state.cube3 = block_state.pos + glm::vec3(0.f, 2.0f, 0.f);
			block_state.cube4 = block_state.pos + glm::vec3(1.f, 0.f, 0.f);
			break;
		case 'I':
			block_state.cube1 = this->block_state.pos;
			block_state.cube2 = this->block_state.pos + glm::vec3(0.f, -1.0f, 0.f);
			block_state.cube3 = this->block_state.pos + glm::vec3(0.f, 1.0f, 0.f);
			block_state.cube4 = this->block_state.pos + glm::vec3(0.f, 2.0f, 0.f);
			break;
		case 'B':
			block_state.cube1 = this->block_state.pos;
			block_state.cube2 = this->block_state.pos + glm::vec3(1.f, 0.0f, 0.f);
			block_state.cube3 = this->block_state.pos + glm::vec3(1.f, 1.0f, 0.f);
			block_state.cube4 = this->block_state.pos + glm::vec3(0.f, 1.0f, 0.f);
			break;
		case 'N':
			block_state.cube1 = this->block_state.pos;
			block_state.cube2 = this->block_state.pos + glm::vec3(0.f, -1.0f, 0.f);
			block_state.cube3 = this->block_state.pos + glm::vec3(-1.f, 0.0f, 0.f);
			block_state.cube4 = this->block_state.pos + glm::vec3(-1.f, 1.0f, 0.f);
			break;
		}
	}

};

struct GameManager
{
	char coin = 'y';
	int score = 0;

	StackManager* stack;
	BlockManager* block;

	GameManager(StackManager* stack, BlockManager* block)
	{
		this->stack = stack;
		this->block = block;
	}

	void clearLayer(int y)
	{
		for (int i = 0; i < 25; ++i)
		{
			stack->occupied[25 * (y) + i] = false;
		}
		for (int i = y+1; i < 11; ++i)
		{
			for (int j = 0; j < 25; ++j)
			{
				stack->occupied[25 * (i-1) + j] = stack->occupied[25 * i + j];
			}
		}
	}

	void calScore()
	{
		int layer[4] = {
			block->block_state.cube1.y + 10,
			block->block_state.cube2.y + 10,
			block->block_state.cube3.y + 10,
			block->block_state.cube4.y + 10
		};

		std::sort(layer, layer+4);

		bool get_score = false;
		int score = 0;

		for(int i = 3; i >=0; --i)
		{
			for (int j = 0; j < 25; ++j)
			{
				get_score = stack->occupied[25*layer[i] + j];

				if (!get_score)
					break;
			}
			if (get_score)
			{
				clearLayer(layer[i]);
				score++;
			}
		}

		this->score += score;
	}

	void restart()
	{
		for (int i = 0; i < sizeof(stack->occupied) / sizeof(bool); ++i)
		{
			stack->occupied[i] = false;
			block->newBlock();
			score = 0;
		}
	}

	bool gameOver()
	{
		bool over = false;
		for (int i = 250; i < sizeof(stack->occupied) / sizeof(bool); ++i)
		{
			if (stack->occupied[i])
			{
				over = true;
				break;
			}
		}
		
		if (over)
		{
			printf("*****Game Over*****\n");
			printf("Score : %d\n", score);
			printf("Do you want another game? (y/n) ");
			scanf(" %c", &coin);
			
			if (coin == 'y' || coin == 'Y')
			{
				restart();
			}
			else if (coin == 'n' || coin == 'N')
			{
				exit(0);
			}
			return true;
		}
		else
			return false;
	}
};