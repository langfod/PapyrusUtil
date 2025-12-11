#include "PackageData.h"

#include "Forms.h"

#include <mutex>

namespace PackageData {
static Packages *s_PackageData = nullptr;
static std::once_flag s_packageDataInitFlag;

static void InitPackageDataOnce() { s_PackageData = new Packages(); }

Packages *GetPackages() {
  std::call_once(s_packageDataInitFlag, InitPackageDataOnce);
  return s_PackageData;
}

/*
 *  Package override serialize
 */

void Packages::LoadStream(std::stringstream &ss) {
  int count;
  ss >> count;
  if (count < 1)
    return;
  // Bounds check to prevent excessive memory allocation from corrupted data
  constexpr int kMaxReasonableActors = 100000;
  if (count > kMaxReasonableActors) {
    logger::error("LoadStream: count {} exceeds maximum {}, truncating", count, kMaxReasonableActors);
    count = kMaxReasonableActors;
  }
  s_dataLock.Enter();
  // Clear existing data - this is a load/replace operation, not append
  Data.clear();
  Data.reserve(count);
  for (int i = 0; i < count; ++i) {
    // Unpack actor
    UInt32 formID;
    ss >> formID;
    int count2;
    ss >> count2;
    // Validate actor
    UInt32 newID = Forms::ResolveFormID(formID);
    RE::TESForm *FormRef = RE::TESForm::LookupByID(newID);
    RE::Actor *ActorRef = FormRef != nullptr ? skyrim_cast<RE::Actor *>(FormRef) : nullptr;
    if (ActorRef == nullptr) {
      for (int n = 0; n < count2; ++n) {
        UInt32 PackID, priority, flag;
        ss >> PackID;
        ss >> priority;
        ss >> flag;
      }
    } else {
      Data[newID].reserve(count2);
      for (int n = 0; n < count2; ++n) {
        // Unpack Package
        UInt32 PackID, priority, flag;
        ss >> PackID;
        ss >> priority;
        ss >> flag;
        // Validate Package
        PackID = Forms::ResolveFormID(PackID);
        RE::TESForm *PackageRef = RE::TESForm::LookupByID(PackID);
        if (PackageRef != nullptr && PackageRef->GetFormType() == RE::FormType::Package) {
          // LOAD Package
          if (priority > 100)
            priority = 100;
          else if (priority < 1)
            priority = 1;
          Data[newID][PackageRef->formID] = Flags(priority, flag == 1 ? 1 : 0);
        }
      }
      Data[newID].shrink_to_fit();
    }
  }
  Data.shrink_to_fit();
  s_dataLock.Leave();
}
void Packages::SaveStream(std::stringstream &ss) {
  s_dataLock.Enter();
  // Cleanup - use erase() return value to get next valid iterator
  for (PackageMap::iterator ActorItr = Data.begin(); ActorItr != Data.end();) {
    if (ActorItr->second.size() < 1)
      ActorItr = Data.erase(ActorItr);
    else
      ++ActorItr;
  }
  // Serialize
  ss << (int)Data.size();
  for (PackageMap::iterator i = Data.begin(); i != Data.end(); ++i) {
    ss << ' ' << i->first;              // Actor formID
    ss << ' ' << (int)i->second.size(); // Actor package count
    for (ActorPackages::iterator n = i->second.begin(); n != i->second.end(); ++n) {
      ss << ' ' << (UInt32)n->first;         // Package
      ss << ' ' << (UInt32)n->second.first;  // Priority
      ss << ' ' << (UInt32)n->second.second; // Flag
    }
  }
  s_dataLock.Leave();
}
void Packages::Revert() {
  s_dataLock.Enter();
  Data.clear();
  s_dataLock.Leave();
}

/*
 *  Package override interface
 */

void Packages::AddPackage(Actor *ActorRef, TESPackage *PackageRef, UInt32 priority, UInt32 flags) {
  if (ActorRef && PackageRef) {
    if (priority > 100)
      priority = 100;
    else if (priority < 1)
      priority = 1;
    if (flags != 1)
      flags = 0;

    s_dataLock.Enter();
    Data[ActorRef->formID][PackageRef->formID] = Flags(priority, flags);
    s_dataLock.Leave();
  }
}

bool Packages::RemovePackage(Actor *ActorRef, TESPackage *PackageRef) {
  bool removed = false;
  if (ActorRef && PackageRef) {
    s_dataLock.Enter();
    if (Data.size() > 0) {
      ActorPackages *ActorPacks = GetActor(ActorRef);
      if (ActorPacks != NULL) {
        // Remove
        ActorPackages::iterator PacksItr = ActorPacks->find(PackageRef->formID);
        if (PacksItr != ActorPacks->end())
          ActorPacks->erase(PackageRef->formID);
        removed = ActorPacks->find(PackageRef->formID) == ActorPacks->end();
        // Cleanup
        if (Data[ActorRef->formID].size() == 0)
          Data.erase(ActorRef->formID);
      }
    }
    s_dataLock.Leave();
  }
  return removed;
}

UInt32 Packages::CountPackages(const Actor *ActorRef) {
  UInt32 count = 0;
  if (ActorRef) {
    s_dataLock.Enter();
    if (Data.size() > 0) {
      ActorPackages *ActorPacks = GetActor(ActorRef);
      count = ActorPacks != nullptr ? static_cast<UInt32>(ActorPacks->size()) : 0;
    }
    s_dataLock.Leave();
  }
  return count;
}

UInt32 Packages::ClearActor(Actor *ActorRef) {
  UInt32 count = 0;
  if (ActorRef) {
    s_dataLock.Enter();
    if (Data.size() > 0) {
      auto itr = Data.find(ActorRef->formID);
      if (itr != Data.end()) {
        count = static_cast<UInt32>(itr->second.size());
        Data.erase(itr);
      }
    }
    s_dataLock.Leave();
  }
  return count;
}

UInt32 Packages::ClearPackage(TESPackage *PackageRef) {
  UInt32 count = 0;
  if (PackageRef) {
    s_dataLock.Enter();
    if (Data.size() > 0) {
      for (PackageMap::iterator ActorItr = Data.begin(); ActorItr != Data.end(); ++ActorItr) {
        ActorPackages *ActorPacks = &ActorItr->second;
        ActorPackages::iterator PacksItr = ActorPacks->find(PackageRef->formID);
        if (PacksItr != ActorPacks->end()) {
          ActorPacks->erase(PackageRef->formID);
          count++;
        }
      }
    }
    s_dataLock.Leave();
  }
  return count;
};

} // namespace PackageData

/*
 *  Package override handling
 */

#include "Forms.h"
#include "Offsets.h"

#include <atomic>

namespace PackageData {

RE::TESPackage *DecidePackage(RE::Actor *ActorID, RE::TESPackage *PackageID) {
  if (s_PackageData && ActorID != 0)
    return s_PackageData->DecidePackage(ActorID, PackageID);
  else
    return PackageID;
}
RE::TESPackage *Packages::DecidePackage(RE::Actor *ActorID, RE::TESPackage *PackageID) {
  s_dataLock.Enter();
  if (Data.size() == 0 || ActorID == 0) {
    s_dataLock.Leave();
    return PackageID;
  }
  ActorPackages *Overrides = GetActor((RE::TESForm *)ActorID);
  if (Overrides != nullptr) {
    UInt32 pickedPack = 0;
    Flags pickedFlags = Flags(0, 0);
    for (ActorPackages::iterator itr = Overrides->begin(); itr != Overrides->end(); ++itr) {
      if (itr->second.first >= pickedFlags.first) {
        pickedPack = itr->first;
        pickedFlags = itr->second;
        logger::info("Package[{}] Priority[{}] Flag[{}]", pickedPack, pickedFlags.first, pickedFlags.second);
      }
    }
    RE::TESPackage *pid = skyrim_cast<RE::TESPackage *>(RE::TESForm::LookupByID(pickedPack));
    RE::TESForm *FormRef = pid == nullptr ? nullptr : (RE::TESForm *)pid;
    if (FormRef && FormRef->GetFormType() == RE::FormType::Package) {
      if (pickedFlags.second == 1 || IsValidPackage(pid, ActorID)) {
        logger::info("Override Picked -- Package[{:x}] Priority[{}] Flag[{}]", (uintptr_t)pid, pickedFlags.first, pickedFlags.second);
        PackageID = pid;
      }
    }
  }
  s_dataLock.Leave();
  return PackageID;
}

typedef int (*_IsValid)(RE::TESPackage *PackageID, RE::Actor *ActorID);
bool Packages::IsValidPackage(RE::TESPackage *PackageID, RE::Actor *ActorID) {
  REL::Relocation<_IsValid> IsValid{Offsets::IsValidPackage};
  return (IsValid(PackageID, ActorID) != 0);
}

// Thread-safe globals for package end tracking
static std::atomic<RE::FormID> EndPackID{0};
static std::atomic<RE::FormID> EndActorID{0};

void PackageEnded() {
  RE::FormID packId = EndPackID.exchange(0);
  RE::FormID actorId = EndActorID.exchange(0);
  if (s_PackageData && packId != 0 && actorId != 0) {
    s_PackageData->PackageEnded(actorId, packId);
  }
}
void Packages::PackageEnded(RE::FormID actorId, RE::FormID packId) {
  s_dataLock.Enter();
  if (Data.size() == 0) {
    s_dataLock.Leave();
    return;
  }

  RE::TESForm *ActorRef = RE::TESForm::LookupByID(actorId);
  RE::TESForm *PackageRef = RE::TESForm::LookupByID(packId);
  if (ActorRef && PackageRef) {
    ActorPackages *ActorPacks = GetActor(ActorRef);
    if (ActorPacks != nullptr) {
      // Remove
      ActorPackages::iterator PacksItr = ActorPacks->find(PackageRef->formID);
      if (PacksItr != ActorPacks->end())
        ActorPacks->erase(PackageRef->formID);
      // Cleanup
      if (Data[ActorRef->formID].size() == 0)
        Data.erase(ActorRef->formID);
    }
  }
  s_dataLock.Leave();
}

// Function pointer type for package start hook
typedef RE::TESPackage *(*_PackageStartOrig)(void *, RE::Actor *);

// Original function pointer storage
_PackageStartOrig PackageStartOrigFunc = nullptr;

RE::TESPackage *PackageStartHooked(void *pthis, RE::Actor *actor) {
  RE::TESPackage *pkg = PackageStartOrigFunc(pthis, actor);

  if (actor && actor->formID != 0) {
    return DecidePackage(actor, pkg);
  }

  return pkg;
}

void InitPlugin() {
  logger::info("PackageData::InitPlugin - Setting up package override hooks...");

  auto &trampoline = SKSE::GetTrampoline();

  // Hook PackageStart function
  // The hook point is at PackageStart_Base + PackageStart_Offset
  REL::Relocation<std::uintptr_t> packageStartHook{Offsets::PackageStart_Base, Offsets::PackageStart_Offset};
  PackageStartOrigFunc = reinterpret_cast<_PackageStartOrig>(trampoline.write_call<5>(packageStartHook.address(), PackageStartHooked));

  logger::info("PackageData::InitPlugin - Package override hooks installed successfully");
}

} // namespace PackageData
