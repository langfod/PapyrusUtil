#pragma once

// CommonLibSSE-NG Precompiled Header
// This provides modern CommonLibSSE-NG API and compatibility types

// IMPORTANT: CommonLibSSE-NG headers MUST come before any Windows API includes
// or headers that pull in Windows.h (like boost/filesystem)

#pragma warning(push)
// CommonLibSSE-NG headers - MUST BE FIRST
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>
#pragma warning(pop)

// spdlog for logging - always include basic_file_sink
#include <spdlog/sinks/basic_file_sink.h>
#ifndef NDEBUG
#	include <spdlog/sinks/msvc_sink.h>
#endif

// C++ Standard Library (these don't include Windows.h)
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// Windows API - safe to include after CommonLibSSE-NG
#include <ShlObj.h>

// Boost throw_exception definition - needed when exceptions are disabled
// Must be defined before including Boost headers
#include <boost/config.hpp>
#include <boost/throw_exception.hpp>

namespace boost {
    BOOST_NORETURN inline void throw_exception(std::exception const& e) {
        SKSE::stl::report_and_fail(e.what());
    }

    BOOST_NORETURN inline void throw_exception(std::exception const& e, boost::source_location const&) {
        SKSE::stl::report_and_fail(e.what());
    }
}

// Boost libraries
#include <boost/algorithm/string.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/random.hpp>

// JSON library
#include <json/json.h>

using namespace std::literals;

namespace logger = SKSE::log;

namespace util
{
	using SKSE::stl::report_and_fail;
}

#define DLLEXPORT __declspec(dllexport)

// Plugin version constant
#define PAPYRUSUTIL_VERSION 45

// Type aliases for compatibility with old SKSE64 code
// These provide a bridge from SKSE64 types to CommonLibSSE-NG types
using UInt8 = std::uint8_t;
using UInt16 = std::uint16_t;
using UInt32 = std::uint32_t;
using UInt64 = std::uint64_t;
using SInt8 = std::int8_t;
using SInt16 = std::int16_t;
using SInt32 = std::int32_t;
using SInt64 = std::int64_t;

// Form types from RE namespace - for compatibility
// TODO: use RE:: namespace directly in code
using TESForm = RE::TESForm;
using Actor = RE::Actor;
using TESObjectREFR = RE::TESObjectREFR;
using BGSKeyword = RE::BGSKeyword;
using BSFixedString = RE::BSFixedString;
using TESQuest = RE::TESQuest;
using TESGlobal = RE::TESGlobal;
using TESObjectCELL = RE::TESObjectCELL;
using TESWorldSpace = RE::TESWorldSpace;
using SpellItem = RE::SpellItem;
using AlchemyItem = RE::AlchemyItem;
using EffectSetting = RE::EffectSetting;
using TESShout = RE::TESShout;
using TESRace = RE::TESRace;
using TESFaction = RE::TESFaction;
using BGSPerk = RE::BGSPerk;
using BGSMusicType = RE::BGSMusicType;
using TESWeather = RE::TESWeather;
using TESPackage = RE::TESPackage;
using BGSLocation = RE::BGSLocation;
using TESWordOfPower = RE::TESWordOfPower;
using EnchantmentItem = RE::EnchantmentItem;
using TESAmmo = RE::TESAmmo;
using TESObjectARMO = RE::TESObjectARMO;
using TESObjectWEAP = RE::TESObjectWEAP;
using TESObjectBOOK = RE::TESObjectBOOK;
using TESObjectMISC = RE::TESObjectMISC;
using IngredientItem = RE::IngredientItem;
using ScrollItem = RE::ScrollItem;
using TESKey = RE::TESKey;
using TESSoulGem = RE::TESSoulGem;
using TESLevItem = RE::TESLevItem;
using TESFlora = RE::TESFlora;
using TESFurniture = RE::TESFurniture;
using BGSApparatus = RE::BGSApparatus;
using TESObjectACTI = RE::TESObjectACTI;
using TESObjectDOOR = RE::TESObjectDOOR;
using TESObjectLIGH = RE::TESObjectLIGH;
using TESObjectSTAT = RE::TESObjectSTAT;
using TESObjectTREE = RE::TESObjectTREE;
using BGSMovableStatic = RE::BGSMovableStatic;
using BGSTalkingActivator = RE::BGSTalkingActivator;
using BGSTextureSet = RE::BGSTextureSet;
using BGSHeadPart = RE::BGSHeadPart;
using TESNPC = RE::TESNPC;
using TESLevCharacter = RE::TESLevCharacter;
using TESImageSpace = RE::TESImageSpace;
using TESSound = RE::TESSound;
using BGSSoundDescriptorForm = RE::BGSSoundDescriptorForm;
using TESCombatStyle = RE::TESCombatStyle;
using TESEffectShader = RE::TESEffectShader;
using BGSExplosion = RE::BGSExplosion;
using BGSProjectile = RE::BGSProjectile;
using BGSHazard = RE::BGSHazard;
using BGSImpactDataSet = RE::BGSImpactDataSet;
using BGSFootstepSet = RE::BGSFootstepSet;
using BGSOutfit = RE::BGSOutfit;
using BGSArtObject = RE::BGSArtObject;
using BGSMaterialObject = RE::BGSMaterialObject;
using BGSEncounterZone = RE::BGSEncounterZone;
using BGSLightingTemplate = RE::BGSLightingTemplate;
using BGSRelationship = RE::BGSRelationship;
using BGSScene = RE::BGSScene;
using BGSAssociationType = RE::BGSAssociationType;
using BGSListForm = RE::BGSListForm;
using BGSColorForm = RE::BGSColorForm;
using BGSReferenceEffect = RE::BGSReferenceEffect;
using TESRegion = RE::TESRegion;
using TESIdleForm = RE::TESIdleForm;
using TESEyes = RE::TESEyes;
using TESClass = RE::TESClass;
using TESObjectANIO = RE::TESObjectANIO;
using TESWaterForm = RE::TESWaterForm;
using TESObjectLAND = RE::TESObjectLAND;
using NavMesh = RE::NavMesh;

// VM types from RE namespace  
using StaticFunctionTag = RE::StaticFunctionTag;

// Template aliases for Papyrus arrays
template<typename T>
using VMArray = std::vector<T>;

template<typename T>
using VMResultArray = std::vector<T>;

// Form lookup functions (replaces LookupFormByID)
inline TESForm* LookupFormByID(UInt32 formID) {
    return RE::TESForm::LookupByID(formID);
}

template<typename T>
inline T* LookupFormByID(UInt32 formID) {
    return RE::TESForm::LookupByID<T>(formID);
}

// Plugin handle type
using PluginHandle = SKSE::PluginHandle;
inline constexpr PluginHandle kPluginHandle_Invalid = SKSE::kInvalidPluginHandle;

// Critical section replacement using std::recursive_mutex
class ICriticalSection {
public:
    void Enter() { m_mutex.lock(); }
    void Leave() { m_mutex.unlock(); }
    [[nodiscard]] bool TryEnter() { return m_mutex.try_lock(); }
private:
    std::recursive_mutex m_mutex;
};

// Legacy SKSE64 logging macro compatibility
// These are no-ops for commented debug code that uses printf-style formatting
// For active logging, use logger::info/debug/error with fmt-style {} formatting
#define _MESSAGE(fmt, ...) ((void)0)
#define _ERROR(fmt, ...) ((void)0)
#define _WARNING(fmt, ...) ((void)0)
#define _DMESSAGE(fmt, ...) ((void)0)
