/**
 * PapyrusUtil Native API
 *
 * This header provides a C++ API for other SKSE plugins to directly call
 * PapyrusUtil functions without going through Papyrus scripts.
 *
 * The API exposes three main systems:
 * - StorageUtil: In-memory key-value storage (persisted with savegame)
 * - JsonUtil: JSON file-based external storage
 * - ActorUtil: Package override system for actors
 *
 * USAGE:
 * 1. Include this header in your SKSE plugin
 * 2. Call PapyrusUtilAPI::GetAPI() after SKSE kPostLoad message
 * 3. Check the returned pointer and use the API functions
 *
 * Example:
 *   #include "PapyrusUtilAPI.h"
 *
 *   void OnSKSEMessage(SKSE::MessagingInterface::Message* msg) {
 *       if (msg->type == SKSE::MessagingInterface::kPostLoad) {
 *           if (auto api = PapyrusUtilAPI::GetAPI()) {
 *               // StorageUtil: Store data on a form (or use 0 for global)
 *               api->Storage.SetIntValue(0x14, "mymod_counter", 42);
 *               int32_t val = api->Storage.GetIntValue(0x14, "mymod_counter", 0);
 *
 *               // JsonUtil: Store data in external JSON files
 *               api->Json.Load("MyModConfig");
 *               api->Json.SetStringValue("MyModConfig", "setting", "enabled");
 *               api->Json.Save("MyModConfig", true);
 *
 *               // ActorUtil: Add package overrides
 *               api->Actor.AddPackageOverride(playerFormID, packageFormID, 100, 0);
 *           }
 *       }
 *   }
 */

#pragma once

#include <cstdint>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace PapyrusUtilAPI {

// =============================================================================
// Interface Version Enum
// =============================================================================

enum class InterfaceVersion : uint32_t {
  kV1 = 1,

  kLatest = kV1
};

// =============================================================================
// StorageUtil Interface - In-memory persistent storage
// =============================================================================

/**
 * StorageUtil provides key-value storage that persists with the savegame.
 * Data can be stored globally (objFormID = 0) or on specific forms.
 */
struct IStorageUtilInterface {
  // -------------------------------------------------------------------------
  // Value Operations (single key-value pairs)
  // -------------------------------------------------------------------------

  // Int values
  int32_t (*SetIntValue)(uint32_t objFormID, const char *key, int32_t value);
  int32_t (*GetIntValue)(uint32_t objFormID, const char *key, int32_t defaultVal);
  int32_t (*AdjustIntValue)(uint32_t objFormID, const char *key, int32_t adjustBy);
  bool (*HasIntValue)(uint32_t objFormID, const char *key);
  bool (*UnsetIntValue)(uint32_t objFormID, const char *key);

  // Float values
  float (*SetFloatValue)(uint32_t objFormID, const char *key, float value);
  float (*GetFloatValue)(uint32_t objFormID, const char *key, float defaultVal);
  float (*AdjustFloatValue)(uint32_t objFormID, const char *key, float adjustBy);
  bool (*HasFloatValue)(uint32_t objFormID, const char *key);
  bool (*UnsetFloatValue)(uint32_t objFormID, const char *key);

  // String values
  // NOTE: All functions returning const char* allocate new memory.
  // Caller MUST free returned strings with FreeString(), including:
  // - SetStringValue (returns copy of stored value)
  // - GetStringValue (returns copy of stored/default value)
  const char *(*SetStringValue)(uint32_t objFormID, const char *key, const char *value);
  const char *(*GetStringValue)(uint32_t objFormID, const char *key, const char *defaultVal);
  bool (*HasStringValue)(uint32_t objFormID, const char *key);
  bool (*UnsetStringValue)(uint32_t objFormID, const char *key);

  // Form values (stored/returned as FormIDs)
  uint32_t (*SetFormValue)(uint32_t objFormID, const char *key, uint32_t formID);
  uint32_t (*GetFormValue)(uint32_t objFormID, const char *key, uint32_t defaultFormID);
  bool (*HasFormValue)(uint32_t objFormID, const char *key);
  bool (*UnsetFormValue)(uint32_t objFormID, const char *key);

  // -------------------------------------------------------------------------
  // List Operations (arrays stored by key)
  // -------------------------------------------------------------------------

  // Int lists
  int32_t (*IntListAdd)(uint32_t objFormID, const char *key, int32_t value, bool allowDuplicate);
  int32_t (*IntListGet)(uint32_t objFormID, const char *key, uint32_t index);
  int32_t (*IntListSet)(uint32_t objFormID, const char *key, uint32_t index, int32_t value);
  uint32_t (*IntListRemove)(uint32_t objFormID, const char *key, int32_t value, bool allInstances);
  bool (*IntListRemoveAt)(uint32_t objFormID, const char *key, uint32_t index);
  bool (*IntListInsertAt)(uint32_t objFormID, const char *key, uint32_t index, int32_t value);
  uint32_t (*IntListClear)(uint32_t objFormID, const char *key);
  uint32_t (*IntListCount)(uint32_t objFormID, const char *key);
  int32_t (*IntListFind)(uint32_t objFormID, const char *key, int32_t value);
  bool (*IntListHas)(uint32_t objFormID, const char *key, int32_t value);
  void (*IntListSort)(uint32_t objFormID, const char *key);

  // Float lists
  int32_t (*FloatListAdd)(uint32_t objFormID, const char *key, float value, bool allowDuplicate);
  float (*FloatListGet)(uint32_t objFormID, const char *key, uint32_t index);
  float (*FloatListSet)(uint32_t objFormID, const char *key, uint32_t index, float value);
  uint32_t (*FloatListRemove)(uint32_t objFormID, const char *key, float value, bool allInstances);
  bool (*FloatListRemoveAt)(uint32_t objFormID, const char *key, uint32_t index);
  bool (*FloatListInsertAt)(uint32_t objFormID, const char *key, uint32_t index, float value);
  uint32_t (*FloatListClear)(uint32_t objFormID, const char *key);
  uint32_t (*FloatListCount)(uint32_t objFormID, const char *key);
  int32_t (*FloatListFind)(uint32_t objFormID, const char *key, float value);
  bool (*FloatListHas)(uint32_t objFormID, const char *key, float value);
  void (*FloatListSort)(uint32_t objFormID, const char *key);

  // String lists
  // NOTE: StringListGet and StringListSet return newly allocated strings.
  // Caller MUST free these with FreeString().
  int32_t (*StringListAdd)(uint32_t objFormID, const char *key, const char *value, bool allowDuplicate);
  const char *(*StringListGet)(uint32_t objFormID, const char *key, uint32_t index);
  const char *(*StringListSet)(uint32_t objFormID, const char *key, uint32_t index, const char *value);
  uint32_t (*StringListRemove)(uint32_t objFormID, const char *key, const char *value, bool allInstances);
  bool (*StringListRemoveAt)(uint32_t objFormID, const char *key, uint32_t index);
  bool (*StringListInsertAt)(uint32_t objFormID, const char *key, uint32_t index, const char *value);
  uint32_t (*StringListClear)(uint32_t objFormID, const char *key);
  uint32_t (*StringListCount)(uint32_t objFormID, const char *key);
  int32_t (*StringListFind)(uint32_t objFormID, const char *key, const char *value);
  bool (*StringListHas)(uint32_t objFormID, const char *key, const char *value);
  void (*StringListSort)(uint32_t objFormID, const char *key);

  // Form lists (stored/returned as FormIDs)
  int32_t (*FormListAdd)(uint32_t objFormID, const char *key, uint32_t formID, bool allowDuplicate);
  uint32_t (*FormListGet)(uint32_t objFormID, const char *key, uint32_t index);
  uint32_t (*FormListSet)(uint32_t objFormID, const char *key, uint32_t index, uint32_t formID);
  uint32_t (*FormListRemove)(uint32_t objFormID, const char *key, uint32_t formID, bool allInstances);
  bool (*FormListRemoveAt)(uint32_t objFormID, const char *key, uint32_t index);
  bool (*FormListInsertAt)(uint32_t objFormID, const char *key, uint32_t index, uint32_t formID);
  uint32_t (*FormListClear)(uint32_t objFormID, const char *key);
  uint32_t (*FormListCount)(uint32_t objFormID, const char *key);
  int32_t (*FormListFind)(uint32_t objFormID, const char *key, uint32_t formID);
  bool (*FormListHas)(uint32_t objFormID, const char *key, uint32_t formID);

  // -------------------------------------------------------------------------
  // Utility Operations
  // -------------------------------------------------------------------------

  /** Delete all values stored on a specific form */
  void (*DeleteValues)(uint32_t objFormID);

  /** Delete all values from all forms */
  void (*DeleteAllValues)();
};

// =============================================================================
// JsonUtil Interface - External JSON file storage
// =============================================================================

/**
 * JsonUtil provides JSON file-based storage in Data/SKSE/Plugins/StorageUtilData/.
 * Files are loaded into memory, modified, then saved back to disk.
 */
struct IJsonUtilInterface {
  // -------------------------------------------------------------------------
  // File Operations
  // -------------------------------------------------------------------------

  /** Load a JSON file into memory. Returns true if successful. */
  bool (*Load)(const char *filename);

  /** Save a JSON file to disk. styled=true for pretty-printed output. */
  bool (*Save)(const char *filename, bool styled);

  /** Unload a JSON file from memory. saveChanges will save before unloading. */
  bool (*Unload)(const char *filename, bool saveChanges, bool minify);

  /** Check if a file has pending unsaved changes. */
  bool (*IsPendingSave)(const char *filename);

  /** Check if a file is loaded and valid. */
  bool (*IsGood)(const char *filename);

  /** Get any parse errors from the file (caller must free with FreeString). */
  const char *(*GetErrors)(const char *filename);

  /** Clear all data in a file. */
  void (*ClearAll)(const char *filename);

  // -------------------------------------------------------------------------
  // Value Operations
  // -------------------------------------------------------------------------

  // Int values
  int32_t (*SetIntValue)(const char *filename, const char *key, int32_t value);
  int32_t (*GetIntValue)(const char *filename, const char *key, int32_t defaultVal);
  int32_t (*AdjustIntValue)(const char *filename, const char *key, int32_t adjustBy);
  bool (*HasIntValue)(const char *filename, const char *key);
  bool (*UnsetIntValue)(const char *filename, const char *key);

  // Float values
  float (*SetFloatValue)(const char *filename, const char *key, float value);
  float (*GetFloatValue)(const char *filename, const char *key, float defaultVal);
  float (*AdjustFloatValue)(const char *filename, const char *key, float adjustBy);
  bool (*HasFloatValue)(const char *filename, const char *key);
  bool (*UnsetFloatValue)(const char *filename, const char *key);

  // String values
  // NOTE: SetStringValue and GetStringValue return newly allocated strings.
  // Caller MUST free these with FreeString().
  const char *(*SetStringValue)(const char *filename, const char *key, const char *value);
  const char *(*GetStringValue)(const char *filename, const char *key, const char *defaultVal);
  bool (*HasStringValue)(const char *filename, const char *key);
  bool (*UnsetStringValue)(const char *filename, const char *key);

  // Form values (stored/returned as FormIDs)
  uint32_t (*SetFormValue)(const char *filename, const char *key, uint32_t formID);
  uint32_t (*GetFormValue)(const char *filename, const char *key, uint32_t defaultFormID);
  bool (*HasFormValue)(const char *filename, const char *key);
  bool (*UnsetFormValue)(const char *filename, const char *key);

  // -------------------------------------------------------------------------
  // List Operations
  // -------------------------------------------------------------------------

  // Int lists
  int32_t (*IntListAdd)(const char *filename, const char *key, int32_t value, bool allowDuplicate);
  int32_t (*IntListGet)(const char *filename, const char *key, uint32_t index);
  int32_t (*IntListSet)(const char *filename, const char *key, uint32_t index, int32_t value);
  uint32_t (*IntListRemove)(const char *filename, const char *key, int32_t value, bool allInstances);
  bool (*IntListRemoveAt)(const char *filename, const char *key, uint32_t index);
  bool (*IntListInsertAt)(const char *filename, const char *key, uint32_t index, int32_t value);
  uint32_t (*IntListClear)(const char *filename, const char *key);
  uint32_t (*IntListCount)(const char *filename, const char *key);
  int32_t (*IntListFind)(const char *filename, const char *key, int32_t value);
  bool (*IntListHas)(const char *filename, const char *key, int32_t value);

  // Float lists
  int32_t (*FloatListAdd)(const char *filename, const char *key, float value, bool allowDuplicate);
  float (*FloatListGet)(const char *filename, const char *key, uint32_t index);
  float (*FloatListSet)(const char *filename, const char *key, uint32_t index, float value);
  uint32_t (*FloatListRemove)(const char *filename, const char *key, float value, bool allInstances);
  bool (*FloatListRemoveAt)(const char *filename, const char *key, uint32_t index);
  bool (*FloatListInsertAt)(const char *filename, const char *key, uint32_t index, float value);
  uint32_t (*FloatListClear)(const char *filename, const char *key);
  uint32_t (*FloatListCount)(const char *filename, const char *key);
  int32_t (*FloatListFind)(const char *filename, const char *key, float value);
  bool (*FloatListHas)(const char *filename, const char *key, float value);

  // String lists
  // NOTE: StringListGet and StringListSet return newly allocated strings.
  // Caller MUST free these with FreeString().
  int32_t (*StringListAdd)(const char *filename, const char *key, const char *value, bool allowDuplicate);
  const char *(*StringListGet)(const char *filename, const char *key, uint32_t index);
  const char *(*StringListSet)(const char *filename, const char *key, uint32_t index, const char *value);
  uint32_t (*StringListRemove)(const char *filename, const char *key, const char *value, bool allInstances);
  bool (*StringListRemoveAt)(const char *filename, const char *key, uint32_t index);
  bool (*StringListInsertAt)(const char *filename, const char *key, uint32_t index, const char *value);
  uint32_t (*StringListClear)(const char *filename, const char *key);
  uint32_t (*StringListCount)(const char *filename, const char *key);
  int32_t (*StringListFind)(const char *filename, const char *key, const char *value);
  bool (*StringListHas)(const char *filename, const char *key, const char *value);

  // Form lists (stored/returned as FormIDs)
  int32_t (*FormListAdd)(const char *filename, const char *key, uint32_t formID, bool allowDuplicate);
  uint32_t (*FormListGet)(const char *filename, const char *key, uint32_t index);
  uint32_t (*FormListSet)(const char *filename, const char *key, uint32_t index, uint32_t formID);
  uint32_t (*FormListRemove)(const char *filename, const char *key, uint32_t formID, bool allInstances);
  bool (*FormListRemoveAt)(const char *filename, const char *key, uint32_t index);
  bool (*FormListInsertAt)(const char *filename, const char *key, uint32_t index, uint32_t formID);
  uint32_t (*FormListClear)(const char *filename, const char *key);
  uint32_t (*FormListCount)(const char *filename, const char *key);
  int32_t (*FormListFind)(const char *filename, const char *key, uint32_t formID);
  bool (*FormListHas)(const char *filename, const char *key, uint32_t formID);

  // -------------------------------------------------------------------------
  // Path-based Operations (JSON path access like ".foo.bar[0]")
  // -------------------------------------------------------------------------

  /** Set a value at a JSON path. */
  void (*SetPathIntValue)(const char *filename, const char *path, int32_t value);
  void (*SetPathFloatValue)(const char *filename, const char *path, float value);
  void (*SetPathStringValue)(const char *filename, const char *path, const char *value);
  void (*SetPathFormValue)(const char *filename, const char *path, uint32_t formID);

  /** Get a value at a JSON path.\n   *  NOTE: GetPathStringValue returns a newly allocated string.\n   *  Caller MUST free it with
   * FreeString(). */
  int32_t (*GetPathIntValue)(const char *filename, const char *path, int32_t defaultVal);
  float (*GetPathFloatValue)(const char *filename, const char *path, float defaultVal);
  const char *(*GetPathStringValue)(const char *filename, const char *path, const char *defaultVal);
  uint32_t (*GetPathFormValue)(const char *filename, const char *path, uint32_t defaultFormID);

  /** Check if a path exists and can be resolved. */
  bool (*CanResolvePath)(const char *filename, const char *path);

  /** Clear data at a JSON path. */
  void (*ClearPath)(const char *filename, const char *path);

  /** Get number of elements at a path (for arrays/objects). */
  int32_t (*PathCount)(const char *filename, const char *path);
};

// =============================================================================
// ActorUtil Interface - Package override system
// =============================================================================

/**
 * ActorUtil provides package override functionality for actors.
 * Allows adding temporary AI packages that take priority over normal packages.
 */
struct IActorUtilInterface {
  /**
   * Add a package override to an actor.
   * @param actorFormID The actor's FormID
   * @param packageFormID The package's FormID
   * @param priority Higher values = higher priority (typically 0-100)
   * @param flags Package flags (see TESPackage::GeneralData::FLAG)
   */
  void (*AddPackageOverride)(uint32_t actorFormID, uint32_t packageFormID, uint32_t priority, uint32_t flags);

  /** Remove a specific package override from an actor. Returns true if found and removed. */
  bool (*RemovePackageOverride)(uint32_t actorFormID, uint32_t packageFormID);

  /** Count the number of package overrides on an actor. */
  uint32_t (*CountPackageOverride)(uint32_t actorFormID);

  /** Clear all package overrides from an actor. Returns the number removed. */
  uint32_t (*ClearPackageOverride)(uint32_t actorFormID);

  /** Remove all instances of a package from all actors. Returns the number removed. */
  uint32_t (*RemoveAllPackageOverride)(uint32_t packageFormID);
};

// =============================================================================
// Main API Interface Structure (V1)
// =============================================================================

/**
 * The main API interface struct.
 * Obtain this via GetAPI() or RequestPluginAPI().
 */
struct IPapyrusUtilInterface {
  // =========================================================================
  // Interface Metadata
  // =========================================================================

  uint32_t interfaceVersion; // Interface version (matches InterfaceVersion enum)
  uint32_t pluginVersion;    // PapyrusUtil plugin version

  // =========================================================================
  // Sub-interfaces
  // =========================================================================

  IStorageUtilInterface Storage;
  IJsonUtilInterface Json;
  IActorUtilInterface Actor;

  // =========================================================================
  // Memory Management
  // =========================================================================

  /** Free a string returned by the API. */
  void (*FreeString)(const char *str);
};

// =============================================================================
// API Request Function Type (exported by PapyrusUtil.dll)
// =============================================================================

using _RequestPluginAPI = IPapyrusUtilInterface *(*)(InterfaceVersion version);

// =============================================================================
// Consumer Helper Functions
// =============================================================================

/**
 * Request the PapyrusUtil API interface.
 * Call this during or after SKSE kPostLoad message handling.
 *
 * @param version The interface version to request (default: kLatest)
 * @return Pointer to the API interface, or nullptr if unavailable/incompatible
 */
[[nodiscard]] inline IPapyrusUtilInterface *RequestPluginAPI(InterfaceVersion version = InterfaceVersion::kLatest) {
  HMODULE pluginHandle = GetModuleHandleW(L"PapyrusUtil.dll");
  if (!pluginHandle) {
    return nullptr;
  }

  auto requestFunc = reinterpret_cast<_RequestPluginAPI>(GetProcAddress(pluginHandle, "PapyrusUtilAPI_RequestInterface"));

  if (!requestFunc) {
    return nullptr;
  }

  return requestFunc(version);
}

/**
 * Convenience function to get the latest API version.
 * Equivalent to RequestPluginAPI(InterfaceVersion::kLatest).
 */
[[nodiscard]] inline IPapyrusUtilInterface *GetAPI() { return RequestPluginAPI(InterfaceVersion::kLatest); }

/**
 * Check if PapyrusUtil API is available without obtaining it.
 */
[[nodiscard]] inline bool IsAvailable() {
  HMODULE pluginHandle = GetModuleHandleW(L"PapyrusUtil.dll");
  if (!pluginHandle) {
    return false;
  }
  return GetProcAddress(pluginHandle, "PapyrusUtilAPI_RequestInterface") != nullptr;
}

} // namespace PapyrusUtilAPI
