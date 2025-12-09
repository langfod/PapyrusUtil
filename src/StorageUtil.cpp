#include "StorageUtil.h"

#include "Data.h"
#include "Forms.h"

// CommonLibSSE-NG types provided via PCH.h

#include <boost/algorithm/string.hpp>

namespace StorageUtil {
using namespace Forms;

// bool IsValidKey(BSFixedString &key) { return !(key == NULL || !key.data() || strlen(key.data()) == 0); }
inline bool IsValidKey(BSFixedString &key) { return key.data() && strlen(key.data()) > 0; }

inline bool IsEmpty(SInt32 &value) { return value == 0; }
inline bool IsEmpty(float &value) { return value == 0.0f; }
inline bool IsEmpty(BSFixedString &value) { return !value.data() || value.data()[0] == '\0'; }
inline bool IsEmpty(TESForm *value) { return value == NULL; }

template <typename T> inline T Empty() { return T(); }
template <> inline SInt32 Empty<SInt32>() { return 0; }
template <> inline float Empty<float>() { return 0.0f; }
template <> inline BSFixedString Empty<BSFixedString>() { return BSFixedString(""); }
template <> inline TESForm *Empty<TESForm *>() { return NULL; }

// Flat key=>value storage
template <typename T, typename S> T SetValue(StaticFunctionTag *base, TESForm *obj, BSFixedString key, T value) {
  Data::Values<T, S> *Data = Data::GetValues<T, S>();
  if (!Data || !IsValidKey(key))
    return Empty<T>();
  else
    Data->SetValue(GetFormKey(obj), key.data(), value);
  return value;
}

template <typename T, typename S> T GetValue(StaticFunctionTag *base, TESForm *obj, BSFixedString key, T missing) {
  Data::Values<T, S> *Data = Data::GetValues<T, S>();
  if (!Data || !IsValidKey(key))
    return missing;
  else
    return Data->GetValue(GetFormKey(obj), key.data(), missing);
}

template <typename T, typename S> T AdjustValue(StaticFunctionTag *base, TESForm *obj, BSFixedString key, T value) {
  Data::Values<T, S> *Data = Data::GetValues<T, S>();
  if (!Data || !IsValidKey(key))
    return Empty<T>();
  else
    return Data->AdjustValue(GetFormKey(obj), key.data(), value);
}

template <typename T, typename S> T PluckValue(StaticFunctionTag *base, TESForm *obj, BSFixedString key, T missing) {
  Data::Values<T, S> *Data = Data::GetValues<T, S>();
  if (!Data || !IsValidKey(key))
    return missing;
  else
    return Data->PluckValue(GetFormKey(obj), key.data(), missing);
}

template <typename T, typename S> bool UnsetValue(StaticFunctionTag *base, TESForm *obj, BSFixedString key) {
  Data::Values<T, S> *Data = Data::GetValues<T, S>();
  if (!Data || !IsValidKey(key))
    return false;
  else
    return Data->UnsetValue(GetFormKey(obj), key.data());
}

template <typename T, typename S> bool HasValue(StaticFunctionTag *base, TESForm *obj, BSFixedString key) {
  Data::Values<T, S> *Data = Data::GetValues<T, S>();
  if (!Data || !IsValidKey(key))
    return false;
  else
    return Data->HasValue(GetFormKey(obj), key.data());
}

// Lists key=>vector=>value storage
template <typename T, typename S> SInt32 ListAdd(StaticFunctionTag *base, TESForm *obj, BSFixedString key, T value, bool allowDuplicate) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return -1;
  else
    return Data->ListAdd(GetFormKey(obj), key.data(), value, allowDuplicate);
}

template <typename T, typename S> T ListGet(StaticFunctionTag *base, TESForm *obj, BSFixedString key, UInt32 index) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return Empty<T>();
  else
    return Data->ListGet(GetFormKey(obj), key.data(), index);
}

template <typename T, typename S> T ListSet(StaticFunctionTag *base, TESForm *obj, BSFixedString key, UInt32 index, T value) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return Empty<T>();
  else
    return Data->ListSet(GetFormKey(obj), key.data(), index, value);
}

template <typename T, typename S> T ListAdjust(StaticFunctionTag *base, TESForm *obj, BSFixedString key, UInt32 index, T value) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return Empty<T>();
  else
    return Data->ListAdjust(GetFormKey(obj), key.data(), index, value);
}

template <typename T, typename S> UInt32 ListRemove(StaticFunctionTag *base, TESForm *obj, BSFixedString key, T value, bool allInstances) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return 0;
  else
    return Data->ListRemove(GetFormKey(obj), key.data(), value, allInstances);
}

template <typename T, typename S> bool ListInsertAt(StaticFunctionTag *base, TESForm *obj, BSFixedString key, UInt32 index, T value) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return false;
  else
    return Data->ListInsertAt(GetFormKey(obj), key.data(), index, value);
}

template <typename T, typename S> T ListPluck(StaticFunctionTag *base, TESForm *obj, BSFixedString key, UInt32 index, T value) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return Empty<T>();
  else
    return Data->ListPluck(GetFormKey(obj), key.data(), index, value);
}

template <typename T, typename S> T ListShift(StaticFunctionTag *base, TESForm *obj, BSFixedString key) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return Empty<T>();
  else
    return Data->ListShift(GetFormKey(obj), key.data());
}

template <typename T, typename S> T ListPop(StaticFunctionTag *base, TESForm *obj, BSFixedString key) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return Empty<T>();
  else
    return Data->ListPop(GetFormKey(obj), key.data());
}

template <typename T, typename S> bool ListRemoveAt(StaticFunctionTag *base, TESForm *obj, BSFixedString key, UInt32 index) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return false;
  else
    return Data->ListRemoveAt(GetFormKey(obj), key.data(), index);
}

template <typename T, typename S> UInt32 ListClear(StaticFunctionTag *base, TESForm *obj, BSFixedString key) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return 0;
  else
    return Data->ListClear(GetFormKey(obj), key.data());
}

template <typename T, typename S> UInt32 ListCount(StaticFunctionTag *base, TESForm *obj, BSFixedString key) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return 0;
  else
    return Data->ListCount(GetFormKey(obj), key.data());
}

template <typename T, typename S> UInt32 ListCountValue(StaticFunctionTag *base, TESForm *obj, BSFixedString key, T value, bool exclude) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return 0;
  else
    return Data->ListCountValue(GetFormKey(obj), key.data(), value, exclude);
}

template <typename T, typename S> SInt32 ListFind(StaticFunctionTag *base, TESForm *obj, BSFixedString key, T value) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return -1;
  else
    return Data->ListFind(GetFormKey(obj), key.data(), value);
}

template <typename T, typename S> bool ListHas(StaticFunctionTag *base, TESForm *obj, BSFixedString key, T value) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return false;
  else
    return Data->ListHas(GetFormKey(obj), key.data(), value);
}

template <typename T, typename S> void ListSort(StaticFunctionTag *base, TESForm *obj, BSFixedString key) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (Data && IsValidKey(key))
    Data->ListSort(GetFormKey(obj), key.data());
}

template <typename T, typename S>
void ListSlice(StaticFunctionTag *base, TESForm *obj, BSFixedString key, RE::reference_array<T> Output, UInt32 startIndex) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (Data && Output.size() > 0 && IsValidKey(key)) {
    std::vector<T> temp(Output.begin(), Output.end());
    Data->ListSlice(GetFormKey(obj), key.data(), temp, startIndex);
    // Copy results back to Output
    for (std::size_t i = 0; i < temp.size() && i < Output.size(); ++i) {
      Output[i] = temp[i];
    }
  }
}

template <typename T, typename S> SInt32 ListResize(StaticFunctionTag *base, TESForm *obj, BSFixedString key, UInt32 length, T filler) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key) || length > 500)
    return 0;
  else
    return Data->ListResize(GetFormKey(obj), key.data(), length, filler);
}

template <typename T, typename S> bool ListCopy(StaticFunctionTag *base, TESForm *obj, BSFixedString key, RE::reference_array<T> Input) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key) || Input.size() < 1)
    return false;
  else {
    std::vector<T> temp(Input.begin(), Input.end());
    return Data->ListCopy(GetFormKey(obj), key.data(), temp);
  }
}

template <typename T, typename S> VMResultArray<T> ToArray(StaticFunctionTag *base, TESForm *obj, BSFixedString key) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  VMResultArray<T> arr;
  if (Data && IsValidKey(key))
    arr = Data->ToArray(GetFormKey(obj), key.data());
  return arr;
}

// Debug functions
UInt32 Cleanup(StaticFunctionTag *base) {
  int removed = 0;
  // Values
  if (auto *data = Data::GetValues<SInt32, SInt32>())
    removed += data->Cleanup();
  if (auto *data = Data::GetValues<float, float>())
    removed += data->Cleanup();
  if (auto *data = Data::GetValues<BSFixedString, std::string>())
    removed += data->Cleanup();
  if (auto *data = Data::GetValues<TESForm *, UInt32>())
    removed += data->Cleanup();
  // Lists
  if (auto *data = Data::GetLists<SInt32, SInt32>())
    removed += data->Cleanup();
  if (auto *data = Data::GetLists<float, float>())
    removed += data->Cleanup();
  if (auto *data = Data::GetLists<BSFixedString, std::string>())
    removed += data->Cleanup();
  if (auto *data = Data::GetLists<TESForm *, UInt32>())
    removed += data->Cleanup();
  return removed;
}

void DeleteValues(StaticFunctionTag *base, TESForm *obj) {
  UInt64 key = GetFormKey(obj);
  // Values
  if (auto *data = Data::GetValues<SInt32, SInt32>())
    data->RemoveForm(key);
  if (auto *data = Data::GetValues<float, float>())
    data->RemoveForm(key);
  if (auto *data = Data::GetValues<BSFixedString, std::string>())
    data->RemoveForm(key);
  if (auto *data = Data::GetValues<TESForm *, UInt32>())
    data->RemoveForm(key);
  // Lists
  if (auto *data = Data::GetLists<SInt32, SInt32>())
    data->RemoveForm(key);
  if (auto *data = Data::GetLists<float, float>())
    data->RemoveForm(key);
  if (auto *data = Data::GetLists<BSFixedString, std::string>())
    data->RemoveForm(key);
  if (auto *data = Data::GetLists<TESForm *, UInt32>())
    data->RemoveForm(key);
}

void DeleteAllValues(StaticFunctionTag *base) {
  // Values
  if (auto *data = Data::GetValues<SInt32, SInt32>())
    data->Revert();
  if (auto *data = Data::GetValues<float, float>())
    data->Revert();
  if (auto *data = Data::GetValues<BSFixedString, std::string>())
    data->Revert();
  if (auto *data = Data::GetValues<TESForm *, UInt32>())
    data->Revert();
  // Lists
  if (auto *data = Data::GetLists<SInt32, SInt32>())
    data->Revert();
  if (auto *data = Data::GetLists<float, float>())
    data->Revert();
  if (auto *data = Data::GetLists<BSFixedString, std::string>())
    data->Revert();
  if (auto *data = Data::GetLists<TESForm *, UInt32>())
    data->Revert();
}

template <typename T, typename S> UInt32 GetObjectCount(StaticFunctionTag *base) {
  return static_cast<UInt32>(Data::GetValues<T, S>()->GetObjCount());
}

template <typename T, typename S> UInt32 GetListsObjectCount(StaticFunctionTag *base) {
  return static_cast<UInt32>(Data::GetLists<T, S>()->GetObjCount());
}

template <typename T, typename S> UInt32 GetKeyCount(StaticFunctionTag *base, TESForm *obj) {
  return static_cast<UInt32>(Data::GetValues<T, S>()->GetKeyCount(GetFormKey(obj)));
}

template <typename T, typename S> UInt32 GetListsKeyCount(StaticFunctionTag *base, TESForm *obj) {
  return static_cast<UInt32>(Data::GetLists<T, S>()->GetKeyCount(GetFormKey(obj)));
}

template <typename T, typename S> TESForm *GetNthObj(StaticFunctionTag *base, UInt32 i) { return Data::GetValues<T, S>()->GetNthObj(i); }

template <typename T, typename S> TESForm *GetListsNthObj(StaticFunctionTag *base, UInt32 i) {
  return Data::GetLists<T, S>()->GetNthObj(i);
}

template <typename T, typename S> BSFixedString GetNthKey(StaticFunctionTag *base, TESForm *obj, UInt32 i) {
  const std::string key = Data::GetValues<T, S>()->GetNthKey(GetFormKey(obj), i);
  return BSFixedString(key.c_str());
}

template <typename T, typename S> BSFixedString GetListsNthKey(StaticFunctionTag *base, TESForm *obj, UInt32 i) {
  const std::string key = Data::GetLists<T, S>()->GetNthKey(GetFormKey(obj), i);
  return BSFixedString(key.c_str());
}

template <typename T, typename S> VMResultArray<TESForm *> GetListsAllObj(StaticFunctionTag *base) {
  return Data::GetLists<T, S>()->GetAllObj();
}

template <typename T, typename S> VMResultArray<TESForm *> GetValuesAllObj(StaticFunctionTag *base) {
  return Data::GetValues<T, S>()->GetAllObj();
}

template <typename T, typename S> VMResultArray<BSFixedString> GetListsObjKeys(StaticFunctionTag *base, TESForm *obj) {
  return Data::GetLists<T, S>()->GetAllObjKeys(GetFormKey(obj));
}

template <typename T, typename S> VMResultArray<BSFixedString> GetValuesObjKeys(StaticFunctionTag *base, TESForm *obj) {
  return Data::GetValues<T, S>()->GetAllObjKeys(GetFormKey(obj));
}

template <typename T, typename S> UInt32 CountValuesPrefixKey(StaticFunctionTag *base, BSFixedString prefix) {
  return Data::GetValues<T, S>()->CountPrefixKey(prefix.data());
}

template <typename T, typename S> UInt32 CountListsPrefixKey(StaticFunctionTag *base, BSFixedString prefix) {
  return Data::GetLists<T, S>()->CountPrefixKey(prefix.data());
}

UInt32 CountAllPrefix(StaticFunctionTag *base, BSFixedString prefix) {
  UInt32 count = 0;
  std::string key = prefix.data();
  if (!key.empty()) {
    count += Data::GetValues<SInt32, SInt32>()->CountPrefixKey(key);
    count += Data::GetValues<float, float>()->CountPrefixKey(key);
    count += Data::GetValues<BSFixedString, std::string>()->CountPrefixKey(key);
    count += Data::GetValues<TESForm *, UInt32>()->CountPrefixKey(key);
    // Lists
    count += Data::GetLists<SInt32, SInt32>()->CountPrefixKey(key);
    count += Data::GetLists<float, float>()->CountPrefixKey(key);
    count += Data::GetLists<BSFixedString, std::string>()->CountPrefixKey(key);
    count += Data::GetLists<TESForm *, UInt32>()->CountPrefixKey(key);
  }
  return count;
}

template <typename T, typename S> UInt32 ObjCountValuesPrefixKey(StaticFunctionTag *base, TESForm *obj, BSFixedString prefix) {
  return Data::GetValues<T, S>()->CountPrefixKey(GetFormKey(obj), prefix.data());
}

template <typename T, typename S> UInt32 ObjCountListsPrefixKey(StaticFunctionTag *base, TESForm *obj, BSFixedString prefix) {
  return Data::GetLists<T, S>()->CountPrefixKey(GetFormKey(obj), prefix.data());
}

UInt32 ObjCountAllPrefix(StaticFunctionTag *base, TESForm *obj, BSFixedString prefix) {
  UInt32 count = 0;
  UInt64 objkey = GetFormKey(obj);
  std::string key = prefix.data();
  if (!key.empty()) {
    count += Data::GetValues<SInt32, SInt32>()->CountPrefixKey(objkey, key);
    count += Data::GetValues<float, float>()->CountPrefixKey(objkey, key);
    count += Data::GetValues<BSFixedString, std::string>()->CountPrefixKey(objkey, key);
    count += Data::GetValues<TESForm *, UInt32>()->CountPrefixKey(objkey, key);
    // Lists
    count += Data::GetLists<SInt32, SInt32>()->CountPrefixKey(objkey, key);
    count += Data::GetLists<float, float>()->CountPrefixKey(objkey, key);
    count += Data::GetLists<BSFixedString, std::string>()->CountPrefixKey(objkey, key);
    count += Data::GetLists<TESForm *, UInt32>()->CountPrefixKey(objkey, key);
  }
  return count;
}

template <typename T, typename S> UInt32 ClearValuesPrefixKey(StaticFunctionTag *base, BSFixedString prefix) {
  return Data::GetValues<T, S>()->ClearPrefixKey(prefix.data());
}

template <typename T, typename S> UInt32 ClearListsPrefixKey(StaticFunctionTag *base, BSFixedString prefix) {
  return Data::GetLists<T, S>()->ClearPrefixKey(prefix.data());
}

UInt32 ClearAllPrefix(StaticFunctionTag *base, BSFixedString prefix) {
  UInt32 count = 0;
  std::string key = prefix.data();
  if (!key.empty()) {
    count += Data::GetValues<SInt32, SInt32>()->ClearPrefixKey(key);
    count += Data::GetValues<float, float>()->ClearPrefixKey(key);
    count += Data::GetValues<BSFixedString, std::string>()->ClearPrefixKey(key);
    count += Data::GetValues<TESForm *, UInt32>()->ClearPrefixKey(key);
    // Lists
    count += Data::GetLists<SInt32, SInt32>()->ClearPrefixKey(key);
    count += Data::GetLists<float, float>()->ClearPrefixKey(key);
    count += Data::GetLists<BSFixedString, std::string>()->ClearPrefixKey(key);
    count += Data::GetLists<TESForm *, UInt32>()->ClearPrefixKey(key);
  }
  return count;
}

template <typename T, typename S> UInt32 ObjClearValuesPrefixKey(StaticFunctionTag *base, TESForm *obj, BSFixedString prefix) {
  return Data::GetValues<T, S>()->ClearPrefixKey(GetFormKey(obj), prefix.data());
}

template <typename T, typename S> UInt32 ObjClearListsPrefixKey(StaticFunctionTag *base, TESForm *obj, BSFixedString prefix) {
  return Data::GetLists<T, S>()->ClearPrefixKey(GetFormKey(obj), prefix.data());
}

UInt32 ObjClearAllPrefix(StaticFunctionTag *base, TESForm *obj, BSFixedString prefix) {
  UInt32 count = 0;
  UInt64 objkey = GetFormKey(obj);
  std::string key = prefix.data();
  if (!key.empty()) {
    count += Data::GetValues<SInt32, SInt32>()->ClearPrefixKey(objkey, key);
    count += Data::GetValues<float, float>()->ClearPrefixKey(objkey, key);
    count += Data::GetValues<BSFixedString, std::string>()->ClearPrefixKey(objkey, key);
    count += Data::GetValues<TESForm *, UInt32>()->ClearPrefixKey(objkey, key);
    // Lists
    count += Data::GetLists<SInt32, SInt32>()->ClearPrefixKey(objkey, key);
    count += Data::GetLists<float, float>()->ClearPrefixKey(objkey, key);
    count += Data::GetLists<BSFixedString, std::string>()->ClearPrefixKey(objkey, key);
    count += Data::GetLists<TESForm *, UInt32>()->ClearPrefixKey(objkey, key);
  }
  return count;
}

VMResultArray<TESForm *> FilterByTypes(StaticFunctionTag *base, TESForm *obj, BSFixedString key, VMArray<UInt32> types, bool matching) {
  Data::Lists<TESForm *, UInt32> *Data = Data::GetLists<TESForm *, UInt32>();
  if (!Data || !IsValidKey(key)) {
    VMResultArray<TESForm *> arr;
    return arr;
  } else
    return Data->FilterByTypes(GetFormKey(obj), key.data(), types, matching);
}

template <typename T, typename S> T ListRandom(StaticFunctionTag *base, TESForm *obj, BSFixedString key) {
  Data::Lists<T, S> *Data = Data::GetLists<T, S>();
  if (!Data || !IsValidKey(key))
    return Empty<T>();
  else
    return Data->ListRandom(GetFormKey(obj), key.data());
}

} // namespace StorageUtil

#ifdef _GLOBAL_EXTERNAL
#include "External.h"
using namespace External;

namespace StorageUtil {
// Global external StorageUtil.json file
template <typename T> T FileSetValue(StaticFunctionTag *base, BSFixedString key, T value) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return Empty<T>();
  else
    return ParseValue<T>(File->SetValue(List<T>(), key.data(), MakeValue<T>(value)));
}

template <typename T> T FileGetValue(StaticFunctionTag *base, BSFixedString key, T missing) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return Empty<T>();
  else
    return ParseValue<T>(File->GetValue(List<T>(), key.data(), MakeValue<T>(missing)));
}

template <typename T> T FileAdjustValue(StaticFunctionTag *base, BSFixedString key, T value) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return Empty<T>();
  else
    return ParseValue<T>(File->AdjustValue(List<T>(), key.data(), MakeValue<T>(value)));
}

template <typename T> bool FileUnsetValue(StaticFunctionTag *base, BSFixedString key) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->UnsetValue(List<T>(), key.data());
}

template <typename T> bool FileHasValue(StaticFunctionTag *base, BSFixedString key) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->HasValue(List<T>(), key.data());
}

template <typename T> SInt32 FileListAdd(StaticFunctionTag *base, BSFixedString key, T value, bool allowDuplicate) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return -1;
  else
    return File->ListAdd(List<T>(), key.data(), MakeValue<T>(value), allowDuplicate);
}

template <typename T> T FileListGet(StaticFunctionTag *base, BSFixedString key, UInt32 index) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return Empty<T>();
  else
    return ParseValue<T>(File->ListGet(List<T>(), key.data(), index));
}

template <typename T> T FileListSet(StaticFunctionTag *base, BSFixedString key, UInt32 index, T value) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return Empty<T>();
  else
    return ParseValue<T>(File->ListSet(List<T>(), key.data(), index, MakeValue<T>(value)));
}

template <typename T> T FileListAdjust(StaticFunctionTag *base, BSFixedString key, UInt32 index, T adjustBy) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return Empty<T>();
  else
    return ParseValue<T>(File->ListAdjust(key.data(), index, adjustBy));
}

template <typename T> UInt32 FileListRemove(StaticFunctionTag *base, BSFixedString key, T value, bool allInstances) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return 0;
  else
    return File->ListRemove(List<T>(), key.data(), MakeValue<T>(value), allInstances);
}

template <typename T> bool FileListRemoveAt(StaticFunctionTag *base, BSFixedString key, UInt32 index) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->ListRemoveAt(List<T>(), key.data(), index);
}

template <typename T> bool FileListInsertAt(StaticFunctionTag *base, BSFixedString key, UInt32 index, T value) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->ListInsertAt(List<T>(), key.data(), index, MakeValue<T>(value));
}

template <typename T> UInt32 FileListClear(StaticFunctionTag *base, BSFixedString key) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return 0;
  else
    return File->ListClear(List<T>(), key.data());
}

template <typename T> UInt32 FileListCount(StaticFunctionTag *base, BSFixedString key) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return 0;
  return File->ListCount(List<T>(), key.data());
}

template <typename T> SInt32 FileListFind(StaticFunctionTag *base, BSFixedString key, T value) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return -1;
  else
    return File->ListFind(List<T>(), key.data(), MakeValue<T>(value));
}

template <typename T> bool FileListHas(StaticFunctionTag *base, BSFixedString key, T value) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->ListFind(List<T>(), key.data(), MakeValue<T>(value)) != -1;
}

template <typename T> SInt32 FileListResize(StaticFunctionTag *base, BSFixedString key, UInt32 length, T filler) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key) || length > 500)
    return 0;
  else
    return File->ListResize(List<T>(), key.data(), length, MakeValue<T>(filler));
}

template <typename T> void FileListSlice(StaticFunctionTag *base, BSFixedString key, RE::reference_array<T> Output, UInt32 startIndex) {
  ExternalFile *File = GetSingleton();
  if (File && Output.size() > 0 && IsValidKey(key)) {
    std::vector<T> temp(Output.begin(), Output.end());
    File->ListSlice<T>(key.data(), temp, startIndex);
    // Copy results back to Output
    for (std::size_t i = 0; i < temp.size() && i < Output.size(); ++i) {
      Output[i] = temp[i];
    }
  }
}

template <typename T> bool FileListCopy(StaticFunctionTag *base, BSFixedString key, RE::reference_array<T> Input) {
  ExternalFile *File = GetSingleton();
  if (!File || !IsValidKey(key) || Input.size() < 1)
    return false;
  else {
    std::vector<T> temp(Input.begin(), Input.end());
    return File->ListCopy<T>(key.data(), temp);
  }
}

void SaveExternalFile(StaticFunctionTag *base) {
  External::ExternalFile *File = External::GetSingleton();
  if (File)
    File->SaveFile(false);
}
} // namespace StorageUtil
#endif

void StorageUtil::RegisterFuncs(RE::BSScript::IVirtualMachine *vm) {
  vm->RegisterFunction("SetIntValue"sv, "StorageUtil"sv, SetValue<SInt32, SInt32>);
  vm->RegisterFunction("SetFloatValue"sv, "StorageUtil"sv, SetValue<float, float>);
  vm->RegisterFunction("SetStringValue"sv, "StorageUtil"sv, SetValue<BSFixedString, std::string>);
  vm->RegisterFunction("SetFormValue"sv, "StorageUtil"sv, SetValue<TESForm *, UInt32>);

  vm->RegisterFunction("GetIntValue"sv, "StorageUtil"sv, GetValue<SInt32, SInt32>);
  vm->RegisterFunction("GetFloatValue"sv, "StorageUtil"sv, GetValue<float, float>);
  vm->RegisterFunction("GetStringValue"sv, "StorageUtil"sv, GetValue<BSFixedString, std::string>);
  vm->RegisterFunction("GetFormValue"sv, "StorageUtil"sv, GetValue<TESForm *, UInt32>);

  vm->RegisterFunction("AdjustIntValue"sv, "StorageUtil"sv, AdjustValue<SInt32, SInt32>);
  vm->RegisterFunction("AdjustFloatValue"sv, "StorageUtil"sv, AdjustValue<float, float>);

  vm->RegisterFunction("PluckIntValue"sv, "StorageUtil"sv, PluckValue<SInt32, SInt32>);
  vm->RegisterFunction("PluckFloatValue"sv, "StorageUtil"sv, PluckValue<float, float>);
  vm->RegisterFunction("PluckStringValue"sv, "StorageUtil"sv, PluckValue<BSFixedString, std::string>);
  vm->RegisterFunction("PluckFormValue"sv, "StorageUtil"sv, PluckValue<TESForm *, UInt32>);

  vm->RegisterFunction("UnsetIntValue"sv, "StorageUtil"sv, UnsetValue<SInt32, SInt32>);
  vm->RegisterFunction("UnsetFloatValue"sv, "StorageUtil"sv, UnsetValue<float, float>);
  vm->RegisterFunction("UnsetStringValue"sv, "StorageUtil"sv, UnsetValue<BSFixedString, std::string>);
  vm->RegisterFunction("UnsetFormValue"sv, "StorageUtil"sv, UnsetValue<TESForm *, UInt32>);

  vm->RegisterFunction("HasIntValue"sv, "StorageUtil"sv, HasValue<SInt32, SInt32>);
  vm->RegisterFunction("HasFloatValue"sv, "StorageUtil"sv, HasValue<float, float>);
  vm->RegisterFunction("HasStringValue"sv, "StorageUtil"sv, HasValue<BSFixedString, std::string>);
  vm->RegisterFunction("HasFormValue"sv, "StorageUtil"sv, HasValue<TESForm *, UInt32>);

  // Lists
  vm->RegisterFunction("IntListAdd"sv, "StorageUtil"sv, ListAdd<SInt32, SInt32>);
  vm->RegisterFunction("FloatListAdd"sv, "StorageUtil"sv, ListAdd<float, float>);
  vm->RegisterFunction("StringListAdd"sv, "StorageUtil"sv, ListAdd<BSFixedString, std::string>);
  vm->RegisterFunction("FormListAdd"sv, "StorageUtil"sv, ListAdd<TESForm *, UInt32>);

  vm->RegisterFunction("IntListGet"sv, "StorageUtil"sv, ListGet<SInt32, SInt32>);
  vm->RegisterFunction("FloatListGet"sv, "StorageUtil"sv, ListGet<float, float>);
  vm->RegisterFunction("StringListGet"sv, "StorageUtil"sv, ListGet<BSFixedString, std::string>);
  vm->RegisterFunction("FormListGet"sv, "StorageUtil"sv, ListGet<TESForm *, UInt32>);

  vm->RegisterFunction("IntListSet"sv, "StorageUtil"sv, ListSet<SInt32, SInt32>);
  vm->RegisterFunction("FloatListSet"sv, "StorageUtil"sv, ListSet<float, float>);
  vm->RegisterFunction("StringListSet"sv, "StorageUtil"sv, ListSet<BSFixedString, std::string>);
  vm->RegisterFunction("FormListSet"sv, "StorageUtil"sv, ListSet<TESForm *, UInt32>);

  vm->RegisterFunction("IntListPluck"sv, "StorageUtil"sv, ListPluck<SInt32, SInt32>);
  vm->RegisterFunction("FloatListPluck"sv, "StorageUtil"sv, ListPluck<float, float>);
  vm->RegisterFunction("StringListPluck"sv, "StorageUtil"sv, ListPluck<BSFixedString, std::string>);
  vm->RegisterFunction("FormListPluck"sv, "StorageUtil"sv, ListPluck<TESForm *, UInt32>);

  vm->RegisterFunction("IntListShift"sv, "StorageUtil"sv, ListShift<SInt32, SInt32>);
  vm->RegisterFunction("FloatListShift"sv, "StorageUtil"sv, ListShift<float, float>);
  vm->RegisterFunction("StringListShift"sv, "StorageUtil"sv, ListShift<BSFixedString, std::string>);
  vm->RegisterFunction("FormListShift"sv, "StorageUtil"sv, ListShift<TESForm *, UInt32>);

  vm->RegisterFunction("IntListPop"sv, "StorageUtil"sv, ListPop<SInt32, SInt32>);
  vm->RegisterFunction("FloatListPop"sv, "StorageUtil"sv, ListPop<float, float>);
  vm->RegisterFunction("StringListPop"sv, "StorageUtil"sv, ListPop<BSFixedString, std::string>);
  vm->RegisterFunction("FormListPop"sv, "StorageUtil"sv, ListPop<TESForm *, UInt32>);

  vm->RegisterFunction("IntListAdjust"sv, "StorageUtil"sv, ListAdjust<SInt32, SInt32>);
  vm->RegisterFunction("FloatListAdjust"sv, "StorageUtil"sv, ListAdjust<float, float>);

  vm->RegisterFunction("IntListRemove"sv, "StorageUtil"sv, ListRemove<SInt32, SInt32>);
  vm->RegisterFunction("FloatListRemove"sv, "StorageUtil"sv, ListRemove<float, float>);
  vm->RegisterFunction("StringListRemove"sv, "StorageUtil"sv, ListRemove<BSFixedString, std::string>);
  vm->RegisterFunction("FormListRemove"sv, "StorageUtil"sv, ListRemove<TESForm *, UInt32>);

  vm->RegisterFunction("IntListInsert"sv, "StorageUtil"sv, ListInsertAt<SInt32, SInt32>);
  vm->RegisterFunction("FloatListInsert"sv, "StorageUtil"sv, ListInsertAt<float, float>);
  vm->RegisterFunction("StringListInsert"sv, "StorageUtil"sv, ListInsertAt<BSFixedString, std::string>);
  vm->RegisterFunction("FormListInsert"sv, "StorageUtil"sv, ListInsertAt<TESForm *, UInt32>);

  vm->RegisterFunction("IntListRemoveAt"sv, "StorageUtil"sv, ListRemoveAt<SInt32, SInt32>);
  vm->RegisterFunction("FloatListRemoveAt"sv, "StorageUtil"sv, ListRemoveAt<float, float>);
  vm->RegisterFunction("StringListRemoveAt"sv, "StorageUtil"sv, ListRemoveAt<BSFixedString, std::string>);
  vm->RegisterFunction("FormListRemoveAt"sv, "StorageUtil"sv, ListRemoveAt<TESForm *, UInt32>);

  vm->RegisterFunction("IntListClear"sv, "StorageUtil"sv, ListClear<SInt32, SInt32>);
  vm->RegisterFunction("FloatListClear"sv, "StorageUtil"sv, ListClear<float, float>);
  vm->RegisterFunction("StringListClear"sv, "StorageUtil"sv, ListClear<BSFixedString, std::string>);
  vm->RegisterFunction("FormListClear"sv, "StorageUtil"sv, ListClear<TESForm *, UInt32>);

  vm->RegisterFunction("IntListCount"sv, "StorageUtil"sv, ListCount<SInt32, SInt32>);
  vm->RegisterFunction("FloatListCount"sv, "StorageUtil"sv, ListCount<float, float>);
  vm->RegisterFunction("StringListCount"sv, "StorageUtil"sv, ListCount<BSFixedString, std::string>);
  vm->RegisterFunction("FormListCount"sv, "StorageUtil"sv, ListCount<TESForm *, UInt32>);

  vm->RegisterFunction("IntListCountValue"sv, "StorageUtil"sv, ListCountValue<SInt32, SInt32>);
  vm->RegisterFunction("FloatListCountValue"sv, "StorageUtil"sv, ListCountValue<float, float>);
  vm->RegisterFunction("StringListCountValue"sv, "StorageUtil"sv, ListCountValue<BSFixedString, std::string>);
  vm->RegisterFunction("FormListCountValue"sv, "StorageUtil"sv, ListCountValue<TESForm *, UInt32>);

  vm->RegisterFunction("IntListFind"sv, "StorageUtil"sv, ListFind<SInt32, SInt32>);
  vm->RegisterFunction("FloatListFind"sv, "StorageUtil"sv, ListFind<float, float>);
  vm->RegisterFunction("StringListFind"sv, "StorageUtil"sv, ListFind<BSFixedString, std::string>);
  vm->RegisterFunction("FormListFind"sv, "StorageUtil"sv, ListFind<TESForm *, UInt32>);

  vm->RegisterFunction("IntListHas"sv, "StorageUtil"sv, ListHas<SInt32, SInt32>);
  vm->RegisterFunction("FloatListHas"sv, "StorageUtil"sv, ListHas<float, float>);
  vm->RegisterFunction("StringListHas"sv, "StorageUtil"sv, ListHas<BSFixedString, std::string>);
  vm->RegisterFunction("FormListHas"sv, "StorageUtil"sv, ListHas<TESForm *, UInt32>);

  vm->RegisterFunction("IntListSort"sv, "StorageUtil"sv, ListSort<SInt32, SInt32>);
  vm->RegisterFunction("FloatListSort"sv, "StorageUtil"sv, ListSort<float, float>);
  vm->RegisterFunction("StringListSort"sv, "StorageUtil"sv, ListSort<BSFixedString, std::string>);
  vm->RegisterFunction("FormListSort"sv, "StorageUtil"sv, ListSort<TESForm *, UInt32>);

  // TODO: These functions use reference_array which requires special handling for CommonLibSSE-NG
  // vm->RegisterFunction("IntListSlice"sv, "StorageUtil"sv, ListSlice<SInt32, SInt32>);
  // vm->RegisterFunction("FloatListSlice"sv, "StorageUtil"sv, ListSlice<float, float>);
  // vm->RegisterFunction("StringListSlice"sv, "StorageUtil"sv, ListSlice<BSFixedString, std::string>);
  // vm->RegisterFunction("FormListSlice"sv, "StorageUtil"sv, ListSlice<TESForm *, UInt32>);

  vm->RegisterFunction("IntListResize"sv, "StorageUtil"sv, ListResize<SInt32, SInt32>);
  vm->RegisterFunction("FloatListResize"sv, "StorageUtil"sv, ListResize<float, float>);
  vm->RegisterFunction("StringListResize"sv, "StorageUtil"sv, ListResize<BSFixedString, std::string>);
  vm->RegisterFunction("FormListResize"sv, "StorageUtil"sv, ListResize<TESForm *, UInt32>);

  // TODO: These functions use reference_array which requires special handling for CommonLibSSE-NG
  // vm->RegisterFunction("IntListCopy"sv, "StorageUtil"sv, ListCopy<SInt32, SInt32>);
  // vm->RegisterFunction("FloatListCopy"sv, "StorageUtil"sv, ListCopy<float, float>);
  // vm->RegisterFunction("StringListCopy"sv, "StorageUtil"sv, ListCopy<BSFixedString, std::string>);
  // vm->RegisterFunction("FormListCopy"sv, "StorageUtil"sv, ListCopy<TESForm *, UInt32>);

  vm->RegisterFunction("IntListToArray"sv, "StorageUtil"sv, ToArray<SInt32, SInt32>);
  vm->RegisterFunction("FloatListToArray"sv, "StorageUtil"sv, ToArray<float, float>);
  vm->RegisterFunction("StringListToArray"sv, "StorageUtil"sv, ToArray<BSFixedString, std::string>);
  vm->RegisterFunction("FormListToArray"sv, "StorageUtil"sv, ToArray<TESForm *, UInt32>);

  // Prefix functions
  vm->RegisterFunction("CountIntValuePrefix"sv, "StorageUtil"sv, CountValuesPrefixKey<SInt32, SInt32>);
  vm->RegisterFunction("CountFloatValuePrefix"sv, "StorageUtil"sv, CountValuesPrefixKey<float, float>);
  vm->RegisterFunction("CountStringValuePrefix"sv, "StorageUtil"sv, CountValuesPrefixKey<BSFixedString, std::string>);
  vm->RegisterFunction("CountFormValuePrefix"sv, "StorageUtil"sv, CountValuesPrefixKey<TESForm *, UInt32>);

  vm->RegisterFunction("CountIntListPrefix"sv, "StorageUtil"sv, CountListsPrefixKey<SInt32, SInt32>);
  vm->RegisterFunction("CountFloatListPrefix"sv, "StorageUtil"sv, CountListsPrefixKey<float, float>);
  vm->RegisterFunction("CountStringListPrefix"sv, "StorageUtil"sv, CountListsPrefixKey<BSFixedString, std::string>);
  vm->RegisterFunction("CountFormListPrefix"sv, "StorageUtil"sv, CountListsPrefixKey<TESForm *, UInt32>);

  vm->RegisterFunction("CountAllPrefix"sv, "StorageUtil"sv, CountAllPrefix);

  vm->RegisterFunction("ClearIntValuePrefix"sv, "StorageUtil"sv, ClearValuesPrefixKey<SInt32, SInt32>);
  vm->RegisterFunction("ClearFloatValuePrefix"sv, "StorageUtil"sv, ClearValuesPrefixKey<float, float>);
  vm->RegisterFunction("ClearStringValuePrefix"sv, "StorageUtil"sv, ClearValuesPrefixKey<BSFixedString, std::string>);
  vm->RegisterFunction("ClearFormValuePrefix"sv, "StorageUtil"sv, ClearValuesPrefixKey<TESForm *, UInt32>);

  vm->RegisterFunction("ClearIntListPrefix"sv, "StorageUtil"sv, ClearListsPrefixKey<SInt32, SInt32>);
  vm->RegisterFunction("ClearFloatListPrefix"sv, "StorageUtil"sv, ClearListsPrefixKey<float, float>);
  vm->RegisterFunction("ClearStringListPrefix"sv, "StorageUtil"sv, ClearListsPrefixKey<BSFixedString, std::string>);
  vm->RegisterFunction("ClearFormListPrefix"sv, "StorageUtil"sv, ClearListsPrefixKey<TESForm *, UInt32>);

  vm->RegisterFunction("ClearAllPrefix"sv, "StorageUtil"sv, ClearAllPrefix);

  // Object prefix functions
  vm->RegisterFunction("CountObjIntValuePrefix"sv, "StorageUtil"sv, ObjCountValuesPrefixKey<SInt32, SInt32>);
  vm->RegisterFunction("CountObjFloatValuePrefix"sv, "StorageUtil"sv, ObjCountValuesPrefixKey<float, float>);
  vm->RegisterFunction("CountObjStringValuePrefix"sv, "StorageUtil"sv, ObjCountValuesPrefixKey<BSFixedString, std::string>);
  vm->RegisterFunction("CountObjFormValuePrefix"sv, "StorageUtil"sv, ObjCountValuesPrefixKey<TESForm *, UInt32>);

  vm->RegisterFunction("CountObjIntListPrefix"sv, "StorageUtil"sv, ObjCountListsPrefixKey<SInt32, SInt32>);
  vm->RegisterFunction("CountObjFloatListPrefix"sv, "StorageUtil"sv, ObjCountListsPrefixKey<float, float>);
  vm->RegisterFunction("CountObjStringListPrefix"sv, "StorageUtil"sv, ObjCountListsPrefixKey<BSFixedString, std::string>);
  vm->RegisterFunction("CountObjFormListPrefix"sv, "StorageUtil"sv, ObjCountListsPrefixKey<TESForm *, UInt32>);

  vm->RegisterFunction("CountAllObjPrefix"sv, "StorageUtil"sv, ObjCountAllPrefix);

  vm->RegisterFunction("ClearObjIntValuePrefix"sv, "StorageUtil"sv, ObjClearValuesPrefixKey<SInt32, SInt32>);
  vm->RegisterFunction("ClearObjFloatValuePrefix"sv, "StorageUtil"sv, ObjClearValuesPrefixKey<float, float>);
  vm->RegisterFunction("ClearObjStringValuePrefix"sv, "StorageUtil"sv, ObjClearValuesPrefixKey<BSFixedString, std::string>);
  vm->RegisterFunction("ClearObjFormValuePrefix"sv, "StorageUtil"sv, ObjClearValuesPrefixKey<TESForm *, UInt32>);

  vm->RegisterFunction("ClearObjIntListPrefix"sv, "StorageUtil"sv, ObjClearListsPrefixKey<SInt32, SInt32>);
  vm->RegisterFunction("ClearObjFloatListPrefix"sv, "StorageUtil"sv, ObjClearListsPrefixKey<float, float>);
  vm->RegisterFunction("ClearObjStringListPrefix"sv, "StorageUtil"sv, ObjClearListsPrefixKey<BSFixedString, std::string>);
  vm->RegisterFunction("ClearObjFormListPrefix"sv, "StorageUtil"sv, ObjClearListsPrefixKey<TESForm *, UInt32>);

  vm->RegisterFunction("ClearAllObjPrefix"sv, "StorageUtil"sv, ObjClearAllPrefix);

  vm->RegisterFunction("FormListFilterByTypes"sv, "StorageUtil"sv, FilterByTypes);

  // Random
  vm->RegisterFunction("IntListRandom"sv, "StorageUtil"sv, ListRandom<SInt32, SInt32>);
  vm->RegisterFunction("FloatListRandom"sv, "StorageUtil"sv, ListRandom<float, float>);
  vm->RegisterFunction("StringListRandom"sv, "StorageUtil"sv, ListRandom<BSFixedString, std::string>);
  vm->RegisterFunction("FormListRandom"sv, "StorageUtil"sv, ListRandom<TESForm *, UInt32>);

  // Debug Functions
  vm->RegisterFunction("debug_DeleteValues"sv, "StorageUtil"sv, DeleteValues);
  vm->RegisterFunction("debug_DeleteAllValues"sv, "StorageUtil"sv, DeleteAllValues);
  vm->RegisterFunction("debug_Cleanup"sv, "StorageUtil"sv, Cleanup);

  vm->RegisterFunction("debug_GetIntObjectCount"sv, "StorageUtil"sv, GetObjectCount<SInt32, SInt32>);
  vm->RegisterFunction("debug_GetFloatObjectCount"sv, "StorageUtil"sv, GetObjectCount<float, float>);
  vm->RegisterFunction("debug_GetStringObjectCount"sv, "StorageUtil"sv, GetObjectCount<BSFixedString, std::string>);
  vm->RegisterFunction("debug_GetFormObjectCount"sv, "StorageUtil"sv, GetObjectCount<TESForm *, UInt32>);

  vm->RegisterFunction("debug_GetIntListObjectCount"sv, "StorageUtil"sv, GetListsObjectCount<SInt32, SInt32>);
  vm->RegisterFunction("debug_GetFloatListObjectCount"sv, "StorageUtil"sv, GetListsObjectCount<float, float>);
  vm->RegisterFunction("debug_GetStringListObjectCount"sv, "StorageUtil"sv, GetListsObjectCount<BSFixedString, std::string>);
  vm->RegisterFunction("debug_GetFormListObjectCount"sv, "StorageUtil"sv, GetListsObjectCount<TESForm *, UInt32>);

  vm->RegisterFunction("debug_GetIntKeysCount"sv, "StorageUtil"sv, GetKeyCount<SInt32, SInt32>);
  vm->RegisterFunction("debug_GetFloatKeysCount"sv, "StorageUtil"sv, GetKeyCount<float, float>);
  vm->RegisterFunction("debug_GetStringKeysCount"sv, "StorageUtil"sv, GetKeyCount<BSFixedString, std::string>);
  vm->RegisterFunction("debug_GetFormKeysCount"sv, "StorageUtil"sv, GetKeyCount<TESForm *, UInt32>);

  vm->RegisterFunction("debug_GetIntListKeysCount"sv, "StorageUtil"sv, GetListsKeyCount<SInt32, SInt32>);
  vm->RegisterFunction("debug_GetFloatListKeysCount"sv, "StorageUtil"sv, GetListsKeyCount<float, float>);
  vm->RegisterFunction("debug_GetStringListKeysCount"sv, "StorageUtil"sv, GetListsKeyCount<BSFixedString, std::string>);
  vm->RegisterFunction("debug_GetFormListKeysCount"sv, "StorageUtil"sv, GetListsKeyCount<TESForm *, UInt32>);

  vm->RegisterFunction("debug_GetIntObject"sv, "StorageUtil"sv, GetNthObj<SInt32, SInt32>);
  vm->RegisterFunction("debug_GetFloatObject"sv, "StorageUtil"sv, GetNthObj<float, float>);
  vm->RegisterFunction("debug_GetStringObject"sv, "StorageUtil"sv, GetNthObj<BSFixedString, std::string>);
  vm->RegisterFunction("debug_GetFormObject"sv, "StorageUtil"sv, GetNthObj<TESForm *, UInt32>);

  vm->RegisterFunction("debug_GetIntListObject"sv, "StorageUtil"sv, GetListsNthObj<SInt32, SInt32>);
  vm->RegisterFunction("debug_GetFloatListObject"sv, "StorageUtil"sv, GetListsNthObj<float, float>);
  vm->RegisterFunction("debug_GetStringListObject"sv, "StorageUtil"sv, GetListsNthObj<BSFixedString, std::string>);
  vm->RegisterFunction("debug_GetFormListObject"sv, "StorageUtil"sv, GetListsNthObj<TESForm *, UInt32>);

  vm->RegisterFunction("debug_GetIntKey"sv, "StorageUtil"sv, GetNthKey<SInt32, SInt32>);
  vm->RegisterFunction("debug_GetFloatKey"sv, "StorageUtil"sv, GetNthKey<float, float>);
  vm->RegisterFunction("debug_GetStringKey"sv, "StorageUtil"sv, GetNthKey<BSFixedString, std::string>);
  vm->RegisterFunction("debug_GetFormKey"sv, "StorageUtil"sv, GetNthKey<TESForm *, UInt32>);

  vm->RegisterFunction("debug_GetIntListKey"sv, "StorageUtil"sv, GetListsNthKey<SInt32, SInt32>);
  vm->RegisterFunction("debug_GetFloatListKey"sv, "StorageUtil"sv, GetListsNthKey<float, float>);
  vm->RegisterFunction("debug_GetStringListKey"sv, "StorageUtil"sv, GetListsNthKey<BSFixedString, std::string>);
  vm->RegisterFunction("debug_GetFormListKey"sv, "StorageUtil"sv, GetListsNthKey<TESForm *, UInt32>);

  vm->RegisterFunction("debug_AllIntListObjs"sv, "StorageUtil"sv, GetListsAllObj<SInt32, SInt32>);
  vm->RegisterFunction("debug_AllFloatListObjs"sv, "StorageUtil"sv, GetListsAllObj<float, float>);
  vm->RegisterFunction("debug_AllStringListObjs"sv, "StorageUtil"sv, GetListsAllObj<BSFixedString, std::string>);
  vm->RegisterFunction("debug_AllFormListObjs"sv, "StorageUtil"sv, GetListsAllObj<TESForm *, UInt32>);

  vm->RegisterFunction("debug_AllIntObjs"sv, "StorageUtil"sv, GetValuesAllObj<SInt32, SInt32>);
  vm->RegisterFunction("debug_AllFloatObjs"sv, "StorageUtil"sv, GetValuesAllObj<float, float>);
  vm->RegisterFunction("debug_AllStringObjs"sv, "StorageUtil"sv, GetValuesAllObj<BSFixedString, std::string>);
  vm->RegisterFunction("debug_AllFormObjs"sv, "StorageUtil"sv, GetValuesAllObj<TESForm *, UInt32>);

  vm->RegisterFunction("debug_AllObjIntListKeys"sv, "StorageUtil"sv, GetListsObjKeys<SInt32, SInt32>);
  vm->RegisterFunction("debug_AllObjFloatListKeys"sv, "StorageUtil"sv, GetListsObjKeys<float, float>);
  vm->RegisterFunction("debug_AllObjStringListKeys"sv, "StorageUtil"sv, GetListsObjKeys<BSFixedString, std::string>);
  vm->RegisterFunction("debug_AllObjFormListKeys"sv, "StorageUtil"sv, GetListsObjKeys<TESForm *, UInt32>);

  vm->RegisterFunction("debug_AllObjIntKeys"sv, "StorageUtil"sv, GetValuesObjKeys<SInt32, SInt32>);
  vm->RegisterFunction("debug_AllObjFloatKeys"sv, "StorageUtil"sv, GetValuesObjKeys<float, float>);
  vm->RegisterFunction("debug_AllObjStringKeys"sv, "StorageUtil"sv, GetValuesObjKeys<BSFixedString, std::string>);
  vm->RegisterFunction("debug_AllObjFormKeys"sv, "StorageUtil"sv, GetValuesObjKeys<TESForm *, UInt32>);

  // Set NoWait flags (callable from tasklets)
  constexpr auto script = "StorageUtil";
  vm->SetCallableFromTasklets(script, "SetIntValue", true);
  vm->SetCallableFromTasklets(script, "SetFloatValue", true);
  vm->SetCallableFromTasklets(script, "SetStringValue", true);
  vm->SetCallableFromTasklets(script, "SetFormValue", true);
  vm->SetCallableFromTasklets(script, "GetIntValue", true);
  vm->SetCallableFromTasklets(script, "GetFloatValue", true);
  vm->SetCallableFromTasklets(script, "GetStringValue", true);
  vm->SetCallableFromTasklets(script, "GetFormValue", true);
  vm->SetCallableFromTasklets(script, "UnsetIntValue", true);
  vm->SetCallableFromTasklets(script, "UnsetFloatValue", true);
  vm->SetCallableFromTasklets(script, "UnsetStringValue", true);
  vm->SetCallableFromTasklets(script, "UnsetFormValue", true);
  vm->SetCallableFromTasklets(script, "HasIntValue", true);
  vm->SetCallableFromTasklets(script, "HasFloatValue", true);
  vm->SetCallableFromTasklets(script, "HasStringValue", true);
  vm->SetCallableFromTasklets(script, "HasFormValue", true);
  vm->SetCallableFromTasklets(script, "IntListAdd", true);
  vm->SetCallableFromTasklets(script, "FloatListAdd", true);
  vm->SetCallableFromTasklets(script, "StringListAdd", true);
  vm->SetCallableFromTasklets(script, "FormListAdd", true);
  vm->SetCallableFromTasklets(script, "IntListGet", true);
  vm->SetCallableFromTasklets(script, "FloatListGet", true);
  vm->SetCallableFromTasklets(script, "StringListGet", true);
  vm->SetCallableFromTasklets(script, "FormListGet", true);
  vm->SetCallableFromTasklets(script, "IntListSet", true);
  vm->SetCallableFromTasklets(script, "FloatListSet", true);
  vm->SetCallableFromTasklets(script, "StringListSet", true);
  vm->SetCallableFromTasklets(script, "FormListSet", true);
  vm->SetCallableFromTasklets(script, "IntListPluck", true);
  vm->SetCallableFromTasklets(script, "FloatListPluck", true);
  vm->SetCallableFromTasklets(script, "StringListPluck", true);
  vm->SetCallableFromTasklets(script, "FormListPluck", true);
  vm->SetCallableFromTasklets(script, "IntListShift", true);
  vm->SetCallableFromTasklets(script, "FloatListShift", true);
  vm->SetCallableFromTasklets(script, "StringListShift", true);
  vm->SetCallableFromTasklets(script, "FormListShift", true);
  vm->SetCallableFromTasklets(script, "IntListPop", true);
  vm->SetCallableFromTasklets(script, "FloatListPop", true);
  vm->SetCallableFromTasklets(script, "StringListPop", true);
  vm->SetCallableFromTasklets(script, "FormListPop", true);
  vm->SetCallableFromTasklets(script, "IntListRemoveAt", true);
  vm->SetCallableFromTasklets(script, "FloatListRemoveAt", true);
  vm->SetCallableFromTasklets(script, "StringListRemoveAt", true);
  vm->SetCallableFromTasklets(script, "FormListRemoveAt", true);
  vm->SetCallableFromTasklets(script, "IntListClear", true);
  vm->SetCallableFromTasklets(script, "FloatListClear", true);
  vm->SetCallableFromTasklets(script, "StringListClear", true);
  vm->SetCallableFromTasklets(script, "FormListClear", true);
  vm->SetCallableFromTasklets(script, "IntListCount", true);
  vm->SetCallableFromTasklets(script, "FloatListCount", true);
  vm->SetCallableFromTasklets(script, "StringListCount", true);
  vm->SetCallableFromTasklets(script, "FormListCount", true);
  vm->SetCallableFromTasklets(script, "IntListCountValue", true);
  vm->SetCallableFromTasklets(script, "FloatListCountValue", true);
  vm->SetCallableFromTasklets(script, "StringListCountValue", true);
  vm->SetCallableFromTasklets(script, "FormListCountValue", true);
  vm->SetCallableFromTasklets(script, "IntListFind", true);
  vm->SetCallableFromTasklets(script, "FloatListFind", true);
  vm->SetCallableFromTasklets(script, "StringListFind", true);
  vm->SetCallableFromTasklets(script, "FormListFind", true);
  vm->SetCallableFromTasklets(script, "IntListHas", true);
  vm->SetCallableFromTasklets(script, "FloatListHas", true);
  vm->SetCallableFromTasklets(script, "StringListHas", true);
  vm->SetCallableFromTasklets(script, "FormListHas", true);
  vm->SetCallableFromTasklets(script, "IntListSort", true);
  vm->SetCallableFromTasklets(script, "FloatListSort", true);
  vm->SetCallableFromTasklets(script, "StringListSort", true);
  vm->SetCallableFromTasklets(script, "FormListSort", true);
  vm->SetCallableFromTasklets(script, "IntListSlice", true);
  vm->SetCallableFromTasklets(script, "FloatListSlice", true);
  vm->SetCallableFromTasklets(script, "StringListSlice", true);
  vm->SetCallableFromTasklets(script, "FormListSlice", true);
  vm->SetCallableFromTasklets(script, "IntListResize", true);
  vm->SetCallableFromTasklets(script, "FloatListResize", true);
  vm->SetCallableFromTasklets(script, "StringListResize", true);
  vm->SetCallableFromTasklets(script, "FormListResize", true);
  vm->SetCallableFromTasklets(script, "IntListCopy", true);
  vm->SetCallableFromTasklets(script, "FloatListCopy", true);
  vm->SetCallableFromTasklets(script, "StringListCopy", true);
  vm->SetCallableFromTasklets(script, "FormListCopy", true);
  vm->SetCallableFromTasklets(script, "IntListToArray", true);
  vm->SetCallableFromTasklets(script, "FloatListToArray", true);
  vm->SetCallableFromTasklets(script, "StringListToArray", true);
  vm->SetCallableFromTasklets(script, "FormListToArray", true);
  vm->SetCallableFromTasklets(script, "CountIntValuePrefix", true);
  vm->SetCallableFromTasklets(script, "CountFloatValuePrefix", true);
  vm->SetCallableFromTasklets(script, "CountStringValuePrefix", true);
  vm->SetCallableFromTasklets(script, "CountFormValuePrefix", true);
  vm->SetCallableFromTasklets(script, "CountIntListPrefix", true);
  vm->SetCallableFromTasklets(script, "CountFloatListPrefix", true);
  vm->SetCallableFromTasklets(script, "CountStringListPrefix", true);
  vm->SetCallableFromTasklets(script, "CountFormListPrefix", true);
  vm->SetCallableFromTasklets(script, "CountObjIntValuePrefix", true);
  vm->SetCallableFromTasklets(script, "CountObjFloatValuePrefix", true);
  vm->SetCallableFromTasklets(script, "CountObjStringValuePrefix", true);
  vm->SetCallableFromTasklets(script, "CountObjFormValuePrefix", true);
  vm->SetCallableFromTasklets(script, "CountObjIntListPrefix", true);
  vm->SetCallableFromTasklets(script, "CountObjFloatListPrefix", true);
  vm->SetCallableFromTasklets(script, "CountObjStringListPrefix", true);
  vm->SetCallableFromTasklets(script, "CountObjFormListPrefix", true);
  vm->SetCallableFromTasklets(script, "ClearObjIntValuePrefix", true);
  vm->SetCallableFromTasklets(script, "ClearObjFloatValuePrefix", true);
  vm->SetCallableFromTasklets(script, "ClearObjStringValuePrefix", true);
  vm->SetCallableFromTasklets(script, "ClearObjFormValuePrefix", true);
  vm->SetCallableFromTasklets(script, "ClearObjIntListPrefix", true);
  vm->SetCallableFromTasklets(script, "ClearObjFloatListPrefix", true);
  vm->SetCallableFromTasklets(script, "ClearObjStringListPrefix", true);
  vm->SetCallableFromTasklets(script, "ClearObjFormListPrefix", true);
  vm->SetCallableFromTasklets(script, "IntListRandom", true);
  vm->SetCallableFromTasklets(script, "FloatListRandom", true);
  vm->SetCallableFromTasklets(script, "StringListRandom", true);
  vm->SetCallableFromTasklets(script, "FormListRandom", true);
  vm->SetCallableFromTasklets(script, "debug_DeleteValues", true);
  vm->SetCallableFromTasklets(script, "debug_DeleteAllValues", true);
  vm->SetCallableFromTasklets(script, "debug_GetIntObjectCount", true);
  vm->SetCallableFromTasklets(script, "debug_GetFloatObjectCount", true);
  vm->SetCallableFromTasklets(script, "debug_GetStringObjectCount", true);
  vm->SetCallableFromTasklets(script, "debug_GetFormObjectCount", true);
  vm->SetCallableFromTasklets(script, "debug_GetIntListObjectCount", true);
  vm->SetCallableFromTasklets(script, "debug_GetFloatListObjectCount", true);
  vm->SetCallableFromTasklets(script, "debug_GetStringListObjectCount", true);
  vm->SetCallableFromTasklets(script, "debug_GetFormListObjectCount", true);
  vm->SetCallableFromTasklets(script, "debug_GetIntObject", true);
  vm->SetCallableFromTasklets(script, "debug_GetFloatObject", true);
  vm->SetCallableFromTasklets(script, "debug_GetStringObject", true);
  vm->SetCallableFromTasklets(script, "debug_GetFormObject", true);
  vm->SetCallableFromTasklets(script, "debug_GetIntListObject", true);
  vm->SetCallableFromTasklets(script, "debug_GetFloatListObject", true);
  vm->SetCallableFromTasklets(script, "debug_GetStringListObject", true);
  vm->SetCallableFromTasklets(script, "debug_GetFormListObject", true);
  vm->SetCallableFromTasklets(script, "debug_GetIntKey", true);
  vm->SetCallableFromTasklets(script, "debug_GetFloatKey", true);
  vm->SetCallableFromTasklets(script, "debug_GetStringKey", true);
  vm->SetCallableFromTasklets(script, "debug_GetFormKey", true);
  vm->SetCallableFromTasklets(script, "debug_GetIntListKey", true);
  vm->SetCallableFromTasklets(script, "debug_GetFloatListKey", true);
  vm->SetCallableFromTasklets(script, "debug_GetStringListKey", true);
  vm->SetCallableFromTasklets(script, "debug_GetFormListKey", true);

#ifdef _GLOBAL_EXTERNAL
  // Global file values
  vm->RegisterFunction("FileSetIntValue"sv, "StorageUtil"sv, FileSetValue<SInt32>);
  vm->RegisterFunction("FileSetFloatValue"sv, "StorageUtil"sv, FileSetValue<float>);
  vm->RegisterFunction("FileSetStringValue"sv, "StorageUtil"sv, FileSetValue<BSFixedString>);
  vm->RegisterFunction("FileSetFormValue"sv, "StorageUtil"sv, FileSetValue<TESForm *>);

  vm->RegisterFunction("FileGetIntValue"sv, "StorageUtil"sv, FileGetValue<SInt32>);
  vm->RegisterFunction("FileGetFloatValue"sv, "StorageUtil"sv, FileGetValue<float>);
  vm->RegisterFunction("FileGetStringValue"sv, "StorageUtil"sv, FileGetValue<BSFixedString>);
  vm->RegisterFunction("FileGetFormValue"sv, "StorageUtil"sv, FileGetValue<TESForm *>);

  vm->RegisterFunction("FileAdjustIntValue"sv, "StorageUtil"sv, FileAdjustValue<SInt32>);
  vm->RegisterFunction("FileAdjustFloatValue"sv, "StorageUtil"sv, FileAdjustValue<float>);

  vm->RegisterFunction("FileUnsetIntValue"sv, "StorageUtil"sv, FileUnsetValue<SInt32>);
  vm->RegisterFunction("FileUnsetFloatValue"sv, "StorageUtil"sv, FileUnsetValue<float>);
  vm->RegisterFunction("FileUnsetStringValue"sv, "StorageUtil"sv, FileUnsetValue<BSFixedString>);
  vm->RegisterFunction("FileUnsetFormValue"sv, "StorageUtil"sv, FileUnsetValue<TESForm *>);

  vm->RegisterFunction("FileHasIntValue"sv, "StorageUtil"sv, FileHasValue<SInt32>);
  vm->RegisterFunction("FileHasFloatValue"sv, "StorageUtil"sv, FileHasValue<float>);
  vm->RegisterFunction("FileHasStringValue"sv, "StorageUtil"sv, FileHasValue<BSFixedString>);
  vm->RegisterFunction("FileHasFormValue"sv, "StorageUtil"sv, FileHasValue<TESForm *>);

  // Global file lists
  vm->RegisterFunction("FileIntListAdd"sv, "StorageUtil"sv, FileListAdd<SInt32>);
  vm->RegisterFunction("FileFloatListAdd"sv, "StorageUtil"sv, FileListAdd<float>);
  vm->RegisterFunction("FileStringListAdd"sv, "StorageUtil"sv, FileListAdd<BSFixedString>);
  vm->RegisterFunction("FileFormListAdd"sv, "StorageUtil"sv, FileListAdd<TESForm *>);

  vm->RegisterFunction("FileIntListGet"sv, "StorageUtil"sv, FileListGet<SInt32>);
  vm->RegisterFunction("FileFloatListGet"sv, "StorageUtil"sv, FileListGet<float>);
  vm->RegisterFunction("FileStringListGet"sv, "StorageUtil"sv, FileListGet<BSFixedString>);
  vm->RegisterFunction("FileFormListGet"sv, "StorageUtil"sv, FileListGet<TESForm *>);

  vm->RegisterFunction("FileIntListSet"sv, "StorageUtil"sv, FileListSet<SInt32>);
  vm->RegisterFunction("FileFloatListSet"sv, "StorageUtil"sv, FileListSet<float>);
  vm->RegisterFunction("FileStringListSet"sv, "StorageUtil"sv, FileListSet<BSFixedString>);
  vm->RegisterFunction("FileFormListSet"sv, "StorageUtil"sv, FileListSet<TESForm *>);

  vm->RegisterFunction("FileIntListAdjust"sv, "StorageUtil"sv, FileListAdjust<SInt32>);
  vm->RegisterFunction("FileFloatListAdjust"sv, "StorageUtil"sv, FileListAdjust<float>);

  vm->RegisterFunction("FileIntListRemove"sv, "StorageUtil"sv, FileListRemove<SInt32>);
  vm->RegisterFunction("FileFloatListRemove"sv, "StorageUtil"sv, FileListRemove<float>);
  vm->RegisterFunction("FileStringListRemove"sv, "StorageUtil"sv, FileListRemove<BSFixedString>);
  vm->RegisterFunction("FileFormListRemove"sv, "StorageUtil"sv, FileListRemove<TESForm *>);

  vm->RegisterFunction("FileIntListRemoveAt"sv, "StorageUtil"sv, FileListRemoveAt<SInt32>);
  vm->RegisterFunction("FileFloatListRemoveAt"sv, "StorageUtil"sv, FileListRemoveAt<float>);
  vm->RegisterFunction("FileStringListRemoveAt"sv, "StorageUtil"sv, FileListRemoveAt<BSFixedString>);
  vm->RegisterFunction("FileFormListRemoveAt"sv, "StorageUtil"sv, FileListRemoveAt<TESForm *>);

  vm->RegisterFunction("FileIntListInsertAt"sv, "StorageUtil"sv, FileListInsertAt<SInt32>);
  vm->RegisterFunction("FileFloatListInsertAt"sv, "StorageUtil"sv, FileListInsertAt<float>);
  vm->RegisterFunction("FileStringListInsertAt"sv, "StorageUtil"sv, FileListInsertAt<BSFixedString>);
  vm->RegisterFunction("FileFormListInsertAt"sv, "StorageUtil"sv, FileListInsertAt<TESForm *>);

  vm->RegisterFunction("FileIntListClear"sv, "StorageUtil"sv, FileListClear<SInt32>);
  vm->RegisterFunction("FileFloatListClear"sv, "StorageUtil"sv, FileListClear<float>);
  vm->RegisterFunction("FileStringListClear"sv, "StorageUtil"sv, FileListClear<BSFixedString>);
  vm->RegisterFunction("FileFormListClear"sv, "StorageUtil"sv, FileListClear<TESForm *>);

  vm->RegisterFunction("FileIntListCount"sv, "StorageUtil"sv, FileListCount<SInt32>);
  vm->RegisterFunction("FileFloatListCount"sv, "StorageUtil"sv, FileListCount<float>);
  vm->RegisterFunction("FileStringListCount"sv, "StorageUtil"sv, FileListCount<BSFixedString>);
  vm->RegisterFunction("FileFormListCount"sv, "StorageUtil"sv, FileListCount<TESForm *>);

  vm->RegisterFunction("FileIntListFind"sv, "StorageUtil"sv, FileListFind<SInt32>);
  vm->RegisterFunction("FileFloatListFind"sv, "StorageUtil"sv, FileListFind<float>);
  vm->RegisterFunction("FileStringListFind"sv, "StorageUtil"sv, FileListFind<BSFixedString>);
  vm->RegisterFunction("FileFormListFind"sv, "StorageUtil"sv, FileListFind<TESForm *>);

  vm->RegisterFunction("FileIntListHas"sv, "StorageUtil"sv, FileListHas<SInt32>);
  vm->RegisterFunction("FileFloatListHas"sv, "StorageUtil"sv, FileListHas<float>);
  vm->RegisterFunction("FileStringListHas"sv, "StorageUtil"sv, FileListHas<BSFixedString>);
  vm->RegisterFunction("FileFormListHas"sv, "StorageUtil"sv, FileListHas<TESForm *>);

  vm->RegisterFunction("FileIntListResize"sv, "StorageUtil"sv, FileListResize<SInt32>);
  vm->RegisterFunction("FileFloatListResize"sv, "StorageUtil"sv, FileListResize<float>);
  vm->RegisterFunction("FileStringListResize"sv, "StorageUtil"sv, FileListResize<BSFixedString>);
  vm->RegisterFunction("FileFormListResize"sv, "StorageUtil"sv, FileListResize<TESForm *>);

  // TODO: These functions use reference_array which requires special handling for CommonLibSSE-NG
  // vm->RegisterFunction("FileIntListSlice"sv, "StorageUtil"sv, FileListSlice<SInt32>);
  // vm->RegisterFunction("FileFloatListSlice"sv, "StorageUtil"sv, FileListSlice<float>);
  // vm->RegisterFunction("FileStringListSlice"sv, "StorageUtil"sv, FileListSlice<BSFixedString>);
  // vm->RegisterFunction("FileFormListSlice"sv, "StorageUtil"sv, FileListSlice<TESForm *>);

  // TODO: These functions use reference_array which requires special handling for CommonLibSSE-NG
  // vm->RegisterFunction("FileIntListCopy"sv, "StorageUtil"sv, FileListCopy<SInt32>);
  // vm->RegisterFunction("FileFloatListCopy"sv, "StorageUtil"sv, FileListCopy<float>);
  // vm->RegisterFunction("FileStringListCopy"sv, "StorageUtil"sv, FileListCopy<BSFixedString>);
  // vm->RegisterFunction("FileFormListCopy"sv, "StorageUtil"sv, FileListCopy<TESForm *>);

  vm->RegisterFunction("SaveExternalFile"sv, "StorageUtil"sv, SaveExternalFile);

  // Set NoWait flags for _GLOBAL_EXTERNAL functions
  vm->SetCallableFromTasklets(script, "FileSetIntValue", true);
  vm->SetCallableFromTasklets(script, "FileSetFloatValue", true);
  vm->SetCallableFromTasklets(script, "FileSetStringValue", true);
  vm->SetCallableFromTasklets(script, "FileSetFormValue", true);
  vm->SetCallableFromTasklets(script, "FileUnsetIntValue", true);
  vm->SetCallableFromTasklets(script, "FileUnsetFloatValue", true);
  vm->SetCallableFromTasklets(script, "FileUnsetStringValue", true);
  vm->SetCallableFromTasklets(script, "FileUnsetFormValue", true);
  vm->SetCallableFromTasklets(script, "FileHasIntValue", true);
  vm->SetCallableFromTasklets(script, "FileHasFloatValue", true);
  vm->SetCallableFromTasklets(script, "FileHasStringValue", true);
  vm->SetCallableFromTasklets(script, "FileHasFormValue", true);
  vm->SetCallableFromTasklets(script, "FileIntListAdd", true);
  vm->SetCallableFromTasklets(script, "FileFloatListAdd", true);
  vm->SetCallableFromTasklets(script, "FileStringListAdd", true);
  vm->SetCallableFromTasklets(script, "FileFormListAdd", true);
  vm->SetCallableFromTasklets(script, "FileIntListGet", true);
  vm->SetCallableFromTasklets(script, "FileFloatListGet", true);
  vm->SetCallableFromTasklets(script, "FileStringListGet", true);
  vm->SetCallableFromTasklets(script, "FileFormListGet", true);
  vm->SetCallableFromTasklets(script, "FileIntListSet", true);
  vm->SetCallableFromTasklets(script, "FileFloatListSet", true);
  vm->SetCallableFromTasklets(script, "FileStringListSet", true);
  vm->SetCallableFromTasklets(script, "FileFormListSet", true);
  vm->SetCallableFromTasklets(script, "FileIntListRemoveAt", true);
  vm->SetCallableFromTasklets(script, "FileFloatListRemoveAt", true);
  vm->SetCallableFromTasklets(script, "FileStringListRemoveAt", true);
  vm->SetCallableFromTasklets(script, "FileFormListRemoveAt", true);
  vm->SetCallableFromTasklets(script, "FileIntListInsertAt", true);
  vm->SetCallableFromTasklets(script, "FileFloatListInsertAt", true);
  vm->SetCallableFromTasklets(script, "FileStringListInsertAt", true);
  vm->SetCallableFromTasklets(script, "FileFormListInsertAt", true);
  vm->SetCallableFromTasklets(script, "FileIntListCount", true);
  vm->SetCallableFromTasklets(script, "FileFloatListCount", true);
  vm->SetCallableFromTasklets(script, "FileStringListCount", true);
  vm->SetCallableFromTasklets(script, "FileFormListCount", true);
  vm->SetCallableFromTasklets(script, "FileIntListFind", true);
  vm->SetCallableFromTasklets(script, "FileFloatListFind", true);
  vm->SetCallableFromTasklets(script, "FileStringListFind", true);
  vm->SetCallableFromTasklets(script, "FileFormListFind", true);
  vm->SetCallableFromTasklets(script, "FileIntListHas", true);
  vm->SetCallableFromTasklets(script, "FileFloatListHas", true);
  vm->SetCallableFromTasklets(script, "FileStringListHas", true);
  vm->SetCallableFromTasklets(script, "FileFormListHas", true);
  vm->SetCallableFromTasklets(script, "FileIntListSlice", true);
  vm->SetCallableFromTasklets(script, "FileFloatListSlice", true);
  vm->SetCallableFromTasklets(script, "FileStringListSlice", true);
  vm->SetCallableFromTasklets(script, "FileFormListSlice", true);
  vm->SetCallableFromTasklets(script, "FileIntListResize", true);
  vm->SetCallableFromTasklets(script, "FileFloatListResize", true);
  vm->SetCallableFromTasklets(script, "FileStringListResize", true);
  vm->SetCallableFromTasklets(script, "FileFormListResize", true);
  vm->SetCallableFromTasklets(script, "FileIntListCopy", true);
  vm->SetCallableFromTasklets(script, "FileFloatListCopy", true);
  vm->SetCallableFromTasklets(script, "FileStringListCopy", true);
  vm->SetCallableFromTasklets(script, "FileFormListCopy", true);
  vm->SetCallableFromTasklets(script, "SaveExternalFile", true);
#endif
}