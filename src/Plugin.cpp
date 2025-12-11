#include "Plugin.h"
#include "PCH.h"


#include "PackageData.h"

#include "ActorUtil.h"
#include "JsonUtil.h"
#include "MiscUtil.h"
#include "PapyrusUtil.h"
#include "Serialize.h"
#include "StorageUtil.h"

namespace Plugin {

UInt32 GetVersion(RE::StaticFunctionTag *base) { return PAPYRUSUTIL_VERSION; }

void InitPlugin() {
  logger::info("Init...");

  PackageData::InitPlugin();

  logger::info("-done");
}

bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine *vm) {
  logger::info("Registering Papyrus functions...");

  // Register GetVersion function
  vm->RegisterFunction("GetVersion", "PapyrusUtil", GetVersion);
  vm->SetCallableFromTasklets("PapyrusUtil", "GetVersion", true);

  // Register other module functions
  PapyrusUtil::RegisterFuncs(vm);
  StorageUtil::RegisterFuncs(vm);
  JsonUtil::RegisterFuncs(vm);
  ActorUtil::RegisterFuncs(vm);
  MiscUtil::RegisterFuncs(vm);

  logger::info("-done");
  return true;
}

void HandleSKSEMessage(SKSE::MessagingInterface::Message *msg) {
  // Handle SKSE messages if needed
}

} // namespace Plugin
