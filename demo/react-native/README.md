# Porcupine Demo App
To run the React Native Porcupine demo app you'll first need to set up your React Native environment. For this, 
please refer to [React Native's documentation](https://reactnative.dev/docs/environment-setup). Once your environment has been set up, you can run the following commands from this repo location.

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

Replace your `AccessKey`, in [`App.tsx`](App.tsx) file:

```typescript
_accessKey: string ="${YOUR_ACCESS_KEY_HERE}" // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
```

### Running On Android
```console
yarn android-install    # sets up environment
yarn android-run        # builds and deploys to Android
```

### Running On iOS

```console
yarn ios-install        # sets up environment
yarn ios-run            # builds and deploys to iOS
```