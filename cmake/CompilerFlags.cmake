# CompilerFlags.cmake
# Configures compiler-specific flags and optimizations for CommonLibSSE-NG

if(MSVC)
    # ---- Compilation speed optimizations ----
    string(APPEND CMAKE_CXX_FLAGS " /d2ReducedOptimizeHugeFunctions")

    # ---- Code generation optimizations ----
    string(APPEND CMAKE_CXX_FLAGS " /Gy") # Function-level linking
    string(APPEND CMAKE_CXX_FLAGS " /Gw") # Optimize global data
    #string(APPEND CMAKE_CXX_FLAGS " /arch:AVX2") # Use AVX2 SIMD instructions

    # ---- C++23 strict conformance flags ----
    string(APPEND CMAKE_CXX_FLAGS " /permissive-") # Strict standards conformance
    string(APPEND CMAKE_CXX_FLAGS " /Zc:inline") # Remove unreferenced COMDAT
    string(APPEND CMAKE_CXX_FLAGS " /Zc:alignedNew") # Aligned allocation
    string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus") # Correct __cplusplus value
    string(APPEND CMAKE_CXX_FLAGS " /Zc:externConstexpr")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:forScope")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:hiddenFriend")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:implicitNoexcept")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:lambda")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:noexceptTypes")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:preprocessor")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:referenceBinding")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:rvalueCast")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:sizedDealloc")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:strictStrings")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:ternary")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:threadSafeInit")
    string(APPEND CMAKE_CXX_FLAGS " /Zc:wchar_t")
    string(APPEND CMAKE_CXX_FLAGS " /GL") # Whole program optimization

    # ---- Warning configuration ----
    string(APPEND CMAKE_CXX_FLAGS " /W4") # Warning level 4
    string(APPEND CMAKE_CXX_FLAGS " /utf-8") # UTF-8 source and execution charset

    #  Papyrus native function declarations parameter is required by the registration API but not actually needed in the implementation.
    string(APPEND CMAKE_CXX_FLAGS " /wd4100") # Disable C4100: unreferenced formal parameter

    # ---- Exception handling: Use /EHa for SEH compatibility ----
    # Remove default /EHsc from base flags first to avoid D9025 warning
    string(REGEX REPLACE "/EH[scra-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/EH[scra-z]+" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

    # ---- Apply to all build configurations ----
    foreach(flag_var
        CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_DEBUG)
        # Use asynchronous exception handling (/EHa) for SEH compatibility
        string(REGEX REPLACE "/EH[scra-z]+" "" ${flag_var} "${${flag_var}}")
        string(APPEND ${flag_var} " /EHa /bigobj")
        # Static runtime linking (/MT)
        string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endforeach()

    # ---- Release build PDB generation ----
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF /LTCG")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF /LTCG")
endif()
