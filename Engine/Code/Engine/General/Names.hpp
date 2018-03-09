//Idea from Steve Rabin's talk in GDC 2015 The Simplest AI Trick in the Book
#pragma once
#include "Engine/Core/EngineBase.hpp"
//Idea from Steve Rabin's talk in GDC 2015 The Simplest AI Trick in the Book
/*/
#define REGISTER_ENUM(x) x,
typedef enum
{
#include "Engine/General/Names.hpp"
	MSG_NUM
} MSG_Name;
#undef REGISTER_ENUM

#define REGISTER_ENUM(x) #x,
static const char* MsgNameText[] =
{
#include "Engine/General/Names.hpp"
	"Invalid"
};
#undef REGISTER_ENUM
//*/
/*/
#define REGISTER_ENUM(x) x,
typedef enum
{
#include "Engine/General/Names.hpp"
	MSG_NUM
} MSG_Name;
#undef REGISTER_ENUM

#define REGISTER_ENUM(x) #x,
static const char* MsgNameText[] =
{
#include "Engine/General/Names.hpp"
	"Invalid"
};
#undef REGISTER_ENUM
//*/