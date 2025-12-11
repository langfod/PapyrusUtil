#include "Data.h"
#include "External.h"
#include "PCH.h"

#include <mutex>

namespace Data {

// Flat obj=>key=>value storage (non-static for external linkage)
intv *intValues = nullptr;
flov *floatValues = nullptr;
strv *stringValues = nullptr;
forv *formValues = nullptr;

// Vector obj=>key=>vector[i] storage (non-static for external linkage)
intl *intLists = nullptr;
flol *floatLists = nullptr;
strl *stringLists = nullptr;
forl *formLists = nullptr;

// Thread-safe lazy initialization using std::call_once
static std::once_flag s_initValuesFlag;
static std::once_flag s_initListsFlag;

static void InitValuesOnce() {
  if (!intValues)
    intValues = new intv();
  if (!floatValues)
    floatValues = new flov();
  if (!stringValues)
    stringValues = new strv();
  if (!formValues)
    formValues = new forv();
}

static void InitListsOnce() {
  if (!intLists)
    intLists = new intl();
  if (!floatLists)
    floatLists = new flol();
  if (!stringLists)
    stringLists = new strl();
  if (!formLists)
    formLists = new forl();
}

template <> intv *GetValues<SInt32, SInt32>() {
  std::call_once(s_initValuesFlag, InitValuesOnce);
  return intValues;
}
template <> flov *GetValues<float, float>() {
  std::call_once(s_initValuesFlag, InitValuesOnce);
  return floatValues;
}
template <> strv *GetValues<RE::BSFixedString, std::string>() {
  std::call_once(s_initValuesFlag, InitValuesOnce);
  return stringValues;
}
template <> forv *GetValues<RE::TESForm *, UInt32>() {
  std::call_once(s_initValuesFlag, InitValuesOnce);
  return formValues;
}

template <> intl *GetLists<SInt32, SInt32>() {
  std::call_once(s_initListsFlag, InitListsOnce);
  return intLists;
}
template <> flol *GetLists<float, float>() {
  std::call_once(s_initListsFlag, InitListsOnce);
  return floatLists;
}
template <> strl *GetLists<RE::BSFixedString, std::string>() {
  std::call_once(s_initListsFlag, InitListsOnce);
  return stringLists;
}
template <> forl *GetLists<RE::TESForm *, UInt32>() {
  std::call_once(s_initListsFlag, InitListsOnce);
  return formLists;
}

void InitLists() {
  // Use the same thread-safe initialization as GetValues()/GetLists()
  // to avoid race conditions if called concurrently
  std::call_once(s_initValuesFlag, InitValuesOnce);
  std::call_once(s_initListsFlag, InitListsOnce);
}

/*
 * Define methods
 */

template <typename T, typename S> void Values<T, S>::SetValue(UInt64 obj, std::string key, T value) {
  s_dataLock.Enter();
  boost::to_lower(key);
  Data[obj][key] = cast(value);
  s_dataLock.Leave();
}

template <typename T, typename S> T Values<T, S>::GetValue(UInt64 obj, std::string key, T value) {
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    boost::to_lower(key);
    typename Obj::iterator itr2 = itr->second.find(key);
    if (itr2 != itr->second.end())
      value = cast(itr2->second);
  }
  s_dataLock.Leave();
  return value;
}

template <typename T, typename S> T Values<T, S>::AdjustValue(UInt64 obj, std::string key, T value) {
  s_dataLock.Enter();
  boost::to_lower(key);
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    typename Obj::iterator itr2 = itr->second.find(key);
    if (itr2 != itr->second.end())
      value = (value + cast(itr2->second));
  }
  Data[obj][key] = value;
  s_dataLock.Leave();
  return value;
}
template <> RE::BSFixedString Values<RE::BSFixedString, std::string>::AdjustValue(UInt64 obj, std::string key, RE::BSFixedString value) {
  return value;
}
template <> RE::TESForm *Values<RE::TESForm *, UInt32>::AdjustValue(UInt64 obj, std::string key, RE::TESForm *value) { return value; }

template <typename T, typename S> bool Values<T, S>::UnsetValue(UInt64 obj, std::string key) {
  bool unset = false;
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    boost::to_lower(key);
    typename Obj::iterator itr2 = itr->second.find(key);
    if (itr2 != itr->second.end()) {
      unset = true;
      itr->second.erase(itr2);
      if (itr->second.size() == 0)
        Data.erase(itr);
    }
  }
  s_dataLock.Leave();
  return unset;
}

template <typename T, typename S> T Values<T, S>::PluckValue(UInt64 obj, std::string key, T value) {
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    boost::to_lower(key);
    typename Obj::iterator itr2 = itr->second.find(key);
    if (itr2 != itr->second.end()) {
      value = cast(itr2->second);
      itr->second.erase(itr2);
      if (itr->second.size() == 0)
        Data.erase(itr);
    }
  }
  s_dataLock.Leave();
  return value;
}

template <typename T, typename S> bool Values<T, S>::HasValue(UInt64 obj, std::string key) {
  bool found = false;
  s_dataLock.Enter();

  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    boost::to_lower(key);
    found = itr->second.find(key) != itr->second.end();
  }

  s_dataLock.Leave();
  return found;
}

template <typename T, typename S> int Lists<T, S>::ListAdd(UInt64 obj, std::string key, T value, bool allowDuplicate) {
  s_dataLock.Enter();

  boost::to_lower(key);
  // Check for duplicate inside the lock to avoid TOCTOU
  if (!allowDuplicate) {
    List *vector = GetVector(obj, key);
    if (vector != nullptr && std::find(vector->begin(), vector->end(), cast(value)) != vector->end()) {
      s_dataLock.Leave();
      return -1;
    }
  }
  int index = static_cast<int>(Data[obj][key].size());
  Data[obj][key].push_back(cast(value));

  s_dataLock.Leave();
  return index;
}

template <typename T, typename S> T Lists<T, S>::ListGet(UInt64 obj, std::string key, UInt32 index) {
  S value = S();
  s_dataLock.Enter();

  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL && index >= 0 && index < vector->size())
    value = vector->at(index);

  s_dataLock.Leave();
  return cast(value);
}

template <typename T, typename S> T Lists<T, S>::ListSet(UInt64 obj, std::string key, UInt32 index, T value) {
  s_dataLock.Enter();

  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL && index >= 0 && index < vector->size())
    vector->at(index) = cast(value);
  else if (index == 0 && (vector == NULL || vector->empty()))
    Data[obj][key].push_back(cast(value));
  else {
    S var = S();
    value = cast(var);
  }

  s_dataLock.Leave();
  return value;
}

template <typename T, typename S> T Lists<T, S>::ListAdjust(UInt64 obj, std::string key, UInt32 index, T value) {
  s_dataLock.Enter();

  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL && index >= 0 && index < vector->size()) {
    vector->at(index) += cast(value);
    value = cast(vector->at(index));
  } else {
    S var = S();
    value = cast(var);
  }

  s_dataLock.Leave();
  return value;
}
template <>
RE::BSFixedString Lists<RE::BSFixedString, std::string>::ListAdjust(UInt64 obj, std::string key, UInt32 index, RE::BSFixedString value) {
  return value;
}
template <> RE::TESForm *Lists<RE::TESForm *, UInt32>::ListAdjust(UInt64 obj, std::string key, UInt32 index, RE::TESForm *value) {
  return value;
}

template <typename T, typename S> int Lists<T, S>::ListRemove(UInt64 obj, std::string key, T value, bool allInstances) {
  int removed = 0;
  s_dataLock.Enter();

  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL) {
    // Remove
    int count = static_cast<int>(vector->size());
    if (allInstances)
      vector->erase(std::remove(vector->begin(), vector->end(), cast(value)), vector->end());
    else {
      typename List::iterator itr = std::find(vector->begin(), vector->end(), cast(value));
      if (itr != vector->end())
        vector->erase(itr);
    }
    removed = count - static_cast<int>(vector->size());
    // Cleanup
    if (vector->empty()) {
      Data[obj].erase(key);
      if (Data[obj].size() == 0)
        Data.erase(obj);
    }
  }

  s_dataLock.Leave();
  return removed;
}

template <typename T, typename S> bool Lists<T, S>::ListInsertAt(UInt64 obj, std::string key, UInt32 index, T value) {
  bool inserted = false;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL && !vector->empty() && index < vector->size()) {
    try {
      typename List::iterator itr = vector->begin();
      std::advance(itr, index);
      vector->insert(itr, cast(value));
      // vector->at(index) = value;
      inserted = true;
    } catch (const std::out_of_range &) {
      inserted = false;
    }
  } else if (index == 0 && (vector == NULL || vector->empty())) {
    Data[obj][key].push_back(cast(value));
    inserted = true;
  }
  s_dataLock.Leave();
  return inserted;
}

template <typename T, typename S> bool Lists<T, S>::ListRemoveAt(UInt64 obj, std::string key, UInt32 index) {
  bool removed = false;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL && !vector->empty() && index < vector->size()) {
    vector->erase(vector->begin() + index);
    removed = true;
    // Cleanup
    if (vector->empty()) {
      Data[obj].erase(key);
      if (Data[obj].size() == 0)
        Data.erase(obj);
    }
  }
  s_dataLock.Leave();
  return removed;
}

template <typename T, typename S> T Lists<T, S>::ListPluck(UInt64 obj, std::string key, UInt32 index, T value) {
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL && !vector->empty() && index < vector->size()) {
    value = cast(vector->at(index));
    vector->erase(vector->begin() + index);
    // Cleanup
    if (vector->empty()) {
      Data[obj].erase(key);
      if (Data[obj].size() == 0)
        Data.erase(obj);
    }
  }
  s_dataLock.Leave();
  return value;
}
template <typename T, typename S> T Lists<T, S>::ListShift(UInt64 obj, std::string key) {
  S value = S();
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL && !vector->empty()) {
    value = vector->at(0);
    vector->erase(vector->begin());
    // Cleanup
    if (vector->empty()) {
      Data[obj].erase(key);
      if (Data[obj].size() == 0)
        Data.erase(obj);
    }
  }
  s_dataLock.Leave();
  return cast(value);
}
template <typename T, typename S> T Lists<T, S>::ListPop(UInt64 obj, std::string key) {
  S value = S();
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL && !vector->empty()) {
    value = vector->at(vector->size() - 1);
    vector->pop_back();
    // Cleanup
    if (vector->empty()) {
      Data[obj].erase(key);
      if (Data[obj].size() == 0)
        Data.erase(obj);
    }
  }
  s_dataLock.Leave();
  return cast(value);
}

template <typename T, typename S> int Lists<T, S>::ListClear(UInt64 obj, std::string key) {
  int removed = 0;
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    boost::to_lower(key);
    typename Obj::iterator itr2 = itr->second.find(key);
    if (itr2 != itr->second.end()) {
      removed = static_cast<int>(itr2->second.size());
      itr->second.erase(itr2);
      // Cleanup
      if (itr->second.size() == 0)
        Data.erase(itr);
    }
  }
  s_dataLock.Leave();
  return removed;
}

template <typename T, typename S> int Lists<T, S>::ListCount(UInt64 obj, std::string key) {
  int count = 0;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL)
    count = static_cast<int>(vector->size());
  s_dataLock.Leave();
  return count;
}

template <typename T, typename S> int Lists<T, S>::ListCountValue(UInt64 obj, std::string key, T value, bool exclude) {
  int count = 0;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL) {
    S var = cast(value);
    for (typename List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
      if (var == *itr)
        count += 1;
    }
    if (exclude)
      count = static_cast<int>(vector->size()) - count;
  }
  s_dataLock.Leave();
  return count;
}

template <typename T, typename S> int Lists<T, S>::ListFind(UInt64 obj, std::string key, T value) {
  int index = -1;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL) {
    typename List::iterator itr = std::find(vector->begin(), vector->end(), cast(value));
    if (itr != vector->end())
      index = static_cast<int>(itr - vector->begin());
  }
  s_dataLock.Leave();
  return index;
}

template <typename T, typename S> bool Lists<T, S>::ListHas(UInt64 obj, std::string key, T value) {
  bool found = false;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  found = vector != NULL && std::find(vector->begin(), vector->end(), cast(value)) != vector->end();
  s_dataLock.Leave();
  return found;
}

template <typename T, typename S> void Lists<T, S>::ListSort(UInt64 obj, std::string key) {
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL)
    std::sort(vector->begin(), vector->end());
  s_dataLock.Leave();
}

template <typename T, typename S> void Lists<T, S>::ListSlice(UInt64 obj, std::string key, std::vector<T> &Output, UInt32 startIndex) {
  if (Output.size() > 0) {
    s_dataLock.Enter();
    boost::to_lower(key);
    List *vector = GetVector(obj, key);
    if (vector != nullptr && startIndex < vector->size()) {
      typename List::iterator itr = vector->begin();
      std::advance(itr, startIndex);
      UInt32 length = static_cast<UInt32>(Output.size());
      for (UInt32 index = 0; index < length && itr != vector->end(); ++itr, ++index) {
        T val = cast(*itr);
        Output[index] = val;
      }
    }
    s_dataLock.Leave();
  }
}

template <typename T, typename S> int Lists<T, S>::ListResize(UInt64 obj, std::string key, UInt32 length, T filler) {
  if (length == 0)
    return ListClear(obj, key) * -1;
  s_dataLock.Enter();
  boost::to_lower(key);
  int start = static_cast<int>(Data[obj][key].size());
  Data[obj][key].resize(length, cast(filler));
  int end = static_cast<int>(Data[obj][key].size());
  s_dataLock.Leave();
  return end - start;
}

template <typename T, typename S> bool Lists<T, S>::ListCopy(UInt64 obj, std::string key, const std::vector<T> &Input) {
  UInt32 length = static_cast<UInt32>(Input.size());
  if (length < 1)
    return false;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != nullptr)
    vector->clear();
  Data[obj][key].reserve(length);
  for (UInt32 i = 0; i < length; ++i) {
    T var = Input[i];
    S value = cast(var);
    Data[obj][key].push_back(value);
  }
  s_dataLock.Leave();
  return true;
}

template <typename T, typename S> std::vector<T> Lists<T, S>::ToArray(UInt64 obj, std::string key) {
  std::vector<T> arr;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != nullptr) {
    arr.reserve(vector->size());
    for (typename List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
      T var = cast(*itr);
      arr.push_back(var);
    }
  }
  s_dataLock.Leave();
  return arr;
}

template <typename T, typename S> int Values<T, S>::CountPrefixKey(std::string prefix) {
  if (prefix.length() < 1)
    return 0;
  int count = 0;
  s_dataLock.Enter();
  boost::to_lower(prefix);
  for (typename Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
    Obj &ObjRef = itr->second;
    for (typename Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end(); ++itr2) {
      if (boost::starts_with(std::string(itr2->first), prefix))
        count++;
    }
  }
  s_dataLock.Leave();
  return count;
}

template <typename T, typename S> int Values<T, S>::CountPrefixKey(UInt64 obj, std::string prefix) {
  if (prefix.length() < 1)
    return 0;
  int count = 0;
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    boost::to_lower(prefix);
    Obj &ObjRef = itr->second;
    for (typename Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end(); ++itr2) {
      if (boost::starts_with(std::string(itr2->first), prefix))
        count++;
    }
  }
  s_dataLock.Leave();
  return count;
}

template <typename T, typename S> int Lists<T, S>::CountPrefixKey(std::string prefix) {
  if (prefix.length() < 1)
    return 0;
  int count = 0;
  s_dataLock.Enter();
  boost::to_lower(prefix);
  for (typename Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
    Obj &ObjRef = itr->second;
    for (typename Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end(); ++itr2) {
      if (boost::starts_with(std::string(itr2->first), prefix))
        count++;
    }
  }
  s_dataLock.Leave();
  return count;
}

template <typename T, typename S> int Lists<T, S>::CountPrefixKey(UInt64 obj, std::string prefix) {
  if (prefix.length() < 1)
    return 0;
  int count = 0;
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    boost::to_lower(prefix);
    Obj &ObjRef = itr->second;
    for (typename Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end(); ++itr2) {
      if (boost::starts_with(std::string(itr2->first), prefix))
        count++;
    }
  }
  s_dataLock.Leave();
  return count;
}

template <typename T, typename S> int Values<T, S>::ClearPrefixKey(UInt64 obj, std::string prefix) {
  if (prefix.length() < 1)
    return 0;

  int count = 0;
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    boost::to_lower(prefix);
    Obj &ObjRef = itr->second;
    for (typename Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end();) {
      std::string key = itr2->first;
      if (boost::starts_with(key, prefix)) {
        itr2 = ObjRef.erase(itr2);
        count++;
      } else
        ++itr2;
    }
  }
  s_dataLock.Leave();
  return count;
}

template <typename T, typename S> int Lists<T, S>::ClearPrefixKey(UInt64 obj, std::string prefix) {
  if (prefix.length() < 1)
    return 0;

  int count = 0;
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    boost::to_lower(prefix);
    Obj &ObjRef = itr->second;
    for (typename Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end();) {
      std::string key = itr2->first;
      if (boost::starts_with(key, prefix)) {
        itr2 = ObjRef.erase(itr2);
        count++;
      } else
        ++itr2;
    }
  }
  s_dataLock.Leave();
  return count;
}

template <typename T, typename S> int Values<T, S>::ClearPrefixKey(std::string prefix) {
  if (prefix.length() < 1)
    return 0;

  int count = 0;
  s_dataLock.Enter();
  boost::to_lower(prefix);
  for (typename Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
    Obj &ObjRef = itr->second;
    for (typename Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end();) {
      std::string key = itr2->first;
      if (boost::starts_with(key, prefix)) {
        itr2 = ObjRef.erase(itr2);
        count++;
      } else
        ++itr2;
    }
  }
  s_dataLock.Leave();
  return count;
}

template <typename T, typename S> int Lists<T, S>::ClearPrefixKey(std::string prefix) {
  if (prefix.length() < 1)
    return 0;

  int count = 0;
  s_dataLock.Enter();
  boost::to_lower(prefix);
  for (typename Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
    Obj &ObjRef = itr->second;
    for (typename Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end();) {
      std::string key = itr2->first;
      if (boost::starts_with(key, prefix)) {
        itr2 = ObjRef.erase(itr2);
        count++;
      } else
        ++itr2;
    }
  }
  s_dataLock.Leave();
  return count;
}

inline bool CheckType(RE::TESForm *ref, const std::vector<UInt8> &validtypes) {
  return std::find(validtypes.begin(), validtypes.end(), static_cast<UInt8>(ref->GetFormType())) != validtypes.end();
}
template <>
std::vector<RE::TESForm *> Lists<RE::TESForm *, UInt32>::FilterByTypes(UInt64 obj, std::string key, const std::vector<UInt32> &types,
                                                                       bool matching) {
  std::vector<RE::TESForm *> output;
  if (types.size() < 1)
    return output;
  // Get easier to search type list.
  std::vector<UInt8> valid;
  valid.reserve(types.size());
  for (UInt32 idx = 0; idx < types.size(); ++idx) {
    UInt32 t = types[idx];
    valid.push_back((UInt8)t);
  }
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != nullptr && !vector->empty()) {
    output.reserve(vector->size());
    for (typename List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
      RE::TESForm *ref = cast(*itr);
      if (ref != nullptr && matching == (std::find(valid.begin(), valid.end(), static_cast<UInt8>(ref->GetFormType())) != valid.end())) {
        output.push_back(ref);
      }
    }
    output.shrink_to_fit();
  }
  s_dataLock.Leave();
  return output;
}

template <typename T, typename S>
std::vector<T> Lists<T, S>::FilterByTypes(UInt64 obj, std::string key, const std::vector<UInt32> &types, bool matching) {
  std::vector<T> output;
  return output;
}

// Special case for strings
template <> int Lists<RE::BSFixedString, std::string>::ListFind(UInt64 obj, std::string key, RE::BSFixedString value) {
  int index = -1;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != nullptr) {
    std::string var = value.c_str();
    for (typename List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
      if (boost::iequals(var, *itr)) {
        index = static_cast<int>(itr - vector->begin());
        break;
      }
    }
  }
  s_dataLock.Leave();
  return index;
}

template <> bool Lists<RE::BSFixedString, std::string>::ListHas(UInt64 obj, std::string key, RE::BSFixedString value) {
  return ListFind(obj, key, value) != -1;
}

template <> int Lists<RE::BSFixedString, std::string>::ListRemove(UInt64 obj, std::string key, RE::BSFixedString value, bool allInstances) {
  int removed = 0;
  s_dataLock.Enter();

  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != nullptr) {
    // Remove
    int initialSize = static_cast<int>(vector->size());
    std::string var = value.c_str();
    for (typename List::iterator itr = vector->begin(); itr != vector->end();) {
      if (boost::iequals(var, *itr)) {
        itr = vector->erase(itr);
        if (!allInstances)
          break;
      } else
        ++itr;
    }
    removed = initialSize - static_cast<int>(vector->size());
    // Cleanup
    if (vector->empty()) {
      Data[obj].erase(key);
      if (Data[obj].size() == 0)
        Data.erase(obj);
    }
  }

  s_dataLock.Leave();
  return removed;
}

template <> int Lists<RE::BSFixedString, std::string>::ListCountValue(UInt64 obj, std::string key, RE::BSFixedString value, bool exclude) {
  int count = 0;
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != nullptr) {
    std::string var = value.c_str();
    for (typename List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
      if (boost::iequals(var, *itr))
        count += 1;
    }
    if (exclude)
      count = static_cast<int>(vector->size() - count);
  }
  s_dataLock.Leave();
  return count;
}

// static boost::random_device rd;
static boost::random::mt19937 rnd;
static int randomIndex(int size) {
  // rnd.seed(rd());
  return boost::random::uniform_int_distribution<>(0, (size - 1))(rnd);
}

template <typename T, typename S> T Lists<T, S>::ListRandom(UInt64 obj, std::string key) {
  S value = S();
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != NULL && !vector->empty()) {
    int index = randomIndex(static_cast<int>(vector->size()));
    //_MESSAGE("ListRandom - Size(%d) - randomIndex(%d)", root[type][key].size(), index);
    value = vector->at(index);
  }
  s_dataLock.Leave();
  return cast(value);
}

/*TODO*/
template <typename T, typename S> std::vector<T> Lists<T, S>::ListRandomArray(UInt64 obj, std::string key, UInt32 count, bool duplicates) {
  std::vector<T> output;
  output.reserve(count);
  s_dataLock.Enter();
  boost::to_lower(key);
  List *vector = GetVector(obj, key);
  if (vector != nullptr && !vector->empty()) {
  }

  s_dataLock.Leave();
  return output;
}

/*
 * DEBUG methods
 */

template <typename T, typename S> void Values<T, S>::RemoveForm(UInt64 &obj) {
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    UInt32 type = (UInt32)(obj >> 32);
    UInt32 id = (UInt32)(obj);
    logger::info("Removed VALUE handle[{}][{}] with id: {}", obj, type, id);
    Data.erase(itr);
  }
  s_dataLock.Leave();
}

template <typename T, typename S> void Lists<T, S>::RemoveForm(UInt64 &obj) {
  s_dataLock.Enter();
  typename Map::iterator itr = Data.find(obj);
  if (itr != Data.end()) {
    UInt32 type = (UInt32)(obj >> 32);
    UInt32 id = (UInt32)(obj);
    logger::info("Removed LIST handle[{}][{}] with id: {}", obj, type, id);
    Data.erase(itr);
  }
  s_dataLock.Leave();
}

template <typename T, typename S> int Values<T, S>::Cleanup() {
  int removed = 0;
  s_dataLock.Enter();
  for (typename Map::iterator itr = Data.begin(); itr != Data.end();) {
    if (itr->first != 0) {
      UInt32 id = (itr->first & 0xFFFFFFFF);
      RE::TESForm *ptr = id <= 0 ? nullptr : RE::TESForm::LookupByID(id);

      if (itr->second.empty() || !Forms::IsValidObject(ptr, itr->first)) {
        itr = Data.erase(itr);
        removed++;
        continue;
      }
    }
    itr++;
  }
  s_dataLock.Leave();
  return removed;
}

template <typename T, typename S> int Lists<T, S>::Cleanup() {
  int removed = 0;
  s_dataLock.Enter();
  for (typename Map::iterator itr = Data.begin(); itr != Data.end();) {
    if (itr->first != 0) {
      UInt32 id = (itr->first & 0xFFFFFFFF);
      RE::TESForm *ptr = id <= 0 ? nullptr : RE::TESForm::LookupByID(id);

      if (itr->second.empty() || !Forms::IsValidObject(ptr, itr->first)) {
        itr = Data.erase(itr);
        removed++;
        continue;
      }
    }
    itr++;
  }
  s_dataLock.Leave();
  return removed;
}

template <typename T, typename S> RE::TESForm *Values<T, S>::GetNthObj(UInt32 i) {
  s_dataLock.Enter();
  RE::TESForm *result = nullptr;
  if (Data.size() > i) {
    typename Map::iterator itr = Data.begin();
    std::advance(itr, i);
    if (itr != Data.end())
      result = RE::TESForm::LookupByID((UInt32)(itr->first & 0xFFFFFFFF));
  }
  s_dataLock.Leave();
  return result;
}

template <typename T, typename S> RE::TESForm *Lists<T, S>::GetNthObj(UInt32 i) {
  s_dataLock.Enter();
  RE::TESForm *result = nullptr;
  if (Data.size() > i) {
    typename Map::iterator itr = Data.begin();
    std::advance(itr, i);
    if (itr != Data.end())
      result = RE::TESForm::LookupByID((UInt32)(itr->first & 0xFFFFFFFF));
  }
  s_dataLock.Leave();
  return result;
}

template <typename T, typename S> const std::string Values<T, S>::GetNthKey(UInt64 obj, UInt32 i) {
  std::string result;
  s_dataLock.Enter();
  typename Map::iterator mapItr = Data.find(obj);
  if (mapItr != Data.end() && mapItr->second.size() > i) {
    typename Obj::iterator itr = mapItr->second.begin();
    std::advance(itr, i);
    if (itr != mapItr->second.end())
      result = itr->first;
  }
  s_dataLock.Leave();
  return result;
}

template <typename T, typename S> const std::string Lists<T, S>::GetNthKey(UInt64 obj, UInt32 i) {
  std::string result;
  s_dataLock.Enter();
  typename Map::iterator mapItr = Data.find(obj);
  if (mapItr != Data.end() && mapItr->second.size() > i) {
    typename Obj::iterator itr = mapItr->second.begin();
    std::advance(itr, i);
    if (itr != mapItr->second.end())
      result = itr->first;
  }
  s_dataLock.Leave();
  return result;
}

template <typename T, typename S> std::vector<RE::TESForm *> Values<T, S>::GetAllObj() {
  std::vector<RE::TESForm *> arr;
  s_dataLock.Enter();
  arr.reserve(Data.size());
  for (typename Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
    RE::TESForm *FormRef = RE::TESForm::LookupByID((UInt32)(itr->first & 0xFFFFFFFF));
    arr.push_back(FormRef);
  }
  s_dataLock.Leave();
  return arr;
}

template <typename T, typename S> std::vector<RE::TESForm *> Lists<T, S>::GetAllObj() {
  std::vector<RE::TESForm *> arr;
  s_dataLock.Enter();
  arr.reserve(Data.size());
  for (typename Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
    RE::TESForm *FormRef = RE::TESForm::LookupByID((UInt32)(itr->first & 0xFFFFFFFF));
    arr.push_back(FormRef);
  }
  s_dataLock.Leave();
  return arr;
}

template <typename T, typename S> std::vector<RE::BSFixedString> Values<T, S>::GetAllObjKeys(UInt64 obj) {
  std::vector<RE::BSFixedString> arr;
  s_dataLock.Enter();
  if (Data.size() > 0 && Data.find(obj) != Data.end()) {
    for (typename Obj::iterator itr = Data[obj].begin(); itr != Data[obj].end(); ++itr) {
      arr.push_back(RE::BSFixedString(itr->first.c_str()));
    }
  }
  s_dataLock.Leave();
  return arr;
}

template <typename T, typename S> std::vector<RE::BSFixedString> Lists<T, S>::GetAllObjKeys(UInt64 obj) {
  std::vector<RE::BSFixedString> arr;
  s_dataLock.Enter();
  if (Data.size() > 0 && Data.find(obj) != Data.end()) {
    for (typename Obj::iterator itr = Data[obj].begin(); itr != Data[obj].end(); ++itr) {
      arr.push_back(RE::BSFixedString(itr->first.c_str()));
    }
  }
  s_dataLock.Leave();
  return arr;
}

// LEGACY style LOADING in single stream
template <typename T, typename S> void Values<T, S>::LoadStream(std::stringstream &ss) {
  int count;
  ss >> count;
  if (count < 1)
    return;
  s_dataLock.Enter();
  Data.reserve(count);
  for (int i = 0; i < count; i++) {
    UInt64 objKey;
    ss >> objKey;
    int count2;
    ss >> count2;

    // Check if objKey still exists and prevent it falling into global 0 key
    if (objKey != 0) {
      RE::TESForm *ObjRef = Forms::ResolveFormKey(objKey);
      objKey = Forms::GetFormKey(ObjRef);
      if (ObjRef == nullptr || objKey == 0) {
        // Object no longer exists - don't load it's content
        logger::info("\tDiscarding Empty Form");
        for (int n = 0; n < count2; n++) {
          std::string key;
          S value;
          ss >> key;
          ss >> value;
        }
        continue;
      }
    }

    // Valid object or global key
    Data[objKey].reserve(count2);
    for (int n = 0; n < count2; n++) {
      // Key
      std::string key;
      ss >> key;
      DecodeValue(key);
      // Value
      S value;
      ss >> value;
      DecodeValue(value);
      // Save to data store
      Data[objKey][key] = value;
    }
  }
  Data.shrink_to_fit();
  if (count != static_cast<int>(Data.size())) {
    int size = static_cast<int>(Data.size());
    logger::info("\tData Shrink: {} -> {}", count, size);
  }
  s_dataLock.Leave();
}

template <typename T, typename S> void Lists<T, S>::LoadStream(std::stringstream &ss) {
  int count;
  ss >> count;
  if (count < 1)
    return;
  s_dataLock.Enter();
  Data.reserve(count);
  for (int i = 0; i < count; ++i) {
    UInt64 objKey;
    ss >> objKey;
    int count2;
    ss >> count2;

    // Check if objKey still exists and prevent it falling into global 0 key
    if (objKey != 0) {
      RE::TESForm *ObjRef = Forms::ResolveFormKey(objKey);
      objKey = Forms::GetFormKey(ObjRef);
      if (ObjRef == nullptr || objKey == 0) {
        // Object no longer exists - don't load it's content
        logger::info("\tDiscarding Empty Form");
        for (int n = 0; n < count2; ++n) {
          std::string key;
          int count3;
          ss >> key;
          ss >> count3;
          for (int k = 0; k < count3; ++k) {
            S value;
            ss >> value;
          }
        }
        continue;
      }
    }

    // Valid object or global key
    Data[objKey].reserve(count2);
    for (int n = 0; n < count2; ++n) {
      // Key
      std::string key;
      ss >> key;
      DecodeValue(key);
      boost::to_lower(key);
      // Vector
      int count3;
      ss >> count3;
      Data[objKey][key].reserve(count3);
      for (int k = 0; k < count3; ++k) {
        S value;
        ss >> value;
        DecodeValue(value);
        Data[objKey][key].push_back(value);
      }
    }
  }
  Data.shrink_to_fit();
  if (count != static_cast<int>(Data.size())) {
    int size = static_cast<int>(Data.size());
    logger::info("\t--Data Shrink: {} -> {}", count, size);
  }
  s_dataLock.Leave();
}

template <typename T, typename S> void Values<T, S>::SaveStream(std::stringstream &ss) {
  s_dataLock.Enter();
  if (Data.size() < 1) {
    s_dataLock.Leave();
    return;
  }
  ss << (int)Data.size();
  for (typename Map::iterator i = Data.begin(); i != Data.end(); ++i) {
    int size = static_cast<int>(i->second.size());
    if (size > 0) {
      ss << ' ' << i->first;
      ss << ' ' << size;
      for (typename Obj::iterator n = i->second.begin(); n != i->second.end(); ++n) {
        // Key
        std::string key = n->first;
        EncodeValue(key);
        ss << ' ' << key;
        // Value
        S val = n->second;
        EncodeValue(val);
        ss << ' ' << val;
      }
    } else {
      logger::info("\t Discarding empty: {}", i->first);
    }
  }
  s_dataLock.Leave();
}

template <typename T, typename S> void Lists<T, S>::SaveStream(std::stringstream &ss) {
  s_dataLock.Enter();
  if (Data.size() < 1) {
    s_dataLock.Leave();
    return;
  }
  ss << (int)Data.size();
  for (typename Map::iterator i = Data.begin(); i != Data.end(); ++i) {
    int size = static_cast<int>(i->second.size());
    if (size > 0) {
      ss << ' ' << i->first;
      ss << ' ' << size;
      for (typename Obj::iterator n = i->second.begin(); n != i->second.end(); ++n) {
        // Key
        std::string key = n->first;
        EncodeValue(key);
        ss << ' ' << key;
        // Vector
        ss << ' ' << (int)n->second.size();
        for (typename List::iterator itr = n->second.begin(); itr != n->second.end(); ++itr) {
          S value = *itr;
          EncodeValue(value);
          ss << ' ' << value;
        }
      }
    } else {
      logger::info("\t Discarding empty: {}", i->first);
    }
  }
  s_dataLock.Leave();
}

template <typename T, typename S> void Values<T, S>::Revert() {
  s_dataLock.Enter();
  Data.clear();
  s_dataLock.Leave();
}

template <typename T, typename S> void Lists<T, S>::Revert() {
  s_dataLock.Enter();
  Data.clear();
  s_dataLock.Leave();
}

template class Values<SInt32, SInt32>;
template class Values<float, float>;
template class Values<RE::BSFixedString, std::string>;
template class Values<RE::TESForm *, UInt32>;
template class Lists<SInt32, SInt32>;
template class Lists<float, float>;
template class Lists<RE::BSFixedString, std::string>;
template class Lists<RE::TESForm *, UInt32>;

} // namespace Data