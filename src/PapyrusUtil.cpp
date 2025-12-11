#include "PapyrusUtil.h"

#include <algorithm>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "Forms.h"

namespace PapyrusUtil {

template <typename T> VMResultArray<T> CreateArray(StaticFunctionTag *, UInt32 size, T filler) {
  VMResultArray<T> arr;
  arr.resize(size, filler);
  return arr;
}

template <typename T> VMResultArray<T> ResizeArray(StaticFunctionTag *, VMArray<T> arr, UInt32 size, T filler) {
  VMResultArray<T> Output;
  try {
    Output.resize(size, filler);
    UInt32 length = std::min(static_cast<UInt32>(arr.size()), size);
    for (UInt32 idx = 0; idx < length; ++idx) {
      T value = arr[idx];
      Output[idx] = value;
    }
  } catch (std::exception &e) {
    logger::error("Error Exception: {}", e.what());
  }
  return Output;
}

template <> VMResultArray<BSFixedString> ResizeArray(StaticFunctionTag *, VMArray<BSFixedString> arr, UInt32 size, BSFixedString filler) {
  VMResultArray<BSFixedString> Output;
  try {
    Output.resize(size, filler);
    UInt32 length = std::min(static_cast<UInt32>(arr.size()), size);
    for (UInt32 idx = 0; idx < length; ++idx) {
      BSFixedString value = arr[idx];
      if (value.data())
        Output[idx] = value;
      else
        Output[idx] = BSFixedString("");
    }
  } catch (std::exception &e) {
    logger::error("Error Exception: {}", e.what());
  }
  return Output;
}

template <typename T> VMResultArray<T> PushArray(StaticFunctionTag *, VMArray<T> arr, T push) {
  return ResizeArray(NULL, arr, (static_cast<UInt32>(arr.size()) + 1), push);
}

template <typename T> UInt32 CountValues(StaticFunctionTag *, VMArray<T> arr, T find) {
  UInt32 count(0), length(static_cast<UInt32>(arr.size()));
  if (length > 0) {
    for (UInt32 idx = 0; idx < length; ++idx) {
      T var = arr[idx];
      if (var == find)
        count += 1;
    }
  }
  return count;
}

template <> UInt32 CountValues(StaticFunctionTag *, VMArray<BSFixedString> arr, BSFixedString find) {
  UInt32 count(0), length(static_cast<UInt32>(arr.size()));
  if (length > 0) {
    const char *str = find.data();
    for (UInt32 idx = 0; idx < length; ++idx) {
      BSFixedString var = arr[idx];
      if (!var.data())
        var = BSFixedString("");
      if (boost::iequals(var.data(), str))
        count += 1;
    }
  }
  return count;
}

template <typename T> VMResultArray<T> ClearArray(StaticFunctionTag *, VMArray<T> arr, T remove) {
  VMResultArray<T> Output;
  if (arr.size() > 0) {
    UInt32 length(static_cast<UInt32>(arr.size())), count(CountValues(NULL, arr, remove));
    if (count != length && (length - count) > 0) {
      Output.reserve((length - count));
      for (UInt32 idx = 0; idx < length; ++idx) {
        T var = arr[idx];
        if (var != remove)
          Output.push_back(var);
      }
    }
  }
  return Output;
}

// FIXME: returns none instead of empty array when removing all elements
template <> VMResultArray<BSFixedString> ClearArray(StaticFunctionTag *, VMArray<BSFixedString> arr, BSFixedString remove) {
  VMResultArray<BSFixedString> Output;
  if (arr.size() > 0) {
    UInt32 length(static_cast<UInt32>(arr.size())), count(CountValues(NULL, arr, remove));
    if (count != length && (length - count) > 0) {
      Output.reserve((length - count));
      const char *str = remove.data();
      for (UInt32 idx = 0; idx < length; ++idx) {
        BSFixedString var = arr[idx];
        if (!var.data())
          var = BSFixedString("");
        if (!boost::iequals(var.data(), str))
          Output.push_back(var);
      }
    }
  }
  return Output;
}

template <typename T> VMResultArray<T> MergeArray(StaticFunctionTag *, VMArray<T> arr1, VMArray<T> arr2, bool removeDupe) {
  VMResultArray<T> Output;
  UInt32 length1(static_cast<UInt32>(arr1.size())), length2(static_cast<UInt32>(arr2.size()));
  Output.reserve((length1 + length2));
  for (UInt32 idx = 0; idx < length1; ++idx) {
    T var = arr1[idx];
    if (!removeDupe || std::find(Output.begin(), Output.end(), var) == Output.end())
      Output.push_back(var);
  }
  for (UInt32 idx = 0; idx < length2; ++idx) {
    T var = arr2[idx];
    if (!removeDupe || std::find(Output.begin(), Output.end(), var) == Output.end())
      Output.push_back(var);
  }
  if (removeDupe)
    Output.shrink_to_fit();
  return Output;
}

template <typename T> VMResultArray<T> RemoveDupe(StaticFunctionTag *, VMArray<T> arr1) {
  VMResultArray<T> Output;
  UInt32 length1(static_cast<UInt32>(arr1.size()));
  Output.reserve(length1);
  for (UInt32 idx = 0; idx < length1; ++idx) {
    T var = arr1[idx];
    if (std::find(Output.begin(), Output.end(), var) == Output.end()) {
      Output.push_back(var);
      // length2 += 1;
    }
  }
  Output.shrink_to_fit();
  return Output;
}

template <> VMResultArray<BSFixedString> RemoveDupe(StaticFunctionTag *, VMArray<BSFixedString> arr1) {
  VMResultArray<BSFixedString> Output;
  UInt32 length1(static_cast<UInt32>(arr1.size()));
  Output.reserve(length1);
  for (UInt32 idx = 0; idx < length1; ++idx) {
    BSFixedString var = arr1[idx];
    if (!var.data())
      var = BSFixedString("");
    if (std::find(Output.begin(), Output.end(), var) == Output.end()) {
      Output.push_back(var);
      // length2 += 1;
    }
  }
  Output.shrink_to_fit();
  return Output;
}

template <typename T> bool HasValue(VMArray<T> arr, T find) {
  UInt32 length(static_cast<UInt32>(arr.size()));
  if (length > 0) {
    for (UInt32 idx = 0; idx < length; ++idx) {
      T var = arr[idx];
      if (var == find)
        return true;
    }
  }
  return false;
}

template <> bool HasValue(VMArray<BSFixedString> arr, BSFixedString find) {
  UInt32 length(static_cast<UInt32>(arr.size()));
  if (length > 0) {
    const char *str = find.data();
    for (UInt32 idx = 0; idx < length; ++idx) {
      BSFixedString var = arr[idx];
      if (!var.data())
        var = BSFixedString("");
      if (boost::iequals(var.data(), str))
        return true;
    }
  }
  return false;
}

template <typename T> VMResultArray<T> GetDiff(StaticFunctionTag *, VMArray<T> arr1, VMArray<T> arr2, bool checkBoth, bool allowDupe) {
  VMResultArray<T> Output;
  UInt32 length1(static_cast<UInt32>(arr1.size())), length2(static_cast<UInt32>(arr2.size()));
  Output.reserve((length1 + length2));

  for (UInt32 idx = 0; idx < length1; ++idx) {
    T var = arr1[idx];
    if ((!HasValue<T>(arr2, var)) && (allowDupe || std::find(Output.begin(), Output.end(), var) == Output.end())) {
      Output.push_back(var);
    }
  }

  if (checkBoth) {
    for (UInt32 idx = 0; idx < length2; ++idx) {
      T var = arr2[idx];
      if ((!HasValue<T>(arr1, var)) && (allowDupe || std::find(Output.begin(), Output.end(), var) == Output.end())) {
        Output.push_back(var);
      }
    }
  }

  Output.shrink_to_fit();
  return Output;
}

template <>
VMResultArray<BSFixedString> GetDiff(StaticFunctionTag *, VMArray<BSFixedString> arr1, VMArray<BSFixedString> arr2, bool checkBoth,
                                     bool allowDupe) {
  VMResultArray<BSFixedString> Output;
  UInt32 length1(static_cast<UInt32>(arr1.size())), length2(static_cast<UInt32>(arr2.size()));
  Output.reserve((length1 + length2));

  for (UInt32 idx = 0; idx < length1; ++idx) {
    BSFixedString var = arr1[idx];
    if (!var.data())
      var = BSFixedString("");
    if ((!HasValue<BSFixedString>(arr2, var)) && (allowDupe || std::find(Output.begin(), Output.end(), var) == Output.end())) {
      Output.push_back(var);
    }
  }

  if (checkBoth) {
    for (UInt32 idx = 0; idx < length2; ++idx) {
      BSFixedString var = arr2[idx];
      if (!var.data())
        var = BSFixedString("");
      if ((!HasValue<BSFixedString>(arr1, var)) && (allowDupe || std::find(Output.begin(), Output.end(), var) == Output.end())) {
        Output.push_back(var);
      }
    }
  }

  Output.shrink_to_fit();
  return Output;
}

template <typename T> VMResultArray<T> GetMatching(StaticFunctionTag *, VMArray<T> arr1, VMArray<T> arr2) {
  VMResultArray<T> Output;
  UInt32 length1(static_cast<UInt32>(arr1.size())), length2(static_cast<UInt32>(arr2.size()));
  Output.reserve((length1 + length2));

  for (UInt32 idx = 0; idx < length1; ++idx) {
    T var = arr1[idx];
    if ((HasValue<T>(arr2, var)) && std::find(Output.begin(), Output.end(), var) == Output.end()) {
      Output.push_back(var);
    }
  }

  Output.shrink_to_fit();
  return Output;
}

template <> VMResultArray<BSFixedString> GetMatching(StaticFunctionTag *, VMArray<BSFixedString> arr1, VMArray<BSFixedString> arr2) {
  VMResultArray<BSFixedString> Output;
  UInt32 length1(static_cast<UInt32>(arr1.size())), length2(static_cast<UInt32>(arr2.size()));
  Output.reserve((length1 + length2));

  for (UInt32 idx = 0; idx < length1; ++idx) {
    BSFixedString var = arr1[idx];
    if (!var.data())
      var = BSFixedString("");
    if ((HasValue<BSFixedString>(arr2, var)) && std::find(Output.begin(), Output.end(), var) == Output.end()) {
      Output.push_back(var);
    }
  }

  Output.shrink_to_fit();
  return Output;
}

template <typename T> VMResultArray<T> SliceArray(StaticFunctionTag *, VMArray<T> Input, UInt32 idx, SInt32 end_idx) {
  VMResultArray<T> Output;
  if (end_idx < 0 || end_idx >= static_cast<SInt32>(Input.size()))
    end_idx = static_cast<SInt32>(Input.size()) - 1;
  if (Input.empty() || idx >= Input.size())
    return Output;

  Output.reserve(((end_idx - idx) + 1));
  for (; idx <= static_cast<UInt32>(end_idx); ++idx) {
    T var = Input[idx];
    Output.push_back(var);
  }

  return Output;
}

template <typename T> void SortArray(StaticFunctionTag *, VMArray<T> Input, bool descending) {
  if (Input.size() < 1)
    return;

  UInt32 length = static_cast<UInt32>(Input.size());
  std::vector<T> arr;
  arr.resize(length);
  for (UInt32 idx = 0; idx < length; ++idx) {
    T value = Input[idx];
    arr[idx] = value;
  }

  std::sort(arr.begin(), arr.end());
  if (descending)
    std::reverse(arr.begin(), arr.end());

  for (UInt32 idx = 0; idx < length; ++idx) {
    Input[idx] = arr[idx];
  }
}

template <> void SortArray(StaticFunctionTag *, VMArray<BSFixedString> Input, bool descending) {
  if (Input.size() < 1)
    return;

  UInt32 length = static_cast<UInt32>(Input.size());
  std::vector<std::string> arr;
  arr.resize(length);
  for (UInt32 idx = 0; idx < length; ++idx) {
    BSFixedString value = Input[idx];
    if (value.data() && value.data()[0] != '\0')
      arr[idx] = value.data();
    else
      arr[idx] = "";
  }

  std::sort(arr.begin(), arr.end());
  if (descending)
    std::reverse(arr.begin(), arr.end());

  for (UInt32 idx = 0; idx < length; ++idx) {
    BSFixedString value = BSFixedString(arr[idx].c_str());
    Input[idx] = value;
  }
}

template <typename T> T AddValues(StaticFunctionTag *, VMArray<T> Values) {
  T out = 0;
  UInt32 length(static_cast<UInt32>(Values.size()));
  for (UInt32 idx = 0; idx < length; ++idx) {
    T var = Values[idx];
    out += var;
  }
  return out;
}

template <typename T> T ClampValue(StaticFunctionTag *, T var, T min, T max) {
  if (var >= max)
    return max;
  else if (var <= min)
    return min;
  else
    return var;
}

template <typename T> T WrapValue(StaticFunctionTag *, T var, T end, T start) {
  if (var > end)
    return start;
  else if (var < start)
    return end;
  else
    return var;
}

template <typename T> T SignValue(StaticFunctionTag *, bool sign, T var) {
  return ((sign && var > 0) || (!sign && var < 0)) ? var * -1 : var;
}

VMResultArray<BSFixedString> StringSplit(StaticFunctionTag *, BSFixedString argstring, BSFixedString delimiter) {
  VMResultArray<BSFixedString> Output;
  if (!argstring.data() || !delimiter.data() || boost::iequals(argstring.data(), delimiter.data()))
    return Output;
  // Get non BS string vector
  std::vector<std::string> args;
  std::string delim = delimiter.data();
  std::string range = argstring.data();
  // Canonicalize delimiter case: ireplace_all is case-insensitive, so this normalizes
  // all case variations of the delimiter (e.g., "and", "And", "AND") to consistent casing
  // before splitting with the case-sensitive first_finder
  boost::ireplace_all(range, delim, delim);
  boost::iter_split(args, range, boost::first_finder(delim));
  // Init to size
  Output.reserve(args.size());
  // Fill BSFixedString array
  for (std::vector<std::string>::iterator itr = args.begin(); itr != args.end(); ++itr) {
    boost::trim((*itr));
    BSFixedString str((*itr).c_str());
    Output.push_back(str);
  }
  return Output;
}

BSFixedString StringJoin(StaticFunctionTag *, VMArray<BSFixedString> args, BSFixedString delimiter) {
  std::stringstream ss;
  UInt32 length(static_cast<UInt32>(args.size()));
  const char *delim = delimiter.data();
  for (UInt32 idx = 0; idx < length;) {
    BSFixedString arg = args[idx];
    if (!arg.data())
      arg = BSFixedString("");
    ss << arg.data();
    ++idx;
    if (idx < length)
      ss << delim;
  }
  const std::string &tmp = ss.str();
  const char *str = tmp.c_str();
  return BSFixedString(str);
}

bool TestForm(StaticFunctionTag *, TESForm *ref) {
  if (!ref) {
    return false;
  }
  UInt64 fkey = Forms::GetFormKey(ref);
  logger::info("TestForm(0x{:X}) / {}", ref->formID, ref->formID);
  logger::info("\tFormKey: (0x{:X}) / {}", fkey, fkey);

  UInt64 key = ref->formID;
  UInt64 testi = ((UInt64) static_cast<int>(ref->GetFormType())) << 32;
  logger::info("\tFormType: {}", static_cast<int>(ref->GetFormType()));
  logger::info("\tShift: (0x{:X}) / {}", (int)testi, testi);
  key |= testi;
  logger::info("\tShifted: (0x{:X}) / {}", (int)key, key);

  UInt32 type = (UInt32)(key >> 32);
  UInt32 id = (UInt32)(key);
  logger::info("\tShifted Type: {}", type);
  logger::info("\tShifted ID: (0x{:X}) / {}", (int)id, id);

  UInt32 baseID = id & 0x00FFFFFF;
  logger::info("\tbaseID: 0x{:X}", (int)baseID);

  UInt8 oldMod = (UInt8)(id >> 24);

  logger::info("\toldMod: 0x{:X} / {}", (int)oldMod, (int)oldMod);

  return true;
}

} // namespace PapyrusUtil

void PapyrusUtil::RegisterFuncs(RE::BSScript::IVirtualMachine *vm) {
  constexpr auto script = "PapyrusUtil";

  // Sort Arrays
  vm->RegisterFunction("SortIntArray"sv, script, SortArray<SInt32>);
  vm->RegisterFunction("SortFloatArray"sv, script, SortArray<float>);
  vm->RegisterFunction("SortStringArray"sv, script, SortArray<RE::BSFixedString>);

  // CreateArray
  vm->RegisterFunction("ActorArray"sv, script, CreateArray<RE::Actor *>);
  vm->RegisterFunction("ObjRefArray"sv, script, CreateArray<RE::TESObjectREFR *>);

  // ResizeArray
  vm->RegisterFunction("ResizeActorArray"sv, script, ResizeArray<RE::Actor *>);
  vm->RegisterFunction("ResizeObjRefArray"sv, script, ResizeArray<RE::TESObjectREFR *>);

  // PushArray
  vm->RegisterFunction("PushFloat"sv, script, PushArray<float>);
  vm->RegisterFunction("PushInt"sv, script, PushArray<SInt32>);
  vm->RegisterFunction("PushString"sv, script, PushArray<RE::BSFixedString>);
  vm->RegisterFunction("PushForm"sv, script, PushArray<RE::TESForm *>);
  vm->RegisterFunction("PushAlias"sv, script, PushArray<RE::BGSBaseAlias *>);
  vm->RegisterFunction("PushActor"sv, script, PushArray<RE::Actor *>);
  vm->RegisterFunction("PushObjRef"sv, script, PushArray<RE::TESObjectREFR *>);

  // Count value
  vm->RegisterFunction("CountBool"sv, script, CountValues<bool>);
  vm->RegisterFunction("CountFloat"sv, script, CountValues<float>);
  vm->RegisterFunction("CountInt"sv, script, CountValues<SInt32>);
  vm->RegisterFunction("CountString"sv, script, CountValues<RE::BSFixedString>);
  vm->RegisterFunction("CountForm"sv, script, CountValues<RE::TESForm *>);
  vm->RegisterFunction("CountAlias"sv, script, CountValues<RE::BGSBaseAlias *>);
  vm->RegisterFunction("CountActor"sv, script, CountValues<RE::Actor *>);
  vm->RegisterFunction("CountObjRef"sv, script, CountValues<RE::TESObjectREFR *>);

  // ClearArray (Remove)
  vm->RegisterFunction("RemoveFloat"sv, script, ClearArray<float>);
  vm->RegisterFunction("RemoveInt"sv, script, ClearArray<SInt32>);
  vm->RegisterFunction("RemoveString"sv, script, ClearArray<RE::BSFixedString>);
  vm->RegisterFunction("RemoveForm"sv, script, ClearArray<RE::TESForm *>);
  vm->RegisterFunction("RemoveAlias"sv, script, ClearArray<RE::BGSBaseAlias *>);
  vm->RegisterFunction("RemoveActor"sv, script, ClearArray<RE::Actor *>);
  vm->RegisterFunction("RemoveObjRef"sv, script, ClearArray<RE::TESObjectREFR *>);

  // RemoveDupe
  vm->RegisterFunction("RemoveDupeFloat"sv, script, RemoveDupe<float>);
  vm->RegisterFunction("RemoveDupeInt"sv, script, RemoveDupe<SInt32>);
  vm->RegisterFunction("RemoveDupeString"sv, script, RemoveDupe<RE::BSFixedString>);
  vm->RegisterFunction("RemoveDupeForm"sv, script, RemoveDupe<RE::TESForm *>);
  vm->RegisterFunction("RemoveDupeAlias"sv, script, RemoveDupe<RE::BGSBaseAlias *>);
  vm->RegisterFunction("RemoveDupeActor"sv, script, RemoveDupe<RE::Actor *>);
  vm->RegisterFunction("RemoveDupeObjRef"sv, script, RemoveDupe<RE::TESObjectREFR *>);

  // GetDiff
  vm->RegisterFunction("GetDiffFloat"sv, script, GetDiff<float>);
  vm->RegisterFunction("GetDiffInt"sv, script, GetDiff<SInt32>);
  vm->RegisterFunction("GetDiffString"sv, script, GetDiff<RE::BSFixedString>);
  vm->RegisterFunction("GetDiffForm"sv, script, GetDiff<RE::TESForm *>);
  vm->RegisterFunction("GetDiffAlias"sv, script, GetDiff<RE::BGSBaseAlias *>);
  vm->RegisterFunction("GetDiffActor"sv, script, GetDiff<RE::Actor *>);
  vm->RegisterFunction("GetDiffObjRef"sv, script, GetDiff<RE::TESObjectREFR *>);

  // GetMatching
  vm->RegisterFunction("GetMatchingFloat"sv, script, GetMatching<float>);
  vm->RegisterFunction("GetMatchingInt"sv, script, GetMatching<SInt32>);
  vm->RegisterFunction("GetMatchingString"sv, script, GetMatching<RE::BSFixedString>);
  vm->RegisterFunction("GetMatchingForm"sv, script, GetMatching<RE::TESForm *>);
  vm->RegisterFunction("GetMatchingAlias"sv, script, GetMatching<RE::BGSBaseAlias *>);
  vm->RegisterFunction("GetMatchingActor"sv, script, GetMatching<RE::Actor *>);
  vm->RegisterFunction("GetMatchingObjRef"sv, script, GetMatching<RE::TESObjectREFR *>);

  // MergeArray
  vm->RegisterFunction("MergeFloatArray"sv, script, MergeArray<float>);
  vm->RegisterFunction("MergeIntArray"sv, script, MergeArray<SInt32>);
  vm->RegisterFunction("MergeStringArray"sv, script, MergeArray<RE::BSFixedString>);
  vm->RegisterFunction("MergeFormArray"sv, script, MergeArray<RE::TESForm *>);
  vm->RegisterFunction("MergeAliasArray"sv, script, MergeArray<RE::BGSBaseAlias *>);
  vm->RegisterFunction("MergeActorArray"sv, script, MergeArray<RE::Actor *>);
  vm->RegisterFunction("MergeObjRefArray"sv, script, MergeArray<RE::TESObjectREFR *>);

  // SliceArray
  vm->RegisterFunction("SliceFloatArray"sv, script, SliceArray<float>);
  vm->RegisterFunction("SliceIntArray"sv, script, SliceArray<SInt32>);
  vm->RegisterFunction("SliceStringArray"sv, script, SliceArray<RE::BSFixedString>);
  vm->RegisterFunction("SliceFormArray"sv, script, SliceArray<RE::TESForm *>);
  vm->RegisterFunction("SliceAliasArray"sv, script, SliceArray<RE::BGSBaseAlias *>);
  vm->RegisterFunction("SliceActorArray"sv, script, SliceArray<RE::Actor *>);
  vm->RegisterFunction("SliceObjRefArray"sv, script, SliceArray<RE::TESObjectREFR *>);

  // String functions
  vm->RegisterFunction("StringSplit"sv, script, StringSplit);
  vm->RegisterFunction("StringJoin"sv, script, StringJoin);

  // Misc float/int
  vm->RegisterFunction("AddFloatValues"sv, script, AddValues<float>);
  vm->RegisterFunction("AddIntValues"sv, script, AddValues<SInt32>);

  vm->RegisterFunction("ClampFloat"sv, script, ClampValue<float>);
  vm->RegisterFunction("ClampInt"sv, script, ClampValue<SInt32>);

  vm->RegisterFunction("WrapFloat"sv, script, WrapValue<float>);
  vm->RegisterFunction("WrapInt"sv, script, WrapValue<SInt32>);

  vm->RegisterFunction("SignFloat"sv, script, SignValue<float>);
  vm->RegisterFunction("SignInt"sv, script, SignValue<SInt32>);

  // Set NoWait flags (callable from tasklets)
  vm->SetCallableFromTasklets(script, "SortIntArray", true);
  vm->SetCallableFromTasklets(script, "SortFloatArray", true);
  vm->SetCallableFromTasklets(script, "SortStringArray", true);
  vm->SetCallableFromTasklets(script, "ActorArray", true);
  vm->SetCallableFromTasklets(script, "ObjRefArray", true);
  vm->SetCallableFromTasklets(script, "ResizeActorArray", true);
  vm->SetCallableFromTasklets(script, "ResizeObjRefArray", true);
  vm->SetCallableFromTasklets(script, "PushFloat", true);
  vm->SetCallableFromTasklets(script, "PushInt", true);
  vm->SetCallableFromTasklets(script, "PushString", true);
  vm->SetCallableFromTasklets(script, "PushForm", true);
  vm->SetCallableFromTasklets(script, "PushAlias", true);
  vm->SetCallableFromTasklets(script, "PushActor", true);
  vm->SetCallableFromTasklets(script, "PushObjRef", true);
  vm->SetCallableFromTasklets(script, "CountBool", true);
  vm->SetCallableFromTasklets(script, "CountFloat", true);
  vm->SetCallableFromTasklets(script, "CountInt", true);
  vm->SetCallableFromTasklets(script, "CountString", true);
  vm->SetCallableFromTasklets(script, "CountForm", true);
  vm->SetCallableFromTasklets(script, "CountAlias", true);
  vm->SetCallableFromTasklets(script, "CountActor", true);
  vm->SetCallableFromTasklets(script, "CountObjRef", true);
  vm->SetCallableFromTasklets(script, "RemoveFloat", true);
  vm->SetCallableFromTasklets(script, "RemoveInt", true);
  vm->SetCallableFromTasklets(script, "RemoveString", true);
  vm->SetCallableFromTasklets(script, "RemoveForm", true);
  vm->SetCallableFromTasklets(script, "RemoveAlias", true);
  vm->SetCallableFromTasklets(script, "RemoveActor", true);
  vm->SetCallableFromTasklets(script, "RemoveObjRef", true);
  vm->SetCallableFromTasklets(script, "RemoveDupeFloat", true);
  vm->SetCallableFromTasklets(script, "RemoveDupeInt", true);
  vm->SetCallableFromTasklets(script, "RemoveDupeString", true);
  vm->SetCallableFromTasklets(script, "RemoveDupeForm", true);
  vm->SetCallableFromTasklets(script, "RemoveDupeAlias", true);
  vm->SetCallableFromTasklets(script, "RemoveDupeActor", true);
  vm->SetCallableFromTasklets(script, "RemoveDupeObjRef", true);
  vm->SetCallableFromTasklets(script, "GetDiffFloat", true);
  vm->SetCallableFromTasklets(script, "GetDiffInt", true);
  vm->SetCallableFromTasklets(script, "GetDiffString", true);
  vm->SetCallableFromTasklets(script, "GetDiffForm", true);
  vm->SetCallableFromTasklets(script, "GetDiffAlias", true);
  vm->SetCallableFromTasklets(script, "GetDiffActor", true);
  vm->SetCallableFromTasklets(script, "GetDiffObjRef", true);
  vm->SetCallableFromTasklets(script, "GetMatchingFloat", true);
  vm->SetCallableFromTasklets(script, "GetMatchingInt", true);
  vm->SetCallableFromTasklets(script, "GetMatchingString", true);
  vm->SetCallableFromTasklets(script, "GetMatchingForm", true);
  vm->SetCallableFromTasklets(script, "GetMatchingAlias", true);
  vm->SetCallableFromTasklets(script, "GetMatchingActor", true);
  vm->SetCallableFromTasklets(script, "GetMatchingObjRef", true);
  vm->SetCallableFromTasklets(script, "MergeFloatArray", true);
  vm->SetCallableFromTasklets(script, "MergeIntArray", true);
  vm->SetCallableFromTasklets(script, "MergeStringArray", true);
  vm->SetCallableFromTasklets(script, "MergeFormArray", true);
  vm->SetCallableFromTasklets(script, "MergeAliasArray", true);
  vm->SetCallableFromTasklets(script, "MergeActorArray", true);
  vm->SetCallableFromTasklets(script, "MergeObjRefArray", true);
  vm->SetCallableFromTasklets(script, "SliceFloatArray", true);
  vm->SetCallableFromTasklets(script, "SliceIntArray", true);
  vm->SetCallableFromTasklets(script, "SliceStringArray", true);
  vm->SetCallableFromTasklets(script, "SliceFormArray", true);
  vm->SetCallableFromTasklets(script, "SliceAliasArray", true);
  vm->SetCallableFromTasklets(script, "SliceActorArray", true);
  vm->SetCallableFromTasklets(script, "SliceObjRefArray", true);
  vm->SetCallableFromTasklets(script, "StringSplit", true);
  vm->SetCallableFromTasklets(script, "StringJoin", true);
}
