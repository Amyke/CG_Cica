#include <GL\glew.h>
#include <GL\GL.h>
#include "TextureObject.h"

#include <SDL.h>
#include <SDL_image.h>

template <TextureType type, bool Repeat>
inline TextureObject<type, Repeat>::TextureObject()
{
	glGenTextures(1, &m_id);
}

template <TextureType type, bool Repeat>
inline TextureObject<type, Repeat>::TextureObject(const std::string &s)
{
    glGenTextures(1, &m_id);
	AttachFromFile(s);
}

template <TextureType type, bool Repeat>
inline TextureObject<type, Repeat>::~TextureObject()
{
	Clean();
}

template <TextureType type, bool Repeat>
inline TextureObject<type, Repeat>::TextureObject(TextureObject && rhs)
{
	if (&rhs == this)
		return;

	m_id = rhs.m_id;
	rhs.m_id = 0;
}

template <TextureType type, bool Repeat>
inline TextureObject<type, Repeat> & TextureObject<type, Repeat>::operator=(TextureObject && rhs)
{
	if (&rhs == this)
		return *this;

	m_id = rhs.m_id;
	rhs.m_id = 0;

	return *this;
}

template <TextureType type, bool Repeat>
inline TextureObject<type, Repeat>& TextureObject<type, Repeat>::operator=(const std::string & s)
{
	AttachFromFile(s);
	return *this;
}

template <TextureType type, bool Repeat>
inline void TextureObject<type, Repeat>::AttachFromFile(const std::string& filename, bool generateMipMap, GLuint role)
{
	SDL_Surface* loaded_img = IMG_Load(filename.c_str());

	int img_mode = 0;

	if (loaded_img == 0)
	{
		std::cerr << "[AttachFromFile] Error loading image file " << filename << std::endl;
		return;
	}

	if (loaded_img->format->BytesPerPixel == 4)
		img_mode = GL_RGBA;
	else
		img_mode = GL_RGB;

	glBindTexture(static_cast<GLenum>(type), m_id);

    if (Repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else if (generateMipMap) {
        glGenerateMipmap(static_cast<GLenum>(type));
    }

	glTexParameteri(static_cast<GLenum>(type), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(static_cast<GLenum>(type), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        static_cast<GLenum>(type),		// melyik binding point-on van a textúra erõforrás, amihez tárolást rendelünk
        0,								// melyik részletességi szint adatait határozzuk meg
        img_mode,							// textúra belsõ tárolási formátuma (GPU-n)
        loaded_img->w, loaded_img->h,	// szélesség, magasság
        0,								// nulla kell, hogy legyen ( https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml )
        img_mode,						// forrás (=CPU-n) formátuma
        GL_UNSIGNED_BYTE,				// forrás egy pixelének egy csatornáját hogyan tároljuk
        loaded_img->pixels);			// forráshoz pointer

    glBindTexture(static_cast<GLenum>(type), 0);

	SDL_FreeSurface(loaded_img);
}

template <TextureType type, bool Repeat>
inline void TextureObject<type, Repeat>::FromFile(const std::string& s)
{
	AttachFromFile(s);
}

template <TextureType type, bool Repeat>
inline void TextureObject<type, Repeat>::Clean()
{
	if (m_id != 0)
	{
		glDeleteTextures(1, &m_id);
		m_id = 0;
	}
}
