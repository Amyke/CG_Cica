#pragma once

// C++ includes
#include <memory>
#include <vector>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "gCamera.h"

#include "ProgramObject.h"
#include "BufferObject.h"
#include "VertexArrayObject.h"
#include "TextureObject.h"

#include "Object.h"
#include "Scene.h"

class PlayerCat;

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:
	// segédeljárások
	void TextureFromFileAttach(const char* filename, GLuint role) const;

    void createScene();

	// shaderekhez szükséges változók
	ProgramObject		m_program;			// shaderek programja
	ProgramObject		m_programSkybox;	// skybox shaderek

    Scene m_scene;

	gCamera				m_camera;
    bool                m_freeCamera = false;

    glm::vec3           m_light1;
    glm::vec3           m_light2;

	// nyers OGL azonosítók
	GLuint				m_skyboxTexture;

    PlayerCat*          m_player;
    std::vector<std::unique_ptr<Entity>> entities;
};

