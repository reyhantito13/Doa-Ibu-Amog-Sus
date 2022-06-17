#ifndef DEMO_H
#define DEMO_H

#include "Game.h"
#include "Setting.h"
#include "Texture.h"
#include "Sprite.h"
#include "Music.h"
#include "Sound.h"
#include "Text.h"
#include "Scene.h"

namespace Engine {
	class Demo :
		public Engine::Game
	{
	public:
		Demo(Setting* setting);
		~Demo();
		virtual void Init();
		virtual void Update();
		virtual void Render();

	private:
		Texture* backgroundTexture = NULL;
		Texture* platformTexture = NULL;
		Texture* playerTexture = NULL;
		Texture* dotTexture = NULL;
		Sprite* backgroundSprite = NULL;
		Sprite* platformSprite1 = NULL;
		Sprite* platformSprite2 = NULL;
		Sprite* platformSprite3 = NULL;
		Sprite* playerSprite = NULL;
		Sprite* dotSprite1 = NULL;
		Sprite* dotSprite2 = NULL;
		Sprite* dotSprite3 = NULL;
		Sprite* dotSprite4 = NULL;
		Music* music = NULL;;
		Sound* sound = NULL;
		Text* text = NULL;

	};
}
#endif

