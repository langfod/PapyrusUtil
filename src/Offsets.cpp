#include "PCH.h"
#include "Offsets.h"

namespace Plugin {

    bool InitializeOffsets() {
        
        try {
            // Validate all critical addresses from Address Library
            bool allValid = true;
            
            auto toggleFreeCamAddr = Offsets::ToggleFreeCam.address();
            auto isValidPackageAddr = Offsets::IsValidPackage.address();
            auto packageStartOrigAddr = Offsets::PackageStartOrig.address();
            auto packageEndOrigAddr = Offsets::PackageEndOrig.address();
            auto packageStartBaseAddr = Offsets::PackageStart_Base.address();
            auto packageEndBaseAddr = Offsets::PackageEnd_Base.address();
            
            if (toggleFreeCamAddr == 0) {
                logger::error("Failed to resolve ToggleFreeCam address (ID 50809)");
                allValid = false;
            }
            if (isValidPackageAddr == 0) {
                logger::error("Failed to resolve IsValidPackage address (ID 29619)");
                allValid = false;
            }
            if (packageStartOrigAddr == 0) {
                logger::error("Failed to resolve PackageStartOrig address (ID 12057)");
                allValid = false;
            }
            if (packageEndOrigAddr == 0) {
                logger::error("Failed to resolve PackageEndOrig address (ID 69166)");
                allValid = false;
            }
            if (packageStartBaseAddr == 0) {
                logger::error("Failed to resolve PackageStart_Base address (ID 37398)");
                allValid = false;
            }
            if (packageEndBaseAddr == 0) {
                logger::error("Failed to resolve PackageEnd_Base address (ID 53984)");
                allValid = false;
            }
            
            if (!allValid) {
                logger::error("One or more Address Library offsets failed to resolve");
                return false;
            }

            // Log the resolved addresses for debugging
            logger::info("Address Library offsets initialized successfully:");
            logger::info("  ToggleFreeCam (50809): {:X}", toggleFreeCamAddr);
            logger::info("  IsValidPackage (29619): {:X}", isValidPackageAddr);
            logger::info("  PackageStartOrig (12057): {:X}", packageStartOrigAddr);
            logger::info("  PackageEndOrig (69166): {:X}", packageEndOrigAddr);
            logger::info("  PackageStart_Enter (37398+0x47): {:X}", 
                packageStartBaseAddr + Offsets::PackageStart_Offset);
            logger::info("  PackageEnd_Enter (53984+0x103): {:X}", 
                packageEndBaseAddr + Offsets::PackageEnd_Offset);

            return true;
        }
        catch (const std::exception& e) {
            logger::error("Exception while initializing offsets: {}", e.what());
            return false;
        }
    }

    bool DumpSpecificVersion() {       
        auto runtime = REL::Module::get().version();
        logger::info("Runtime version: {}.{}.{}.{}", 
            runtime.major(), runtime.minor(), runtime.patch(), runtime.build());
        
        return true;
    }

}