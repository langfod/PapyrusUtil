#pragma once

#include "PCH.h"

namespace ActorUtil {
void RegisterFuncs(RE::BSScript::IVirtualMachine *vm);

void AddPackage(RE::StaticFunctionTag *base, RE::Actor *ActorRef, RE::TESPackage *PackageRef, UInt32 priority, UInt32 flags);
bool RemovePackage(RE::StaticFunctionTag *base, RE::Actor *ActorRef, RE::TESPackage *PackageRef);
UInt32 CountPackages(RE::StaticFunctionTag *base, const RE::Actor *ActorRef);
UInt32 ClearActor(RE::StaticFunctionTag *base, RE::Actor *ActorRef);
UInt32 ClearPackage(RE::StaticFunctionTag *base, RE::TESPackage *PackageRef);
} // namespace ActorUtil