#include "ActorUtil.h"

#include "PackageData.h"

namespace ActorUtil {

void AddPackage(RE::StaticFunctionTag *, RE::Actor *ActorRef, RE::TESPackage *PackageRef, UInt32 priority, UInt32 flags) {
  PackageData::Packages *Overrides = PackageData::GetPackages();
  if (Overrides)
    Overrides->AddPackage(ActorRef, PackageRef, priority, flags);
}

bool RemovePackage(RE::StaticFunctionTag *, RE::Actor *ActorRef, RE::TESPackage *PackageRef) {
  PackageData::Packages *Overrides = PackageData::GetPackages();
  return Overrides ? Overrides->RemovePackage(ActorRef, PackageRef) : false;
}

UInt32 CountPackages(RE::StaticFunctionTag *, const RE::Actor *ActorRef) {
  PackageData::Packages *Overrides = PackageData::GetPackages();
  return Overrides ? Overrides->CountPackages(ActorRef) : 0;
}

UInt32 ClearActor(RE::StaticFunctionTag *, RE::Actor *ActorRef) {
  PackageData::Packages *Overrides = PackageData::GetPackages();
  return Overrides ? Overrides->ClearActor(ActorRef) : 0;
}

UInt32 ClearPackage(RE::StaticFunctionTag *, RE::TESPackage *PackageRef) {
  PackageData::Packages *Overrides = PackageData::GetPackages();
  return Overrides ? Overrides->ClearPackage(PackageRef) : 0;
}
} // namespace ActorUtil

void ActorUtil::RegisterFuncs(RE::BSScript::IVirtualMachine *vm) {
  if (!vm) {
    return;
  }

  constexpr auto script = "ActorUtil";

  vm->RegisterFunction("AddPackageOverride"sv, script, AddPackage);
  vm->RegisterFunction("RemovePackageOverride"sv, script, RemovePackage);
  vm->RegisterFunction("CountPackageOverride"sv, script, CountPackages);
  vm->RegisterFunction("ClearPackageOverride"sv, script, ClearActor);
  vm->RegisterFunction("RemoveAllPackageOverride"sv, script, ClearPackage);

  // Set NoWait flags (callable from tasklets)
  vm->SetCallableFromTasklets(script, "AddPackageOverride", true);
  vm->SetCallableFromTasklets(script, "RemovePackageOverride", true);
  vm->SetCallableFromTasklets(script, "CountPackageOverride", true);
  vm->SetCallableFromTasklets(script, "ClearPackageOverride", true);
  vm->SetCallableFromTasklets(script, "RemoveAllPackageOverride", true);
}