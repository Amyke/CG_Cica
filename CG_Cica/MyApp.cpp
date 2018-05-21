#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>
#include <imgui/imgui.h>

struct Vertex
{
    glm::vec3 p;
    glm::vec3 c;
    glm::vec2 texCoord;
};


//henger kirajzol�sa
std::vector<Vertex> createCylinder(std::size_t definition) {
    const double radius = 0.5; //k�r sug�r

    std::vector<Vertex> vertices;

    const glm::vec3 bottom(0, 0, 0);
    vertices.push_back(Vertex{ bottom, bottom });

    for (std::size_t i = 0; i < definition; ++i){
        //k�rgy�r� egy pontja
        Vertex v;
        double u = i / static_cast<double>(definition); //vet�t�s 0-1 k�z�
        u *= 2 * M_PI; //teljes k�r rajzol�sa
        // alja
        v.p = glm::vec3(radius * std::cos(u), 0, radius * std::sin(u));
        v.c = glm::normalize(v.p);
        vertices.push_back(v);
        // teteje
        v.p += glm::vec3(0, 4, 0);
        v.c = glm::normalize(v.p);
        vertices.push_back(v);
    }

    const glm::vec3 top(0, 6, 0);
    vertices.push_back(Vertex{ top, top });

    return vertices;
}


std::vector<GLushort> createCylinderIndices(std::size_t definition) {
    const std::size_t definition2 = definition * 2;
    std::vector<GLushort> indices; 

    GLushort origin = 0; //k�rlap k�z�ppont
    GLushort top = definition2 + 1; //k�p cs�csa

    for (GLushort j = origin; j < definition2; j+=2)
    {
        //k�rlapon l�v� h�romsz�vek
        indices.push_back((j + 3) % definition2);
        indices.push_back(origin);
        indices.push_back(j + 1);


        //f�ls� cs�ccsal val� �sszek�t�s
        indices.push_back(j + 2);
        indices.push_back(top);
        indices.push_back(((j + 2) % definition2) + 2);

        //k�rlapok �sszek�t�se
        indices.push_back((j + 3) % definition2);
        indices.push_back(j + 1);
        indices.push_back(j + 2);

        indices.push_back((j + 3) % definition2);
        indices.push_back(j + 2);
        indices.push_back(((j + 2) % definition2) + 2);
    }

    return indices;
}


//n�gyzet pontok kirajzol�sa
std::vector<Vertex> createPlane(std::size_t definition)
{
    std::vector<Vertex> vertices;
    vertices.reserve(std::pow(definition + 1, 2));
    glm::vec3 color{ 0.0, 0.5, 0.8 };
    for (std::size_t z = 0; z <= definition; ++z)
    {
        float texU = (z % 75) / 74.0;
        for (std::size_t x = 0; x <= definition; ++x)
        {
            float texV = (x % 75) / 74.0;
            vertices.push_back({ glm::vec3{ x / 10.0, 1.0, z / 10.0 }, color, glm::vec2{ texU, texV } });
        }
    }
    return vertices;
}

//a cs�csok �sszek�t�se h�romsz�gek �ltal
std::vector<GLushort> createIndices(std::size_t definition)
{
    std::vector<GLushort> indices;
    indices.reserve(std::pow(definition + 1, 2) * 6);
    for (std::size_t z = 0; z < definition; ++z)
    {
        for (std::size_t x = 1; x <= definition; ++x)
        {
            GLushort me = z * (definition + 1) + x;
            GLushort nextCol = (z + 1) * (definition + 1) + x;

            indices.push_back(me);
            indices.push_back(me - 1);
            indices.push_back(nextCol - 1);

            indices.push_back(me);
            indices.push_back(nextCol - 1);
            indices.push_back(nextCol);
        }
    }
    return indices;
}


CMyApp::CMyApp(void)
{
	m_camera.SetView(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


CMyApp::~CMyApp(void)
{
	std::cout << "dtor!\n";
}

bool CMyApp::Init()
{
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)

	//
	// shaderek bet�lt�se
	//

	// a shadereket t�rol� program l�trehoz�sa az OpenGL-hez hasonl� m�don:
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

	// shader program r�vid l�trehoz�sa, egyetlen f�ggv�nyh�v�ssal a fenti h�rom:
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
	// geometria defini�l�sa (std::vector<...>) �s GPU pufferekbe (m_buffer*) val� felt�lt�se BufferData-val
	//

	// vertexek poz�ci�i:
	/*
	Az m_waterPlaneVertices konstruktora m�r l�trehozott egy GPU puffer azonos�t�t �s a most k�vetkez� BufferData h�v�s ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_waterPlaneVertices t�pusa ArrayBuffer) �s
	2. glBufferData seg�ts�g�vel �tt�lti a GPU-ra az argumentumban adott t�rol� �rt�keit

	*/
    const std::size_t waterPlaneDefinition = 200;
    const auto waterPlaneVertices = createPlane(waterPlaneDefinition);
	m_waterPlaneVertices.BufferData(waterPlaneVertices);
    m_waterPlaneIndices.BufferData(createIndices(waterPlaneDefinition));

    m_light1 = glm::vec3(waterPlaneVertices.front().p.x, 6, waterPlaneVertices.front().p.z);
    m_light2 = glm::vec3(waterPlaneVertices.back().p.x, 6.0, waterPlaneVertices.back().p.z);

	// geometria VAO-ban val� regisztr�l�sa
    m_waterPlaneVao.Init({
        {CreateAttribute<0, glm::vec3, 0, sizeof Vertex>, m_waterPlaneVertices},
        {CreateAttribute<1, glm::vec3, sizeof glm::vec3, sizeof Vertex>, m_waterPlaneVertices},
        {CreateAttribute<2, glm::vec2, 2 * sizeof glm::vec3, sizeof Vertex>, m_waterPlaneVertices}
    }, m_waterPlaneIndices);

    const std::size_t lighthouseDefinition = 100;
    m_lighthouseVertices.BufferData(createCylinder(lighthouseDefinition));
    m_lighthouseIndices.BufferData(createCylinderIndices(lighthouseDefinition));

    m_lighthouseVao.Init({
        { CreateAttribute<0, glm::vec3, 0, sizeof Vertex>, m_lighthouseVertices },
        { CreateAttribute<1, glm::vec3, sizeof glm::vec3, sizeof Vertex>, m_lighthouseVertices },
        { CreateAttribute<2, glm::vec2, 2 * sizeof glm::vec3, sizeof Vertex>, m_lighthouseVertices }
    }, m_lighthouseIndices);

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

    glGenTextures(1, &m_waterTexture);
    glBindTexture(GL_TEXTURE_2D, m_waterTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    TextureFromFileAttach("water.png", GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_waterNormalMap);
    glBindTexture(GL_TEXTURE_2D, m_waterNormalMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    TextureFromFileAttach("water_normal.png", GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::TextureFromFileAttach(const char* filename, GLuint role) const
{
	SDL_Surface* loaded_img = IMG_Load(filename);

	int img_mode = 0;

	if (loaded_img == 0)
	{
		std::cout << "[TextureFromFile] Hiba a k�p bet�lt�se k�zben: " << filename << std::endl;
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

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program.Use();

    glm::mat4 MVP = m_camera.GetViewProj();
	m_program.SetUniform("MVP", MVP);
    m_program.SetUniform("time", SDL_GetTicks() / 10'000.0f);

    glm::mat4 worldIT = glm::transpose(glm::inverse(glm::mat4(1)));
    m_program.SetUniform("worldIT", worldIT);

    m_program.SetUniform("eye_pos", m_camera.GetEye());
    glm::vec3 light1 = MVP * glm::vec4(m_light1, 1);
    glm::vec3 light2 = MVP * glm::vec4(m_light2, 1);
    m_program.SetUniform("light1_pos", light1);
    m_program.SetUniform("light2_pos", light2);

    m_lighthouseVao.Bind();
    m_program.SetUniform("mode", 0);
    m_program.SetUniform("use_texture", GL_FALSE);

    glDrawElements(GL_TRIANGLES, m_lighthouseIndices.sizeInBytes() / sizeof GLushort, GL_UNSIGNED_SHORT, nullptr);

    glm::mat4 tr = glm::translate(glm::vec3(m_light2.x, 0, m_light2.z));
    m_program.SetUniform("MVP", MVP * tr);
    glm::mat4 trIT = glm::transpose(glm::inverse(tr));
    m_program.SetUniform("worldIT", trIT);
    glDrawElements(GL_TRIANGLES, m_lighthouseIndices.sizeInBytes() / sizeof GLushort, GL_UNSIGNED_SHORT, nullptr);

    m_lighthouseVao.Unbind();

    // water plane
    m_waterPlaneVao.Bind();

    m_program.SetUniform("MVP", MVP);
    m_program.SetUniform("worldIT", worldIT);

    m_program.SetUniform("mode", 1);

    m_program.SetTexture("texture_", 0, m_waterTexture);
    m_program.SetTexture("normalMap", 1, m_waterNormalMap);

    m_program.SetUniform("use_texture", GL_TRUE);

    glDrawElements(GL_TRIANGLES, m_waterPlaneIndices.sizeInBytes() / sizeof GLushort, GL_UNSIGNED_SHORT, nullptr);

    m_waterPlaneVao.Unbind();

    m_program.Unuse();

	//TODO: skybox

	// ments�k el az el�z� Z-test eredm�nyt, azaz azt a rel�ci�t, ami alapj�n update-elj�k a pixelt.
	/*GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenl�t haszn�ljunk, mert mindent kitolunk a t�voli v�g�s�kokra
	glDepthFunc(GL_LEQUAL);

	m_vao.Bind();

	m_programSkybox.Use();
	m_programSkybox.SetUniform("MVP", m_camera.GetViewProj() * glm::translate( m_camera.GetEye()) );
	
	// cube map text�ra be�ll�t�sa 0-�s mintav�telez�re �s annak a shaderre be�ll�t�sa
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
	glUniform1i(m_programSkybox.GetLocation("skyboxTexture"), 0);

	// az el�z� h�rom sor <=> m_programSkybox.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// v�g�l �ll�tsuk vissza
	glDepthFunc(prevDepthFnc);
    */
    //TODO: GUI
	//ImGui Testwindow
	ImGui::ShowTestWindow();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
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

// a k�t param�terbe az �j ablakm�ret sz�less�ge (_w) �s magass�ga (_h) tal�lhat�
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}