#include "MyApp.h"

#include <cmath>
#include <vector>
#include <array>
#include <list>
#include <tuple>

#include <imgui/imgui.h>

#include "Cat.h"
#include "ObjectFactory.h"
#include "ObjParser_OGL3.h"
#include "Turret.h"

template<typename T>
void setLightUniform(ProgramObject& shader, int i, std::string name, const T& value);

struct SetLightPosition : Node {
    SetLightPosition(int i_, const Light& light_, const Cat& cat_)
        : i(i_), light(light_), cat(cat_) {
    }

    void render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) {
        glm::mat4 MVP = vp * m;
        glm::vec3 position = MVP * glm::vec4(light.position, 1);
        setLightUniform(shader, i, "position", position);
        glm::vec3 direction = MVP * glm::vec4(cat.getDirection(), 1);
        setLightUniform(shader, i, "coneDirection", direction);
    }

    int i;
    const Light& light;
    const Cat& cat;
};


CMyApp::CMyApp(void)
    : m_scene(std::make_unique<Node>())
{
	m_camera.SetView(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


CMyApp::~CMyApp(void)
{
	std::cout << "dtor!\n";
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	//
	// shaderek betöltése
	//

	// a shadereket tároló program létrehozása az OpenGL-hez hasonló módon:
	m_program.AttachShaders({
		{ GL_VERTEX_SHADER, "myVert.vert"},
		{ GL_FRAGMENT_SHADER, "myFrag.frag"}
	});

	// attributomok osszerendelese a VAO es shader kozt
	m_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_col" },				// VAO 1-es csatorna menjen a vs_in_col-ba
        { 2, "vs_in_texture" }
	});

	m_program.LinkProgram();

	// shader program rövid létrehozása, egyetlen függvényhívással a fenti három:
	m_programSkybox.Init(
	{
		{ GL_VERTEX_SHADER, "skybox.vert" },
		{ GL_FRAGMENT_SHADER, "skybox.frag" }
	},
	{
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
	}
	);


	//
	// geometria definiálása (std::vector<...>) és GPU pufferekbe (m_buffer*) való feltöltése BufferData-val
	//

	// vertexek pozíciói:
	/*
	Az m_waterPlaneVertices konstruktora már létrehozott egy GPU puffer azonosítót és a most következõ BufferData hívás ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_waterPlaneVertices típusa ArrayBuffer) és
	2. glBufferData segítségével áttölti a GPU-ra az argumentumban adott tároló értékeit

	*/

    m_lights = {
        Light{
            glm::vec3{ -20, 6, -20 },
            glm::vec3{ 0.4, 0.4, 0.4 },
            glm::vec3{ 0.8, 0.8, 0.2 },
            glm::vec3{ 0.0, 0.6, 0.2 },
            0.0f,
            1.0f,
            180.0f,
            glm::vec3{ 1, 1, 1 }
        },
        Light{
            glm::vec3{ 20, 6, 20 },
            glm::vec3{ 0.2, 0.2, 0.2 },
            glm::vec3{ 0.2, 0.2, 0.5 },
            glm::vec3{ 1.0, 0.2, 0.2 },
            0.0f,
            1.0f,
            180.0f,
            glm::vec3{ 1, 1, 1 }
        },
        Light{
            glm::vec3{ 1, 1, 1 },
            glm::vec3{ 1, 1, 1 },
            glm::vec3{ 0, 1, 0 },
            glm::vec3{ 0.0, 1.2, 0 },
            0.0f,
            0.0f,
            30.0f,
            glm::vec3{ -1, -1, -1 }
        }
    };

	// skybox
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenTextures(1, &m_skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	TextureFromFileAttach("xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	TextureFromFileAttach("xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	TextureFromFileAttach("ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	TextureFromFileAttach("yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	TextureFromFileAttach("zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	TextureFromFileAttach("zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

    std::vector<glm::vec3> directions{
        { 1, 0, 0 },
        { 1, 0, 1 },
        { 1, 0, -1 },
        { 0, 0, 1 }
    };
    for (auto dir : directions) {
        auto cat = std::make_unique<AICat>();
        cat->change_direction(dir);
        entities.emplace_back(std::move(cat));
    }
    auto playerCat = std::make_unique<PlayerCat>();
    m_player = playerCat.get();
    entities.emplace_back(std::move(playerCat));

    createScene();

	return true;
}

void CMyApp::TextureFromFileAttach(const char* filename, GLuint role) const
{
	SDL_Surface* loaded_img = IMG_Load(filename);

	int img_mode = 0;

	if (loaded_img == 0)
	{
		std::cout << "[TextureFromFile] Hiba a kép betöltése közben: " << filename << std::endl;
		return;
	}

	if (loaded_img->format->BytesPerPixel == 4)
		img_mode = GL_RGBA;
	else
		img_mode = GL_RGB;

	glTexImage2D(role, 0, img_mode, loaded_img->w, loaded_img->h, 0, img_mode, GL_UNSIGNED_BYTE, loaded_img->pixels);

	SDL_FreeSurface(loaded_img);
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_skyboxTexture);
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

    for (auto& entity : entities) {
        entity->update(delta_time);
    }
    if (!m_freeCamera) {
        m_camera.Follow(m_player->position);
    }
	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

template<typename T>
void setLightUniform(ProgramObject& shader, int i, std::string name, const T& value) {
    std::string uniformName = "lights[" + std::to_string(i) + "]." + name;
    shader.SetUniform(uniformName.c_str(), value);
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program.Use();

    m_program.SetUniform("time", SDL_GetTicks() / 10'000.0f);
    m_program.SetUniform("eye_pos", m_camera.GetEye());

    glm::mat4 MVP = m_camera.GetViewProj();

    m_program.SetUniform("num_lights", static_cast<int>(m_lights.size()));
    for (int i = 0; i < m_lights.size(); ++i) {
        const auto& light = m_lights[i];
        glm::vec3 position = MVP * glm::vec4(light.position, 1);
        setLightUniform(m_program, i, "position", position);
        setLightUniform(m_program, i, "ambientIntensity", light.ambientIntensity);
        setLightUniform(m_program, i, "diffuseIntensity", light.diffuseIntensity);
        setLightUniform(m_program, i, "specularIntensity", light.specularIntensity);
        setLightUniform(m_program, i, "attenuation", light.attenuation);
        setLightUniform(m_program, i, "ambientCoefficient", light.ambientCoefficient);
        setLightUniform(m_program, i, "coneAngle", light.coneAngle);
        setLightUniform(m_program, i, "coneDirection", light.coneDirection);
    }

    m_scene.render(m_program, m_camera.GetProj(), m_camera.GetViewMatrix());

    m_program.Unuse();

	//TODO: skybox

	// mentsük el az elõzõ Z-test eredményt, azaz azt a relációt, ami alapján update-eljük a pixelt.
	/*GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenlõt használjunk, mert mindent kitolunk a távoli vágósíkokra
	glDepthFunc(GL_LEQUAL);

	m_vao.Bind();

	m_programSkybox.Use();
	m_programSkybox.SetUniform("MVP", m_camera.GetViewProj() * glm::translate( m_camera.GetEye()) );
	
	// cube map textúra beállítása 0-ás mintavételezõre és annak a shaderre beállítása
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
	glUniform1i(m_programSkybox.GetLocation("skyboxTexture"), 0);

	// az elõzõ három sor <=> m_programSkybox.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// végül állítsuk vissza
	glDepthFunc(prevDepthFnc);
    */
    //TODO: GUI
	//ImGui Testwindow
	ImGui::ShowTestWindow();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
    if (m_freeCamera) {
        m_camera.KeyboardDown(key);
    } else {
        m_player->KeyboardDown(key);
    }
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
    if (key.keysym.sym == SDLK_SPACE) {
        m_freeCamera = !m_freeCamera;
        return;
    }

    if (m_freeCamera) {
        m_camera.KeyboardUp(key);
    } else {
        m_player->KeyboardUp(key);
    }
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
    m_camera.MouseMove(mouse);
}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}

std::shared_ptr<Node> createWaterPlane() {
    const std::size_t waterPlaneDefinition = 200;
    Object waterPlaneObj = ObjectFactory::createPlane(waterPlaneDefinition);
    waterPlaneObj.texture = std::make_unique<TextureObject<>>("water.png");
    waterPlaneObj.normal_map = std::make_unique<TextureObject<>>("water_normal.png");

    auto waterMode = std::make_shared<ShaderModeNode>(ShaderMode::Water);

    auto waterScale = std::make_shared<TransformationNode>(
        glm::scale(glm::vec3(30.0, 1.0, 30.0))
    );
    auto waterplane = std::make_shared<ObjectNode>(std::move(waterPlaneObj));
    waterScale->add_child(std::move(waterplane));
    waterMode->add_child(std::move(waterScale));

    return waterMode;
}

std::shared_ptr<Node> createMountainRange() {
    Object mountainPlaneObject = ObjectFactory::createMountain(200);
    mountainPlaneObject.normal_map = std::make_unique<TextureObject<>>("mountain_normal.bmp");
    auto mountainPlane = std::make_shared<ObjectNode>(std::move(mountainPlaneObject));

    auto mountainScale = std::make_shared<TransformationNode>(
        glm::scale(glm::vec3(1, 1, 1)) *
        glm::scale(glm::vec3(40, 1, 40))
    );
    mountainScale->add_child(std::move(mountainPlane));
    return mountainScale;
}

std::shared_ptr<Node> createLighthouse() {
    const std::size_t lighthouseDefinition = 100;
    Object lighthouseObj = ObjectFactory::createLighthouse(lighthouseDefinition);
    return std::make_shared<ObjectNode>(std::move(lighthouseObj));
}

std::shared_ptr<Node> createCica() {
    auto cicaObj = ObjParser::parse("cat.obj");
    auto cica = std::make_shared<ObjectNode>(std::move(cicaObj));
    auto cicaPosition = std::make_shared<TransformationNode>(
        glm::translate(glm::vec3(0, -0.75, 0)) *
        glm::scale(glm::vec3(3, 3, 3))
    );
    cicaPosition->add_child(cica);
    return cicaPosition;
}

std::shared_ptr<Node> createTurret() {
    auto turretBodyObj = ObjectFactory::createTurretBody(6);
    auto turretBarrelObj = ObjectFactory::createTurretBarrel(50);

    auto turretBarrelRot = std::make_shared<TransformationNode>(
        glm::translate(glm::vec3(0, 3.5, 0)) *
        glm::scale(glm::vec3(0.75, 0.75, 0.75)) *
        glm::rotate<float>(-0.5 * M_PI, glm::vec3(0, 0, 1))
    );
    turretBarrelRot->add_child(std::make_shared<ObjectNode>(std::move(turretBarrelObj)));

    auto turretNode = std::make_shared<TransformationNode>(
        glm::scale(glm::vec3(0.20, 0.20, 0.20)) *
        glm::translate(glm::vec3(0, 1, 0))
    );
    turretNode->add_child(turretBarrelRot);
    turretNode->add_child(std::make_shared<ObjectNode>(std::move(turretBodyObj)));

    return turretNode;
}

void CMyApp::createScene() {
    m_scene.root->add_child(createWaterPlane());

    auto normalMode = std::make_shared<ShaderModeNode>(ShaderMode::Normal);

    normalMode->add_child(createMountainRange());

    auto lighthouse = createLighthouse();

    auto light1 = m_lights[0].position;
    auto transformLB = std::make_shared<TransformationNode>(
        glm::translate(glm::vec3(light1.x, 0, light1.z))
    );
    transformLB->add_child(lighthouse);
    normalMode->add_child(std::move(transformLB));

    auto light2 = m_lights[1].position;
    auto transformRT = std::make_shared<TransformationNode>(
        glm::translate(glm::vec3(light2.x, 0, light2.z))
    );
    transformRT->add_child(lighthouse);
    normalMode->add_child(std::move(transformRT));

    auto cica = createCica();
    auto turret = createTurret();

    for (const auto& entity : entities) {
        auto cicaEnt = dynamic_cast<const Cat*>(entity.get());
        if (cicaEnt == nullptr) {
            continue;
        }
        auto cicaNode = std::make_shared<EntityNode>(*cicaEnt);
        cicaNode->add_child(cica);

        auto turretEnt = std::make_shared<EntityNode>(cicaEnt->getTurret());
        turretEnt->add_child(turret);
        cicaNode->add_child(turretEnt);
        if (dynamic_cast<const PlayerCat*>(cicaEnt) != nullptr) {
            cicaNode->add_child(std::make_shared<SetLightPosition>(2, m_lights[2], *cicaEnt));
        }
        normalMode->add_child(cicaNode);
    }

    m_scene.root->add_child(std::move(normalMode));
}
