#pragma once

#include "Forms.h"
#include "PCH.h"

inline SInt32 cast(SInt32 &v) { return v; }
inline float cast(float &v) { return v; }
inline std::string cast(RE::BSFixedString &v) { return v.c_str(); }
inline RE::BSFixedString cast(std::string &v) { return RE::BSFixedString(v.c_str()); }
inline UInt32 cast(RE::TESForm *v) { return v == nullptr ? 0 : v->formID; }
inline RE::TESForm *cast(UInt32 &v) { return v == 0 ? nullptr : RE::TESForm::LookupByID(v); }

namespace Data {
void InitLists();

template <typename S> inline void EncodeValue(S &v) {}
inline void EncodeValue(std::string &v) {
  if (v.empty())
    v += (char)0x1B;
  else
    std::replace(v.begin(), v.end(), (char)' ', (char)0x7);
}

template <typename S> inline void DecodeValue(S &v) {}
inline void DecodeValue(UInt32 &v) { v = Forms::ResolveFormID(v); }
inline void DecodeValue(std::string &v) {
  if (v.size() == 1 && v[0] == (char)0x1B)
    v.clear();
  else
    std::replace(v.begin(), v.end(), (char)0x7, (char)' ');
}

/*
 * Declare classes methods
 */

template <typename T, typename S> class Values {
private:
  ICriticalSection s_dataLock;

  typedef boost::container::flat_map<std::string, S> Obj;
  typedef boost::container::flat_map<UInt64, Obj> Map;
  Map Data;

public:
  void SetValue(UInt64 obj, std::string key, T value);
  T GetValue(UInt64 obj, std::string key, T value);
  T AdjustValue(UInt64 obj, std::string key, T value);
  T PluckValue(UInt64 obj, std::string key, T value);
  bool UnsetValue(UInt64 obj, std::string key);
  bool HasValue(UInt64 obj, std::string key);
  int ClearPrefixKey(std::string prefix);
  int CountPrefixKey(std::string prefix);
  int ClearPrefixKey(UInt64 obj, std::string prefix);
  int CountPrefixKey(UInt64 obj, std::string prefix);

  // Serialization
  void Revert();
  void LoadStream(std::stringstream &ss);
  void SaveStream(std::stringstream &ss);

  // Debug
  inline std::size_t GetObjCount() { return Data.size(); }
  inline std::size_t GetKeyCount(UInt64 obj) { return Data.find(obj) != Data.end() ? Data[obj].size() : 0; }
  RE::TESForm *GetNthObj(UInt32 i);
  const std::string GetNthKey(UInt64 obj, UInt32 i);
  std::vector<RE::TESForm *> GetAllObj();
  std::vector<RE::BSFixedString> GetAllObjKeys(UInt64 obj);
  void RemoveForm(UInt64 &obj);
  int Cleanup();

  // Serialization interface
  [[nodiscard]] std::size_t size() const { return Data.size(); }
  [[nodiscard]] bool empty() const { return Data.empty(); }
};

template <typename T, typename S> class Lists {
private:
  ICriticalSection s_dataLock;

  typedef std::vector<S> List;
  typedef boost::container::flat_map<std::string, List> Obj;
  typedef boost::container::flat_map<UInt64, Obj> Map;
  Map Data;

public:
  int ListAdd(UInt64 obj, std::string key, T value, bool allowDuplicate);
  T ListGet(UInt64 obj, std::string key, UInt32 index);
  T ListSet(UInt64 obj, std::string key, UInt32 index, T value);
  T ListAdjust(UInt64 obj, std::string key, UInt32 index, T value);
  T ListPluck(UInt64 obj, std::string key, UInt32 index, T value);
  T ListShift(UInt64 obj, std::string key);
  T ListPop(UInt64 obj, std::string key);

  int ListRemove(UInt64 obj, std::string key, T value, bool allInstances);
  bool ListInsertAt(UInt64 obj, std::string key, UInt32 index, T value);
  bool ListRemoveAt(UInt64 obj, std::string key, UInt32 index);
  int ListClear(UInt64 obj, std::string key);
  int ListCount(UInt64 obj, std::string key);
  int ListCountValue(UInt64 obj, std::string key, T value, bool exclude);
  int ListFind(UInt64 obj, std::string key, T value);
  bool ListHas(UInt64 obj, std::string key, T value);
  void ListSort(UInt64 obj, std::string key);
  void ListSlice(UInt64 obj, std::string key, std::vector<T> &Output, UInt32 startIndex);
  int ListResize(UInt64 obj, std::string key, UInt32 length, T filler);
  bool ListCopy(UInt64 obj, std::string key, const std::vector<T> &Input);
  std::vector<T> ToArray(UInt64 obj, std::string key);
  std::vector<T> FilterByTypes(UInt64 obj, std::string key, const std::vector<UInt32> &types, bool matching);
  int ClearPrefixKey(std::string prefix);
  int CountPrefixKey(std::string prefix);
  int ClearPrefixKey(UInt64 obj, std::string prefix);
  int CountPrefixKey(UInt64 obj, std::string prefix);

  T ListRandom(UInt64 obj, std::string key);
  std::vector<T> ListRandomArray(UInt64 obj, std::string key, UInt32 count, bool duplicates);

  // Returns pointer to internal storage. Caller must hold s_dataLock.
  List *GetVector(const UInt64 &obj, const std::string &key) {
    typename Map::iterator itr = Data.find(obj);
    if (itr == Data.end())
      return nullptr;
    typename Obj::iterator itr2 = itr->second.find(key);
    if (itr2 == itr->second.end())
      return nullptr;
    return &itr2->second;
  }

  // Serialization
  void Revert();
  void LoadStream(std::stringstream &ss);
  void SaveStream(std::stringstream &ss);

  // Debug
  inline std::size_t GetObjCount() { return Data.size(); }
  inline std::size_t GetKeyCount(UInt64 obj) { return Data.find(obj) != Data.end() ? Data[obj].size() : 0; }
  RE::TESForm *GetNthObj(UInt32 i);
  const std::string GetNthKey(UInt64 obj, UInt32 i);
  std::vector<RE::TESForm *> GetAllObj();
  std::vector<RE::BSFixedString> GetAllObjKeys(UInt64 obj);
  void RemoveForm(UInt64 &obj);
  int Cleanup();

  // Serialization interface
  [[nodiscard]] std::size_t size() const { return Data.size(); }
  [[nodiscard]] bool empty() const { return Data.empty(); }
};

/*
 * Data storage holders
 */
// TODO: Change storage method
typedef Values<RE::TESForm *, std::pair<UInt32, std::string>> formv;
typedef Lists<RE::TESForm *, std::pair<UInt32, std::string>> forml;

typedef Values<SInt32, SInt32> intv;
typedef Values<float, float> flov;
typedef Values<RE::BSFixedString, std::string> strv;
typedef Values<RE::TESForm *, UInt32> forv;

template <typename T, typename S> Values<T, S> *GetValues();
template <> intv *GetValues<SInt32, SInt32>();
template <> flov *GetValues<float, float>();
template <> strv *GetValues<RE::BSFixedString, std::string>();
template <> forv *GetValues<RE::TESForm *, UInt32>();

typedef Lists<SInt32, SInt32> intl;
typedef Lists<float, float> flol;
typedef Lists<RE::BSFixedString, std::string> strl;
typedef Lists<RE::TESForm *, UInt32> forl;

template <typename T, typename S> Lists<T, S> *GetLists();
template <> intl *GetLists<SInt32, SInt32>();
template <> flol *GetLists<float, float>();
template <> strl *GetLists<RE::BSFixedString, std::string>();
template <> forl *GetLists<RE::TESForm *, UInt32>();

// Serialization callbacks (for CommonLibSSE-NG)
void Serialization_Save(SKSE::SerializationInterface *a_intfc);
void Serialization_Load(SKSE::SerializationInterface *a_intfc);
void Serialization_Revert(SKSE::SerializationInterface *a_intfc);
} // namespace Data