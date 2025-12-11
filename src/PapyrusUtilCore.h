#pragma once

/**
 * PapyrusUtil Core Library
 *
 * This header provides the core implementation functions that are shared between:
 * - Papyrus script bindings (PapyrusUtil.cpp, MiscUtil.cpp, ActorUtil.cpp)
 * - Native C++ API (PapyrusUtilAPI.cpp)
 *
 * All functions use standard C++ types (std::vector, std::string) internally.
 * Wrappers are responsible for converting to/from their specific types
 * (VMArray for Papyrus, C arrays for the native API).
 */

#include <cstdint>
#include <string>
#include <vector>

// Forward declarations for game types
namespace RE {
class Actor;
class TESPackage;
class TESRace;
} // namespace RE

namespace PapyrusUtilCore {

// =========================================================================
// String Functions
// =========================================================================

/**
 * Split a string by delimiter.
 * @param str The string to split
 * @param delimiter The delimiter to split by
 * @return Vector of split strings (trimmed)
 */
std::vector<std::string> StringSplit(const std::string &str, const std::string &delimiter);

/**
 * Join strings with a delimiter.
 * @param strings Vector of strings to join
 * @param delimiter The delimiter to join with
 * @return Joined string
 */
std::string StringJoin(const std::vector<std::string> &strings, const std::string &delimiter);

// =========================================================================
// Generic Array Functions (templated)
// =========================================================================

/**
 * Sort an array in place.
 * @param arr Vector to sort
 * @param descending If true, sort in descending order
 */
template <typename T> void SortArray(std::vector<T> &arr, bool descending);

/**
 * Count occurrences of a value.
 * @param arr Vector to search
 * @param value Value to count
 * @return Number of occurrences
 * @note For float types, uses exact equality comparison. This works well for
 *       user-provided discrete values but may yield unexpected results for
 *       values computed via arithmetic due to floating-point precision errors.
 */
template <typename T> uint32_t CountValue(const std::vector<T> &arr, const T &value);

/**
 * Remove all occurrences of a value.
 * @param arr Source vector
 * @param value Value to remove
 * @return New vector with value removed
 */
template <typename T> std::vector<T> RemoveValue(const std::vector<T> &arr, const T &value);

/**
 * Remove duplicate values.
 * @param arr Source vector
 * @return New vector with duplicates removed
 */
template <typename T> std::vector<T> RemoveDuplicates(const std::vector<T> &arr);

/**
 * Slice an array.
 * @param arr Source vector
 * @param startIdx Starting index (inclusive)
 * @param endIdx Ending index (inclusive), -1 or >= size means end
 * @return Sliced vector
 */
template <typename T> std::vector<T> SliceArray(const std::vector<T> &arr, uint32_t startIdx, int32_t endIdx);

/**
 * Merge two arrays.
 * @param arr1 First vector
 * @param arr2 Second vector
 * @param removeDupe If true, remove duplicates from result
 * @return Merged vector
 */
template <typename T> std::vector<T> MergeArrays(const std::vector<T> &arr1, const std::vector<T> &arr2, bool removeDupe);

/**
 * Sum all values in an array.
 * @param arr Vector to sum
 * @return Sum of all values
 */
template <typename T> T AddValues(const std::vector<T> &arr);

// =========================================================================
// String Array Functions (case-insensitive)
// =========================================================================

/**
 * Sort strings in place.
 * @param arr Vector of strings to sort
 * @param descending If true, sort in descending order
 */
void SortStringArray(std::vector<std::string> &arr, bool descending);

/**
 * Count occurrences of a string (case-insensitive).
 * @param arr Vector to search
 * @param value String to count
 * @return Number of occurrences
 */
uint32_t CountString(const std::vector<std::string> &arr, const std::string &value);

/**
 * Remove all occurrences of a string (case-insensitive).
 * @param arr Source vector
 * @param value String to remove
 * @return New vector with value removed
 */
std::vector<std::string> RemoveString(const std::vector<std::string> &arr, const std::string &value);

/**
 * Remove duplicate strings (case-insensitive).
 * @param arr Source vector
 * @return New vector with duplicates removed
 */
std::vector<std::string> RemoveDuplicateStrings(const std::vector<std::string> &arr);

// =========================================================================
// Math Utility Functions
// =========================================================================

/**
 * Clamp an integer to a range.
 */
int32_t ClampInt(int32_t value, int32_t min, int32_t max);

/**
 * Clamp a float to a range.
 */
float ClampFloat(float value, float min, float max);

/**
 * Wrap an integer value at boundaries.
 */
int32_t WrapInt(int32_t value, int32_t end, int32_t start);

/**
 * Wrap a float value at boundaries.
 */
float WrapFloat(float value, float end, float start);

// =========================================================================
// MiscUtil Functions
// =========================================================================

/**
 * Print text to the game console.
 */
void PrintConsole(const std::string &text);

/**
 * Get a race's editor ID by form.
 */
std::string GetRaceEditorID(RE::TESRace *race);

/**
 * Get an actor's race editor ID.
 */
std::string GetActorRaceEditorID(RE::Actor *actor);

// =========================================================================
// ActorUtil Functions
// =========================================================================

/**
 * Add a package to an actor.
 * @return true if successful
 */
bool AddPackageToActor(RE::Actor *actor, RE::TESPackage *package, uint32_t priority, uint32_t flags);

/**
 * Remove a package from an actor.
 * @return true if successful
 */
bool RemovePackageFromActor(RE::Actor *actor, RE::TESPackage *package);

/**
 * Count packages added to an actor through PapyrusUtil.
 */
uint32_t CountActorPackages(RE::Actor *actor);

/**
 * Clear all packages from an actor added through PapyrusUtil.
 * @return Number of packages cleared
 */
uint32_t ClearActorPackages(RE::Actor *actor);

// =========================================================================
// Explicit Template Instantiation Declarations
// =========================================================================

// These are explicitly instantiated in PapyrusUtilCore.cpp for common types
extern template void SortArray<int32_t>(std::vector<int32_t> &, bool);
extern template void SortArray<float>(std::vector<float> &, bool);

extern template uint32_t CountValue<int32_t>(const std::vector<int32_t> &, const int32_t &);
extern template uint32_t CountValue<float>(const std::vector<float> &, const float &);

extern template std::vector<int32_t> RemoveValue<int32_t>(const std::vector<int32_t> &, const int32_t &);
extern template std::vector<float> RemoveValue<float>(const std::vector<float> &, const float &);

extern template std::vector<int32_t> RemoveDuplicates<int32_t>(const std::vector<int32_t> &);
extern template std::vector<float> RemoveDuplicates<float>(const std::vector<float> &);

extern template std::vector<int32_t> SliceArray<int32_t>(const std::vector<int32_t> &, uint32_t, int32_t);
extern template std::vector<float> SliceArray<float>(const std::vector<float> &, uint32_t, int32_t);
extern template std::vector<std::string> SliceArray<std::string>(const std::vector<std::string> &, uint32_t, int32_t);

extern template std::vector<int32_t> MergeArrays<int32_t>(const std::vector<int32_t> &, const std::vector<int32_t> &, bool);
extern template std::vector<float> MergeArrays<float>(const std::vector<float> &, const std::vector<float> &, bool);

extern template int32_t AddValues<int32_t>(const std::vector<int32_t> &);
extern template float AddValues<float>(const std::vector<float> &);

} // namespace PapyrusUtilCore
