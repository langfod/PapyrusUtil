#pragma once

#include "PCH.h"

namespace Forms {

UInt32 GetBaseID(UInt32 formID);
UInt32 GetBaseID(RE::TESForm *obj);
UInt32 GetModIndex(UInt32 formID);
UInt32 GetModIndex(RE::TESForm *obj);
UInt32 GetModIndex(const char *name);

// Load Order
void ClearModList();
void SavePluginsList(SKSE::SerializationInterface *intfc);
void LoadPluginList(SKSE::SerializationInterface *intfc);

// Old
void LoadModList(SKSE::SerializationInterface *intfc);

UInt32 ResolveFormID(UInt32 formID);
RE::TESForm *ResolveFormKey(UInt64 key);

// Form Keys
UInt64 GetNewKey(UInt64 key);
UInt64 GetNewKey(UInt64 key, const std::string &modName);
UInt64 GetFormKey(const RE::TESForm *form);
RE::TESForm *GetFormFromKey(UInt64 key);

inline UInt32 GetKeyType(UInt64 key) { return (UInt32)(key >> 32); }
inline UInt32 GetKeyID(UInt64 key) { return (UInt32)(key); }

// Form Strings
bool IsFormString(const std::string &str);
std::string GetFormString(const RE::TESForm *obj);
RE::TESForm *ParseFormString(const std::string &objString);

// Misc
bool IsValidObject(RE::TESForm *obj, UInt64 formId);

} // namespace Forms