#pragma once
#include <iostream>
#include <string>
#include "SimpleIni.h"

inline int cmp_amount = 23;
inline int start_from = 1;

std::string FilePathFromModFolder(std::string fname);
GJGameLevel* processOutLevelByConfig(int id, GJGameLevel* pGJGameLevel);
std::string saveToMainLevel(int id, GJGameLevel* pGJGameLevel);
void saveLevelData(GJGameLevel* pGJGameLevel);
auto read_file(std::string_view path) -> std::string;
std::vector<std::string> explode(const std::string& str, const char& ch);
std::string framePath(CCNode* node);
std::string truncate(std::string str, size_t width, bool show_ellipsis = true);

template <typename T>
inline bool checkExistence(T filename)
{
	std::ifstream Infield(filename);
	return Infield.good();
}

#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)