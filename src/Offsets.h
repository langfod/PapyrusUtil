#pragma once

#include "PCH.h"

// Offset definitions using CommonLibSSE-NG's REL::ID system
// These IDs are from Address Library and work across all game versions

namespace Offsets {
    // Function IDs for Address Library lookup
    inline constexpr REL::ID ToggleFreeCam{ 50809 };
    inline constexpr REL::ID IsValidPackage{ 29619 };
    inline constexpr REL::ID PackageStartOrig{ 12057 };
    inline constexpr REL::ID PackageEndOrig{ 69166 };
    inline constexpr REL::ID PackageStart_Base{ 37398 };
    inline constexpr REL::ID PackageEnd_Base{ 53984 };

    // Additional offsets from base addresses
    inline constexpr std::ptrdiff_t PackageStart_Offset = 0x47;
    inline constexpr std::ptrdiff_t PackageEnd_Offset = 0x103;
}

namespace Plugin {
    bool InitializeOffsets();
    bool DumpSpecificVersion();
}
