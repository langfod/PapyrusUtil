#pragma once

#include "PCH.h"

namespace StorageUtil {
void RegisterFuncs(RE::BSScript::IVirtualMachine *vm);

// Flat key=>value storage
template <typename T, typename S> T SetValue(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, T value);
template <typename T, typename S> T GetValue(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, T missing);
template <typename T, typename S> T AdjustValue(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, T value);
template <typename T, typename S> bool UnsetValue(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key);
template <typename T, typename S> bool HasValue(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key);

// Lists key=>vector=>value storage
template <typename T, typename S>
SInt32 ListAdd(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, T value, bool allowDuplicate);
template <typename T, typename S> T ListGet(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, UInt32 index);
template <typename T, typename S> T ListSet(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, UInt32 index, T value);
template <typename T, typename S> T ListAdjust(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, UInt32 index, T value);
template <typename T, typename S>
UInt32 ListRemove(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, T value, bool allInstances);
template <typename T, typename S>
bool ListInsertAt(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, UInt32 index, T value);
template <typename T, typename S> bool ListRemoveAt(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, UInt32 index);
template <typename T, typename S> UInt32 ListClear(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key);
template <typename T, typename S> UInt32 ListCount(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key);
template <typename T, typename S>
UInt32 ListCountValue(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, T value, bool exclude);
template <typename T, typename S> SInt32 ListFind(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, T value);
template <typename T, typename S> bool ListHas(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, T value);
template <typename T, typename S> void ListSort(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key);
template <typename T, typename S>
void ListSlice(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, VMArray<T> &Output, UInt32 startIndex);
template <typename T, typename S>
SInt32 ListResize(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, UInt32 length, T filler);
template <typename T, typename S> bool ListCopy(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key, VMArray<T> &Input);
template <typename T, typename S> std::vector<T> ToArray(RE::StaticFunctionTag *base, RE::TESForm *obj, RE::BSFixedString key);

// Debug functions
UInt32 Cleanup(RE::StaticFunctionTag *base);
void DeleteAllValues(RE::StaticFunctionTag *base);
void DeleteValues(RE::StaticFunctionTag *base, RE::TESForm *obj);

template <typename T, typename S> UInt32 GetObjectCount(RE::StaticFunctionTag *base);
template <typename T, typename S> UInt32 GetListsObjectCount(RE::StaticFunctionTag *base);
template <typename T, typename S> UInt32 GetKeyCount(RE::StaticFunctionTag *base, RE::TESForm *obj);
template <typename T, typename S> UInt32 GetListsKeyCount(RE::StaticFunctionTag *base, RE::TESForm *obj);
template <typename T, typename S> RE::TESForm *GetNthObj(RE::StaticFunctionTag *base, UInt32 i);
template <typename T, typename S> RE::TESForm *GetListsNthObj(RE::StaticFunctionTag *base, UInt32 i);
template <typename T, typename S> RE::BSFixedString GetNthKey(RE::StaticFunctionTag *base, RE::TESForm *obj, UInt32 i);
template <typename T, typename S> RE::BSFixedString GetListsNthKey(RE::StaticFunctionTag *base, RE::TESForm *obj, UInt32 i);

#ifdef _GLOBAL_EXTERNAL
// Global external StorageUtil.json file
template <typename T> T FileSetValue(RE::StaticFunctionTag *base, RE::BSFixedString key, T value);
template <typename T> T FileGetValue(RE::StaticFunctionTag *base, RE::BSFixedString key, T missing);
template <typename T> T FileAdjustValue(RE::StaticFunctionTag *base, RE::BSFixedString key, T value);
template <typename T> bool FileUnsetValue(RE::StaticFunctionTag *base, RE::BSFixedString key);
template <typename T> bool FileHasValue(RE::StaticFunctionTag *base, RE::BSFixedString key);
template <typename T> SInt32 FileListAdd(RE::StaticFunctionTag *base, RE::BSFixedString key, T value, bool allowDuplicate);
template <typename T> T FileListGet(RE::StaticFunctionTag *base, RE::BSFixedString key, UInt32 index);
template <typename T> T FileListSet(RE::StaticFunctionTag *base, RE::BSFixedString key, UInt32 index, T value);
template <typename T> T FileListAdjust(RE::StaticFunctionTag *base, RE::BSFixedString key, UInt32 index, T adjustBy);
template <typename T> UInt32 FileListRemove(RE::StaticFunctionTag *base, RE::BSFixedString key, T value, bool allInstances);
template <typename T> bool FileListRemoveAt(RE::StaticFunctionTag *base, RE::BSFixedString key, UInt32 index);
template <typename T> bool FileListInsertAt(RE::StaticFunctionTag *base, RE::BSFixedString key, UInt32 index, T value);
template <typename T> UInt32 FileListClear(RE::StaticFunctionTag *base, RE::BSFixedString key);
template <typename T> UInt32 FileListCount(RE::StaticFunctionTag *base, RE::BSFixedString key);
template <typename T> SInt32 FileListFind(RE::StaticFunctionTag *base, RE::BSFixedString key, T value);
template <typename T> bool FileListHas(RE::StaticFunctionTag *base, RE::BSFixedString key, T value);
template <typename T> SInt32 FileListResize(RE::StaticFunctionTag *base, RE::BSFixedString key, UInt32 length, T filler);
template <typename T> void FileListSlice(RE::StaticFunctionTag *base, RE::BSFixedString key, VMArray<T> &Output, UInt32 startIndex);
template <typename T> bool FileListCopy(RE::StaticFunctionTag *base, RE::BSFixedString key, VMArray<T> &Input);
void SaveExternalFile(RE::StaticFunctionTag *base);
#endif

} // namespace StorageUtil
