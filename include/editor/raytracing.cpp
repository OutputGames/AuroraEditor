#include "raytracing.hpp"

#include "agl/agl.hpp"
#include "agl/agl_ext.hpp"
#include "aurora/gfx/camera.hpp"

using namespace std;

void RaytracingManager::Init()
{
	presentShader = new agl::aglShader(agl::aglShaderSettings{
		{"resources/shaders/texture/main.vert", "resources/shaders/texture/main.frag"},
		VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_COMPARE_OP_ALWAYS
	});

	computeTex = new agl::aglTexture( agl::aglTextureCreationInfo{
		(int)agl::GetSurfaceDetails()->framebuffer->extent.width, (int)agl::GetSurfaceDetails()->framebuffer->extent.height, 4,
		false, nullptr, true
	});

	presentShader->AttachTexture(computeTex, presentShader->GetBindingByName("tex"));

	presentShader->Setup();

	computeShader = new agl::aglComputeShader(agl::aglShaderSettings{ {"","","","resources/shaders/compute/main.comp"} });

	storageBuffer = new agl::aglStorageBuffer(agl::aglBufferSettings{ VK_SHADER_STAGE_COMPUTE_BIT, sizeof(RaytracingData) });
	storageBuffer->AttachToShader(computeShader, computeShader->GetBindingByName("data"));

	computeShader->AttachTexture(computeTex, computeShader->GetBindingByName("tex"));

}

void RaytracingManager::Dispatch()
{
	//computeShader = static_cast<agl::aglComputeShader*>(agl::aglShaderFactory::GetShader(computeShader->id));

	std::vector<agl::aglRenderQueueEntry> pulledEntries = agl::GetSurfaceDetails()->framebuffer->renderPass->renderQueue
		->queueEntries;
	agl::GetSurfaceDetails()->framebuffer->renderPass->renderQueue->queueEntries.clear();

	int w = agl::GetMainFramebufferSize().x;
	int h = agl::GetMainFramebufferSize().y;

	data.camPos = make_vec4(Camera::Main->Entity->Transform.position);
	data.view = Camera::Main->GetViewMatrix();
	data.proj = Camera::Main->GetProjectionMatrix();

	float aspect = (flt w / flt h);

	float focusDistance = 1.0f;

	float planeHeight = focusDistance * tanf(Camera::Main->fov * 0.5f * DEG2RAD) * 2;
	float planeWidth = planeHeight * aspect;

	data.viewParams = make_vec4(vec3{planeWidth,planeHeight, focusDistance});

	storageBuffer->Update(&data, sizeof(data));

	computeShader->BeginDispatchField();

		
	int workgroup_count_x = (w + 7) / 8;
	int workgroup_count_y = (h + 7) / 8; //quick ceiling calculation

	computeShader->Dispatch(agl::GetCurrentImage(), { workgroup_count_x,workgroup_count_y,1 });

	computeShader->EndDispatchField();

	agl::GetSurfaceDetails()->framebuffer->renderPass->renderQueue->AttachQueueEntry({
		aglPrimitives::GetPrims()[aglPrimitives::QUAD], presentShader
	});

}

void RaytracingManager::Reload()
{
	computeShader->Recreate();
}
