#include "Engine/Renderer/Font.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/ParseUtils.hpp"
//////////////////////////////////////////////////////////////////////////
Font::Font(const std::string& filePath)
	: m_filePath(filePath)
	, m_texturePath("Data/Fonts/")
{
	loadFont(m_filePath);
}

void Font::loadFont(const std::string& filePath)
{
	std::vector<unsigned char> buffer;
	ReadBufferToFile(buffer, filePath);

	unsigned int pos = 0;

	// Info line
	skipWord(pos, buffer); //info
	skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); m_name = getValueFromBuffer(pos, buffer);						//get face aka font name
	skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); m_size = std::stoi(getValueFromBuffer(pos, buffer));			//get size
	//#ToDo: handle bold and italic
	skipLine(pos, buffer); //Skip rest of useless stuff

	// Common line
	skipWord(pos, buffer); //commmon
	skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); m_lineHeight = std::stoi(getValueFromBuffer(pos, buffer));		//get lineHeight
	skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); m_base = std::stof(getValueFromBuffer(pos, buffer));			//get base
	skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); m_scaleW = std::stoi(getValueFromBuffer(pos, buffer));			//get scaleW
	skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); m_scaleH = std::stoi(getValueFromBuffer(pos, buffer));			//get scaleH
	skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); m_pages = std::stoi(getValueFromBuffer(pos, buffer));			//get pages
	skipLine(pos, buffer); //Skip rest of useless stuff
	m_divideByLineHeight = float(1.f / m_lineHeight); //set this so we can normalize easier
	m_base *= m_divideByLineHeight;

	// page line #ToDo: handle multi page fonts
	skipWord(pos, buffer); //page
	skipWord(pos, buffer); //id=0
	skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); m_texturePath += getValueFromBuffer(pos, buffer);				//get texture image name #ToDo: fix it to handle different folders
	skipLine(pos, buffer); //Skip rest of useless stuff

	// Char count line
	skipWord(pos, buffer); //chars
	int charCount;
	skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); charCount = std::stoi(getValueFromBuffer(pos, buffer));			//get number of characters
	skipLine(pos, buffer); //Skip rest of useless stuff

	getFontCharacters(pos, buffer, charCount);

	if (pos >= buffer.size())
		return;

	// Kernings count line
	skipWord(pos, buffer); //kernings
	int kernCount;
	getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); kernCount = std::stoi(getValueFromBuffer(pos, buffer));			//get number of kernings
	skipLine(pos, buffer); //Skip rest of useless stuff
	getKernings(pos, buffer, kernCount);
}

float Font::calcTextWidth(const std::string& text, float scale /*= 1.0f*/)
{
	float cursor = 0;

	int currentChar = 0;
	int prev_char_code = -1;

	while (currentChar != (int)text.size())
	{
		FontCharacter glygh = getGlyph(text[currentChar]);

		if (glygh.id == -1)
		{
			glygh = getInvalidGlyph();
		}

		cursor += glygh.xadvance;
		prev_char_code = text[currentChar];
		++currentChar;

		float kerning = getKerning(prev_char_code, text[currentChar]);
		cursor += kerning;
	}

	return cursor * scale; // height is line_height
}

AABB2 Font::GetTexCoordsForGlyph(int glyphUnicode) const
{
	const FontCharacter glygh = getGlyph(glyphUnicode);
	Vector2 uv_top_left = Vector2((float)glygh.x / (float)m_scaleW, (float)glygh.y / (float)m_scaleH);
	Vector2 uv_bottom_right = uv_top_left + Vector2(glygh.width*m_lineHeight / (float)m_scaleW, glygh.height*m_lineHeight / (float)m_scaleH);
	return AABB2(uv_top_left, uv_bottom_right);
}
//////////////////////////////////////////////////////////////////////////
void Font::getFontCharacters(unsigned int& pos, const std::vector<unsigned char>& buffer, int numChars)
{
	for (int i = 0; i < numChars; i++)
	{
		FontCharacter temp;
		skipWord(pos, buffer); //char
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); temp.id = std::stoi(getValueFromBuffer(pos, buffer));			//get id
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); temp.x = std::stoi(getValueFromBuffer(pos, buffer));			//get x
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); temp.y = std::stoi(getValueFromBuffer(pos, buffer));			//get y
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); temp.width = std::stof(getValueFromBuffer(pos, buffer));		//get width
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); temp.height = std::stof(getValueFromBuffer(pos, buffer));		//get height
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); temp.xoffset = std::stof(getValueFromBuffer(pos, buffer));		//get xoffset
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); temp.yoffset = std::stof(getValueFromBuffer(pos, buffer));		//get yoffset
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); temp.xadvance = std::stof(getValueFromBuffer(pos, buffer));		//get xadvance
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); temp.page = std::stoi(getValueFromBuffer(pos, buffer));			//get page
		skipLine(pos, buffer); //Skip rest of useless stuff

		//Normalize some of the values
		
		temp.height *= m_divideByLineHeight;
		temp.width *= m_divideByLineHeight;
		temp.xoffset *= m_divideByLineHeight;
		temp.yoffset *= m_divideByLineHeight;
		temp.xadvance *= m_divideByLineHeight;

		m_chars[temp.id] = temp;
	}
}

void Font::getKernings(unsigned int& pos, const std::vector<unsigned char>& buffer, int numKerns)
{
	for (int i = 0; i < numKerns; i++)
	{
		int first;
		int second;
		float storedValue;
		skipWord(pos, buffer); //kerning
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); first = std::stoi(getValueFromBuffer(pos, buffer));				//get firstCharId
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); second = std::stoi(getValueFromBuffer(pos, buffer));			//get secondCharId
		skipWhiteSpace(pos, buffer); getKeyFromBuffer(pos, buffer); skipEqualsSign(pos, buffer); storedValue = std::stof(getValueFromBuffer(pos, buffer));		//get Kerning to store
		skipLine(pos, buffer); //Skip rest of useless stuff

		storedValue *= m_divideByLineHeight;

		m_kernings[std::pair<int,int>(first, second)] = storedValue;
	}
}
//////////////////////////////////////////////////////////////////////////
const FontCharacter Font::getGlyph(int text) const
{
	std::map<int, FontCharacter>::const_iterator found = m_chars.find(text);
	if (found != m_chars.end())
	{
		FontCharacter result = found->second;
		return result;
	}

	FontCharacter result;
	result.id = -1;
	return result;
}

const FontCharacter Font::getInvalidGlyph() const
{
	std::map<int, FontCharacter>::const_iterator found = m_chars.find(-1);
	ASSERT_OR_DIE(found != m_chars.end(), "")
	FontCharacter result = found->second;
	return result;
}

const float Font::getKerning(int prevCharId, int nextCharId) const
{
	std::map<std::pair<int, int>, float>::const_iterator found = m_kernings.find(std::pair<int, int>(prevCharId, nextCharId));
	if (found != m_kernings.end())
	{
		return found->second;
	}
	return 0;
}
