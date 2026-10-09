#include "pch.h"
#include <iostream>

App::App()
{
	s_pApp = this;
	CPU_CALLBACK_START(OnStart);
	CPU_CALLBACK_UPDATE(OnUpdate);
	CPU_CALLBACK_EXIT(OnExit);
	CPU_CALLBACK_RENDER(OnRender);
}

App::~App()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void App::SpawnMissile()
//{
//	cpu_entity* pMissile = cpuEngine.CreateEntity();
//	pMissile->pMesh = &m_meshMissile;
//	pMissile->transform.SetScaling(0.2f);
//	pMissile->transform.pos = m_pShip->GetEntity()->transform.pos;
//	pMissile->transform.SetRotation(m_pShip->GetEntity()->transform);
//	pMissile->transform.Move(1.5f);
//	m_missiles.push_back(pMissile);
//
//}

void App::OnStart()
{
	// YOUR CODE HERE
	// Render

#ifdef _DEBUG

	cpuEngine.EnableBoxRender();

#endif // DEBUG

	std::srand(std::time(0));

	// Resources
	m_font.Create(cpuDevice.GetHeight() <= 512 ? 14 : 28);
	m_textureBird.Load("bird_amiga.png");
	m_textureEarth.Load("earth.png");
	m_meshShip.CreateSpaceship();
	m_meshMissile.CreateSphere(0.5f);
	m_meshSphere.CreateSphere(2.0f, 8, 8);
	m_meshRail.CreateCylinder(0.5f,5.0f,60);
	m_meshDropRail.CreateTube(0.5f, 3.0f, 60);
	m_meshCube.CreateCube();
	m_meshCenter.CreateSphere();
	m_meshShadow.CreateCircle();
	m_pCenter = cpuEngine.CreateEntity();
	m_pRail = cpuEngine.CreateEntity();
	m_pDropRail = cpuEngine.CreateEntity();
	
	m_rts[0] = cpuEngine.CreateRT();

	m_base_material.color = cpu::ToColor(220, 220, 220);
	m_ball_color.color = cpu::ToColor(255, 0, 0);
	m_materialShadow.color = cpu::ToColor(0, 0, 0);

	// UI
	m_pSprite = cpuEngine.CreateSprite();
	m_pSprite->pTexture = &m_textureBird;
	m_pSprite->CenterAnchor();
	m_pSprite->x = 40;
	m_pSprite->y = 0;

	// Shader
	m_materialShip.color = cpu::ToColor(255, 128, 0);
	m_materialMissile.ps = MissileShader;
	m_materialMoon.ps = MoonShader;
	m_materialEarth.pTexture = &m_textureEarth;

	m_pDropRail->pMesh = &m_meshDropRail;
	m_pDropRail->pMaterial = &m_base_material;

	m_pDropRail->transform.pos.x = 0.0f;
	m_pDropRail->transform.pos.y = 40.0f;
	m_pDropRail->transform.pos.z = 0.0f;

	m_pRail->pMesh = &m_meshRail;
	m_pRail->pMaterial = &m_materialMoon;

	m_pRail->transform.pos.x = 0.0f;
	m_pRail->transform.pos.y = -1.7f;
	m_pRail->transform.pos.z = 0.0f;

#ifdef _DEBUG

	m_pCenter->pMesh = &m_meshCenter;
	m_pCenter->pMaterial = &m_base_material;

#endif // DEBUG

	m_pCenter->transform.pos.x = 0.0f;
	m_pCenter->transform.pos.y = 0.0f;
	m_pCenter->transform.pos.z = 0.0f;

	// 3D
	m_missileSpeed = 10.0f;

	// Ship
	m_pShip = new Ship;
	m_pShip->Create(&m_meshShip, &m_materialShip);
	m_pShip->GetFSM()->ToState(CPU_ID(StateShipIdle));

	ScheduleNextSpawn();

#ifdef _DEBUG

	cpuEngine.GetCamera()->transform.pos = XMFLOAT3(0.0f, 50.0f, 0.0f);

#else

	cpuEngine.GetCamera()->transform.pos = XMFLOAT3(0.0f, 30.0f, -20.0f);

#endif 
}

void App::OnUpdate()
{
	float dt = cpuTime.delta;
	float time = cpuTime.total;

	m_pSprite->y = 60 + cpu::RoundToInt(sinf(time) * 20.0f);

	m_speed = XM_PI * 0.5f;

	if (cpuInput.IsUp())
		cpuEngine.GetCamera()->transform.AddYPR(0.0f,dt * -m_speed,0.0f);
	if (cpuInput.IsDown())
		cpuEngine.GetCamera()->transform.AddYPR(0.0f,dt * m_speed,0.0f);
	if (cpuInput.IsLeft())
		m_velocity = -3.0f;
	else if (cpuInput.IsRight())
		m_velocity = 3.0f;
	else
		m_velocity = 0.0f;

	m_angle += m_velocity * dt;

	// Turn camera

#ifdef _DEBUG

	cpuEngine.GetCamera()->transform.LookAt(m_pCenter->transform.pos.x, m_pCenter->transform.pos.y, m_pCenter->transform.pos.z,CPU_VEC3_RIGHT);

#else

	cpuEngine.GetCamera()->transform.LookAt(m_pCenter->transform.pos.x, m_pCenter->transform.pos.y, m_pCenter->transform.pos.z, CPU_VEC3_UP);

#endif
	
	m_pShip->GetEntity()->transform.OrbitAroundAxis(m_pCenter->transform.pos, CPU_VEC3_UP, 3.0f, m_angle);
	m_pShip->GetEntity()->transform.LookAt(m_pCenter->transform.pos.x, m_pCenter->transform.pos.y, m_pCenter->transform.pos.z, CPU_VEC3_UP);

	// Spawn aléatoire
	m_spawnTimer += dt;
	if (m_spawnTimer >= m_nextSpawn)
	{
		SpawnMissileFromRing();
		ScheduleNextSpawn();
	}

	// Move missiles
	for (auto it = mm.GetToDestruct().begin(); it != mm.GetToDestruct().end(); ++it)
	{
		cpu_entity* pMissile = (*it).first;
		cpu_entity* pSHadow = (*it).second;
		pMissile->transform.Move(dt * m_missileSpeed);

		if (m_pShip->GetEntity()->aabb.Contains(pMissile->transform.pos))
		{
			m_score++;
			cpuEngine.Release(pMissile);
			cpuEngine.Release(pSHadow);
		}
		else if (pMissile->transform.pos.y <= m_pRail->transform.pos.y)
		{
			m_pv--;
			cpuEngine.Release(pMissile);
			cpuEngine.Release(pSHadow);
		}
		else if (pMissile->lifetime > 10.0f)
		{
			cpuEngine.Release(pMissile);
			cpuEngine.Release(pSHadow);
		}
			
	}

	// Purge missiles
	for (auto it = mm.GetToDestruct().begin(); it != mm.GetToDestruct().end(); )
	{
		if ((*it).first->dead)
			it = mm.GetToDestruct().erase(it);
		else
			++it;
	}

	// Quit
	if (cpuInput.IsBackPressed() || m_pv == 0)
		cpuEngine.Quit();
}

void App::OnExit()
{
	// YOUR CODE HERE
	if (m_pShip)
		m_pShip->Destroy();
	CPU_DELPTR(m_pShip);
	mm.GetToDestruct().clear();

}

void App::OnRender(int pass)
{
	switch (pass)
	{
	case CPU_PASS_PARTICLE_BEGIN:
	{
		// Blur particles
		//cpuEngine.SetRT(m_rts[0]);
		//cpuEngine.ClearColor();
		break;
	}
	case CPU_PASS_PARTICLE_END:
	{
		// Blur particles
		//cpuEngine.Blur(10);
		//cpuEngine.SetMainRT();
		//cpuEngine.AlphaBlend(m_rts[0]);
		break;
	}
	case CPU_PASS_UI_END:
	{
		// Debug
#ifdef _DEBUG

		cpu_stats& stats = *cpuEngine.GetStats();
		std::string info = CPU_STR(cpuTime.fps) + " fps, ";
		info += CPU_STR(stats.drawnTriangleCount) + " triangles, ";
		info += CPU_STR(stats.clipEntityCount) + " clipped entities\n";
		info += CPU_STR(cpuEngine.GetCamera()->transform.pos.x) + " : X, ";
		info += CPU_STR(cpuEngine.GetCamera()->transform.pos.y) + " : Y, ";
		info += CPU_STR(cpuEngine.GetCamera()->transform.pos.z) + " : Z ";

#else

		std::string info = CPU_STR(cpuTime.fps) + " fps, ";
		info += CPU_STR(m_score) + " : score, ";
		info += CPU_STR(m_pv) + " : vie";

#endif // _DEBUG

		if (m_pv <= 0)
		{
			info += " - GAME OVER";
			break;
		}
			


		// Ray cast
		cpu_ray ray;
		cpuEngine.GetCursorRay(ray);
		cpu_hit hit;
		cpu_entity* pEntity = cpuEngine.HitEntity(hit, ray);
		if (pEntity)
		{
			info += "\nHIT: ";
			info += CPU_STR(pEntity->index).c_str();
		}

		XMFLOAT3 tint = { 1.0f, 1.0f, 0.8f };
		cpuDevice.DrawText(&m_font, info.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), 10, CPU_TEXT_CENTER, &tint);
		break;
	}
	}
}

float App::RandRange(float a, float b)
{
	return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

void App::ScheduleNextSpawn()
{
	m_spawnTimer = 0.0f;
	m_nextSpawn = RandRange(m_spawnMin, m_spawnMax);
}

void App::SpawnMissileFromRing()
{
	float a = RandRange(0.0f, XM_2PI);
	float x = m_pDropRail->transform.pos.x + cosf(a) * m_spawnRadius;
	float z = m_pDropRail->transform.pos.z + sinf(a) * m_spawnRadius;
	float y = m_pDropRail->transform.pos.y;

	cpu_entity* pMissile = cpuEngine.CreateEntity();
	pMissile->pMesh = &m_meshMissile;
	pMissile->pMaterial = &m_materialMissile;
	pMissile->transform.SetScaling(1.0f);
	pMissile->transform.pos = XMFLOAT3(x, y, z);

	// Orienter le missile droit vers le bas
	pMissile->transform.LookAt(x, y - 10.0f, z, CPU_VEC3_RIGHT);

	cpu_entity* pShadow = cpuEngine.CreateEntity();
	pShadow->pMesh = &m_meshShadow;
	pShadow->pMaterial = &m_materialShadow;
	pShadow->transform.pos = XMFLOAT3(x, m_pRail->transform.pos.y + 0.5f, z);

	mm.GetToDestruct().push_back({ pMissile,pShadow });

}

void App::MyPixelShader(cpu_ps_io& io)
{
	// YOUR CODE HERE
	io.color = io.p.color;
}

void App::MissileShader(cpu_ps_io& io)
{
	// garder seulement le rouge du pixel éclairé
	io.color.x = io.p.color.x;
}

void App::MoonShader(cpu_ps_io& io)
{
	float time = cpuTime.total;
	float scale = ((sinf(time * 3.0f) * 0.5f) + 0.5f) * 0.5f + 0.5f;
	io.color.x = io.p.color.x * scale;
	io.color.y = io.p.color.y * scale;
	io.color.z = io.p.color.z;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Ship::Ship()
{
	m_pEntity = nullptr;
	m_pFSM = nullptr;
}

Ship::~Ship()
{
}

void Ship::Create(cpu_mesh* pMesh, cpu_material* pMaterial)
{
	m_pEntity = cpuEngine.CreateEntity();
	m_pEntity->pMesh = pMesh;
	m_pEntity->pMaterial = pMaterial;
	m_pEntity->transform.pos.z = 5.0f;
	m_pEntity->transform.pos.y = -3.0f;

	m_pFSM = cpuEngine.CreateFSM(this);
	m_pFSM->SetPostGlobal<StateShipGlobal>();
	m_pFSM->Add<StateShipIdle>();
}

void Ship::Destroy()
{
	m_pFSM = cpuEngine.Release(m_pFSM);
	m_pEntity = cpuEngine.Release(m_pEntity);
}

void Ship::Update()
{
	float dt = cpuTime.delta;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateShipGlobal::OnEnter(Ship& cur, int from, void* pParam)
{
}

void StateShipGlobal::OnExecute(Ship& cur)
{
	cur.Update();
}

void StateShipGlobal::OnExit(Ship& cur, int to)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateShipIdle::OnEnter(Ship& cur, int from, void* pParam)
{
}

void StateShipIdle::OnExecute(Ship& cur)
{
}

void StateShipIdle::OnExit(Ship& cur, int to)
{
}
