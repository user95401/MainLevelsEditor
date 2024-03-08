#pragma once
#include <iostream>
#include <string>
std::string FilePathFromModFolder(std::string fname);
GJGameLevel* processOutLevelByConfig(int id, GJGameLevel* pGJGameLevel);

// Allows you to access protected members from any class
// by generating a class that inherits the target class
// to then access the member.
// Takes in a pointer btw
// Example:
//   CCNode* foo;
//   public_cast(foo, m_fRotation) = 10.f;
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