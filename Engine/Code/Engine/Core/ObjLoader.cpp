#include "Engine/Core/ObjLoader.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "EngineBase.hpp"

struct Face
{
	std::vector<Vertex3D> faceVertexes;
};

std::vector<Vector3> vertexes;
std::vector<Vector2> texCords;
std::vector<Vector3> normals;
std::vector<Face> faces;
//////////////////////////////////////////////////////////////////////////
bool isWorthlessInfo(unsigned int pos, const std::vector<unsigned char>& buffer);
bool isWorthlessInfo(const std::string& info);
std::string skipWorthlessInfo(unsigned int& pos, const std::vector<unsigned char>& buffer);
void getVertex(unsigned int& pos, const std::vector<unsigned char>& buffer);
void getTexCord(unsigned int& pos, const std::vector<unsigned char>& buffer);
void getNormal(unsigned int& pos, const std::vector<unsigned char>& buffer);
void getFace(unsigned int& pos, const std::vector<unsigned char>& buffer);
void readOBJ(const std::vector<unsigned char>& buffer);
void storeVertex(const std::string& value, Face& face);
//////////////////////////////////////////////////////////////////////////


void getVertex(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	std::string valueLine;
	valueLine = getLine(pos, buffer);
	std::vector<std::string> values;
	splitArgs(valueLine, values);

	Vector3 newVertex(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));
	vertexes.push_back(newVertex);
}


void getTexCord(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	std::string valueLine;
	valueLine = getLine(pos, buffer);
	std::vector<std::string> values;
	splitArgs(valueLine, values);

	Vector2 newTexCord(std::stof(values[0]), std::stof(values[1]));
	texCords.push_back(newTexCord);
}


void getNormal(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	std::string valueLine;
	valueLine = getLine(pos, buffer);
	std::vector<std::string> values;
	splitArgs(valueLine, values);

	Vector3 newNormal(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));
	normals.push_back(newNormal);
}

void getFace(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	std::string valueLine;
	valueLine = getLine(pos, buffer);
	std::vector<std::string> values;
	splitArgs(valueLine, values);

	Face newFace;
	for (unsigned int i = 0; i < values.size(); ++i)
	{
		storeVertex(values[i], newFace);
	}
	if (newFace.faceVertexes.empty())
		return;


	faces.push_back(newFace);
}



std::string skipWorthlessInfo(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	std::string value;
	value = getValueFromBuffer(pos, buffer);
	while (pos < buffer.size() && isWorthlessInfo(value))
	{
		value = getValueFromBuffer(pos, buffer);
	}
	return value;
}

bool isWorthlessInfo(unsigned int pos, const std::vector<unsigned char>& buffer)
{
	if (buffer[pos] == 'v')
		return false;
	if (buffer[pos] == 't')
		return false;
	if (buffer[pos] == 'n')
		return false;
	if (buffer[pos] == 'f')
		return false;

	return true;
}


bool isWorthlessInfo(const std::string& info)
{
	if (info == "v")
		return false;
	if (info == "vn")
		return false;
	if (info == "vt")
		return false;
	if (info == "f")
		return false;

	return true;
}


void storeVertex(const std::string& value, Face& face)
{
	std::string vertValue;
	unsigned int valuePos = 0;
	while (valuePos < value.size() && !isWhiteSpace(value[valuePos]) && value[valuePos] != '/')
	{
		vertValue += value[valuePos];
		++valuePos;
	}
	++valuePos; //Get off our current /

	std::string texValue;
	while (valuePos < value.size() && !isWhiteSpace(value[valuePos]) && value[valuePos] != '/')
	{
		texValue += value[valuePos];
		++valuePos;
	}
	++valuePos; //Get off our current /

	std::string normValue;
	while (valuePos < value.size() && !isWhiteSpace(value[valuePos]) && value[valuePos] != '/')
	{
		normValue += value[valuePos];
		++valuePos;
	}

	if (vertValue.empty())
		return;
	unsigned int vertIndex = std::stoi(vertValue);
	Vertex3D newVertex3D(vertexes[vertIndex - 1]);

	if (!texValue.empty())
	{
		unsigned int texIndex = std::stoi(texValue);
		newVertex3D.m_textureCoords = texCords[texIndex-1];
	}
	if (!normValue.empty())
	{
		unsigned int normIndex = std::stoi(normValue);
		newVertex3D.m_normal = normals[normIndex-1];
	}
	
	face.faceVertexes.push_back(newVertex3D);
}



void readOBJ(const std::vector<unsigned char>& buffer)
{
	unsigned int pos = 0;
	while (pos < buffer.size())
	{
		std::string	value;
		value = skipWorthlessInfo(pos, buffer);
		skipWhiteSpace(pos, buffer);
		if (value == "v")
		{
			getVertex(pos, buffer);
		}
		else if (value == "vt")
		{
			getTexCord(pos, buffer);
		}
		else if (value == "vn")
		{
			getNormal(pos, buffer);
		}
		else if (value == "f")
		{
			getFace(pos, buffer);
		}
	}
}

unsigned int LoadOBJ(const std::string& filePath, std::vector<Vertex3D>& vertexBuffer)
{
	std::vector<unsigned char> readFileBuffer;
	ReadBufferToFile(readFileBuffer, filePath);

	vertexes.clear();
	texCords.clear();
	normals.clear();
	faces.clear();

	int reserveSize = readFileBuffer.size() / 2;
	vertexes.reserve(reserveSize);
	texCords.reserve(reserveSize);
	normals.reserve(reserveSize);
	faces.reserve(reserveSize);


	readOBJ(readFileBuffer);

	for (unsigned int i = 0; i < faces.size(); ++i)
	{
		for (unsigned int faceVertexIndex = 0; faceVertexIndex < faces[i].faceVertexes.size(); faceVertexIndex++)
		{
			vertexBuffer.push_back(faces[i].faceVertexes[faceVertexIndex]);
		}
	}

	return faces[0].faceVertexes.size();
}
