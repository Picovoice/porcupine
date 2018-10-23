# Prerequisites
Dotnet core 2.1 or dotnet framework 4.6, Mono should also work only this is untested.
The unit tests uses the MSTest framework (this is managed by nuget) so when using nuget restore it will install all the required files.

-----------------------------
To read the sound files it uses some code from [this](https://www.codeproject.com/Articles/35725/C-WAV-file-class-audio-mixing-and-some-light-audio) blog from [Codeproject](https://www.codeproject.com).
The code for reading the WAV files (in the folder [WAVUtilities](/PorcupineTest/WAVUtilities)) is licensed under the [CPOL](https://www.codeproject.com/info/cpol10.aspx) license and is written by the CodeProject user [CalicoSkies](https://www.codeproject.com/script/Membership/View.aspx?mid=172176).

-----------------------------
# Running unit tests
The easiest method is using visual studio to run the unit tests. 


# Binding Class
The csharp bindings use the standard [P/INVOKE functionality](https://docs.microsoft.com/en-gb/cpp/dotnet/how-to-call-native-dlls-from-managed-code-using-pinvoke?view=vs-2017)