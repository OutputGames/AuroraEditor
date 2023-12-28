#if !defined(RAY_HPP)
#define RAY_HPP

#include "agl/agl.hpp"
#include "aurora/utils/utils.hpp"

struct RaytracingData
{
	mat4 view;
	mat4 proj;
	vec4 camPos;
	vec4 viewParams;
};

struct RaytracingManager
{
    void Init();
	void Dispatch();
	void Reload();

private:

	agl::aglShader* presentShader;
	agl::aglTexture* computeTex;
	agl::aglComputeShader* computeShader;
	agl::aglStorageBuffer* storageBuffer;

	RaytracingData data;

};


#endif // RAY_HPP
