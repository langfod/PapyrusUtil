#include "JsonUtil.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <string>

// CommonLibSSE-NG types provided via PCH.h

#include "Data.h"
#include "External.h"

namespace JsonUtil {
using namespace External;

// Sanitize filename to prevent directory traversal attacks
// Returns empty string if the path is invalid/malicious
inline std::string SanitizePath(const std::string &input) {
  if (input.empty())
    return "";

  // Check for directory traversal sequences
  if (input.find("..") != std::string::npos) {
    logger::warn("Path traversal attempt blocked: {}", input);
    return "";
  }

  // Check for absolute paths (Windows drive letters or UNC paths)
  if (input.length() >= 2 && input[1] == ':') {
    logger::warn("Absolute path blocked: {}", input);
    return "";
  }
  if (input.length() >= 2 && input[0] == '\\' && input[1] == '\\') {
    logger::warn("UNC path blocked: {}", input);
    return "";
  }
  if (input[0] == '/' || input[0] == '\\') {
    logger::warn("Absolute path blocked: {}", input);
    return "";
  }

  return input;
}

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

std::string Filename(std::string name) {
  if (name.empty())
    name = "noname.json";
  else {
    // Sanitize to prevent directory traversal
    std::string sanitized = SanitizePath(name);
    if (sanitized.empty())
      return ""; // Invalid path
    name = sanitized;
    // Use proper extension check (case-insensitive) instead of substring search
    if (!boost::iends_with(name, ".json"))
      name += ".json";
  }
  return "Data\\SKSE\\Plugins\\StorageUtilData\\" + name;
}

bool SaveJson(StaticFunctionTag *base, BSFixedString name, bool styled) {
  ExternalFile *File = GetFile(name.data());
  if (!File)
    return false;
  else {
    bool output = File->SaveFile(styled);
    External::UnloadFile(name.data());
    return output;
  }
}
bool LoadJson(StaticFunctionTag *base, BSFixedString name) {
  ExternalFile *File = GetFile(name.data());
  if (!File)
    return false;
  else
    return File->LoadFile();
}

void ClearAll(StaticFunctionTag *base, BSFixedString name) {
  ExternalFile *File = GetFile(name.data());
  if (File)
    File->ClearAll();
}

bool UnloadFile(StaticFunctionTag *base, BSFixedString name, bool savechanges, bool minify) {
  bool output = External::RevertFile(name.data(), savechanges, minify);
  if (output)
    External::UnloadFile(name.data());
  return output;
}
bool IsPendingSave(StaticFunctionTag *base, BSFixedString name) { return External::ChangesPending(name.data()); }

bool IsGood(StaticFunctionTag *base, BSFixedString name) {
  ExternalFile *File = GetFile(name.data());
  return File && File->IsGood();
}

BSFixedString GetErrors(StaticFunctionTag *base, BSFixedString name) {
  ExternalFile *File = GetFile(name.data());
  if (File)
    return BSFixedString(File->GetErrors().c_str());
  else
    return BSFixedString("");
}

void ClearPath(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  //_MESSAGE("ClearPath(%s, %s)", name.data(), path.data());
  if (FileExists(name.data())) {
    ExternalFile *File = GetFile(name.data());
    if (File && IsValidKey(path))
      File->ClearPath(path.data());
  }
}

void ClearPathIndex(StaticFunctionTag *base, BSFixedString name, BSFixedString path, SInt32 idx) {
  //_MESSAGE("ClearPath(%s, %s)", name.data(), path.data());
  if (FileExists(name.data())) {
    ExternalFile *File = GetFile(name.data());
    if (File && IsValidKey(path))
      File->ClearPathIndex(path.data(), idx);
  }
}

template <typename T> void SetPathValue(StaticFunctionTag *base, BSFixedString name, BSFixedString path, T value) {
  ExternalFile *File = GetFile(name.data());
  if (File && IsValidKey(path))
    File->SetPathValue<T>(path.data(), MakeValue<T>(value));
}
/*template <> void SetPathValue(StaticFunctionTag* base, BSFixedString name, BSFixedString path, BSFixedString value) {
        ExternalFile* File = GetFile(name.data());
        if (File && IsValidKey(path)) {
                File->SetPathValue(std::string(path.data()), std::string(value.data()));
        }
}*/

template <typename T> T GetPathValue(StaticFunctionTag *base, BSFixedString name, BSFixedString path, T missing) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(path))
    return missing;
  else
    return File->GetPathValue<T>(path.data(), missing);
}

template <typename T> void SetPathArray(StaticFunctionTag *base, BSFixedString name, BSFixedString path, VMArray<T> arr, bool append) {
  ExternalFile *File = GetFile(name.data());
  if (File && IsValidKey(path))
    File->SetPathArray<T>(path.data(), arr, append);
}

bool SetRawPathValue(StaticFunctionTag *base, BSFixedString name, BSFixedString path, BSFixedString raw) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(path))
    return false;
  else
    return File->SetRawPathValue(path.data(), raw.data());
}

/*template <> BSFixedString GetPathValue(StaticFunctionTag* base, BSFixedString name, BSFixedString path, BSFixedString missing) {
        ExternalFile* File = GetFile(name.data());
        if (!File || !IsValidKey(path)) return missing;
        std::string value = File->GetPathValue(path.data(), std::string(missing.data()));
        return BSFixedString(value.c_str());
}*/

template <typename T> VMResultArray<T> PathElements(StaticFunctionTag *base, BSFixedString name, BSFixedString path, T invalidType) {
  ExternalFile *File = GetFile(name.data());
  if (File && IsValidKey(path))
    return File->PathElements<T>(path.data(), invalidType);
  VMResultArray<T> arr;
  return arr;
}
VMResultArray<BSFixedString> PathMembers(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  ExternalFile *File = GetFile(name.data());
  if (File && IsValidKey(path))
    return File->PathMembers(path.data());
  VMResultArray<BSFixedString> arr;
  return arr;
}
SInt32 PathCount(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  ExternalFile *File = GetFile(name.data());
  return (File && IsValidKey(path)) ? File->PathCount(path.data()) : -1;
}
bool CanResolve(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  ExternalFile *File = GetFile(name.data());
  return File && IsValidKey(path) && File->CanResolve(path.data());
}
bool IsObject(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  ExternalFile *File = GetFile(name.data());
  return File && IsValidKey(path) && File->IsObject(path.data());
}
bool IsArray(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  ExternalFile *File = GetFile(name.data());
  return File && IsValidKey(path) && File->IsArray(path.data());
}
bool IsString(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  ExternalFile *File = GetFile(name.data());
  return File && IsValidKey(path) && File->IsString(path.data());
}
bool IsNumber(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  ExternalFile *File = GetFile(name.data());
  return File && IsValidKey(path) && File->IsNumber(path.data());
}
bool IsBool(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  ExternalFile *File = GetFile(name.data());
  return File && IsValidKey(path) && File->IsBool(path.data());
}
bool IsForm(StaticFunctionTag *base, BSFixedString name, BSFixedString path) {
  ExternalFile *File = GetFile(name.data());
  return File && IsValidKey(path) && File->IsForm(path.data());
}

template <typename T> SInt32 FindPathElement(StaticFunctionTag *base, BSFixedString name, BSFixedString path, T value) {
  ExternalFile *File = GetFile(name.data());
  return (File && IsValidKey(path)) ? File->FindPathElement<T>(path.data(), MakeValue<T>(value)) : -1;
}

template <typename T> T SetValue2(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T value) {
  ExternalFile *File = GetFile(name.data());
  if (!File || IsEmpty(key))
    return T{};
  else
    File->SetValue(Type<T>(), key.data(), MakeValue<T>(value));
  return value;
}

template <typename T> T GetValue2(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T missing) {
  ExternalFile *File = GetFile(name.data());
  if (!File || IsEmpty(key))
    return T{};
  else
    return ParseValue<T>(File->GetValue(Type<T>(), key.data(), MakeValue<T>(missing)));
}

template <typename T> T SetValue(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T value) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return T{};
  else
    File->SetValue(Type<T>(), key.data(), MakeValue<T>(value));
  return value;
}

template <typename T> T GetValue(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T missing) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return T{};
  else
    return ParseValue<T>(File->GetValue(Type<T>(), key.data(), MakeValue<T>(missing)));
}

template <typename T> T AdjustValue(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T value) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return T{};
  else
    return ParseValue<T>(File->AdjustValue(Type<T>(), key.data(), MakeValue<T>(value)));
}

template <typename T> bool UnsetValue(StaticFunctionTag *base, BSFixedString name, BSFixedString key) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->UnsetValue(Type<T>(), key.data());
}

template <typename T> bool HasValue(StaticFunctionTag *base, BSFixedString name, BSFixedString key) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->HasValue(Type<T>(), key.data());
}

template <typename T> SInt32 ListAdd(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T value, bool allowDuplicate) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return -1;
  else
    return File->ListAdd(List<T>(), key.data(), MakeValue<T>(value), allowDuplicate);
}

template <typename T> T ListGet(StaticFunctionTag *base, BSFixedString name, BSFixedString key, UInt32 index) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return T{};
  else
    return ParseValue<T>(File->ListGet(List<T>(), key.data(), index));
}

template <typename T> T ListSet(StaticFunctionTag *base, BSFixedString name, BSFixedString key, UInt32 index, T value) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return T{};
  else
    return ParseValue<T>(File->ListSet(List<T>(), key.data(), index, MakeValue<T>(value)));
}

template <typename T> T ListAdjust(StaticFunctionTag *base, BSFixedString name, BSFixedString key, UInt32 index, T adjustBy) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return T{};
  else
    return ParseValue<T>(File->ListAdjust(key.data(), index, adjustBy));
}

template <typename T> UInt32 ListRemove(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T value, bool allInstances) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return 0;
  else
    return File->ListRemove(List<T>(), key.data(), MakeValue<T>(value), allInstances);
}

template <typename T> bool ListRemoveAt(StaticFunctionTag *base, BSFixedString name, BSFixedString key, UInt32 index) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->ListRemoveAt(List<T>(), key.data(), index);
}

template <typename T> bool ListInsertAt(StaticFunctionTag *base, BSFixedString name, BSFixedString key, UInt32 index, T value) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->ListInsertAt(List<T>(), key.data(), index, MakeValue<T>(value));
}

template <typename T> UInt32 ListClear(StaticFunctionTag *base, BSFixedString name, BSFixedString key) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return 0;
  else
    return File->ListClear(List<T>(), key.data());
}

template <typename T> UInt32 ListCount(StaticFunctionTag *base, BSFixedString name, BSFixedString key) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return 0;
  return File->ListCount(List<T>(), key.data());
}

template <typename T> UInt32 ListCountValue(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T value, bool exclude) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return 0;
  return File->ListCountValue(List<T>(), key.data(), MakeValue<T>(value), exclude);
}

template <typename T> SInt32 ListFind(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T value) {
  ExternalFile *File = GetFile(name.data());

  if (!File) {
    logger::info("Invalid File: {}", name.data());
    return -1;
  } else if (!IsValidKey(key)) {
    logger::info("Invalid Key: {}", key.data());
    return -1;
  }
  // if (!File || !IsValidKey(key)) return -1;
  else
    return File->ListFind(List<T>(), key.data(), MakeValue<T>(value));
}

template <typename T> bool ListHas(StaticFunctionTag *base, BSFixedString name, BSFixedString key, T value) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return false;
  else
    return File->ListFind(List<T>(), key.data(), MakeValue<T>(value)) != -1;
}

template <typename T> SInt32 ListResize(StaticFunctionTag *base, BSFixedString name, BSFixedString key, UInt32 length, T filler) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key) || length > 500)
    return 0;
  else
    return File->ListResize(List<T>(), key.data(), length, MakeValue<T>(filler));
}

template <typename T> void ListSlice(StaticFunctionTag *base, BSFixedString name, BSFixedString key, VMArray<T> Output, UInt32 startIndex) {
  ExternalFile *File = GetFile(name.data());
  if (File && Output.size() > 0 && IsValidKey(key))
    File->ListSlice<T>(key.data(), Output, startIndex);
}

template <typename T> bool ListCopy(StaticFunctionTag *base, BSFixedString name, BSFixedString key, VMArray<T> Input) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key) || Input.size() < 1)
    return false;
  else
    return File->ListCopy<T>(key.data(), Input);
}
template <typename T> VMResultArray<T> ToArray(StaticFunctionTag *base, BSFixedString name, BSFixedString key) {
  ExternalFile *File = GetFile(name.data());
  VMResultArray<T> arr;
  if (File && IsValidKey(key))
    arr = File->ToArray<T>(key.data());
  return arr;
}

template <typename T> UInt32 CountValuePrefix(StaticFunctionTag *base, BSFixedString name, BSFixedString prefix) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(prefix))
    return 0;
  else
    return File->CountPrefix(Type<T>(), prefix.data());
}

template <typename T> UInt32 CountListPrefix(StaticFunctionTag *base, BSFixedString name, BSFixedString prefix) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(prefix))
    return 0;
  else
    return File->CountPrefix(List<T>(), prefix.data());
}

UInt32 CountAllPrefix(StaticFunctionTag *base, BSFixedString name, BSFixedString prefix) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(prefix))
    return 0;
  int count = 0;
  count += File->CountPrefix(Type<SInt32>(), prefix.data());
  count += File->CountPrefix(Type<float>(), prefix.data());
  count += File->CountPrefix(Type<BSFixedString>(), prefix.data());
  count += File->CountPrefix(Type<TESForm *>(), prefix.data());
  count += File->CountPrefix(List<SInt32>(), prefix.data());
  count += File->CountPrefix(List<float>(), prefix.data());
  count += File->CountPrefix(List<BSFixedString>(), prefix.data());
  count += File->CountPrefix(List<TESForm *>(), prefix.data());
  return count;
}

template <typename T> T ListRandom(StaticFunctionTag *base, BSFixedString name, BSFixedString key) {
  ExternalFile *File = GetFile(name.data());
  if (!File || !IsValidKey(key))
    return T{};
  else
    return ParseValue<T>(File->ListRandom(List<T>(), key.data()));
}

/*template <typename T>
UInt32 ClearValuePrefix(StaticFunctionTag* base, BSFixedString name, BSFixedString prefix) {
        ExternalFile* File = GetFile(name.data());
        if (!File || !IsValidKey(prefix)) return 0;
        else return File->ClearPrefix(Type<T>(), prefix.data());
}

template <typename T>
UInt32 ClearListPrefix(StaticFunctionTag* base, BSFixedString name, BSFixedString prefix) {
        ExternalFile* File = GetFile(name.data());
        if (!File || !IsValidKey(prefix)) return 0;
        else return File->ClearPrefix(List<T>(), prefix.data());
}

UInt32 ClearAllPrefix(StaticFunctionTag* base, BSFixedString name, BSFixedString prefix) {
        ExternalFile* File = GetFile(name.data());
        if (!File || !IsValidKey(prefix)) return 0;
        int count = 0;
        count += File->ClearPrefix(Type<SInt32>(), prefix.data());
        count += File->ClearPrefix(Type<float>(), prefix.data());
        count += File->ClearPrefix(Type<BSFixedString>(), prefix.data());
        count += File->ClearPrefix(Type<TESForm*>(), prefix.data());
        count += File->ClearPrefix(List<SInt32>(), prefix.data());
        count += File->ClearPrefix(List<float>(), prefix.data());
        count += File->ClearPrefix(List<BSFixedString>(), prefix.data());
        count += File->ClearPrefix(List<TESForm*>(), prefix.data());
        return count;
}*/

namespace fs = boost::filesystem;

VMResultArray<BSFixedString> JsonFilesInFolder(StaticFunctionTag *base, BSFixedString dirpath) {
  VMResultArray<BSFixedString> arr;
  if (dirpath.data() && dirpath.data()[0] != '\0') {
    // Sanitize path to prevent directory traversal
    std::string sanitized = SanitizePath(dirpath.data());
    if (sanitized.empty())
      return arr; // Invalid path, return empty

    std::string dirstr = "Data\\SKSE\\Plugins\\StorageUtilData\\";
    dirstr.append(sanitized);

    try {
      fs::path someDir(dirstr);
      fs::directory_iterator end_iter;
      if (fs::exists(someDir) && fs::is_directory(someDir)) {
        for (fs::directory_iterator dir_iter(someDir); dir_iter != end_iter; ++dir_iter) {
          if (fs::is_regular_file(dir_iter->status())) {
            fs::path filepath = dir_iter->path();
            std::string file = filepath.filename().generic_string();
            // std::string ext = filepath.extension().generic_string();
            //_MESSAGE("file: %s ext: %s", file.c_str(), ext.c_str());
            if (boost::iequals(filepath.extension().generic_string(), ".json"))
              arr.push_back(BSFixedString(file.c_str()));
          }
        }
      }
    } catch (const fs::filesystem_error &ex) {
      logger::error("JsonFilesInFolder filesystem error: {}", ex.what());
    } catch (const std::exception &ex) {
      logger::error("JsonFilesInFolder error: {}", ex.what());
    }
  }
  return arr;
}

} // namespace JsonUtil

void JsonUtil::RegisterFuncs(RE::BSScript::IVirtualMachine *vm) {

  // File manipulation
  vm->RegisterFunction("Save"sv, "JsonUtil"sv, SaveJson);
  vm->RegisterFunction("Load"sv, "JsonUtil"sv, LoadJson);
  vm->RegisterFunction("ClearAll"sv, "JsonUtil"sv, ClearAll);
  vm->RegisterFunction("Unload"sv, "JsonUtil"sv, UnloadFile);
  vm->RegisterFunction("IsPendingSave"sv, "JsonUtil"sv, IsPendingSave);
  vm->RegisterFunction("IsGood"sv, "JsonUtil"sv, IsGood);
  vm->RegisterFunction("GetErrors"sv, "JsonUtil"sv, GetErrors);
  vm->RegisterFunction("ClearPath"sv, "JsonUtil"sv, ClearPath);
  vm->RegisterFunction("ClearPathIndex"sv, "JsonUtil"sv, ClearPathIndex);

  // Path values
  vm->RegisterFunction("SetPathIntValue"sv, "JsonUtil"sv, SetPathValue<SInt32>);
  vm->RegisterFunction("SetPathFloatValue"sv, "JsonUtil"sv, SetPathValue<float>);
  vm->RegisterFunction("SetPathStringValue"sv, "JsonUtil"sv, SetPathValue<BSFixedString>);
  vm->RegisterFunction("SetPathFormValue"sv, "JsonUtil"sv, SetPathValue<TESForm *>);

  vm->RegisterFunction("GetPathIntValue"sv, "JsonUtil"sv, GetPathValue<SInt32>);
  vm->RegisterFunction("GetPathFloatValue"sv, "JsonUtil"sv, GetPathValue<float>);
  vm->RegisterFunction("GetPathStringValue"sv, "JsonUtil"sv, GetPathValue<BSFixedString>);
  vm->RegisterFunction("GetPathFormValue"sv, "JsonUtil"sv, GetPathValue<TESForm *>);

  vm->RegisterFunction("PathIntElements"sv, "JsonUtil"sv, PathElements<SInt32>);
  vm->RegisterFunction("PathFloatElements"sv, "JsonUtil"sv, PathElements<float>);
  vm->RegisterFunction("PathStringElements"sv, "JsonUtil"sv, PathElements<BSFixedString>);
  vm->RegisterFunction("PathFormElements"sv, "JsonUtil"sv, PathElements<TESForm *>);

  vm->RegisterFunction("SetPathIntArray"sv, "JsonUtil"sv, SetPathArray<SInt32>);
  vm->RegisterFunction("SetPathFloatArray"sv, "JsonUtil"sv, SetPathArray<float>);
  vm->RegisterFunction("SetPathStringArray"sv, "JsonUtil"sv, SetPathArray<BSFixedString>);
  vm->RegisterFunction("SetPathFormArray"sv, "JsonUtil"sv, SetPathArray<TESForm *>);

  vm->RegisterFunction("FindPathIntElement"sv, "JsonUtil"sv, FindPathElement<SInt32>);
  vm->RegisterFunction("FindPathFloatElement"sv, "JsonUtil"sv, FindPathElement<float>);
  vm->RegisterFunction("FindPathStringElement"sv, "JsonUtil"sv, FindPathElement<BSFixedString>);
  vm->RegisterFunction("FindPathFormElement"sv, "JsonUtil"sv, FindPathElement<TESForm *>);

  vm->RegisterFunction("PathMembers"sv, "JsonUtil"sv, PathMembers);
  vm->RegisterFunction("PathCount"sv, "JsonUtil"sv, PathCount);
  vm->RegisterFunction("CanResolvePath"sv, "JsonUtil"sv, CanResolve);
  vm->RegisterFunction("IsPathObject"sv, "JsonUtil"sv, IsObject);
  vm->RegisterFunction("IsPathArray"sv, "JsonUtil"sv, IsArray);
  vm->RegisterFunction("IsPathString"sv, "JsonUtil"sv, IsString);
  vm->RegisterFunction("IsPathNumber"sv, "JsonUtil"sv, IsNumber);
  vm->RegisterFunction("IsPathBool"sv, "JsonUtil"sv, IsBool);
  vm->RegisterFunction("IsPathForm"sv, "JsonUtil"sv, IsForm);
  vm->RegisterFunction("SetRawPathValue"sv, "JsonUtil"sv, SetRawPathValue);

  // Global values
  vm->RegisterFunction("SetIntValue"sv, "JsonUtil"sv, SetValue<SInt32>);
  vm->RegisterFunction("SetFloatValue"sv, "JsonUtil"sv, SetValue<float>);
  vm->RegisterFunction("SetStringValue"sv, "JsonUtil"sv, SetValue<BSFixedString>);
  vm->RegisterFunction("SetFormValue"sv, "JsonUtil"sv, SetValue<TESForm *>);

  vm->RegisterFunction("GetIntValue"sv, "JsonUtil"sv, GetValue<SInt32>);
  vm->RegisterFunction("GetFloatValue"sv, "JsonUtil"sv, GetValue<float>);
  vm->RegisterFunction("GetStringValue"sv, "JsonUtil"sv, GetValue<BSFixedString>);
  vm->RegisterFunction("GetFormValue"sv, "JsonUtil"sv, GetValue<TESForm *>);

  vm->RegisterFunction("AdjustIntValue"sv, "JsonUtil"sv, AdjustValue<SInt32>);
  vm->RegisterFunction("AdjustFloatValue"sv, "JsonUtil"sv, AdjustValue<float>);

  vm->RegisterFunction("UnsetIntValue"sv, "JsonUtil"sv, UnsetValue<SInt32>);
  vm->RegisterFunction("UnsetFloatValue"sv, "JsonUtil"sv, UnsetValue<float>);
  vm->RegisterFunction("UnsetStringValue"sv, "JsonUtil"sv, UnsetValue<BSFixedString>);
  vm->RegisterFunction("UnsetFormValue"sv, "JsonUtil"sv, UnsetValue<TESForm *>);

  vm->RegisterFunction("HasIntValue"sv, "JsonUtil"sv, HasValue<SInt32>);
  vm->RegisterFunction("HasFloatValue"sv, "JsonUtil"sv, HasValue<float>);
  vm->RegisterFunction("HasStringValue"sv, "JsonUtil"sv, HasValue<BSFixedString>);
  vm->RegisterFunction("HasFormValue"sv, "JsonUtil"sv, HasValue<TESForm *>);

  // Global lists
  vm->RegisterFunction("IntListAdd"sv, "JsonUtil"sv, ListAdd<SInt32>);
  vm->RegisterFunction("FloatListAdd"sv, "JsonUtil"sv, ListAdd<float>);
  vm->RegisterFunction("StringListAdd"sv, "JsonUtil"sv, ListAdd<BSFixedString>);
  vm->RegisterFunction("FormListAdd"sv, "JsonUtil"sv, ListAdd<TESForm *>);

  vm->RegisterFunction("IntListGet"sv, "JsonUtil"sv, ListGet<SInt32>);
  vm->RegisterFunction("FloatListGet"sv, "JsonUtil"sv, ListGet<float>);
  vm->RegisterFunction("StringListGet"sv, "JsonUtil"sv, ListGet<BSFixedString>);
  vm->RegisterFunction("FormListGet"sv, "JsonUtil"sv, ListGet<TESForm *>);

  vm->RegisterFunction("IntListAdjust"sv, "JsonUtil"sv, ListAdjust<SInt32>);
  vm->RegisterFunction("FloatListAdjust"sv, "JsonUtil"sv, ListAdjust<float>);

  vm->RegisterFunction("IntListSet"sv, "JsonUtil"sv, ListSet<SInt32>);
  vm->RegisterFunction("FloatListSet"sv, "JsonUtil"sv, ListSet<float>);
  vm->RegisterFunction("StringListSet"sv, "JsonUtil"sv, ListSet<BSFixedString>);
  vm->RegisterFunction("FormListSet"sv, "JsonUtil"sv, ListSet<TESForm *>);

  vm->RegisterFunction("IntListRemove"sv, "JsonUtil"sv, ListRemove<SInt32>);
  vm->RegisterFunction("FloatListRemove"sv, "JsonUtil"sv, ListRemove<float>);
  vm->RegisterFunction("StringListRemove"sv, "JsonUtil"sv, ListRemove<BSFixedString>);
  vm->RegisterFunction("FormListRemove"sv, "JsonUtil"sv, ListRemove<TESForm *>);

  vm->RegisterFunction("IntListRemoveAt"sv, "JsonUtil"sv, ListRemoveAt<SInt32>);
  vm->RegisterFunction("FloatListRemoveAt"sv, "JsonUtil"sv, ListRemoveAt<float>);
  vm->RegisterFunction("StringListRemoveAt"sv, "JsonUtil"sv, ListRemoveAt<BSFixedString>);
  vm->RegisterFunction("FormListRemoveAt"sv, "JsonUtil"sv, ListRemoveAt<TESForm *>);

  vm->RegisterFunction("IntListInsertAt"sv, "JsonUtil"sv, ListInsertAt<SInt32>);
  vm->RegisterFunction("FloatListInsertAt"sv, "JsonUtil"sv, ListInsertAt<float>);
  vm->RegisterFunction("StringListInsertAt"sv, "JsonUtil"sv, ListInsertAt<BSFixedString>);
  vm->RegisterFunction("FormListInsertAt"sv, "JsonUtil"sv, ListInsertAt<TESForm *>);

  vm->RegisterFunction("IntListClear"sv, "JsonUtil"sv, ListClear<SInt32>);
  vm->RegisterFunction("FloatListClear"sv, "JsonUtil"sv, ListClear<float>);
  vm->RegisterFunction("StringListClear"sv, "JsonUtil"sv, ListClear<BSFixedString>);
  vm->RegisterFunction("FormListClear"sv, "JsonUtil"sv, ListClear<TESForm *>);

  vm->RegisterFunction("IntListCount"sv, "JsonUtil"sv, ListCount<SInt32>);
  vm->RegisterFunction("FloatListCount"sv, "JsonUtil"sv, ListCount<float>);
  vm->RegisterFunction("StringListCount"sv, "JsonUtil"sv, ListCount<BSFixedString>);
  vm->RegisterFunction("FormListCount"sv, "JsonUtil"sv, ListCount<TESForm *>);

  vm->RegisterFunction("IntListCountValue"sv, "JsonUtil"sv, ListCountValue<SInt32>);
  vm->RegisterFunction("FloatListCountValue"sv, "JsonUtil"sv, ListCountValue<float>);
  vm->RegisterFunction("StringListCountValue"sv, "JsonUtil"sv, ListCountValue<BSFixedString>);
  vm->RegisterFunction("FormListCountValue"sv, "JsonUtil"sv, ListCountValue<TESForm *>);

  vm->RegisterFunction("IntListFind"sv, "JsonUtil"sv, ListFind<SInt32>);
  vm->RegisterFunction("FloatListFind"sv, "JsonUtil"sv, ListFind<float>);
  vm->RegisterFunction("StringListFind"sv, "JsonUtil"sv, ListFind<BSFixedString>);
  vm->RegisterFunction("FormListFind"sv, "JsonUtil"sv, ListFind<TESForm *>);

  vm->RegisterFunction("IntListHas"sv, "JsonUtil"sv, ListHas<SInt32>);
  vm->RegisterFunction("FloatListHas"sv, "JsonUtil"sv, ListHas<float>);
  vm->RegisterFunction("StringListHas"sv, "JsonUtil"sv, ListHas<BSFixedString>);
  vm->RegisterFunction("FormListHas"sv, "JsonUtil"sv, ListHas<TESForm *>);

  vm->RegisterFunction("IntListSlice"sv, "JsonUtil"sv, ListSlice<SInt32>);
  vm->RegisterFunction("FloatListSlice"sv, "JsonUtil"sv, ListSlice<float>);
  vm->RegisterFunction("StringListSlice"sv, "JsonUtil"sv, ListSlice<BSFixedString>);
  vm->RegisterFunction("FormListSlice"sv, "JsonUtil"sv, ListSlice<TESForm *>);

  vm->RegisterFunction("IntListResize"sv, "JsonUtil"sv, ListResize<SInt32>);
  vm->RegisterFunction("FloatListResize"sv, "JsonUtil"sv, ListResize<float>);
  vm->RegisterFunction("StringListResize"sv, "JsonUtil"sv, ListResize<BSFixedString>);
  vm->RegisterFunction("FormListResize"sv, "JsonUtil"sv, ListResize<TESForm *>);

  vm->RegisterFunction("IntListCopy"sv, "JsonUtil"sv, ListCopy<SInt32>);
  vm->RegisterFunction("FloatListCopy"sv, "JsonUtil"sv, ListCopy<float>);
  vm->RegisterFunction("StringListCopy"sv, "JsonUtil"sv, ListCopy<BSFixedString>);
  vm->RegisterFunction("FormListCopy"sv, "JsonUtil"sv, ListCopy<TESForm *>);

  vm->RegisterFunction("IntListToArray"sv, "JsonUtil"sv, ToArray<SInt32>);
  vm->RegisterFunction("FloatListToArray"sv, "JsonUtil"sv, ToArray<float>);
  vm->RegisterFunction("StringListToArray"sv, "JsonUtil"sv, ToArray<BSFixedString>);
  vm->RegisterFunction("FormListToArray"sv, "JsonUtil"sv, ToArray<TESForm *>);

  vm->RegisterFunction("CountIntValuePrefix"sv, "JsonUtil"sv, CountValuePrefix<SInt32>);
  vm->RegisterFunction("CountFloatValuePrefix"sv, "JsonUtil"sv, CountValuePrefix<float>);
  vm->RegisterFunction("CountStringValuePrefix"sv, "JsonUtil"sv, CountValuePrefix<BSFixedString>);
  vm->RegisterFunction("CountFormValuePrefix"sv, "JsonUtil"sv, CountValuePrefix<TESForm *>);

  vm->RegisterFunction("CountIntListPrefix"sv, "JsonUtil"sv, CountListPrefix<SInt32>);
  vm->RegisterFunction("CountFloatListPrefix"sv, "JsonUtil"sv, CountListPrefix<float>);
  vm->RegisterFunction("CountStringListPrefix"sv, "JsonUtil"sv, CountListPrefix<BSFixedString>);
  vm->RegisterFunction("CountFormListPrefix"sv, "JsonUtil"sv, CountListPrefix<TESForm *>);

  vm->RegisterFunction("IntListRandom"sv, "JsonUtil"sv, ListRandom<SInt32>);
  vm->RegisterFunction("FloatListRandom"sv, "JsonUtil"sv, ListRandom<float>);
  vm->RegisterFunction("StringListRandom"sv, "JsonUtil"sv, ListRandom<BSFixedString>);
  vm->RegisterFunction("FormListRandom"sv, "JsonUtil"sv, ListRandom<TESForm *>);

  vm->RegisterFunction("CountAllPrefix"sv, "JsonUtil"sv, CountAllPrefix);
  vm->RegisterFunction("JsonInFolder"sv, "JsonUtil"sv, JsonFilesInFolder);
}
