#include "MiscUtil.h"

#include <set>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "Offsets.h"

// TODO: test TESTScanCellNPCsByFaction and remove prepend

namespace MiscUtil {

// Fix for input context not switching properly when toggling free camera
// From: https://github.com/Meridiano/SkyrimDLL/blob/main/PapyrusUtilTFC/src/main.cpp
void FixInputContext(RE::PlayerCamera *camera) {
  auto *controls = RE::ControlMap::GetSingleton();
  if (controls) {
    auto *stateA = camera->currentState.get();
    // VR uses a different runtime data structure with different cameraStates array
    auto *stateB = !REL::Module::IsVR() ? camera->GetRuntimeData().cameraStates[RE::CameraState::kFree].get()
                                        : camera->GetVRRuntimeData().cameraStates[RE::CameraState::kFree].get();
    auto context = RE::ControlMap::InputContextID::kTFCMode;
    if (stateA == stateB) {
      controls->PushInputContext(context);
    } else {
      controls->PopInputContext(context);
    }
  }
}

void ToggleFreeCamera(StaticFunctionTag *, bool stopTime) {
  auto *camera = RE::PlayerCamera::GetSingleton();
  if (camera) {
    camera->ToggleFreeCameraMode(stopTime);
    FixInputContext(camera);
  }
}

void SetFreeCameraSpeed(StaticFunctionTag *, float speed) {
  auto *setting = RE::INISettingCollection::GetSingleton()->GetSetting("fFreeCameraTranslationSpeed:Camera");
  if (setting)
    setting->data.f = speed;
}

void SetFreeCameraState(StaticFunctionTag *, bool enable, float speed) {
  auto *camera = RE::PlayerCamera::GetSingleton();
  if (camera) {
    auto *cameraState = camera->currentState.get();
    if (cameraState) {
      bool freeCamera = cameraState->id == RE::CameraState::kFree;
      if (freeCamera != enable) {
        if (enable)
          SetFreeCameraSpeed(nullptr, speed);
        camera->ToggleFreeCameraMode(false);
        FixInputContext(camera);
      }
    }
  }
}

void PrintConsole(StaticFunctionTag *, BSFixedString text) {
  if (!text.data())
    return;
  else if (strlen(text.data()) < 1024)
    RE::ConsoleLog::GetSingleton()->Print(text.data());
  else { // Large strings printed to console crash the game - truncate it
    std::string msg = text.data();
    msg.resize(1020);
    msg.append("...");
    RE::ConsoleLog::GetSingleton()->Print(msg.c_str());
  }
}

void SetMenus(StaticFunctionTag *, bool enabled) { RE::UI::GetSingleton()->ShowMenus(enabled); }

BSFixedString GetRaceEditorID(StaticFunctionTag *, TESRace *RaceRef) { return RaceRef ? RaceRef->GetFormEditorID() : nullptr; }

BSFixedString GetActorRaceEditorID(StaticFunctionTag *, Actor *ActorRef) {
  return ActorRef && ActorRef->GetRace() ? ActorRef->GetRace()->GetFormEditorID() : nullptr;
}

bool HasKeyword2(Actor *ActorRef, BGSKeyword *findKeyword) {
  TESNPC *npc = skyrim_cast<TESNPC *>(ActorRef->GetBaseObject());
  if (!npc)
    return false;

  RE::BGSKeywordForm *pKeywords = skyrim_cast<RE::BGSKeywordForm *>(npc);
  if (!pKeywords) {
    pKeywords = skyrim_cast<RE::BGSKeywordForm *>(static_cast<RE::TESForm *>(npc));
  }
  if (pKeywords)
    return pKeywords->HasKeyword(findKeyword);
  else
    return false;
}

bool HasKeyword(TESObjectREFR *ObjRef, BGSKeyword *findKeyword) {
  RE::BGSKeywordForm *pKeywords = skyrim_cast<RE::BGSKeywordForm *>(ObjRef);
  if (!pKeywords) {
    pKeywords = skyrim_cast<RE::BGSKeywordForm *>(static_cast<RE::TESForm *>(ObjRef));
  }
  if (pKeywords)
    return pKeywords->HasKeyword(findKeyword);
  else
    return false;
}

bool IsWithinRadius(TESObjectREFR *CenterObj, TESObjectREFR *ObjRef, const float distance) {
  RE::NiPoint3 a = CenterObj->GetPosition();
  RE::NiPoint3 b = ObjRef->GetPosition();
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  float dz = a.z - b.z;
  // Use squared distance to avoid sqrt
  float distSq = dx * dx + dy * dy + dz * dz;
  return distSq < (distance * distance);
}

VMResultArray<TESObjectREFR *> ScanCellObjects(StaticFunctionTag *base, UInt32 FindType, TESObjectREFR *CenterObj, float SearchRadius,
                                               BGSKeyword *FindKeyword) {
  VMResultArray<TESObjectREFR *> output;
  if (CenterObj && CenterObj->parentCell) {
    TESObjectCELL *Cell = CenterObj->parentCell;

    logger::info("ScanCellObjects({}, 0x{:X}, {:.2f}) searching cell 0x{:X}", FindType, CenterObj->formID, SearchRadius, Cell->formID);

    Cell->ForEachReference([&](RE::TESObjectREFR *ObjRef) -> RE::BSContainer::ForEachResult {
      if (!ObjRef || !ObjRef->GetBaseObject())
        return RE::BSContainer::ForEachResult::kContinue;
      if (ObjRef->GetBaseObject()->formType.underlying() != FindType)
        return RE::BSContainer::ForEachResult::kContinue; // Wrong type
      if (SearchRadius > 0.0f && !IsWithinRadius(CenterObj, ObjRef, SearchRadius))
        return RE::BSContainer::ForEachResult::kContinue; // Outside search
                                                          // radius
      if (FindKeyword && !HasKeyword(ObjRef, FindKeyword))
        return RE::BSContainer::ForEachResult::kContinue; // Missing keyword
      output.push_back(ObjRef);                           // MATCH
      return RE::BSContainer::ForEachResult::kContinue;
    });
    logger::info("\tResults: {}", output.size());
  }
  return output;
}

VMResultArray<Actor *> ScanCellNPCs(StaticFunctionTag *base, TESObjectREFR *CenterObj, float SearchRadius, BGSKeyword *FindKeyword,
                                    bool ignoredead) {
  VMResultArray<Actor *> output;
  if (CenterObj && CenterObj->parentCell) {
    TESObjectCELL *Cell = CenterObj->parentCell;

    logger::info("ScanCellNPCs(0x{:X}, {:.2f}) searching cell 0x{:X}", CenterObj->formID, SearchRadius, Cell->formID);

    Cell->ForEachReference([&](RE::TESObjectREFR *ObjRef) -> RE::BSContainer::ForEachResult {
      Actor *ActorRef = ObjRef ? skyrim_cast<RE::Actor *>(ObjRef) : nullptr;
      if (!ActorRef || (ignoredead && ActorRef->IsDead(true)))
        return RE::BSContainer::ForEachResult::kContinue;
      if (SearchRadius > 0.0f && !IsWithinRadius(CenterObj, ActorRef, SearchRadius))
        return RE::BSContainer::ForEachResult::kContinue;
      if (FindKeyword && !HasKeyword2(ActorRef, FindKeyword))
        return RE::BSContainer::ForEachResult::kContinue;
      output.push_back(ActorRef);
      return RE::BSContainer::ForEachResult::kContinue;
    });
    logger::info("\tResults: {}", output.size());
  }
  return output;
}

bool IsInFaction(Actor *ActorRef, TESFaction *FactionRef, std::int8_t min, std::int8_t max) {
  if (!ActorRef || !FactionRef)
    return false;

  bool found = false;
  ActorRef->VisitFactions([&](RE::TESFaction *faction, std::int8_t rank) -> bool {
    if (rank >= min && rank <= max && FactionRef->formID == faction->formID) {
      found = true;
      return true; // Stop visiting
    }
    return false; // Continue visiting
  });
  return found;
}

VMResultArray<Actor *> ScanCellNPCsByFaction(StaticFunctionTag *base, TESFaction *FindFaction, TESObjectREFR *CenterObj, float SearchRadius,
                                             std::int32_t gte, std::int32_t lte, bool ignoredead) {
  VMResultArray<Actor *> output;
  if (CenterObj && FindFaction && CenterObj->parentCell) {

    std::int8_t minRank =
        static_cast<std::int8_t>(std::clamp(gte, static_cast<std::int32_t>(INT8_MIN), static_cast<std::int32_t>(INT8_MAX)));
    std::int8_t maxRank =
        static_cast<std::int8_t>(std::clamp(lte, static_cast<std::int32_t>(INT8_MIN), static_cast<std::int32_t>(INT8_MAX)));

    TESObjectCELL *Cell = CenterObj->parentCell;

    logger::info("ScanCellNPCsByFaction(0x{:X}, 0x{:X}, {:.2f}) searching cell 0x{:X}", FindFaction->formID, CenterObj->formID,
                 SearchRadius, Cell->formID);

    Cell->ForEachReference([&](RE::TESObjectREFR *ObjRef) -> RE::BSContainer::ForEachResult {
      Actor *ActorRef = ObjRef ? skyrim_cast<RE::Actor *>(ObjRef) : nullptr;
      if (!ActorRef || (ignoredead && ActorRef->IsDead(true)))
        return RE::BSContainer::ForEachResult::kContinue;
      if (SearchRadius > 0.0f && !IsWithinRadius(CenterObj, ActorRef, SearchRadius))
        return RE::BSContainer::ForEachResult::kContinue;
      if (!IsInFaction(ActorRef, FindFaction, minRank, maxRank))
        return RE::BSContainer::ForEachResult::kContinue;
      output.push_back(ActorRef);
      return RE::BSContainer::ForEachResult::kContinue;
    });
    logger::info("\tResults: {}", output.size());
  }
  return output;
}

namespace fs = boost::filesystem;

// Note: These file functions intentionally allow unrestricted path access
// for modding flexibility. Path validation is the caller's responsibility.
bool FileExists(StaticFunctionTag *base, BSFixedString name) {
  if (!name.data() || name.data()[0] == '\0')
    return false;
  fs::path Path = fs::path(name.data());
  return fs::exists(Path);
}

VMResultArray<BSFixedString> FilesInFolder(StaticFunctionTag *base, BSFixedString dirpath, BSFixedString extension) {
  VMResultArray<BSFixedString> arr;
  if (dirpath.data() && dirpath.data()[0] != '\0') {
    fs::path dir(dirpath.data());
    fs::directory_iterator end_iter;
    if (fs::exists(dir) && fs::is_directory(dir)) {
      std::string ext;
      if (extension.data() && extension.data()[0] == '.')
        ext = extension.data();
      else if (extension.data()) {
        ext = ".";
        ext.append(extension.data());
      } else {
        ext = ".*"; // No extension filter if null
      }
      logger::info("dir: {} ext: {}", dirpath.data(), ext.c_str());
      for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter) {
        if (fs::is_regular_file(dir_iter->status())) {
          fs::path filepath = dir_iter->path();
          std::string file = filepath.filename().generic_string();
          // std::string ext = filepath.extension().generic_string();
          //_MESSAGE("file: %s ext: %s", file.c_str(), ext.c_str());
          if (ext == ".*" || boost::iequals(filepath.extension().generic_string(), ext))
            arr.push_back(BSFixedString(file.c_str()));
        }
      }
    }
  }
  return arr;
}

VMResultArray<BSFixedString> FoldersInFolder(StaticFunctionTag *base, BSFixedString dirpath) {
  VMResultArray<BSFixedString> arr;
  if (dirpath.data() && dirpath.data()[0] != '\0') {
    fs::path dir(dirpath.data());
    fs::directory_iterator end_iter;
    if (fs::exists(dir) && fs::is_directory(dir)) {
      logger::info("dir: {}", dirpath.data());

      for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter) {
        if (fs::is_directory(dir_iter->status())) {
          fs::path filepath = dir_iter->path();
          std::string file = filepath.filename().generic_string();
          arr.push_back(BSFixedString(file.c_str()));
        }
      }
    }
  }
  return arr;
}

BSFixedString ReadFromFile(StaticFunctionTag *base, BSFixedString filename) {
  if (!filename.data() || filename.data()[0] == '\0')
    return BSFixedString("");
  std::string output; // Empty string for all failure cases
  fs::path File(filename.data());
  if (fs::exists(File) && !fs::is_directory(File)) {
    fs::ifstream doc;
    try {
      doc.open(File, std::ios::in | std::ios::binary);
      if (!doc.fail()) {
        std::stringstream ss;
        ss << doc.rdbuf();
        output = ss.str();
      }
    } catch (std::exception &) {
      logger::info("Failed to load/read file: {}", File.generic_string().c_str());
    }
    if (doc.is_open())
      doc.close();
  }
  return BSFixedString(output.c_str());
}

bool WriteToFile(StaticFunctionTag *base, BSFixedString filename, BSFixedString input, bool append, bool timestamp) {
  if (!filename.data() || filename.data()[0] == '\0')
    return false;
  bool output = false;
  fs::path File(filename.data());
  fs::ofstream doc;
  try {
    if (append)
      doc.open(File, std::ios::out | std::ios::app | std::ios::binary);
    else
      doc.open(File, std::ios::out | std::ios::binary);
    if (doc.is_open()) {
      doc << input.data();
      doc.close();
      output = true;
    }
  } catch (std::exception &) {
    logger::info("Failed to load/write file: {}", File.generic_string().c_str());
  }
  if (doc.is_open())
    doc.close();
  return output;
}

} // namespace MiscUtil

void MiscUtil::RegisterFuncs(RE::BSScript::IVirtualMachine *vm) {
  constexpr auto script = "MiscUtil";

  vm->RegisterFunction("ToggleFreeCamera"sv, script, ToggleFreeCamera);

  vm->RegisterFunction("SetFreeCameraSpeed"sv, script, SetFreeCameraSpeed);

  vm->RegisterFunction("SetFreeCameraState"sv, script, SetFreeCameraState);

  vm->RegisterFunction("PrintConsole"sv, script, PrintConsole);

  vm->RegisterFunction("SetMenus"sv, script, SetMenus);

  vm->RegisterFunction("GetRaceEditorID"sv, script, GetRaceEditorID);

  vm->RegisterFunction("GetActorRaceEditorID"sv, script, GetActorRaceEditorID);

  vm->RegisterFunction("FileExists"sv, script, FileExists);

  vm->RegisterFunction("ScanCellObjects"sv, script, ScanCellObjects);
  vm->RegisterFunction("ScanCellNPCs"sv, script, ScanCellNPCs);
  vm->RegisterFunction("ScanCellNPCsByFaction"sv, script, ScanCellNPCsByFaction);

  vm->RegisterFunction("FilesInFolder"sv, script, FilesInFolder);

  vm->RegisterFunction("FoldersInFolder"sv, script, FoldersInFolder);

  vm->RegisterFunction("ReadFromFile"sv, script, ReadFromFile);
  vm->RegisterFunction("WriteToFile"sv, script, WriteToFile);

  // Set NoWait flags (callable from tasklets)
  vm->SetCallableFromTasklets(script, "ToggleFreeCamera", true);
  vm->SetCallableFromTasklets(script, "SetFreeCameraSpeed", true);
  vm->SetCallableFromTasklets(script, "SetFreeCameraState", true);
  vm->SetCallableFromTasklets(script, "PrintConsole", true);
  vm->SetCallableFromTasklets(script, "SetMenus", true);
  vm->SetCallableFromTasklets(script, "GetRaceEditorID", true);
  vm->SetCallableFromTasklets(script, "GetActorRaceEditorID", true);
}