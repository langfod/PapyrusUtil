param(
    [string]$preset = "release",
    [int]$threads = 8
)


Write-Host "Running preset $preset"

# Save current directory, launch VS dev shell, and return to original directory
$vsDevShellPath = "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/Launch-VsDevShell.ps1"
$currentDirectory = $PWD.Path
& $vsDevShellPath -Arch amd64; Set-Location -Path "${currentDirectory}"

# Update git submodules
try {
    git submodule update --init --recursive
    Write-Host " Git submodules updated successfully." -ForegroundColor Green
} catch {
    Write-Host " Error updating git submodules: $_" -ForegroundColor Red
    throw
}

# Build cmake configure arguments
$cmakeArgs = @("-S", ".", "--preset=$preset", "-Wno-dev")

& cmake $cmakeArgs
if ($LASTEXITCODE -ne 0) { exit 1 }

& cmake --build --preset=$preset --parallel $threads
if ($LASTEXITCODE -ne 0) { exit 1 }