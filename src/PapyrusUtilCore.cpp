/**
 * PapyrusUtil Core Library Implementation
 *
 * Shared implementation functions used by both Papyrus bindings and the native C++ API.
 */

#include "PapyrusUtilCore.h"
#include "PCH.h"
#include "PackageData.h"

#include <algorithm>
#include <cmath>
#include <sstream>


#include <boost/algorithm/string.hpp>

namespace PapyrusUtilCore {

// =========================================================================
// String Functions
// =========================================================================

std::vector<std::string> StringSplit(const std::string &str, const std::string &delimiter) {
  std::vector<std::string> result;

  if (str.empty() || delimiter.empty() || boost::iequals(str, delimiter)) {
    return result;
  }

  std::string range = str;
  boost::iter_split(result, range, boost::first_finder(delimiter));

  // Trim each part
  for (auto &part : result) {
    boost::trim(part);
  }

  return result;
}

std::string StringJoin(const std::vector<std::string> &strings, const std::string &delimiter) {
  if (strings.empty()) {
    return "";
  }

  std::stringstream ss;
  for (size_t i = 0; i < strings.size(); ++i) {
    ss << strings[i];
    if (i < strings.size() - 1) {
      ss << delimiter;
    }
  }
  return ss.str();
}

// =========================================================================
// Generic Array Functions (templated)
// =========================================================================

template <typename T> void SortArray(std::vector<T> &arr, bool descending) {
  if (arr.size() < 2)
    return;
  std::sort(arr.begin(), arr.end());
  if (descending) {
    std::reverse(arr.begin(), arr.end());
  }
}

template <typename T> uint32_t CountValue(const std::vector<T> &arr, const T &value) {
  // Note: For float types, uses exact equality comparison. This works well for
  // user-provided discrete values but may yield unexpected results for computed values.
  uint32_t count = 0;
  for (const auto &item : arr) {
    if (item == value) {
      ++count;
    }
  }
  return count;
}

template <typename T> std::vector<T> RemoveValue(const std::vector<T> &arr, const T &value) {
  std::vector<T> result;
  result.reserve(arr.size());
  for (const auto &item : arr) {
    if (item != value) {
      result.push_back(item);
    }
  }
  return result;
}

template <typename T> std::vector<T> RemoveDuplicates(const std::vector<T> &arr) {
  std::vector<T> result;
  result.reserve(arr.size());
  for (const auto &item : arr) {
    if (std::find(result.begin(), result.end(), item) == result.end()) {
      result.push_back(item);
    }
  }
  result.shrink_to_fit();
  return result;
}

template <typename T> std::vector<T> SliceArray(const std::vector<T> &arr, uint32_t startIdx, int32_t endIdx) {
  std::vector<T> result;

  if (arr.empty() || startIdx >= arr.size()) {
    return result;
  }

  // Handle negative or out-of-bounds endIdx
  if (endIdx < 0 || static_cast<uint32_t>(endIdx) >= arr.size()) {
    endIdx = static_cast<int32_t>(arr.size()) - 1;
  }

  if (startIdx > static_cast<uint32_t>(endIdx)) {
    return result;
  }

  uint32_t size = static_cast<uint32_t>(endIdx) - startIdx + 1;
  result.reserve(size);

  for (uint32_t i = startIdx; i <= static_cast<uint32_t>(endIdx); ++i) {
    result.push_back(arr[i]);
  }

  return result;
}

template <typename T> std::vector<T> MergeArrays(const std::vector<T> &arr1, const std::vector<T> &arr2, bool removeDupe) {
  std::vector<T> result;
  result.reserve(arr1.size() + arr2.size());

  for (const auto &item : arr1) {
    if (!removeDupe || std::find(result.begin(), result.end(), item) == result.end()) {
      result.push_back(item);
    }
  }

  for (const auto &item : arr2) {
    if (!removeDupe || std::find(result.begin(), result.end(), item) == result.end()) {
      result.push_back(item);
    }
  }

  if (removeDupe) {
    result.shrink_to_fit();
  }

  return result;
}

template <typename T> T AddValues(const std::vector<T> &arr) {
  T sum = 0;
  for (const auto &item : arr) {
    sum += item;
  }
  return sum;
}

// =========================================================================
// String Array Functions (case-insensitive)
// =========================================================================

void SortStringArray(std::vector<std::string> &arr, bool descending) {
  if (arr.size() < 2)
    return;
  // Case-insensitive sort using boost::ilexicographical_compare
  std::sort(arr.begin(), arr.end(), [](const std::string &a, const std::string &b) { return boost::ilexicographical_compare(a, b); });
  if (descending) {
    std::reverse(arr.begin(), arr.end());
  }
}

uint32_t CountString(const std::vector<std::string> &arr, const std::string &value) {
  uint32_t count = 0;
  for (const auto &item : arr) {
    if (boost::iequals(item, value)) {
      ++count;
    }
  }
  return count;
}

std::vector<std::string> RemoveString(const std::vector<std::string> &arr, const std::string &value) {
  std::vector<std::string> result;
  result.reserve(arr.size());
  for (const auto &item : arr) {
    if (!boost::iequals(item, value)) {
      result.push_back(item);
    }
  }
  return result;
}

std::vector<std::string> RemoveDuplicateStrings(const std::vector<std::string> &arr) {
  std::vector<std::string> result;
  result.reserve(arr.size());
  for (const auto &item : arr) {
    bool found = false;
    for (const auto &existing : result) {
      if (boost::iequals(item, existing)) {
        found = true;
        break;
      }
    }
    if (!found) {
      result.push_back(item);
    }
  }
  result.shrink_to_fit();
  return result;
}

// =========================================================================
// Math Utility Functions
// =========================================================================

int32_t ClampInt(int32_t value, int32_t min, int32_t max) {
  if (value >= max)
    return max;
  if (value <= min)
    return min;
  return value;
}

float ClampFloat(float value, float min, float max) {
  if (value >= max)
    return max;
  if (value <= min)
    return min;
  return value;
}

// Wrap functions: if value exceeds end, return start; if below start, return end.
// Note: Parameters are (value, end, start) for Papyrus API compatibility.
// If start > end, behavior is undefined - callers should ensure start <= end.
int32_t WrapInt(int32_t value, int32_t end, int32_t start) {
  if (value > end)
    return start;
  if (value < start)
    return end;
  return value;
}

float WrapFloat(float value, float end, float start) {
  if (value > end)
    return start;
  if (value < start)
    return end;
  return value;
}

// =========================================================================
// MiscUtil Functions
// =========================================================================

void PrintConsole(const std::string &text) {
  if (text.empty())
    return;

  if (auto console = RE::ConsoleLog::GetSingleton()) {
    console->Print("%s", text.c_str());
  }
}

std::string GetRaceEditorID(RE::TESRace *race) {
  if (!race)
    return "";
  const char *editorID = race->GetFormEditorID();
  return editorID ? editorID : "";
}

std::string GetActorRaceEditorID(RE::Actor *actor) {
  if (!actor)
    return "";
  auto race = actor->GetRace();
  return GetRaceEditorID(race);
}

// =========================================================================
// ActorUtil Functions
// =========================================================================

bool AddPackageToActor(RE::Actor *actor, RE::TESPackage *package, uint32_t priority, uint32_t flags) {
  if (!actor || !package)
    return false;

  if (auto *overrides = PackageData::GetPackages()) {
    overrides->AddPackage(actor, package, priority, flags);
    return true;
  }
  return false;
}

bool RemovePackageFromActor(RE::Actor *actor, RE::TESPackage *package) {
  if (!actor || !package)
    return false;

  if (auto *overrides = PackageData::GetPackages()) {
    return overrides->RemovePackage(actor, package);
  }
  return false;
}

uint32_t CountActorPackages(RE::Actor *actor) {
  if (!actor)
    return 0;
  if (auto *overrides = PackageData::GetPackages()) {
    return overrides->CountPackages(actor);
  }
  return 0;
}

uint32_t ClearActorPackages(RE::Actor *actor) {
  if (!actor)
    return 0;
  if (auto *overrides = PackageData::GetPackages()) {
    return overrides->ClearActor(actor);
  }
  return 0;
}

// =========================================================================
// Explicit Template Instantiations
// =========================================================================

template void SortArray<int32_t>(std::vector<int32_t> &, bool);
template void SortArray<float>(std::vector<float> &, bool);

template uint32_t CountValue<int32_t>(const std::vector<int32_t> &, const int32_t &);
template uint32_t CountValue<float>(const std::vector<float> &, const float &);

template std::vector<int32_t> RemoveValue<int32_t>(const std::vector<int32_t> &, const int32_t &);
template std::vector<float> RemoveValue<float>(const std::vector<float> &, const float &);

template std::vector<int32_t> RemoveDuplicates<int32_t>(const std::vector<int32_t> &);
template std::vector<float> RemoveDuplicates<float>(const std::vector<float> &);

template std::vector<int32_t> SliceArray<int32_t>(const std::vector<int32_t> &, uint32_t, int32_t);
template std::vector<float> SliceArray<float>(const std::vector<float> &, uint32_t, int32_t);
template std::vector<std::string> SliceArray<std::string>(const std::vector<std::string> &, uint32_t, int32_t);

template std::vector<int32_t> MergeArrays<int32_t>(const std::vector<int32_t> &, const std::vector<int32_t> &, bool);
template std::vector<float> MergeArrays<float>(const std::vector<float> &, const std::vector<float> &, bool);

template int32_t AddValues<int32_t>(const std::vector<int32_t> &);
template float AddValues<float>(const std::vector<float> &);

} // namespace PapyrusUtilCore
