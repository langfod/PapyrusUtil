#pragma once

#include "PCH.h"

namespace Plugin {
	void InitPlugin();
	bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm);
	void HandleSKSEMessage(SKSE::MessagingInterface::Message* msg);
	UInt32 GetVersion(RE::StaticFunctionTag* base);
}
