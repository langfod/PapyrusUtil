#pragma once

#include "PCH.h"

//MAKE PATH FUNCTIONS CASE INSENSITIVE

namespace JsonUtil {
	void RegisterFuncs(RE::BSScript::IVirtualMachine* vm);

	bool SaveJson(RE::StaticFunctionTag* base, RE::BSFixedString name, bool styled);
	bool LoadJson(RE::StaticFunctionTag* base, RE::BSFixedString name);
	void ClearAll(RE::StaticFunctionTag* base, RE::BSFixedString name);

	template <typename T> T SetValue(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, T value);
	template <typename T> T GetValue(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, T missing);
	template <typename T> T AdjustValue(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, T value);
	template <typename T> bool UnsetValue(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key);
	template <typename T> bool HasValue(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key);
	template <typename T> SInt32 ListAdd(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, T value, bool allowDuplicate);
	template <typename T> T ListGet(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, UInt32 index);
	template <typename T> T ListSet(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, UInt32 index, T value);
	template <typename T> T ListAdjust(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, UInt32 index, T adjustBy);
	template <typename T> UInt32 ListRemove(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, T value, bool allInstances);
	template <typename T> bool ListRemoveAt(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, UInt32 index);
	template <typename T> bool ListInsertAt(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, UInt32 index, T value);
	template <typename T> UInt32 ListClear(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key);
	template <typename T> UInt32 ListCount(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key);
	template <typename T> UInt32 ListCountValue(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, T value, bool exclude);
	template <typename T> SInt32 ListFind(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, T value);
	template <typename T> bool ListHas(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, T value);
	template <typename T> SInt32 ListResize(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, UInt32 length, T filler);
	template <typename T> void ListSlice(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, VMArray<T> Output, UInt32 startIndex);
	template <typename T> bool ListCopy(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key, VMArray<T> Input);
	template <typename T> std::vector<T> ToArray(RE::StaticFunctionTag* base, RE::BSFixedString name, RE::BSFixedString key);

}
