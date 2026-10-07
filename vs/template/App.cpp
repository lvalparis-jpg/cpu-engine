#include "pch.h"

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

void App::SpawnMissile()
{
	cpu_entity* pMissile = cpuEngine.CreateEntity();
	pMissile->pMesh = &m_meshMissile;
	pMissile->transform.SetScaling(0.2f);
	pMissile->transform.pos = m_pShip->GetEntity()->transform.pos;
	pMissile->transform.SetRotation(m_pShip->GetEntity()->transform);
	pMissile->transform.Move(1.5f);
	m_missiles.push_back(pMissile);
}

void App::OnStart()
{
	// YOUR CODE HERE
	// 
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
	m_pCenter = cpuEngine.CreateEntity();
	m_pRail = cpuEngine.CreateEntity();
	m_pDropRail = cpuEngine.CreateEntity();
	
	m_rts[0] = cpuEngine.CreateRT();

	m_base_material.color = cpu::ToColor(220, 220, 220);
	m_ball_color.color = cpu::ToColor(255, 0, 0);

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
	/*m_pEarth = cpuEngine.CreateEntity();*/
	//m_pEarth->pMesh = &m_meshSphere;
	//m_pEarth->pMaterial = &m_materialEarth;
	/*m_pEarth->transform.pos.x = 3.0f;
	m_pEarth->transform.pos.y = 3.0f;
	m_pEarth->transform.pos.z = 5.0f;*/
	//m_pMoon = cpuEngine.CreateEntity();
	//m_pMoon->pMesh = &m_meshSphere;
	//m_pMoon->pMaterial = &m_materialMoon;
	//m_pMoon->transform.SetScaling(0.1f);

	

	//cpu_mesh mesh;
	//mesh.CreateCube(0.5f, CPU_RED);

	//cpu_material material;
	//material.color = cpu::ToColor(255, 128, 0);
	//cpu_entity* test = cpuEngine.CreateEntity();
	//test->pMesh = &mesh;
	//test->pMaterial = &material;
	//test->transform.pos.x = 3.0f;
	//test->transform.pos.y = 3.0f;
	//test->transform.pos.z = 5.0f;

	// Ship
	m_pShip = new Ship;
	m_pShip->Create(&m_meshShip, &m_materialShip);
	m_pShip->GetFSM()->ToState(CPU_ID(StateShipIdle));

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

	// Turn earth
	//m_pEarth->transform.AddYPR(-dt);

	// Move rock
	/*m_pMoon->transform.OrbitAroundAxis(m_pShip->GetEntity()->transform.pos, CPU_VEC3_RIGHT, 3.0f, time * 2.0f);*/
	/*m_pEmitter->pos = m_pMoon->transform.pos;
	m_pEmitter->dir = m_pMoon->transform.dir;
	m_pEmitter->dir.x = -m_pEmitter->dir.x;
	m_pEmitter->dir.y = -m_pEmitter->dir.y;
	m_pEmitter->dir.z = -m_pEmitter->dir.z;*/

	// Move Camera
	/*if (cpuInput.IsUp())
		cpuEngine.GetCamera()->transform.Move(dt * 1.0f);
	if (cpuInput.IsDown())
		cpuEngine.GetCamera()->transform.Move(-dt * 1.0f);
	if (cpuInput.IsLeft())
		cpuEngine.GetCamera()->transform.AddYPR(-dt * XM_PI);
	if (cpuInput.IsRight())
		cpuEngine.GetCamera()->transform.AddYPR(dt * XM_PI);*/

	m_speed = XM_PI * 0.5f;

	if (cpuInput.IsUp())
		cpuEngine.GetCamera()->transform.AddYPR(0.0f,dt * -m_speed,0.0f);
	if (cpuInput.IsDown())
		cpuEngine.GetCamera()->transform.AddYPR(0.0f,dt * m_speed,0.0f);
	if (cpuInput.IsLeft())
		m_velocity = -2.0f;
	else if (cpuInput.IsRight())
		m_velocity = 2.0f;
	else
		m_velocity = 0.0f;

	m_angle += m_velocity * dt;

	// Turn camera

#ifdef _DEBUG

	cpuEngine.GetCamera()->transform.LookAt(m_pCenter->transform.pos.x, m_pCenter->transform.pos.y, m_pCenter->transform.pos.z,CPU_VEC3_RIGHT);

#else

	//cpuEngine.GetCamera()->transform.OrbitAroundAxis(m_pShip->GetEntity()->transform.pos, CPU_VEC3_UP, 10.0f, m_angle);
	cpuEngine.GetCamera()->transform.LookAt(m_pCenter->transform.pos.x, m_pCenter->transform.pos.y, m_pCenter->transform.pos.z, CPU_VEC3_UP);

#endif
	
	m_pShip->GetEntity()->transform.OrbitAroundAxis(m_pCenter->transform.pos, CPU_VEC3_UP, 3.0f, m_angle);
	m_pShip->GetEntity()->transform.LookAt(m_pCenter->transform.pos.x, m_pCenter->transform.pos.y, m_pCenter->transform.pos.z, CPU_VEC3_UP);
	//cpuEngine.GetCamera()->transform.LookAt(m_pShip->GetEntity()->transform.pos.x, cpuEngine.GetCamera()->transform.pos.y, m_pShip->GetEntity()->transform.pos.z, CPU_VEC3_UP);
	

	// Move missiles
	for (auto it = m_missiles.begin(); it != m_missiles.end(); ++it)
	{
		cpu_entity* pMissile = *it;
		pMissile->transform.Move(dt * m_missileSpeed);
		if (pMissile->transform.pos.y <= m_pDropRail->transform.pos.y)
		{
			m_pv = m_pv - 1;
			cpuEngine.Release(pMissile);
		}
		else if (m_pShip->GetEntity()->aabb.Contains(pMissile->transform.pos))
		{
			m_score = m_score + 1;
			cpuEngine.Release(pMissile);
		}
		else if (pMissile->lifetime > 10.0f)
			cpuEngine.Release(pMissile);
	}

	// Purge missiles
	for (auto it = m_missiles.begin(); it != m_missiles.end(); )
	{
		if ((*it)->dead)
			it = m_missiles.erase(it);
		else
			++it;
	}

	// Quit
	if (cpuInput.IsBackPressed())
		cpuEngine.Quit();
}

void App::OnExit()
{
	// YOUR CODE HERE
	if (m_pShip)
		m_pShip->Destroy();
	CPU_DELPTR(m_pShip);
	m_missiles.clear();
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
		/*info += CPU_STR(m_missiles.size()) + " missiles, ";
		info += CPU_STR(cpuEngine.GetParticleData()->alive) + " particles, ";*/
		/*info += CPU_STR(stats.threadCount) + " threads, ";
		info += CPU_STR(stats.tileCount) + " tiles";*/
		info += CPU_STR(cpuEngine.GetCamera()->transform.pos.x) + " : X, ";
		info += CPU_STR(cpuEngine.GetCamera()->transform.pos.y) + " : Y, ";
		info += CPU_STR(cpuEngine.GetCamera()->transform.pos.z) + " : Z ";

#else

		std::string info = CPU_STR(cpuTime.fps) + " fps, ";
		info += CPU_STR(m_score) + " : score, ";
		info += CPU_STR(m_pv) + " : vie";

#endif // _DEBUG

		

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

	// Turn ship
	//m_pEntity->transform.AddYPR(dt, dt, dt);

	// Move ship
	//m_pEntity->transform.pos.z += dt * 1.0f;

	// Fire
	/*if (cpuInput.vi.IsKey(VK_SPACE))
		cpuApp.SpawnMissile();*/
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
