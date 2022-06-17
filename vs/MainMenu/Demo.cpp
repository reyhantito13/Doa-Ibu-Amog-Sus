#include "Demo.h"


Demo::Demo()
{
}


Demo::~Demo()
{
	if (music != NULL) {
		Mix_FreeMusic(music);
	}
	Mix_CloseAudio();
}


void Demo::Init()
{
	BuildBackgroundGUI();
	BuildLogo();
	InitAudio();
	InitText();
	InitButton();
	this->program_Player = BuildShader("shader.vert", "shader.frag");
	InputMapping("SelectButton", SDLK_SPACE);
	InputMapping("SelectButton", SDLK_RETURN);
	InputMapping("NextButton", SDLK_RIGHT);
	InputMapping("PrevButton", SDLK_LEFT);
	InputMapping("BGM", SDLK_m);
}

void Demo::DeInit() {

}

void Demo::Update(float deltaTime)
{
	if (IsKeyDown("BGM")) {
		if (Mix_PlayingMusic() == 0)
		{
			//Play the music
			Mix_PlayMusic(music, -1);
			SDL_Delay(150);
		}
		//If music is being played
		else
		{
			//If the music is paused
			if (Mix_PausedMusic() == 1)
			{
				//Resume the music
				Mix_ResumeMusic();
				SDL_Delay(150);
			}
			//If the music is playing
			else
			{
				//Pause the music
				Mix_PauseMusic();
				SDL_Delay(150);
			}
		}
	}

	if (IsKeyDown("SelectButton")) {
		if (activeButtonIndex == 1) {
			SDL_Quit();
			exit(0);
		}
	}

	if (IsKeyUp("NextButton")) {
		activeButtonIndex++;
		SDL_Delay(150);

		if (activeButtonIndex > 1) {
				activeButtonIndex = 0;
				SDL_Delay(150);
			}
	}

	if (IsKeyUp("PrevButton")) {
		activeButtonIndex--;
		SDL_Delay(150);
		if (activeButtonIndex < 0) {
			activeButtonIndex = 1;
			SDL_Delay(150);
		}
	}

}

void Demo::Render()
{
	//Setting Viewport
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the background color
	//glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program_Player, "projection"), 1, GL_FALSE, value_ptr(projection));

	DrawBackgroundGUI();
	DrawLogo();
	RenderText("Press M to Play the Music", 10, 10, 1.0f, vec3(244.0f / 255.0f, 12.0f / 255.0f, 116.0f / 255.0f));

	RenderButton();
}

void Demo::InitAudio() {
	int flags = MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_OGG;
	if (flags != Mix_Init(flags)) {
		Err("Unable to initialize mixer: " + string(Mix_GetError()));
	}

	int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int audio_buffers = 4096;

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		Err("Unable to initialize audio: " + string(Mix_GetError()));
	}


	music = Mix_LoadMUS("test.ogg");
	if (music == NULL) {
		Err("Unable to load Music file: " + string(Mix_GetError()));
	}
}

void Demo::InitText() {
	// Init Freetype
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		Err("ERROR::FREETYPE: Could not init FreeType Library");
	}
	FT_Face face;
	if (FT_New_Face(ft, FONTNAME, 0, &face)) {
		Err("ERROR::FREETYPE: Failed to load font");
	}

	FT_Set_Pixel_Sizes(face, 0, FONTSIZE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture_Player
		GLuint texture_Player;
		glGenTextures(1, &texture_Player);
		glBindTexture(GL_TEXTURE_2D, texture_Player);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture_Player options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture_Player,
			ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO_Player);
	glGenBuffers(1, &VBO_Player);
	glBindVertexArray(VAO_Player);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Player);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL,
		GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Demo::RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
	// Activate corresponding render state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	UseShader(this->program_Player);

	glUniform3f(glGetUniformLocation(this->program_Player, "ourColor"), color.x, color.y, color.z);
	glUniform1i(glGetUniformLocation(this->program_Player, "text"), 1);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(this->program_Player, "ourTexture"), 0);
	mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(this->program_Player, "model"), 1, GL_FALSE, value_ptr(model));
	glBindVertexArray(VAO_Player);

	// Iterate through all characters
	string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		GLfloat xposPlayer = x + ch.Bearing.x * scale;
		GLfloat yposPlayer = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO_Player for each character

		GLfloat vertices[] = {
			// Positions   // Texture Coords
			xposPlayer + w,	yposPlayer + h,	1.0f, 1.0f, // Bottom Right
			xposPlayer + w,	yposPlayer,		1.0f, 0.0f, // Top Right
			xposPlayer,		yposPlayer,		0.0f, 0.0f, // Top Left
			xposPlayer,		yposPlayer + h,	0.0f, 1.0f  // Bottom Left 
		};
		// Render glyph texture_Player over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO_Player memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO_Player);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_QUADS, 0, 4);
		// Now advance cursors for next glyph (note that advance is number of 1 / 64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels(2 ^ 6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Demo::InitButton() {
	string buttons[NUM_BUTTON] = { "Play-Idle.png", "Quit-Idle.png"};
	string hover_buttons[NUM_BUTTON] = { "Play-Hover.png", "Quit-Hover.png"};

	glGenTextures(2, &texture_Player[0]);
	glGenTextures(2, &hover_texture[0]);

	for (int i = 0; i < NUM_BUTTON; i++) {
		// Load and create a texture_Player 
		glBindTexture(GL_TEXTURE_2D, texture_Player[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture_Player object

		// Set texture_Player filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture_Player 
		int width, height;
		unsigned char* image = SOIL_load_image(buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture_Player when done, so we won't accidentily mess up our texture_Player.

		// Set up vertex data (and buffer(s)) and attribute pointers
		button_width[i] = (float)width;
		button_height[i] = (float)height;

		// Load and create a texture_Player 
		glBindTexture(GL_TEXTURE_2D, hover_texture[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture_Player object

		// Set texture_Player filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		image = SOIL_load_image(hover_buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture_Player when done, so we won't accidentily mess up our texture_Player.

		 // Set up vertex data (and buffer(s)) and attribute pointers
		hover_button_width[i] = (float)width;
		hover_button_height[i] = (float)height;
	}

	GLfloat vertices[] = {
		// Positions	// Texture Coords
		1,  1,			1.0f, 1.0f, // Bottom Right
		1,  0,			1.0f, 0.0f, // Top Right
		0,  0,			0.0f, 0.0f, // Top Left
		0,  1,			0.0f, 1.0f  // Bottom Left 
	};


	glGenVertexArrays(1, &VAO_Enemy);
	glGenBuffers(1, &VBO_Enemy);
	glBindVertexArray(VAO_Enemy);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Enemy);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Demo::RenderButton() {
	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture_Player units
	int texture_id[NUM_BUTTON] = { GL_TEXTURE1, GL_TEXTURE2};
	// Activate shader
	UseShader(this->program_Player);
	glUniform1i(glGetUniformLocation(this->program_Player, "text"), 0);

	glBindVertexArray(VAO_Enemy);
	for (int i = 0; i < NUM_BUTTON; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, (activeButtonIndex == i) ? hover_texture[i] : texture_Player[i]);
		glUniform1i(glGetUniformLocation(this->program_Player, "ourTexture"), i + 1);

		mat4 model;
		model = translate(model, vec3((GetScreenWidth() / 2 - (2* button_width[i]) + (i + 1) * 250), (GetScreenHeight() - button_height[i] + 200) / 2, 0.0f));
		model = scale(model, vec3(button_width[i], button_height[i], 1));
		glUniformMatrix4fv(glGetUniformLocation(this->program_Player, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Demo::BuildBackgroundGUI()
{
	this->program_bgGUI = BuildShader("background.vert", "background.frag");
	UseShader(this->program_bgGUI);

	// Load and create a texture_Player
	glGenTextures(1, &texture_bgGUI);
	glBindTexture(GL_TEXTURE_2D, texture_bgGUI); // All upcoming GL_TEXTURE_2D operations now have effect on our texture_Player object

	// Set texture_Player filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture_Player
	int width, height;
	unsigned char* image = SOIL_load_image("Background.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture_Player when done, so we won't accidentily mess up our texture_Player.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width_bgGUI = (float)width;
	frame_height_bgGUI = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO_bgGUI);
	glGenBuffers(1, &VBO_bgGUI);
	glGenBuffers(1, &EBO_bgGUI);

	glBindVertexArray(VAO_bgGUI);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_bgGUI);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_bgGUI);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO_Player

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program_bgGUI, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos_bgGUI = GetScreenWidth() - frame_width_bgGUI;
	ypos_bgGUI = GetScreenHeight() - frame_height_bgGUI;

}

void Demo::DrawBackgroundGUI() {
	// Bind Textures using texture_Player units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_bgGUI);
	// Activate shader
	UseShader(this->program_bgGUI);
	glUniform1i(glGetUniformLocation(this->program_bgGUI, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos_bgGUI, ypos_bgGUI, 0.0f));
	// Scale sprite
	model = scale(model, vec3(frame_width_bgGUI, frame_height_bgGUI, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program_bgGUI, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO_bgGUI);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture_Player when done, so we won't accidentily mess up our texture_Player.
}

void Demo::BuildLogo()
{
	this->program_logo = BuildShader("logo.vert", "logo.frag");
	UseShader(this->program_logo);

	// Load and create a texture_Player
	glGenTextures(1, &texture_logo);
	glBindTexture(GL_TEXTURE_2D, texture_logo); // All upcoming GL_TEXTURE_2D operations now have effect on our texture_Player object

	// Set texture_Player filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture_Player
	int width, height;
	unsigned char* image = SOIL_load_image("logo.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture_Player when done, so we won't accidentily mess up our texture_Player.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width_logo = (float)width;
	frame_height_logo = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO_logo);
	glGenBuffers(1, &VBO_logo);
	glGenBuffers(1, &EBO_logo);

	glBindVertexArray(VAO_logo);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_logo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_logo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO_Player

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program_logo, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos_logo = (GetScreenWidth() - frame_width_logo) / 2;
	ypos_logo = (GetScreenHeight() - frame_height_logo) / 2 - 150;

}

void Demo::DrawLogo() {
	// Bind Textures using texture_Player units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_logo);
	// Activate shader
	UseShader(this->program_logo);
	glUniform1i(glGetUniformLocation(this->program_logo, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos_logo, ypos_logo, 0.0f));
	// Scale sprite
	model = scale(model, vec3(frame_width_logo, frame_height_logo, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program_logo, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO_logo);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture_Player when done, so we won't accidentily mess up our texture_Player.
}

int main(int argc, char** argv) {

	Engine::Game &game = Demo();
	game.Start("User Interface Demo", 1280, 720, true, WindowFlag::WINDOWED, 60, 1);

	return 0;
}
