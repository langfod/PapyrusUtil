#pragma once

namespace MiscUtil {
void RegisterFuncs(RE::BSScript::IVirtualMachine *vm);

// void ToggleFreeCamera(RE::StaticFunctionTag* base, bool arg1);
// void SetFreeCameraSpeed(RE::StaticFunctionTag* base, float speed);
// void SetFreeCameraState(RE::StaticFunctionTag* base, bool enable, float speed);
void PrintConsole(RE::StaticFunctionTag *base, RE::BSFixedString text);
// void SetMenus(RE::StaticFunctionTag* base, bool enabled);
RE::BSFixedString GetRaceEditorID(RE::StaticFunctionTag *base, RE::TESRace *RaceRef);
RE::BSFixedString GetActorRaceEditorID(RE::StaticFunctionTag *base, RE::Actor *ActorRef);
} // namespace MiscUtil
