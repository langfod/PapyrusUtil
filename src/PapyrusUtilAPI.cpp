/**
 * PapyrusUtil Native API Implementation
 * 
 * This file implements the exported API interface that allows other SKSE plugins
 * to directly call PapyrusUtil functionality without going through Papyrus scripts.
 * 
 * Provides three main interfaces:
 * - StorageUtil: In-memory persistent key-value storage (via Data.h backend)
 * - JsonUtil: External JSON file-based storage (via External.h backend)
 * - ActorUtil: Package override system (via PackageData.h backend)
 */

#include "../include/PapyrusUtilAPI.h"
#include "Data.h"
#include "External.h"
#include "PackageData.h"
#include "Forms.h"
#include "PCH.h"

#include <cstring>

namespace {

// =============================================================================
// Helper Functions
// =============================================================================

// Duplicate a C string (caller must free with FreeString)
char* DuplicateString(const char* str) {
    if (!str) return nullptr;
    size_t len = strlen(str) + 1;
    char* result = new char[len];
    memcpy(result, str, len);
    return result;
}

char* DuplicateString(const std::string& str) {
    size_t len = str.length() + 1;
    char* result = new char[len];
    memcpy(result, str.c_str(), len);
    return result;
}

char* DuplicateString(const RE::BSFixedString& str) {
    if (!str.data()) return DuplicateString("");
    return DuplicateString(str.data());
}

void Impl_FreeString(const char* str) {
    delete[] str;
}

// Get form key for StorageUtil (0 for global storage)
inline UInt64 GetFormKey(uint32_t formID) {
    if (formID == 0) return 0;
    RE::TESForm* form = RE::TESForm::LookupByID(formID);
    return form ? Forms::GetFormKey(form) : 0;
}

// =============================================================================
// StorageUtil Value Operations
// =============================================================================

// --- Int Values ---
int32_t Impl_Storage_SetIntValue(uint32_t objFormID, const char* key, int32_t value) {
    auto* data = Data::GetValues<SInt32, SInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    data->SetValue(formKey, key, value);
    return value;
}

int32_t Impl_Storage_GetIntValue(uint32_t objFormID, const char* key, int32_t defaultVal) {
    auto* data = Data::GetValues<SInt32, SInt32>();
    if (!data || !key || !key[0]) return defaultVal;
    UInt64 formKey = GetFormKey(objFormID);
    return data->GetValue(formKey, key, defaultVal);
}

int32_t Impl_Storage_AdjustIntValue(uint32_t objFormID, const char* key, int32_t adjustBy) {
    auto* data = Data::GetValues<SInt32, SInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->AdjustValue(formKey, key, adjustBy);
}

bool Impl_Storage_HasIntValue(uint32_t objFormID, const char* key) {
    auto* data = Data::GetValues<SInt32, SInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->HasValue(formKey, key);
}

bool Impl_Storage_UnsetIntValue(uint32_t objFormID, const char* key) {
    auto* data = Data::GetValues<SInt32, SInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->UnsetValue(formKey, key);
}

// --- Float Values ---
float Impl_Storage_SetFloatValue(uint32_t objFormID, const char* key, float value) {
    auto* data = Data::GetValues<float, float>();
    if (!data || !key || !key[0]) return 0.0f;
    UInt64 formKey = GetFormKey(objFormID);
    data->SetValue(formKey, key, value);
    return value;
}

float Impl_Storage_GetFloatValue(uint32_t objFormID, const char* key, float defaultVal) {
    auto* data = Data::GetValues<float, float>();
    if (!data || !key || !key[0]) return defaultVal;
    UInt64 formKey = GetFormKey(objFormID);
    return data->GetValue(formKey, key, defaultVal);
}

float Impl_Storage_AdjustFloatValue(uint32_t objFormID, const char* key, float adjustBy) {
    auto* data = Data::GetValues<float, float>();
    if (!data || !key || !key[0]) return 0.0f;
    UInt64 formKey = GetFormKey(objFormID);
    return data->AdjustValue(formKey, key, adjustBy);
}

bool Impl_Storage_HasFloatValue(uint32_t objFormID, const char* key) {
    auto* data = Data::GetValues<float, float>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->HasValue(formKey, key);
}

bool Impl_Storage_UnsetFloatValue(uint32_t objFormID, const char* key) {
    auto* data = Data::GetValues<float, float>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->UnsetValue(formKey, key);
}

// --- String Values ---
const char* Impl_Storage_SetStringValue(uint32_t objFormID, const char* key, const char* value) {
    auto* data = Data::GetValues<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return DuplicateString("");
    UInt64 formKey = GetFormKey(objFormID);
    RE::BSFixedString bsValue(value ? value : "");
    data->SetValue(formKey, key, bsValue);
    return DuplicateString(value ? value : "");
}

const char* Impl_Storage_GetStringValue(uint32_t objFormID, const char* key, const char* defaultVal) {
    auto* data = Data::GetValues<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return DuplicateString(defaultVal ? defaultVal : "");
    UInt64 formKey = GetFormKey(objFormID);
    RE::BSFixedString bsDefault(defaultVal ? defaultVal : "");
    RE::BSFixedString result = data->GetValue(formKey, key, bsDefault);
    return DuplicateString(result);
}

bool Impl_Storage_HasStringValue(uint32_t objFormID, const char* key) {
    auto* data = Data::GetValues<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->HasValue(formKey, key);
}

bool Impl_Storage_UnsetStringValue(uint32_t objFormID, const char* key) {
    auto* data = Data::GetValues<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->UnsetValue(formKey, key);
}

// --- Form Values ---
uint32_t Impl_Storage_SetFormValue(uint32_t objFormID, const char* key, uint32_t formID) {
    auto* data = Data::GetValues<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    RE::TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    data->SetValue(formKey, key, form);
    return formID;
}

uint32_t Impl_Storage_GetFormValue(uint32_t objFormID, const char* key, uint32_t defaultFormID) {
    auto* data = Data::GetValues<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return defaultFormID;
    UInt64 formKey = GetFormKey(objFormID);
    RE::TESForm* defaultForm = defaultFormID ? RE::TESForm::LookupByID(defaultFormID) : nullptr;
    RE::TESForm* result = data->GetValue(formKey, key, defaultForm);
    return result ? result->formID : 0;
}

bool Impl_Storage_HasFormValue(uint32_t objFormID, const char* key) {
    auto* data = Data::GetValues<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->HasValue(formKey, key);
}

bool Impl_Storage_UnsetFormValue(uint32_t objFormID, const char* key) {
    auto* data = Data::GetValues<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->UnsetValue(formKey, key);
}

// =============================================================================
// StorageUtil Int List Operations
// =============================================================================

int32_t Impl_Storage_IntListAdd(uint32_t objFormID, const char* key, int32_t value, bool allowDuplicate) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return -1;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListAdd(formKey, key, value, allowDuplicate);
}

int32_t Impl_Storage_IntListGet(uint32_t objFormID, const char* key, uint32_t index) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListGet(formKey, key, index);
}

int32_t Impl_Storage_IntListSet(uint32_t objFormID, const char* key, uint32_t index, int32_t value) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListSet(formKey, key, index, value);
}

uint32_t Impl_Storage_IntListRemove(uint32_t objFormID, const char* key, int32_t value, bool allInstances) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListRemove(formKey, key, value, allInstances);
}

bool Impl_Storage_IntListRemoveAt(uint32_t objFormID, const char* key, uint32_t index) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListRemoveAt(formKey, key, index);
}

bool Impl_Storage_IntListInsertAt(uint32_t objFormID, const char* key, uint32_t index, int32_t value) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListInsertAt(formKey, key, index, value);
}

uint32_t Impl_Storage_IntListClear(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListClear(formKey, key);
}

uint32_t Impl_Storage_IntListCount(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListCount(formKey, key);
}

int32_t Impl_Storage_IntListFind(uint32_t objFormID, const char* key, int32_t value) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return -1;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListFind(formKey, key, value);
}

bool Impl_Storage_IntListHas(uint32_t objFormID, const char* key, int32_t value) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListHas(formKey, key, value);
}

void Impl_Storage_IntListSort(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<SInt32, SInt32>();
    if (!data || !key || !key[0]) return;
    UInt64 formKey = GetFormKey(objFormID);
    data->ListSort(formKey, key);
}

// =============================================================================
// StorageUtil Float List Operations
// =============================================================================

int32_t Impl_Storage_FloatListAdd(uint32_t objFormID, const char* key, float value, bool allowDuplicate) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return -1;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListAdd(formKey, key, value, allowDuplicate);
}

float Impl_Storage_FloatListGet(uint32_t objFormID, const char* key, uint32_t index) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return 0.0f;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListGet(formKey, key, index);
}

float Impl_Storage_FloatListSet(uint32_t objFormID, const char* key, uint32_t index, float value) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return 0.0f;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListSet(formKey, key, index, value);
}

uint32_t Impl_Storage_FloatListRemove(uint32_t objFormID, const char* key, float value, bool allInstances) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListRemove(formKey, key, value, allInstances);
}

bool Impl_Storage_FloatListRemoveAt(uint32_t objFormID, const char* key, uint32_t index) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListRemoveAt(formKey, key, index);
}

bool Impl_Storage_FloatListInsertAt(uint32_t objFormID, const char* key, uint32_t index, float value) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListInsertAt(formKey, key, index, value);
}

uint32_t Impl_Storage_FloatListClear(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListClear(formKey, key);
}

uint32_t Impl_Storage_FloatListCount(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListCount(formKey, key);
}

int32_t Impl_Storage_FloatListFind(uint32_t objFormID, const char* key, float value) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return -1;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListFind(formKey, key, value);
}

bool Impl_Storage_FloatListHas(uint32_t objFormID, const char* key, float value) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListHas(formKey, key, value);
}

void Impl_Storage_FloatListSort(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<float, float>();
    if (!data || !key || !key[0]) return;
    UInt64 formKey = GetFormKey(objFormID);
    data->ListSort(formKey, key);
}

// =============================================================================
// StorageUtil String List Operations
// =============================================================================

int32_t Impl_Storage_StringListAdd(uint32_t objFormID, const char* key, const char* value, bool allowDuplicate) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return -1;
    UInt64 formKey = GetFormKey(objFormID);
    RE::BSFixedString bsValue(value ? value : "");
    return data->ListAdd(formKey, key, bsValue, allowDuplicate);
}

const char* Impl_Storage_StringListGet(uint32_t objFormID, const char* key, uint32_t index) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return DuplicateString("");
    UInt64 formKey = GetFormKey(objFormID);
    RE::BSFixedString result = data->ListGet(formKey, key, index);
    return DuplicateString(result);
}

const char* Impl_Storage_StringListSet(uint32_t objFormID, const char* key, uint32_t index, const char* value) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return DuplicateString("");
    UInt64 formKey = GetFormKey(objFormID);
    RE::BSFixedString bsValue(value ? value : "");
    RE::BSFixedString result = data->ListSet(formKey, key, index, bsValue);
    return DuplicateString(result);
}

uint32_t Impl_Storage_StringListRemove(uint32_t objFormID, const char* key, const char* value, bool allInstances) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    RE::BSFixedString bsValue(value ? value : "");
    return data->ListRemove(formKey, key, bsValue, allInstances);
}

bool Impl_Storage_StringListRemoveAt(uint32_t objFormID, const char* key, uint32_t index) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListRemoveAt(formKey, key, index);
}

bool Impl_Storage_StringListInsertAt(uint32_t objFormID, const char* key, uint32_t index, const char* value) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    RE::BSFixedString bsValue(value ? value : "");
    return data->ListInsertAt(formKey, key, index, bsValue);
}

uint32_t Impl_Storage_StringListClear(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListClear(formKey, key);
}

uint32_t Impl_Storage_StringListCount(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListCount(formKey, key);
}

int32_t Impl_Storage_StringListFind(uint32_t objFormID, const char* key, const char* value) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return -1;
    UInt64 formKey = GetFormKey(objFormID);
    RE::BSFixedString bsValue(value ? value : "");
    return data->ListFind(formKey, key, bsValue);
}

bool Impl_Storage_StringListHas(uint32_t objFormID, const char* key, const char* value) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    RE::BSFixedString bsValue(value ? value : "");
    return data->ListHas(formKey, key, bsValue);
}

void Impl_Storage_StringListSort(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<RE::BSFixedString, std::string>();
    if (!data || !key || !key[0]) return;
    UInt64 formKey = GetFormKey(objFormID);
    data->ListSort(formKey, key);
}

// =============================================================================
// StorageUtil Form List Operations
// =============================================================================

int32_t Impl_Storage_FormListAdd(uint32_t objFormID, const char* key, uint32_t formID, bool allowDuplicate) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return -1;
    UInt64 formKey = GetFormKey(objFormID);
    RE::TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return data->ListAdd(formKey, key, form, allowDuplicate);
}

uint32_t Impl_Storage_FormListGet(uint32_t objFormID, const char* key, uint32_t index) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    RE::TESForm* result = data->ListGet(formKey, key, index);
    return result ? result->formID : 0;
}

uint32_t Impl_Storage_FormListSet(uint32_t objFormID, const char* key, uint32_t index, uint32_t formID) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    RE::TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    RE::TESForm* result = data->ListSet(formKey, key, index, form);
    return result ? result->formID : 0;
}

uint32_t Impl_Storage_FormListRemove(uint32_t objFormID, const char* key, uint32_t formID, bool allInstances) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    RE::TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return data->ListRemove(formKey, key, form, allInstances);
}

bool Impl_Storage_FormListRemoveAt(uint32_t objFormID, const char* key, uint32_t index) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListRemoveAt(formKey, key, index);
}

bool Impl_Storage_FormListInsertAt(uint32_t objFormID, const char* key, uint32_t index, uint32_t formID) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    RE::TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return data->ListInsertAt(formKey, key, index, form);
}

uint32_t Impl_Storage_FormListClear(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListClear(formKey, key);
}

uint32_t Impl_Storage_FormListCount(uint32_t objFormID, const char* key) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return 0;
    UInt64 formKey = GetFormKey(objFormID);
    return data->ListCount(formKey, key);
}

int32_t Impl_Storage_FormListFind(uint32_t objFormID, const char* key, uint32_t formID) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return -1;
    UInt64 formKey = GetFormKey(objFormID);
    RE::TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return data->ListFind(formKey, key, form);
}

bool Impl_Storage_FormListHas(uint32_t objFormID, const char* key, uint32_t formID) {
    auto* data = Data::GetLists<RE::TESForm*, UInt32>();
    if (!data || !key || !key[0]) return false;
    UInt64 formKey = GetFormKey(objFormID);
    RE::TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return data->ListHas(formKey, key, form);
}

// =============================================================================
// StorageUtil Utility Operations
// =============================================================================

void Impl_Storage_DeleteValues(uint32_t objFormID) {
    UInt64 formKey = GetFormKey(objFormID);
    
    // Clear all value types
    if (auto* intData = Data::GetValues<SInt32, SInt32>()) {
        intData->RemoveForm(formKey);
    }
    if (auto* floatData = Data::GetValues<float, float>()) {
        floatData->RemoveForm(formKey);
    }
    if (auto* stringData = Data::GetValues<RE::BSFixedString, std::string>()) {
        stringData->RemoveForm(formKey);
    }
    if (auto* formData = Data::GetValues<RE::TESForm*, UInt32>()) {
        formData->RemoveForm(formKey);
    }
    
    // Clear all list types
    if (auto* intList = Data::GetLists<SInt32, SInt32>()) {
        intList->RemoveForm(formKey);
    }
    if (auto* floatList = Data::GetLists<float, float>()) {
        floatList->RemoveForm(formKey);
    }
    if (auto* stringList = Data::GetLists<RE::BSFixedString, std::string>()) {
        stringList->RemoveForm(formKey);
    }
    if (auto* formList = Data::GetLists<RE::TESForm*, UInt32>()) {
        formList->RemoveForm(formKey);
    }
}

void Impl_Storage_DeleteAllValues() {
    // Revert all data storage - clears everything
    Data::Serialization_Revert(nullptr);
}

// =============================================================================
// JsonUtil File Operations
// =============================================================================

bool Impl_Json_Load(const char* filename) {
    if (!filename || !filename[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    return file ? file->LoadFile() : false;
}

bool Impl_Json_Save(const char* filename, bool styled) {
    if (!filename || !filename[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    return file ? file->SaveFile(styled) : false;
}

bool Impl_Json_Unload(const char* filename, bool saveChanges, bool minify) {
    if (!filename || !filename[0]) return false;
    if (saveChanges) {
        External::ExternalFile* file = External::GetFile(filename);
        if (file) {
            file->minify = minify;
            file->SaveFile(!minify);
        }
    }
    return External::UnloadFile(filename);
}

bool Impl_Json_IsPendingSave(const char* filename) {
    if (!filename || !filename[0]) return false;
    return External::ChangesPending(filename);
}

bool Impl_Json_IsGood(const char* filename) {
    if (!filename || !filename[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    return file ? file->IsGood() : false;
}

const char* Impl_Json_GetErrors(const char* filename) {
    if (!filename || !filename[0]) return DuplicateString("");
    External::ExternalFile* file = External::GetFile(filename);
    return file ? DuplicateString(file->GetErrors()) : DuplicateString("");
}

void Impl_Json_ClearAll(const char* filename) {
    if (!filename || !filename[0]) return;
    External::ExternalFile* file = External::GetFile(filename);
    if (file) file->ClearAll();
}

// =============================================================================
// JsonUtil Int Value Operations
// =============================================================================

int32_t Impl_Json_SetIntValue(const char* filename, const char* key, int32_t value) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    file->SetValue(External::Type<SInt32>(), key, External::MakeValue<SInt32>(value));
    return value;
}

int32_t Impl_Json_GetIntValue(const char* filename, const char* key, int32_t defaultVal) {
    if (!filename || !filename[0] || !key || !key[0]) return defaultVal;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return defaultVal;
    return External::ParseValue<SInt32>(
        file->GetValue(External::Type<SInt32>(), key, External::MakeValue<SInt32>(defaultVal)),
        defaultVal
    );
}

int32_t Impl_Json_AdjustIntValue(const char* filename, const char* key, int32_t adjustBy) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return External::ParseValue<SInt32>(
        file->AdjustValue(External::Type<SInt32>(), key, External::MakeValue<SInt32>(adjustBy)),
        0
    );
}

bool Impl_Json_HasIntValue(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->HasValue(External::Type<SInt32>(), key);
}

bool Impl_Json_UnsetIntValue(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->UnsetValue(External::Type<SInt32>(), key);
}

// =============================================================================
// JsonUtil Float Value Operations
// =============================================================================

float Impl_Json_SetFloatValue(const char* filename, const char* key, float value) {
    if (!filename || !filename[0] || !key || !key[0]) return 0.0f;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0.0f;
    file->SetValue(External::Type<float>(), key, External::MakeValue<float>(value));
    return value;
}

float Impl_Json_GetFloatValue(const char* filename, const char* key, float defaultVal) {
    if (!filename || !filename[0] || !key || !key[0]) return defaultVal;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return defaultVal;
    return External::ParseValue<float>(
        file->GetValue(External::Type<float>(), key, External::MakeValue<float>(defaultVal)),
        defaultVal
    );
}

float Impl_Json_AdjustFloatValue(const char* filename, const char* key, float adjustBy) {
    if (!filename || !filename[0] || !key || !key[0]) return 0.0f;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0.0f;
    return External::ParseValue<float>(
        file->AdjustValue(External::Type<float>(), key, External::MakeValue<float>(adjustBy)),
        0.0f
    );
}

bool Impl_Json_HasFloatValue(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->HasValue(External::Type<float>(), key);
}

bool Impl_Json_UnsetFloatValue(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->UnsetValue(External::Type<float>(), key);
}

// =============================================================================
// JsonUtil String Value Operations
// =============================================================================

const char* Impl_Json_SetStringValue(const char* filename, const char* key, const char* value) {
    if (!filename || !filename[0] || !key || !key[0]) return DuplicateString("");
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return DuplicateString("");
    BSFixedString bsValue(value ? value : "");
    file->SetValue(External::Type<BSFixedString>(), key, External::MakeValue<BSFixedString>(bsValue));
    return DuplicateString(value ? value : "");
}

const char* Impl_Json_GetStringValue(const char* filename, const char* key, const char* defaultVal) {
    if (!filename || !filename[0] || !key || !key[0]) return DuplicateString(defaultVal ? defaultVal : "");
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return DuplicateString(defaultVal ? defaultVal : "");
    BSFixedString bsDefault(defaultVal ? defaultVal : "");
    BSFixedString result = External::ParseValue<BSFixedString>(
        file->GetValue(External::Type<BSFixedString>(), key, External::MakeValue<BSFixedString>(bsDefault)),
        bsDefault
    );
    return DuplicateString(result);
}

bool Impl_Json_HasStringValue(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->HasValue(External::Type<BSFixedString>(), key);
}

bool Impl_Json_UnsetStringValue(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->UnsetValue(External::Type<BSFixedString>(), key);
}

// =============================================================================
// JsonUtil Form Value Operations
// =============================================================================

uint32_t Impl_Json_SetFormValue(const char* filename, const char* key, uint32_t formID) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    file->SetValue(External::Type<TESForm*>(), key, External::MakeValue<TESForm*>(form));
    return formID;
}

uint32_t Impl_Json_GetFormValue(const char* filename, const char* key, uint32_t defaultFormID) {
    if (!filename || !filename[0] || !key || !key[0]) return defaultFormID;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return defaultFormID;
    TESForm* defaultForm = defaultFormID ? RE::TESForm::LookupByID(defaultFormID) : nullptr;
    TESForm* result = External::ParseValue<TESForm*>(
        file->GetValue(External::Type<TESForm*>(), key, External::MakeValue<TESForm*>(defaultForm)),
        defaultForm
    );
    return result ? result->formID : 0;
}

bool Impl_Json_HasFormValue(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->HasValue(External::Type<TESForm*>(), key);
}

bool Impl_Json_UnsetFormValue(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->UnsetValue(External::Type<TESForm*>(), key);
}

// =============================================================================
// JsonUtil Int List Operations
// =============================================================================

int32_t Impl_Json_IntListAdd(const char* filename, const char* key, int32_t value, bool allowDuplicate) {
    if (!filename || !filename[0] || !key || !key[0]) return -1;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return -1;
    return file->ListAdd(External::List<SInt32>(), key, External::MakeValue<SInt32>(value), allowDuplicate);
}

int32_t Impl_Json_IntListGet(const char* filename, const char* key, uint32_t index) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return External::ParseValue<SInt32>(file->ListGet(External::List<SInt32>(), key, index), 0);
}

int32_t Impl_Json_IntListSet(const char* filename, const char* key, uint32_t index, int32_t value) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return External::ParseValue<SInt32>(
        file->ListSet(External::List<SInt32>(), key, index, External::MakeValue<SInt32>(value)), 0);
}

uint32_t Impl_Json_IntListRemove(const char* filename, const char* key, int32_t value, bool allInstances) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListRemove(External::List<SInt32>(), key, External::MakeValue<SInt32>(value), allInstances);
}

bool Impl_Json_IntListRemoveAt(const char* filename, const char* key, uint32_t index) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->ListRemoveAt(External::List<SInt32>(), key, index);
}

bool Impl_Json_IntListInsertAt(const char* filename, const char* key, uint32_t index, int32_t value) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->ListInsertAt(External::List<SInt32>(), key, index, External::MakeValue<SInt32>(value));
}

uint32_t Impl_Json_IntListClear(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListClear(External::List<SInt32>(), key);
}

uint32_t Impl_Json_IntListCount(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListCount(External::List<SInt32>(), key);
}

int32_t Impl_Json_IntListFind(const char* filename, const char* key, int32_t value) {
    if (!filename || !filename[0] || !key || !key[0]) return -1;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return -1;
    return file->ListFind(External::List<SInt32>(), key, External::MakeValue<SInt32>(value));
}

bool Impl_Json_IntListHas(const char* filename, const char* key, int32_t value) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->ListFind(External::List<SInt32>(), key, External::MakeValue<SInt32>(value)) != -1;
}

// =============================================================================
// JsonUtil Float List Operations
// =============================================================================

int32_t Impl_Json_FloatListAdd(const char* filename, const char* key, float value, bool allowDuplicate) {
    if (!filename || !filename[0] || !key || !key[0]) return -1;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return -1;
    return file->ListAdd(External::List<float>(), key, External::MakeValue<float>(value), allowDuplicate);
}

float Impl_Json_FloatListGet(const char* filename, const char* key, uint32_t index) {
    if (!filename || !filename[0] || !key || !key[0]) return 0.0f;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0.0f;
    return External::ParseValue<float>(file->ListGet(External::List<float>(), key, index), 0.0f);
}

float Impl_Json_FloatListSet(const char* filename, const char* key, uint32_t index, float value) {
    if (!filename || !filename[0] || !key || !key[0]) return 0.0f;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0.0f;
    return External::ParseValue<float>(
        file->ListSet(External::List<float>(), key, index, External::MakeValue<float>(value)), 0.0f);
}

uint32_t Impl_Json_FloatListRemove(const char* filename, const char* key, float value, bool allInstances) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListRemove(External::List<float>(), key, External::MakeValue<float>(value), allInstances);
}

bool Impl_Json_FloatListRemoveAt(const char* filename, const char* key, uint32_t index) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->ListRemoveAt(External::List<float>(), key, index);
}

bool Impl_Json_FloatListInsertAt(const char* filename, const char* key, uint32_t index, float value) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->ListInsertAt(External::List<float>(), key, index, External::MakeValue<float>(value));
}

uint32_t Impl_Json_FloatListClear(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListClear(External::List<float>(), key);
}

uint32_t Impl_Json_FloatListCount(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListCount(External::List<float>(), key);
}

int32_t Impl_Json_FloatListFind(const char* filename, const char* key, float value) {
    if (!filename || !filename[0] || !key || !key[0]) return -1;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return -1;
    return file->ListFind(External::List<float>(), key, External::MakeValue<float>(value));
}

bool Impl_Json_FloatListHas(const char* filename, const char* key, float value) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->ListFind(External::List<float>(), key, External::MakeValue<float>(value)) != -1;
}

// =============================================================================
// JsonUtil String List Operations
// =============================================================================

int32_t Impl_Json_StringListAdd(const char* filename, const char* key, const char* value, bool allowDuplicate) {
    if (!filename || !filename[0] || !key || !key[0]) return -1;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return -1;
    BSFixedString bsValue(value ? value : "");
    return file->ListAdd(External::List<BSFixedString>(), key, External::MakeValue<BSFixedString>(bsValue), allowDuplicate);
}

const char* Impl_Json_StringListGet(const char* filename, const char* key, uint32_t index) {
    if (!filename || !filename[0] || !key || !key[0]) return DuplicateString("");
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return DuplicateString("");
    BSFixedString result = External::ParseValue<BSFixedString>(
        file->ListGet(External::List<BSFixedString>(), key, index), BSFixedString(""));
    return DuplicateString(result);
}

const char* Impl_Json_StringListSet(const char* filename, const char* key, uint32_t index, const char* value) {
    if (!filename || !filename[0] || !key || !key[0]) return DuplicateString("");
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return DuplicateString("");
    BSFixedString bsValue(value ? value : "");
    BSFixedString result = External::ParseValue<BSFixedString>(
        file->ListSet(External::List<BSFixedString>(), key, index, External::MakeValue<BSFixedString>(bsValue)),
        BSFixedString(""));
    return DuplicateString(result);
}

uint32_t Impl_Json_StringListRemove(const char* filename, const char* key, const char* value, bool allInstances) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    BSFixedString bsValue(value ? value : "");
    return file->ListRemove(External::List<BSFixedString>(), key, External::MakeValue<BSFixedString>(bsValue), allInstances);
}

bool Impl_Json_StringListRemoveAt(const char* filename, const char* key, uint32_t index) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->ListRemoveAt(External::List<BSFixedString>(), key, index);
}

bool Impl_Json_StringListInsertAt(const char* filename, const char* key, uint32_t index, const char* value) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    BSFixedString bsValue(value ? value : "");
    return file->ListInsertAt(External::List<BSFixedString>(), key, index, External::MakeValue<BSFixedString>(bsValue));
}

uint32_t Impl_Json_StringListClear(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListClear(External::List<BSFixedString>(), key);
}

uint32_t Impl_Json_StringListCount(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListCount(External::List<BSFixedString>(), key);
}

int32_t Impl_Json_StringListFind(const char* filename, const char* key, const char* value) {
    if (!filename || !filename[0] || !key || !key[0]) return -1;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return -1;
    BSFixedString bsValue(value ? value : "");
    return file->ListFind(External::List<BSFixedString>(), key, External::MakeValue<BSFixedString>(bsValue));
}

bool Impl_Json_StringListHas(const char* filename, const char* key, const char* value) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    BSFixedString bsValue(value ? value : "");
    return file->ListFind(External::List<BSFixedString>(), key, External::MakeValue<BSFixedString>(bsValue)) != -1;
}

// =============================================================================
// JsonUtil Form List Operations
// =============================================================================

int32_t Impl_Json_FormListAdd(const char* filename, const char* key, uint32_t formID, bool allowDuplicate) {
    if (!filename || !filename[0] || !key || !key[0]) return -1;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return -1;
    TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return file->ListAdd(External::List<TESForm*>(), key, External::MakeValue<TESForm*>(form), allowDuplicate);
}

uint32_t Impl_Json_FormListGet(const char* filename, const char* key, uint32_t index) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    TESForm* result = External::ParseValue<TESForm*>(
        file->ListGet(External::List<TESForm*>(), key, index), nullptr);
    return result ? result->formID : 0;
}

uint32_t Impl_Json_FormListSet(const char* filename, const char* key, uint32_t index, uint32_t formID) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    TESForm* result = External::ParseValue<TESForm*>(
        file->ListSet(External::List<TESForm*>(), key, index, External::MakeValue<TESForm*>(form)),
        nullptr);
    return result ? result->formID : 0;
}

uint32_t Impl_Json_FormListRemove(const char* filename, const char* key, uint32_t formID, bool allInstances) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return file->ListRemove(External::List<TESForm*>(), key, External::MakeValue<TESForm*>(form), allInstances);
}

bool Impl_Json_FormListRemoveAt(const char* filename, const char* key, uint32_t index) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->ListRemoveAt(External::List<TESForm*>(), key, index);
}

bool Impl_Json_FormListInsertAt(const char* filename, const char* key, uint32_t index, uint32_t formID) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return file->ListInsertAt(External::List<TESForm*>(), key, index, External::MakeValue<TESForm*>(form));
}

uint32_t Impl_Json_FormListClear(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListClear(External::List<TESForm*>(), key);
}

uint32_t Impl_Json_FormListCount(const char* filename, const char* key) {
    if (!filename || !filename[0] || !key || !key[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->ListCount(External::List<TESForm*>(), key);
}

int32_t Impl_Json_FormListFind(const char* filename, const char* key, uint32_t formID) {
    if (!filename || !filename[0] || !key || !key[0]) return -1;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return -1;
    TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return file->ListFind(External::List<TESForm*>(), key, External::MakeValue<TESForm*>(form));
}

bool Impl_Json_FormListHas(const char* filename, const char* key, uint32_t formID) {
    if (!filename || !filename[0] || !key || !key[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    return file->ListFind(External::List<TESForm*>(), key, External::MakeValue<TESForm*>(form)) != -1;
}

// =============================================================================
// JsonUtil Path Operations
// =============================================================================

void Impl_Json_SetPathIntValue(const char* filename, const char* path, int32_t value) {
    if (!filename || !filename[0] || !path || !path[0]) return;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return;
    file->SetPathValue<SInt32>(path, External::MakeValue<SInt32>(value));
}

void Impl_Json_SetPathFloatValue(const char* filename, const char* path, float value) {
    if (!filename || !filename[0] || !path || !path[0]) return;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return;
    file->SetPathValue<float>(path, External::MakeValue<float>(value));
}

void Impl_Json_SetPathStringValue(const char* filename, const char* path, const char* value) {
    if (!filename || !filename[0] || !path || !path[0]) return;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return;
    BSFixedString bsValue(value ? value : "");
    file->SetPathValue<BSFixedString>(path, External::MakeValue<BSFixedString>(bsValue));
}

void Impl_Json_SetPathFormValue(const char* filename, const char* path, uint32_t formID) {
    if (!filename || !filename[0] || !path || !path[0]) return;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return;
    TESForm* form = formID ? RE::TESForm::LookupByID(formID) : nullptr;
    file->SetPathValue<TESForm*>(path, External::MakeValue<TESForm*>(form));
}

int32_t Impl_Json_GetPathIntValue(const char* filename, const char* path, int32_t defaultVal) {
    if (!filename || !filename[0] || !path || !path[0]) return defaultVal;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return defaultVal;
    return file->GetPathValue<SInt32>(path, defaultVal);
}

float Impl_Json_GetPathFloatValue(const char* filename, const char* path, float defaultVal) {
    if (!filename || !filename[0] || !path || !path[0]) return defaultVal;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return defaultVal;
    return file->GetPathValue<float>(path, defaultVal);
}

const char* Impl_Json_GetPathStringValue(const char* filename, const char* path, const char* defaultVal) {
    if (!filename || !filename[0] || !path || !path[0]) return DuplicateString(defaultVal ? defaultVal : "");
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return DuplicateString(defaultVal ? defaultVal : "");
    BSFixedString bsDefault(defaultVal ? defaultVal : "");
    BSFixedString result = file->GetPathValue<BSFixedString>(path, bsDefault);
    return DuplicateString(result);
}

uint32_t Impl_Json_GetPathFormValue(const char* filename, const char* path, uint32_t defaultFormID) {
    if (!filename || !filename[0] || !path || !path[0]) return defaultFormID;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return defaultFormID;
    TESForm* defaultForm = defaultFormID ? RE::TESForm::LookupByID(defaultFormID) : nullptr;
    TESForm* result = file->GetPathValue<TESForm*>(path, defaultForm);
    return result ? result->formID : 0;
}

bool Impl_Json_CanResolvePath(const char* filename, const char* path) {
    if (!filename || !filename[0] || !path || !path[0]) return false;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return false;
    return file->CanResolve(path);
}

void Impl_Json_ClearPath(const char* filename, const char* path) {
    if (!filename || !filename[0] || !path || !path[0]) return;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return;
    file->ClearPath(path);
}

int32_t Impl_Json_PathCount(const char* filename, const char* path) {
    if (!filename || !filename[0] || !path || !path[0]) return 0;
    External::ExternalFile* file = External::GetFile(filename);
    if (!file) return 0;
    return file->PathCount(path);
}

// =============================================================================
// ActorUtil Package Operations
// =============================================================================

void Impl_Actor_AddPackageOverride(uint32_t actorFormID, uint32_t packageFormID, uint32_t priority, uint32_t flags) {
    RE::Actor* actor = RE::TESForm::LookupByID<RE::Actor>(actorFormID);
    RE::TESPackage* package = RE::TESForm::LookupByID<RE::TESPackage>(packageFormID);
    PackageData::Packages* pkgs = PackageData::GetPackages();
    if (actor && package && pkgs) {
        pkgs->AddPackage(actor, package, priority, flags);
    }
}

bool Impl_Actor_RemovePackageOverride(uint32_t actorFormID, uint32_t packageFormID) {
    RE::Actor* actor = RE::TESForm::LookupByID<RE::Actor>(actorFormID);
    RE::TESPackage* package = RE::TESForm::LookupByID<RE::TESPackage>(packageFormID);
    PackageData::Packages* pkgs = PackageData::GetPackages();
    if (!actor || !package || !pkgs) return false;
    return pkgs->RemovePackage(actor, package);
}

uint32_t Impl_Actor_CountPackageOverride(uint32_t actorFormID) {
    RE::Actor* actor = RE::TESForm::LookupByID<RE::Actor>(actorFormID);
    PackageData::Packages* pkgs = PackageData::GetPackages();
    if (!actor || !pkgs) return 0;
    return pkgs->CountPackages(actor);
}

uint32_t Impl_Actor_ClearPackageOverride(uint32_t actorFormID) {
    RE::Actor* actor = RE::TESForm::LookupByID<RE::Actor>(actorFormID);
    PackageData::Packages* pkgs = PackageData::GetPackages();
    if (!actor || !pkgs) return 0;
    return pkgs->ClearActor(actor);
}

uint32_t Impl_Actor_RemoveAllPackageOverride(uint32_t packageFormID) {
    RE::TESPackage* package = RE::TESForm::LookupByID<RE::TESPackage>(packageFormID);
    PackageData::Packages* pkgs = PackageData::GetPackages();
    if (!package || !pkgs) return 0;
    return pkgs->ClearPackage(package);
}

} // anonymous namespace

// =============================================================================
// Static Interface Instance
// =============================================================================

static PapyrusUtilAPI::IPapyrusUtilInterface g_interface = {
    // =========================================================================
    // Interface Metadata
    // =========================================================================
    static_cast<uint32_t>(PapyrusUtilAPI::InterfaceVersion::kV1),
    PAPYRUSUTIL_VERSION,

    // =========================================================================
    // Storage Sub-interface
    // =========================================================================
    {
        // Int values
        Impl_Storage_SetIntValue,
        Impl_Storage_GetIntValue,
        Impl_Storage_AdjustIntValue,
        Impl_Storage_HasIntValue,
        Impl_Storage_UnsetIntValue,
        
        // Float values
        Impl_Storage_SetFloatValue,
        Impl_Storage_GetFloatValue,
        Impl_Storage_AdjustFloatValue,
        Impl_Storage_HasFloatValue,
        Impl_Storage_UnsetFloatValue,
        
        // String values
        Impl_Storage_SetStringValue,
        Impl_Storage_GetStringValue,
        Impl_Storage_HasStringValue,
        Impl_Storage_UnsetStringValue,
        
        // Form values
        Impl_Storage_SetFormValue,
        Impl_Storage_GetFormValue,
        Impl_Storage_HasFormValue,
        Impl_Storage_UnsetFormValue,
        
        // Int lists
        Impl_Storage_IntListAdd,
        Impl_Storage_IntListGet,
        Impl_Storage_IntListSet,
        Impl_Storage_IntListRemove,
        Impl_Storage_IntListRemoveAt,
        Impl_Storage_IntListInsertAt,
        Impl_Storage_IntListClear,
        Impl_Storage_IntListCount,
        Impl_Storage_IntListFind,
        Impl_Storage_IntListHas,
        Impl_Storage_IntListSort,
        
        // Float lists
        Impl_Storage_FloatListAdd,
        Impl_Storage_FloatListGet,
        Impl_Storage_FloatListSet,
        Impl_Storage_FloatListRemove,
        Impl_Storage_FloatListRemoveAt,
        Impl_Storage_FloatListInsertAt,
        Impl_Storage_FloatListClear,
        Impl_Storage_FloatListCount,
        Impl_Storage_FloatListFind,
        Impl_Storage_FloatListHas,
        Impl_Storage_FloatListSort,
        
        // String lists
        Impl_Storage_StringListAdd,
        Impl_Storage_StringListGet,
        Impl_Storage_StringListSet,
        Impl_Storage_StringListRemove,
        Impl_Storage_StringListRemoveAt,
        Impl_Storage_StringListInsertAt,
        Impl_Storage_StringListClear,
        Impl_Storage_StringListCount,
        Impl_Storage_StringListFind,
        Impl_Storage_StringListHas,
        Impl_Storage_StringListSort,
        
        // Form lists
        Impl_Storage_FormListAdd,
        Impl_Storage_FormListGet,
        Impl_Storage_FormListSet,
        Impl_Storage_FormListRemove,
        Impl_Storage_FormListRemoveAt,
        Impl_Storage_FormListInsertAt,
        Impl_Storage_FormListClear,
        Impl_Storage_FormListCount,
        Impl_Storage_FormListFind,
        Impl_Storage_FormListHas,
        
        // Utility
        Impl_Storage_DeleteValues,
        Impl_Storage_DeleteAllValues
    },

    // =========================================================================
    // Json Sub-interface
    // =========================================================================
    {
        // File operations
        Impl_Json_Load,
        Impl_Json_Save,
        Impl_Json_Unload,
        Impl_Json_IsPendingSave,
        Impl_Json_IsGood,
        Impl_Json_GetErrors,
        Impl_Json_ClearAll,
        
        // Int values
        Impl_Json_SetIntValue,
        Impl_Json_GetIntValue,
        Impl_Json_AdjustIntValue,
        Impl_Json_HasIntValue,
        Impl_Json_UnsetIntValue,
        
        // Float values
        Impl_Json_SetFloatValue,
        Impl_Json_GetFloatValue,
        Impl_Json_AdjustFloatValue,
        Impl_Json_HasFloatValue,
        Impl_Json_UnsetFloatValue,
        
        // String values
        Impl_Json_SetStringValue,
        Impl_Json_GetStringValue,
        Impl_Json_HasStringValue,
        Impl_Json_UnsetStringValue,
        
        // Form values
        Impl_Json_SetFormValue,
        Impl_Json_GetFormValue,
        Impl_Json_HasFormValue,
        Impl_Json_UnsetFormValue,
        
        // Int lists
        Impl_Json_IntListAdd,
        Impl_Json_IntListGet,
        Impl_Json_IntListSet,
        Impl_Json_IntListRemove,
        Impl_Json_IntListRemoveAt,
        Impl_Json_IntListInsertAt,
        Impl_Json_IntListClear,
        Impl_Json_IntListCount,
        Impl_Json_IntListFind,
        Impl_Json_IntListHas,
        
        // Float lists
        Impl_Json_FloatListAdd,
        Impl_Json_FloatListGet,
        Impl_Json_FloatListSet,
        Impl_Json_FloatListRemove,
        Impl_Json_FloatListRemoveAt,
        Impl_Json_FloatListInsertAt,
        Impl_Json_FloatListClear,
        Impl_Json_FloatListCount,
        Impl_Json_FloatListFind,
        Impl_Json_FloatListHas,
        
        // String lists
        Impl_Json_StringListAdd,
        Impl_Json_StringListGet,
        Impl_Json_StringListSet,
        Impl_Json_StringListRemove,
        Impl_Json_StringListRemoveAt,
        Impl_Json_StringListInsertAt,
        Impl_Json_StringListClear,
        Impl_Json_StringListCount,
        Impl_Json_StringListFind,
        Impl_Json_StringListHas,
        
        // Form lists
        Impl_Json_FormListAdd,
        Impl_Json_FormListGet,
        Impl_Json_FormListSet,
        Impl_Json_FormListRemove,
        Impl_Json_FormListRemoveAt,
        Impl_Json_FormListInsertAt,
        Impl_Json_FormListClear,
        Impl_Json_FormListCount,
        Impl_Json_FormListFind,
        Impl_Json_FormListHas,
        
        // Path operations
        Impl_Json_SetPathIntValue,
        Impl_Json_SetPathFloatValue,
        Impl_Json_SetPathStringValue,
        Impl_Json_SetPathFormValue,
        Impl_Json_GetPathIntValue,
        Impl_Json_GetPathFloatValue,
        Impl_Json_GetPathStringValue,
        Impl_Json_GetPathFormValue,
        Impl_Json_CanResolvePath,
        Impl_Json_ClearPath,
        Impl_Json_PathCount
    },

    // =========================================================================
    // Actor Sub-interface
    // =========================================================================
    {
        Impl_Actor_AddPackageOverride,
        Impl_Actor_RemovePackageOverride,
        Impl_Actor_CountPackageOverride,
        Impl_Actor_ClearPackageOverride,
        Impl_Actor_RemoveAllPackageOverride
    },

    // =========================================================================
    // Memory Management
    // =========================================================================
    Impl_FreeString
};

// =============================================================================
// Exported API Request Function
// =============================================================================

extern "C" {

DLLEXPORT PapyrusUtilAPI::IPapyrusUtilInterface* PapyrusUtilAPI_RequestInterface(PapyrusUtilAPI::InterfaceVersion version) {
    if (version == PapyrusUtilAPI::InterfaceVersion::kV1) {
        logger::info("PapyrusUtilAPI: Interface V1 requested");
        return &g_interface;
    }
    
    logger::warn("PapyrusUtilAPI: Unsupported interface version {} requested", static_cast<uint32_t>(version));
    return nullptr;
}

} // extern "C"