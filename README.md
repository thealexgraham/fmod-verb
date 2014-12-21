fmod-schroederverb
==================
By Alex Graham

FMOD Plugin that implements a Schroeder Reverberator

Super Schroeder Reverb is a plugin for FMOD that implements a reverberation unit as designed by Schroeder. The current implementation uses 4 Comb Filters operating in parallel that feed into two allpass filters which operate sequentially. The comb filters have a lowpass filter which operates on the feedback. There are also two echoes, an early and a late.

Compile and Install
--------------
1. Open fmod_schroeder.xcodeproj (in the root folder) in Xcode (known to work in 6.1.1)
2. Make sure the "32 bit" build scheme is selected
3. Click Product -> Build from the menu bar
4. Install FMOD Studio if necessary (<a href="www.fmod.org/download">Download here</a>)
5. Note the plug-ins folder in FMOD's settings under the Plug-ins tab (IE, ~/FMODPlugins)
6. Copy fmod_schroeder.dylib from fmod_schroeder/_builds/Release to the FMOD Plugin folder

*Note:* The project is set up to be compiled for Macintosh in Xcode (the example plugins that came with the API were setup in dev environments to ensure the correct linking and compiler setup), but the code could be adapted to a Visual Studio project to compile for Windows.
