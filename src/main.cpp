#include <iomanip>

#include "aal/aal.hpp"
#include "aurora/aurora.hpp"

#include "agl/agl.hpp"
#include "agl/agl_ext.hpp"
#include "agl/ext.hpp"
#include "agl/maths.hpp"
#include "agl/re.hpp"
#include "aurora/ecs/audio.hpp"
#include "aurora/ecs/registry.hpp"
#include "aurora/ecs/scene.hpp"
#include "aurora/gfx/camera.hpp"
#include "aurora/gfx/lighting.hpp"
#include "aurora/gfx/renderer.hpp"
#include "aurora/input/input.hpp"
#include "aurora/physics/physics.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "editor/cam.hpp"
#include "editor/player.hpp"
#include "editor/raytracing.hpp"

#include "imgui.h"

int main(int argc, char* argv[])
{
#ifdef DEBUG
	/*
	{
		std::string path("C:/Users/chris/Downloads/Aurora/AuroraCore/bin/Debug/");
		std::string ext(".dll");
		std::string outPath("C:/Users/chris/Downloads/Aurora/TestAuroraProject/bin/Debug/");


		for (auto& p : std::filesystem::recursive_directory_iterator(path))
		{
			if (p.path().extension() == ext) {
				std::string outFile = outPath + p.path().filename().string();

				std::filesystem::copy(p.path(), outFile, std::filesystem::copy_options::overwrite_existing);
			}
		}
	}

	{
		std::string path("C:/Users/chris/Downloads/Aurora/AuroraCore/bin/Debug/");
		std::string ext(".pdb");
		std::string outPath("C:/Users/chris/Downloads/Aurora/TestAuroraProject/bin/Debug/");
		for (auto& p : std::filesystem::recursive_directory_iterator(path))
		{
			if (p.path().extension() == ext) {
				std::string outFile = outPath + p.path().filename().string();

				std::filesystem::copy(p.path(), outFile, std::filesystem::copy_options::overwrite_existing);
			}
		}
	}
	*/
#endif

	aclComponentManager::Init();

	agl_details* details = new agl_details;

	details->applicationName = "AuroraEditor";
	details->engineName = "Aurora";
	details->engineVersion = VK_MAKE_VERSION(1, 0, 0);
	details->applicationVersion = VK_MAKE_VERSION(1, 0, 0);

	details->Width = 800;
	details->Height = 600;

	SDLUtils::Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);

	aclComponentManager::Init();

	ComponentRegistry::RegisterComponent(ComponentRegistry::ComponentRegister<OrbitalCamera, PlayerController>());

	aclPhysicsMgr::Setup();

	agl::agl_init(details);

	SceneCreationSettings scene_creation_settings = {
		"TestScene", true, true
	};

	agl_ext::InstallExtension<aglPrimitives>();

	Scene* scene = Scene::Create(scene_creation_settings);

	srand(time(0));

	bool redo = true;

		if (redo) {

			Ref<Entity> cameraEntity = Scene::Current->entityMgr->CreateEntity("Camera");
			Ref<Entity> entity = Scene::Current->entityMgr->CreateEntity("NewEntity");

			//AudioPlayer* audioPlayer = entity->AttachComponent<AudioPlayer>();

			//audioPlayer->SetAudio(new Sound(new SoundCreateInfo{ Sound::LoadSoundBuffer("resources/news.wav") }));

			float pos = 100;

			Ref<Entity> lightEntity = Scene::Current->entityMgr->CreateEntity("Light");
			Light* light = lightEntity->AttachComponent<Light>();
			light->Color = Color{ 1,1,1 };
			light->Power = 1;

			lightEntity->Transform.position = { 30,30,30 };
			light->Power = 100.0f;

			Camera* camera = cameraEntity->AttachComponent<Camera>();

			//cameraEntity->AttachComponent<OrbitalCamera>();

			camera->target = vec3{ 0 };
			camera->fov = 90.0f;

			cameraEntity->Transform.position = vec3{ 10 };

			agl::aglTexture* equiTex = new agl::aglTexture("resources/textures/noon_grass.hdr", VK_FORMAT_R8G8B8A8_SRGB);
			agl::aglShader* equishader = new agl::aglShader({ {"resources/shaders/equi/main.vert","resources/shaders/equi/main.frag"}, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_COMPARE_OP_ALWAYS });
			agl::aglTexture* cubemapTex = new agl::aglTexture(equishader, agl::aglTextureCreationInfo{ 512,512,equiTex->channels,true, equiTex });


			agl::aglShader* irradianceshader = new agl::aglShader({ {"resources/shaders/equi/main.vert","resources/shaders/irradiance/main.frag"}, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_COMPARE_OP_ALWAYS });
			agl::aglTexture* irradiance = new agl::aglTexture(irradianceshader, agl::aglTextureCreationInfo{ 32,32,cubemapTex->channels,true, cubemapTex });


			agl::aglShader* brdfshader = new agl::aglShader({ {"resources/shaders/brdf/main.vert","resources/shaders/brdf/main.frag"}, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_COMPARE_OP_ALWAYS });
			agl::aglTexture* brdf = new agl::aglTexture(brdfshader, agl::aglTextureCreationInfo{ 512,512,4,false, nullptr });


			agl::aglShader* prefiltershader = new agl::aglShader({ {"resources/shaders/equi/main.vert","resources/shaders/prefilter/main.frag"}, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_COMPARE_OP_ALWAYS });
			agl::aglTexture* prefilter = new agl::aglTexture(prefiltershader, agl::aglTextureCreationInfo{ 128,128,cubemapTex->channels,true, cubemapTex });

			agl::aglShader* shader = new agl::aglShader({ {"resources/shaders/cubemap/main.vert","resources/shaders/cubemap/main.frag"}, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_COMPARE_OP_LESS_OR_EQUAL });
			shader->AttachTexture(cubemapTex, shader->GetBindingByName("tex"));
			MeshRenderer* renderer = entity->AttachComponent<MeshRenderer>();
			renderer->material->shader = shader;
			renderer->mesh = aglPrimitives::GetPrims()[aglPrimitives::CUBE];

			{
				Ref<Entity> plane = Scene::Current->entityMgr->CreateEntity("Plane");
				plane->Transform.scale = { 100,1,100 };
				//plane->Transform.rotation = vec3{ 45,0,0 };
				MeshRenderer* planeRenderer = plane->AttachComponent<MeshRenderer>();
				agl::aglShader* planeShader = new agl::aglShader({
					{"resources/shaders/fresnel/main.vert", "resources/shaders/fresnel/main.frag"}, VK_CULL_MODE_BACK_BIT,
					VK_FRONT_FACE_COUNTER_CLOCKWISE
					});
				planeRenderer->material->shader = planeShader;
				planeRenderer->mesh = aglPrimitives::GetPrims()[aglPrimitives::CUBE];
				auto planeCollider = plane->AttachComponent<BoxCollider>();
				planeCollider->Extents = { 1,1,1 };
				Rigidbody* planeBody = plane->AttachComponent<Rigidbody>();
				planeBody->motionType = Rigidbody::STATIC;
				planeBody->Init();
				planeRenderer->color = { 0.1,0.1,1 };
				planeShader->AttachTexture(brdf, planeShader->GetBindingByName("brdf"));
				planeShader->AttachTexture(irradiance, planeShader->GetBindingByName("irradiance"));
				planeShader->AttachTexture(prefilter, planeShader->GetBindingByName("prefilter"));
			}

			{
				Ref<Entity> plane = Scene::Current->entityMgr->CreateEntity("Plane");
				plane->Transform.scale = { 50,1,50 };
				plane->Transform.rotation = vec3{ -15,0,0 };
				plane->Transform.position = { 0,0,0 };
				MeshRenderer* planeRenderer = plane->AttachComponent<MeshRenderer>();
				agl::aglShader* planeShader = new agl::aglShader({
					{"resources/shaders/fresnel/main.vert", "resources/shaders/fresnel/main.frag"}, VK_CULL_MODE_BACK_BIT,
					VK_FRONT_FACE_COUNTER_CLOCKWISE
					});
				planeRenderer->material->shader = planeShader;
				planeRenderer->mesh = aglPrimitives::GetPrims()[aglPrimitives::CUBE];
				auto planeCollider = plane->AttachComponent<BoxCollider>();
				planeCollider->Extents = { 1,1,1 };
				Rigidbody* planeBody = plane->AttachComponent<Rigidbody>();
				planeBody->motionType = Rigidbody::STATIC;
				planeBody->Init();
				planeRenderer->color = { 0.1,0.1,1 };
				planeShader->AttachTexture(brdf, planeShader->GetBindingByName("brdf"));
				planeShader->AttachTexture(irradiance, planeShader->GetBindingByName("irradiance"));
				planeShader->AttachTexture(prefilter, planeShader->GetBindingByName("prefilter"));
			}

			
			float spacing;
			spacing = 2.1f;
			int amt = 3;


			
			
			for (int x = 0; x < amt; x++)
			{
				for (int y = 0; y < amt; y++)
				{
					for (int z = 0; z < amt; z++)
					{
						agl::aglShader* sphereShader = new agl::aglShader({ {"resources/shaders/fresnel/main.vert", "resources/shaders/fresnel/main.frag"}, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE });
						Ref<Entity> sphere = Scene::Current->entityMgr->CreateEntity("Sphere");
						MeshRenderer* sphereRenderer = sphere->AttachComponent<MeshRenderer>();
						sphereRenderer->material->shader = sphereShader;
						sphereRenderer->mesh = agl::aglMesh::GrabMesh("resources/models/Obj_Box01S/Obj_Box01S.fbx", 0);
						sphere->Transform.position = vec3{
							(flt x - (flt amt / 2)) * spacing,
							((flt y - (flt amt / 2)) * spacing)+(flt amt+7),
							(flt z - (flt amt / 2)) * spacing,
						};

						sphere->Transform.scale = vec3{ 1 };

						
						auto sphereCollider = sphere->AttachComponent<BoxCollider>();

						//sphereCollider->mesh = sphereRenderer->mesh;

						Rigidbody* sphereBody = sphere->AttachComponent<Rigidbody>();
						sphereBody->Init();
						//sphereBody->SetRestitution(0);
						
						// sphereBody->ApplyConstraint(Rigidbody::LOCK_ROTATION_X);
						// sphereBody->ApplyConstraint(Rigidbody::LOCK_ROTATION_Y);
						// sphereBody->ApplyConstraint(Rigidbody::LOCK_ROTATION_Z);
						




						// float zVal = flt z / flt amt;
						// float xVal = flt x / flt amt;
						// float yVal = flt y / flt amt;

						sphereRenderer->color = { aclMath::random(0,1),aclMath::random(0,1),aclMath::random(0,1)};

						sphereRenderer->roughness = aclMath::random(0,1);
						sphereRenderer->metallic = aclMath::random(0,1);

						sphereShader->AttachTexture(brdf, sphereShader->GetBindingByName("brdf"));
						sphereShader->AttachTexture(irradiance, sphereShader->GetBindingByName("irradiance"));
						sphereShader->AttachTexture(prefilter, sphereShader->GetBindingByName("prefilter"));
					}
				}
			}
			

			
			agl::aglShader* sphereShader = new agl::aglShader({ {"resources/shaders/fresnel/main.vert", "resources/shaders/fresnel/main.frag"}, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE });
			Ref<Entity> sphere = Scene::Current->entityMgr->CreateEntity("Player");
			MeshRenderer* sphereRenderer = sphere->AttachComponent<MeshRenderer>();
			sphereRenderer->material->shader = sphereShader;
			sphereRenderer->mesh = aglPrimitives::GetPrims()[aglPrimitives::CAPSULE];
			sphere->Transform.position = vec3{
				0,10,0
			};

			sphere->Transform.rotation = { 90,0,0 };
			sphere->Transform.scale = vec3{ 3 };

			auto sphereCollider = sphere->AttachComponent<CapsuleCollider>();

			sphereCollider->Height = 2;
			sphereCollider->Radius = 1;
			sphereCollider->Center = vec3{ 0,0,0 };

			CharacterController* sphereBody = sphere->AttachComponent<CharacterController>();

			sphereBody->Init();

			auto spherePlayer = sphere->AttachComponent<PlayerController>();

			spherePlayer->Init();

			//sphereBody->SetRestitution(0);

			// sphereBody->ApplyConstraint(Rigidbody::LOCK_ROTATION_X);
			// sphereBody->ApplyConstraint(Rigidbody::LOCK_ROTATION_Y);
			// sphereBody->ApplyConstraint(Rigidbody::LOCK_ROTATION_Z);





			// float zVal = flt z / flt amt;
			// float xVal = flt x / flt amt;
			// float yVal = flt y / flt amt;

			sphereRenderer->color = { aclMath::random(0,1),aclMath::random(0,1),aclMath::random(0,1) };

			sphereRenderer->roughness = aclMath::random(0, 1);
			sphereRenderer->metallic = aclMath::random(0, 1);

			sphereShader->AttachTexture(brdf, sphereShader->GetBindingByName("brdf"));
			sphereShader->AttachTexture(irradiance, sphereShader->GetBindingByName("irradiance"));
			sphereShader->AttachTexture(prefilter, sphereShader->GetBindingByName("prefilter"));

			/*
			for (int x = 0; x < amt; x++)
			{
				for (int y = 0; y < amt; y++)
				{
					agl::aglShader* sphereShader = new agl::aglShader({ {"resources/shaders/fresnel/main.vert", "resources/shaders/fresnel/main.frag"}, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE });
					Ref<Entity> sphere = Scene::Current->entityMgr->CreateEntity("Sphere");
					MeshRenderer* sphereRenderer = sphere->AttachComponent<MeshRenderer>();
					sphereRenderer->material->shader = sphereShader;
					sphereRenderer->mesh = aglPrimitives::GetPrims()[aglPrimitives::SPHERE];
					sphere->Transform.position = vec3{
						(x - (amt / 2)) * spacing,
						((y - (amt / 2)) * spacing),
						0,
					};


					//auto sphereCollider = sphere->AttachComponent<BoxCollider>();
					//Rigidbody* sphereBody = sphere->AttachComponent<Rigidbody>();
					//sphereBody->Init();
					//sphereBody->SetRestitution(0);

					float xVal = flt x / flt amt;
					float yVal = flt y / flt amt;

					sphereRenderer->color = { 1,0,0 };

					sphereRenderer->roughness = xVal;
					sphereRenderer->metallic = yVal;

					sphereShader->AttachTexture(brdf, sphereShader->GetBindingByName("brdf"));
					sphereShader->AttachTexture(irradiance, sphereShader->GetBindingByName("irradiance"));
					sphereShader->AttachTexture(prefilter, sphereShader->GetBindingByName("prefilter"));
				}
			}
			*/

			//agl::aglComputeShader* computeShader = new agl::aglComputeShader(agl::aglShaderSettings{{"","","","resources/shaders/compute/main.comp"}});

			//agl::aglStorageBuffer* storageBuffer = new agl::aglStorageBuffer(agl::aglBufferSettings{VK_SHADER_STAGE_COMPUTE_BIT, sizeof(TestSSBO)});
			//storageBuffer->AttachToShader(computeShader, computeShader->GetBindingByName("data"));

			//agl::aglTexture* computeTex = new agl::aglTexture({details->Width,details->Height,4, false, nullptr, true});

			//computeShader->AttachTexture(computeTex, computeShader->GetBindingByName("image"));


			//RaytracingManager rtm{};

			//rtm.Init();



			scene->Serialize("resources/scenes/TestScene.scene");
		}

	//scene = Scene::Load("resources/scenes/TestScene.scene", scene_creation_settings);


	aim::aimGamepad* gamepad =
		aim::CreateGamepad(0);

	scene->entityMgr->GetEntity("Player")->GetComponent<PlayerController>()->Gamepad = gamepad;
	//scene->entityMgr->GetEntity("Camera")->GetComponent<OrbitalCamera>()->Gamepad = gamepad;

	//scene->Load("resources/scenes/TestScene.scene", scene_creation_settings);

	agl_ext::InstallExtension<aglImGuiExtension>();
	ImGui::SetCurrentContext(aglImGuiExtension::GetContext());

	while (!agl::closeWindow)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			aglImGuiExtension::ProcessSDLEvent(&event);
			agl::PollEvent(event);
			aim::PollEvents(&event);
		}
		agl::event = &event;

		if (gamepad->triggerAxis.y > 0)
			gamepad->Rumble(1, 2);

		agl_ext::Refresh();


		if (gamepad->IsButtonPressed(SDL_CONTROLLER_BUTTON_X)) {
			for (auto registered_entity : Scene::Current->entityMgr->registeredEntities)
			{
				if (registered_entity->GetComponent<Rigidbody>())
				{
					if (registered_entity->GetComponent<Rigidbody>()->motionType == Rigidbody::STATIC)
						continue;
					registered_entity->GetComponent<Rigidbody>()->Reset();
				}
			}
		}

		if (gamepad->IsButtonPressed(SDL_CONTROLLER_BUTTON_DPAD_UP))
		{
			agl::aglShaderFactory::ReloadAllShaders();
		}

		Scene::Current->Update();

		aclPhysicsMgr::Update(agl::deltaTime * 2.0f);

		/*
		computeShader->BeginDispatchField();
		computeShader->Dispatch(agl::GetCurrentImage(), { 500/8,500/8,1 });
		computeShader->EndDispatchField();
		*/

		//rtm.Dispatch();
		agl::record_command_buffer(agl::GetCurrentImage());

		agl_ext::LateRefresh();

		agl::FinishRecordingCommandBuffer(agl::GetCurrentImage());

		aim::Refresh();

		agl::UpdateFrame();

		//ssbo = *(static_cast<TestSSBO*>(storageBuffer->GetData()));

		//ssbo.time = SDL_GetTicks64()/1000.0f;

	}

	aclPhysicsMgr::Uninitialize();

	agl_ext::UninstallAll();
	agl::Destroy();

	return 0;

}
