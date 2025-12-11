# ExternalDependencies.cmake
# Configures external library subdirectories

include(commonlibsse)

# Helper function to link all external dependencies to a target
function(link_external_dependencies target_name)
    target_include_directories(${target_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/external/commonlibsse-ng/include
    )
endfunction()
