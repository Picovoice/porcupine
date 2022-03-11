# AvaloniaVUI Demo

A voice-enabled .NET Core desktop application that uses the cross-platform GUI framework [Avalonia](https://github.com/AvaloniaUI/Avalonia). This code was part of a developer tutorial article that can be found [here](https://medium.com/picovoice/from-gui-to-vui-voice-enabling-a-cross-platform-net-desktop-application-aac44e470790). 

Video tutorial:<br/>
[![Tutorial video](https://img.youtube.com/vi/AU87_4GpIzo/0.jpg)](https://youtu.be/AU87_4GpIzo)

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey`s act as your credentials when using Porcupine SDKs.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://console.picovoice.ai/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

Once you have obtained an `AccessKey`, copy it into the `ACCESS_KEY` variable in `AvaloniaVUI/ViewModelsMainWindowViewModel.cs`.