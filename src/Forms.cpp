#include "Forms.h"
#include "PCH.h"

#include <functional>
#include <unordered_map>

namespace Forms {

static std::unordered_map<UInt32, UInt32> s_savedModIndexMap;

void ClearModList() { s_savedModIndexMap.clear(); }

// Old version. Keep to load existing co-save
void LoadModList(SKSE::SerializationInterface *intfc) {
  logger::info("Loading old mod list:");

  auto *dhand = RE::TESDataHandler::GetSingleton();

  char name[0x104] = {0};
  UInt16 nameLen = 0;

  UInt8 numSavedMods = 0;
  intfc->ReadRecordData(&numSavedMods, sizeof(numSavedMods));
  for (UInt32 i = 0; i < numSavedMods; i++) {
    intfc->ReadRecordData(&nameLen, sizeof(nameLen));
    // Prevent buffer overflow
    if (nameLen >= sizeof(name)) {
      logger::error("\tMod name too long: {} bytes", nameLen);
      // Skip the oversized name data
      for (UInt16 j = 0; j < nameLen; j++) {
        char dummy;
        intfc->ReadRecordData(&dummy, 1);
      }
      s_savedModIndexMap[i] = 0xFF;
      continue;
    }
    intfc->ReadRecordData(&name, nameLen);
    name[nameLen] = 0;

    const auto *modInfo = dhand->LookupModByName(name);
    if (modInfo) {
      UInt32 newIndex = modInfo->GetPartialIndex();
      s_savedModIndexMap[i] = newIndex;
      logger::info("\t({} -> {})\t{}", i, newIndex, name);
    } else {
      s_savedModIndexMap[i] = 0xFF;
      logger::info("\t({} -> UNLOADED)\t{}", i, name);
    }
  }
}

// New save list
void SavePluginsList(SKSE::SerializationInterface *intfc) {
  auto *dhand = RE::TESDataHandler::GetSingleton();

  // Count active mods
  UInt16 modCount = 0;
  for (auto *file : dhand->files) {
    if (file && file->compileIndex != 0xFF) {
      modCount++;
    }
  }

  (void)intfc->OpenRecord('PLGN', 0);
  intfc->WriteRecordData(&modCount, sizeof(modCount));

  logger::info("Saving plugin list:");

  for (auto *modInfo : dhand->files) {
    if (modInfo && modInfo->compileIndex != 0xFF) {
      UInt8 modIndex = static_cast<UInt8>(modInfo->compileIndex);
      intfc->WriteRecordData(&modIndex, sizeof(modIndex));

      if (modIndex == 0xFE) {
        UInt16 lightIndex = modInfo->smallFileCompileIndex;
        intfc->WriteRecordData(&lightIndex, sizeof(lightIndex));
      }

      UInt16 nameLen = static_cast<UInt16>(strlen(modInfo->fileName));
      intfc->WriteRecordData(&nameLen, sizeof(nameLen));
      intfc->WriteRecordData(modInfo->fileName, nameLen);

      if (modIndex != 0xFE) {
        logger::info("\t[{}]\t{}", modIndex, modInfo->fileName);
      } else {
        logger::info("\t[FE:{}]\t{}", modInfo->smallFileCompileIndex, modInfo->fileName);
      }
    }
  }
}

void LoadPluginList(SKSE::SerializationInterface *intfc) {
  auto *dhand = RE::TESDataHandler::GetSingleton();

  logger::info("Loading plugin list:");

  char name[0x104] = {0};
  UInt16 nameLen = 0;

  UInt16 modCount = 0;
  intfc->ReadRecordData(&modCount, sizeof(modCount));
  for (UInt32 i = 0; i < modCount; i++) {
    UInt8 modIndex = 0xFF;
    UInt16 lightModIndex = 0xFFFF;
    intfc->ReadRecordData(&modIndex, sizeof(modIndex));
    if (modIndex == 0xFE) {
      intfc->ReadRecordData(&lightModIndex, sizeof(lightModIndex));
    }

    intfc->ReadRecordData(&nameLen, sizeof(nameLen));
    // Prevent buffer overflow
    if (nameLen >= sizeof(name)) {
      logger::error("\tPlugin name too long: {} bytes", nameLen);
      // Skip the oversized name data
      for (UInt16 j = 0; j < nameLen; j++) {
        char dummy;
        intfc->ReadRecordData(&dummy, 1);
      }
      UInt32 oldIndex = modIndex == 0xFE ? (0xFE000 | lightModIndex) : modIndex;
      s_savedModIndexMap[oldIndex] = 0xFF;
      continue;
    }
    intfc->ReadRecordData(&name, nameLen);
    name[nameLen] = 0;

    UInt32 newIndex = 0xFF;
    UInt32 oldIndex = modIndex == 0xFE ? (0xFE000 | lightModIndex) : modIndex;

    const auto *modInfo = dhand->LookupModByName(name);
    if (modInfo) {
      newIndex = modInfo->GetPartialIndex();
    }

    s_savedModIndexMap[oldIndex] = newIndex;

    logger::info("\t({} -> {})\t{}", oldIndex, newIndex, name);
  }
}

UInt32 ResolveModIndex(UInt32 modIndex) {
  auto it = s_savedModIndexMap.find(modIndex);
  if (it != s_savedModIndexMap.end()) {
    return it->second;
  }
  return 0xFF;
}

UInt32 GetBaseID(UInt32 formID) {
  if (formID == 0)
    return 0;
  return ((formID >> 24 == 0xFE) ? formID & 0x00000FFF : formID & 0x00FFFFFF);
}
UInt32 GetBaseID(RE::TESForm *obj) {
  if (!obj)
    return 0;
  return GetBaseID(obj->formID);
}

// Returns 0xFF for invalid input (0 is a valid mod index - typically Skyrim.esm)
UInt32 GetModIndex(UInt32 formID) {
  if (formID == 0)
    return 0xFF;
  UInt32 modID = formID >> 24;
  if (modID == 0xFE) {
    modID = formID >> 12;
  }
  return modID;
}
UInt32 GetModIndex(RE::TESForm *obj) {
  if (!obj || obj->formID == 0)
    return 0xFF;
  return GetModIndex(obj->formID);
}

UInt32 GetModIndex(const char *name) {
  auto *dhand = RE::TESDataHandler::GetSingleton();
  const auto *modInfo = dhand->LookupModByName(name);
  if (modInfo) {
    return modInfo->GetPartialIndex();
  }
  return 0xFF;
}

// inline UInt8 ResolveModIndex(UInt8 modIndex) { return (modIndex < s_numSavefileMods) ? s_savefileIndexMap[modIndex] : 0xFF; }

// Get current form ID from current load order
UInt32 ResolveFormID(UInt32 formID) {
  /*if (formID == 0) return 0;
  UInt32 baseID = formID & 0x00FFFFFF;

  UInt8 oldMod = (UInt8)(formID >> 24);

  UInt32 newMod = ResolveModIndex(oldMod);
  if (newMod < 0 || newMod > 0xFF || (oldMod != 0xFF && newMod == 0xFF)) return 0;
  else return (((UInt32)newMod) << 24) | baseID;*/

  //_MESSAGE("ResolveFormID(0x%X)", (int)formID);

  if (formID == 0)
    return 0;
  UInt32 modID = formID >> 24;

  //_MESSAGE("\t- modID 1: %X", modID);

  // Light index
  if (modID == 0xFF) {
    //_MESSAGE("\t- modID FF: %X", formID);
    return formID; // FF
  } else if (modID == 0xFE) {
    modID = formID >> 12;
    //_MESSAGE("\t- modID light: %X", modID);
  }

  UInt32 loadedModID = ResolveModIndex(modID);
  if (loadedModID < 0xFF) {
    //_MESSAGE("\t- modID normal: %X", loadedModID);
    return (formID & 0x00FFFFFF) | (((UInt32)loadedModID) << 24); // ESP/ESM
  } else if (loadedModID > 0xFF) {
    //_MESSAGE("\t- modID bigger: %X", loadedModID);
    return (loadedModID << 12) | (formID & 0x00000FFF); // ESL
  }
  //_MESSAGE("\t- ended with 0");

  return 0;
}

RE::TESForm *ResolveFormKey(UInt64 key) {
  if (key < 1)
    return nullptr;
  UInt32 type = (UInt32)(key >> 32);
  UInt32 id = ResolveFormID((UInt32)(key));
  RE::TESForm *form = id <= 0 ? nullptr : RE::TESForm::LookupByID(id);
  return form ? (type == static_cast<UInt32>(form->GetFormType()) ? form : nullptr) : nullptr;
}

// Other
/*int GameGetForm(int formId) {
        if (formId == 0) return formId;
        int getTesFormId = 0x451A30;
        int form = 0;
        _asm
        {
                mov ecx, 0xFF0000
                        push formId
                        call getTesFormId
                        add esp, 4
                        mov form, eax
        }
        return form;
}*/

bool IsValidObject(RE::TESForm *obj, UInt64 formId) {
  if (!obj || formId == 0)
    return false;
  logger::debug("IsValidObject(0x{:X}, 0x{:X})", obj->formID, formId);
  logger::debug("\tType: {}", static_cast<int>(obj->GetFormType()));
  if ((UInt32)(formId >> 32) != 0 && static_cast<UInt32>(obj->GetFormType()) != (UInt32)(formId >> 32))
    return false;
  else if ((formId & 0xFFFFFFFF) != 0 && (UInt32)(formId & 0xFFFFFFFF) != obj->formID)
    return false;
  else
    return true;
}
// Form Keys
UInt64 GetFormKey(const RE::TESForm *form) {
  if (!form)
    return 0;
  UInt64 key = form->formID;
  key |= ((UInt64) static_cast<UInt32>(form->GetFormType())) << 32;
  return key;
}

RE::TESForm *GetFormFromKey(UInt64 key) {
  if (key < 1)
    return nullptr;
  UInt32 id = (UInt32)(key);
  return id != 0 ? RE::TESForm::LookupByID(id) : nullptr;
}

UInt64 GetNewKey(UInt64 key) {
  if (key == 0)
    return 0;
  UInt32 type = (UInt32)(key >> 32);
  UInt32 id = ResolveFormID((UInt32)(key));
  return (((UInt64)type) << 32) | (UInt64)id;
}

// Form Strings
bool ends_with(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool IsFormString(const std::string &str) {
  if (str.size() < 4 || str.find("|") == std::string::npos)
    return false;
  else
    return ends_with(str, ".esp") || ends_with(str, ".esm") || ends_with(str, ".esl") || ends_with(str, ".FF");
}

std::string GetFormString(const RE::TESForm *obj) {
  if (!obj)
    return "0";

  std::stringstream ss;
  UInt32 id = GetBaseID(obj->formID);

  // Set hex base ID
  ss << "0x" << std::hex << id << "|";

  // Set Mod name
  UInt32 index = GetModIndex(obj->formID);
  if (index < 0xFF) {
    // esp & esm objects
    auto *dhand = RE::TESDataHandler::GetSingleton();
    auto *modInfo = dhand->LookupLoadedModByIndex(static_cast<UInt8>(index));
    if (modInfo) {
      ss << modInfo->fileName;
    }
  } else if (index > 0xFF) {
    // esl objects
    UInt32 light = index & 0x00FFF;
    auto *dhand = RE::TESDataHandler::GetSingleton();
    auto *modInfo = dhand->LookupLoadedLightModByIndex(static_cast<UInt16>(light));
    if (modInfo) {
      ss << modInfo->fileName;
    }
  } else {
    // Temp objects - save form type as part of modname
    ss << static_cast<int>(obj->GetFormType()) << ".FF";
  }

  return ss.str();
}

// Light version of boost function. No error checking.
/*template<typename T2, typename T1>
inline T2 lexical_cast(const T1 &in) {
        T2 out;
        std::stringstream ss;
        ss << in;
        ss >> out;
        return out;
}*/

RE::TESForm *ParseFormString(const std::string &objString) {
  if (!IsFormString(objString))
    return nullptr;

  try {
    std::size_t pos = objString.find("|");
    std::string objID = objString.substr(0, pos).c_str();

    UInt32 obj = 0;
    if (objString.rfind("0x", 0) == 0) {
      obj = std::stoul(objID, nullptr, 16);
    } else {
      obj = std::stoul(objID, nullptr, 10);
    }

    std::string mod = objString.substr(pos + 1);

    if (ends_with(objString, ".FF")) {
      // Temp objects - check form type
      mod.resize((mod.length() - 3));
      // Form type is written as decimal in GetFormString, parse as decimal
      UInt8 type = static_cast<UInt8>(std::stoi(mod));
      obj = (((UInt32)0xFF) << 24) | obj;
      RE::TESForm *objform = obj == 0 ? nullptr : RE::TESForm::LookupByID(obj);
      if (objform && static_cast<UInt8>(objform->GetFormType()) == type)
        return objform;
      else {
        return nullptr;
      }
    } else {
      // esp & esm objects
      auto *dhand = RE::TESDataHandler::GetSingleton();
      const auto *modInfo = dhand->LookupModByName(mod.c_str());

      if (modInfo && modInfo->compileIndex != 0xFF) {
        obj = modInfo->GetFormID(obj);
      } else {
        return nullptr; // Couldn't find mod.
      }
    }

    return obj == 0 ? nullptr : RE::TESForm::LookupByID(obj);
  } catch (const std::exception &e) {
    logger::error("ParseFormString failed for '{}': {}", objString, e.what());
    return nullptr;
  }
}
} // namespace Forms
