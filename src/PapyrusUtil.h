#pragma once

#include "PCH.h"

namespace PapyrusUtil {
	void RegisterFuncs(RE::BSScript::IVirtualMachine* vm);

	template <typename T> std::vector<T> ResizeArray(RE::StaticFunctionTag*, std::vector<T> arr, UInt32 size, T filler);
	template <typename T> std::vector<T> PushArray(RE::StaticFunctionTag*, std::vector<T> arr, T push);
	template<typename T> UInt32 CountValues(RE::StaticFunctionTag*, std::vector<T> arr, T find);
	template <typename T> std::vector<T> ClearArray(RE::StaticFunctionTag*, std::vector<T> arr, T remove);
	template <typename T> std::vector<T> MergeArray(RE::StaticFunctionTag*, std::vector<T> arr1, std::vector<T> arr2, bool removeDupe);
	template <typename T> std::vector<T> SliceArray(RE::StaticFunctionTag*, std::vector<T> Input, UInt32 idx, SInt32 end_idx);
	template<typename T> T AddValues(RE::StaticFunctionTag*, std::vector<T> Values);
	template<typename T> T ClampValue(RE::StaticFunctionTag*, T var, T min, T max);
	template<typename T> T WrapValue(RE::StaticFunctionTag*, T var, T end, T start);
	template<typename T> T SignValue(RE::StaticFunctionTag*, bool sign, T var);
	std::vector<RE::BSFixedString> StringSplit(RE::StaticFunctionTag*, RE::BSFixedString argstring, RE::BSFixedString delimiter);
	RE::BSFixedString StringJoin(RE::StaticFunctionTag*, std::vector<RE::BSFixedString> args, RE::BSFixedString delimiter);
}
