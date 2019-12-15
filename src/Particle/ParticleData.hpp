#pragma once

#include <glm/glm.hpp>



struct ParticleData {
	ParticleData() :
			position({{0.f, 0.f, 0.f}}),
			velocity({{0.f, 0.f, 0.f}}),
			color({{0.f, 0.f, 0.f}}),
			rotate(0.f),
			size(0.f),
			age(0.f),
			lifeTime(0.f),
			isAlive(0),
			index(0)
	{}

	cl_float3   position; 	// Center point of particle
	cl_float3   velocity; 	// Current particle velocity
	cl_float4   color;    	// Particle color
	cl_float       rotate;  	// Rotate the particle center
	cl_float       size;    	// Size of the particle
	cl_float       age;
	cl_float       lifeTime;	//If < 0 = Inifite lifeTime
	cl_int			isAlive;
	cl_int			index;
};