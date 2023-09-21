#include "stdafx.h"
#include "SpriteHandler.h"

#include "../../Core/DirectX12Framework.h"

#include "../3rdParty/rapidjson/include/rapidjson/document.h"

#include "Rendering/TextureHandler.h"
#include "../RenderUtility.h"

#pragma comment(lib, "Ws2_32.lib")
#include <fstream>

SpriteHandler::~SpriteHandler()
{
 
}

Sprite SpriteHandler::CreateSpriteFromSheet(std::string sheetName, UINT frame)
{
    Sprite sprite;

    sprite.m_spriteSheet = GetLoadedSpriteAndFrame(sheetName, frame);
    sprite.m_frame = frame;

    return sprite;
}

Text SpriteHandler::CreateTextFromFont(std::string fontName)
{
    Text text;
    text.m_fontID = GetLoadedFont(fontName);
    return text;
}

// All spritesheets should be initialized at the beginning of the program.
void SpriteHandler::CreateSpriteSheet(std::wstring sprite, const UINT widthFrames, const UINT heightFrames)
{
    SpriteData::Sheet spriteSheet; 
    UINT spriteID = m_textureHandler.GetTexture(sprite);

    spriteSheet.m_texture = spriteID;

    TextureHandler::Texture& texture = m_textureHandler.GetTextureData(spriteID);

    // Get Width and Height of sprite sheet
    std::ifstream file(sprite);
    UINT width, height;

    //// Width & height are 4-byte integer starting at offset 12 to 16
    file.seekg(12);
    file.read((char*)&width,  4);
    file.read((char*)&height, 4);
    //file.seekg(16);

    spriteSheet.m_width   = width;
    spriteSheet.m_height  = height;
    spriteSheet.m_texture = spriteID;

    // Get new width and height for each fram on the sheet
    UINT newWidth  = width  / widthFrames;
    UINT newHeight = height / heightFrames;

    UINT i = 0;

    for (UINT y = 0; y < heightFrames; y++)
    {
        float newY = newHeight * y;
        for (UINT x = 0; x < widthFrames; x++)
        {
            float newX = newWidth * x;
            SpriteData::Frame frame;
            frame.m_framePosition = { newX, newY };
            frame.m_height = (UINT)newHeight;
            frame.m_width =  (UINT)newWidth;
            spriteSheet.m_frames.emplace_back(frame);
            i++;
        } 
    }

   AddNewlyCreatedSprite(SpriteData(spriteSheet, m_framework.GetDevice(), SetSpriteName(sprite)));
}

void SpriteHandler::LoadFont(std::string fontJSON)
{  
    
    rapidjson::Document doc;

    char* str = nullptr;

    FILE* f = nullptr;
    fopen_s(&f, fontJSON.c_str(), "rb");
    // Allocate sufficient memory

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    str = (char*)malloc(length + 1);

    // Read entire file
    rewind(f);
    fread(str, sizeof(char), length, f);
    str[length] = '\0';

    fclose(f);

    doc.Parse(str);

    Font::Data data;
    std::string texturePath;
    //if (doc.IsObject()) {
    texturePath = doc["Texture"].GetString();

    data.m_width = doc["Width"].GetUint();
    data.m_height = doc["Height"].GetUint();

    auto letterObj = doc["Letters"].GetObjectW();

    UINT letterWidth = letterObj["Width"].GetUint();
    UINT letterHeight = letterObj["Height"].GetUint();

    UINT x = letterObj["StartX"].GetUint() + 3;
    UINT y = letterObj["StartY"].GetUint() + 3;

    auto jsonArray = letterObj["Chars"].GetArray();
    for (UINT i = 0; i < jsonArray.Size(); i++)
    {
        char letter = *jsonArray[i].GetString();

        if (&letter != "") {
            Font::Letter letterData(letter, { (float)x, (float)y }, {(float)letterWidth, (float)letterHeight});
            
            data.m_letters.emplace(letter, letterData);

        }
 
        if (x >= data.m_width  + 3 - letterWidth) {
            y += letterHeight;
            x = 3;
            x = 3;
        }
        else {
            x += letterWidth;
        }

    }

    UINT textureID = m_textureHandler.GetTexture({ texturePath.begin(), texturePath.end() });
    if (textureID == 0) {
        m_framework.ResetCommandListAndAllocator(nullptr, L"SpriteHandler: Line 153");
        textureID = m_textureHandler.CreateTexture({texturePath.begin(), texturePath.end()});
        m_textureHandler.LoadAllCreatedTexuresToGPU();
    }

    data.m_texture = textureID;

    AddNewlyCreatedFont(Font(data, m_framework.GetDevice(), SetSpriteName({texturePath.begin(), texturePath.end()})));
}

UINT SpriteHandler::GetLoadedSpriteAndFrame(const std::string sprite, const UINT frame)
{
    for (auto& sheet : m_sprites)
    {
        if (sprite == sheet.Name()) {
            if (frame < sheet.GetSheet().m_frames.size())
                return sheet.GetID();
                
            //frame = 0;
            return sheet.GetID();
        }
    }

    return 0;
}

UINT SpriteHandler::GetLoadedFont(std::string fontName)
{
    for (auto& font : m_fonts)
    {
        if (fontName == font.Name()) {
            return font.GetID();
        }
    }
    return 0;
}

void SpriteHandler::AddNewlyCreatedFont(Font font)
{
    m_fonts.emplace_back(font);
    m_fonts[m_fonts.size() - 1].SetID(m_fonts.size());
}

void SpriteHandler::AddNewlyCreatedSprite(SpriteData sprite)
{
    m_sprites.emplace_back(sprite);
    m_sprites[m_sprites.size() - 1].SetID(m_sprites.size()); 
}

std::string SpriteHandler::SetSpriteName(std::wstring spritePath)
{
    std::string newName(spritePath.begin(), spritePath.end());
    size_t firstPos = newName.find_last_of("/");
    newName.replace(0, firstPos + 1, "");

    size_t lastPos = newName.find_first_of(".");
    newName.replace(lastPos, lastPos + 4, "");

    return newName;
}



