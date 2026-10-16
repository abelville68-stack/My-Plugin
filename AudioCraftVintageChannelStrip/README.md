# AudioCraft Vintage Channel Strip (VST3)

A JUCE-based Windows VST3/Standalone project for a British-console-inspired channel strip. Includes preamp drive/input trim, phase/pad/48V controls, high-pass filter, four-band EQ, compressor with parallel blend, and output trim.

## Build on Windows 10/11
1. Install **Visual Studio 2022 Community** with **Desktop development with C++** (MSVC, Windows SDK, CMake tools).
2. Install **Git for Windows** and **CMake** if not included with Visual Studio.
3. Open a Developer PowerShell, `cd` to this project folder.
4. Configure: `cmake -S . -B build -G "Visual Studio 17 2022" -A x64`
5. Build: `cmake --build build --config Release --target AudioCraftVintageChannelStrip_VST3`
6. The VST3 bundle is normally created under `build/AudioCraftVintageChannelStrip_artefacts/Release/VST3/AudioCraft Vintage Channel Strip.vst3` (exact path can vary slightly by JUCE/CMake version).
7. Copy the `.vst3` bundle to `C:\Program Files\Common Files\VST3`, then rescan plugins in your DAW.

CMake downloads JUCE 8.0.6 on first configure, so an internet connection is required for the initial build. This archive contains the source project, not a precompiled Windows binary. A Windows/MSVC build is required to produce the installable VST3.

## Notes
- This is an original console-inspired design and DSP implementation, not an official Neve product or an exact emulation of a specific hardware unit.
- 48V is a control/UI parameter only; a software plugin cannot supply phantom power to a physical microphone.
- The VU meter in this initial UI is decorative. Functional input/output and gain-reduction metering can be added in a follow-up build.
