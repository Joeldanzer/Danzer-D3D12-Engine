#pragma once
#include "Components/Sprite.h"
#include "Components/Text.h"

#include "Fonts/Font.h"
#include "SpriteData.h"

class DirectX12Framework;
class TextureHandler;

class SpriteHandler
{
public:
	SpriteHandler() = delete;
	SpriteHandler(DirectX12Framework& framework, TextureHandler& textureHandler) :
		m_framework(framework),
		m_textureHandler(textureHandler)
	{}
	~SpriteHandler();
	
	Sprite CreateSpriteFromSheet(std::string sheetName, UINT frame);
	Text CreateTextFromFont(std::string fontName);

	UINT GetCreatedSpriteSheet(const std::string sheetName) {
		for (int i = 0; i < m_sprites.size(); i++)
			if (m_sprites[i].Name() == sheetName)
				return i;	
	}

	SpriteData& const GetSprite(UINT id) {
		return m_sprites[id - 1];
	}

	Font& const GetFont(UINT id) {
		return m_fonts[id - 1];
	}

	// Creates a Sprite sheet with a certain number of frames, 
	void CreateSpriteSheet(std::wstring sprite, const UINT widthFrames = 1, const UINT heightFrames = 1); 
	//void CreateSpriteSheet(std::wstring sprite, UINT numberOfFrames, UINT sizeOfEachFrame);
	void LoadFont(std::string fontJSON);

	std::vector<SpriteData>& GetAllSprites() {
		return m_sprites;
	}

	std::vector<Font>& GetAllFonts() {
		return m_fonts;
	}

private:

	UINT GetLoadedSpriteAndFrame(const std::string sprite, const UINT frame);
	UINT GetLoadedFont(std::string fontName);

	void AddNewlyCreatedFont(Font font);
	void AddNewlyCreatedSprite(SpriteData sprite);

	std::string SetSpriteName(std::wstring spritePath);
	
	std::vector<SpriteData> m_sprites;
	std::vector<Font>		m_fonts;

	DirectX12Framework& m_framework;
	TextureHandler&     m_textureHandler;
};

