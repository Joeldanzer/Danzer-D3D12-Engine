#pragma once

class Engine;

class Game
{
public:
	Game(Engine& engine);
	~Game();

	//Main update loop for game
	void Update(const float deltaTime);

private:
	class Impl;
	Impl* m_impl;
};


