#pragma once
#include <iostream>
#include <string>
std::string FilePathFromModFolder(std::string fname);
GJGameLevel* processOutLevelByConfig(int id, GJGameLevel* pGJGameLevel);

#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)