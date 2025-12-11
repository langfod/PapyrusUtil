#pragma once

#include "PCH.h"

#include <boost/array.hpp>
#include <boost/container/flat_map.hpp>
#include <sstream>

// ICriticalSection provided via PCH.h

namespace PackageData {

/**
 * Thread-safe package override storage for actors.
 * All public methods handle synchronization internally - callers do not need to acquire locks.
 */
class Packages {
public:
  // typedef boost::array<UInt32, 2> Flags;
  typedef std::pair<UInt32, UInt32> Flags;
  typedef boost::container::flat_map<UInt32, Flags> ActorPackages;
  typedef boost::container::flat_map<UInt32, ActorPackages> PackageMap;

  // Interface - all methods are thread-safe and handle locking internally
  void AddPackage(RE::Actor *ActorRef, RE::TESPackage *PackageRef, UInt32 priority, UInt32 flags);
  bool RemovePackage(RE::Actor *ActorRef, RE::TESPackage *PackageRef);
  UInt32 CountPackages(const RE::Actor *ActorRef);
  UInt32 ClearActor(RE::Actor *ActorRef);
  UInt32 ClearPackage(RE::TESPackage *PackageRef);

  // System Handling - thread-safe
  void PackageEnded(RE::FormID actorId, RE::FormID packId);
  bool IsValidPackage(RE::TESPackage *PackageID, RE::Actor *ActorID);
  RE::TESPackage *DecidePackage(RE::Actor *ActorID, RE::TESPackage *PackageID);

  // Serialization - not thread-safe, should only be called during save/load
  void LoadStream(std::stringstream &ss);
  void SaveStream(std::stringstream &ss);
  void Revert();

  // Serialization access - returns number of stored objects
  [[nodiscard]] std::size_t size() const { return Data.size(); }
  [[nodiscard]] bool empty() const { return Data.empty(); }

private:
  ICriticalSection s_dataLock;
  PackageMap Data;

  // Internal helper - caller must hold s_dataLock
  ActorPackages *GetActor(const TESForm *FormRef) {
    if (!FormRef)
      return nullptr;
    PackageMap::iterator itr = Data.find(FormRef->formID);
    return itr == Data.end() ? nullptr : &itr->second;
  }

  // Internal helper - caller must hold s_dataLock
  ActorPackages *GetActor(UInt32 formID) {
    PackageMap::iterator itr = Data.find(formID);
    return itr == Data.end() ? nullptr : &itr->second;
  }
};

Packages *GetPackages();
void InitPlugin();

} // namespace PackageData