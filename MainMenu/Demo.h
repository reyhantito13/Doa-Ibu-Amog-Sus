#ifndef DEMO_H
#define DEMO_H


#include <SOIL/SOIL.h>

#include <SDL/SDL_mixer.h>
#include <SDL/SDL_thread.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <map>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Game.h"

using namespace glm;

#define FONTSIZE 20
#define FONTNAME "hongkonghustle.ttf"
#define NUM_BUTTON 2

struct Character {
	GLuint TextureID; // ID handle of the glyph texture_Player
	ivec2 Size; // Size of glyph
	ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

class Demo :
	public Engine::Game
{
public:
	Demo();
	~Demo();
	virtual void Init();
	virtual void DeInit();
	virtual void Update(float deltaTime);
	virtual void Render();
	void InitAudio();
	float frame_width_bgGUI = 0, frame_height_bgGUI = 0, frame_width_logo = 0, frame_height_logo = 0;
private:
	Mix_Music* music = NULL;
	int sfx_channel = -1;
	float xpos_bgGUI = 0, ypos_bgGUI = 0, xpos_logo = 0, ypos_logo = 0;
	GLuint VBO_bgGUI, VAO_bgGUI, EBO_bgGUI, texture_bgGUI, program_bgGUI, VBO_logo, VAO_logo, EBO_logo, texture_logo, program_logo;
	void InitText();
	void RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
	void InitButton();
	void RenderButton();
	void BuildBackgroundGUI();
	void DrawBackgroundGUI();
	void BuildLogo();
	void DrawLogo();
	map<GLchar, Character> Characters;
	GLuint texture_Player[NUM_BUTTON], hover_texture[NUM_BUTTON], VBO_Player, VBO_Enemy, VAO_Player, VAO_Enemy, program_Player;
	float button_width[NUM_BUTTON], button_height[NUM_BUTTON], hover_button_width[NUM_BUTTON], hover_button_height[NUM_BUTTON];
	int activeButtonIndex = 0;
};
#endif

