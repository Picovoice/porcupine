# Porcupine Demo App
To run the React Native Porcupine demo app you'll first need to setup your React Native environment. For this, 
please refer to [React Native's documentation](https://reactnative.dev/docs/environment-setup). Once your environment has been set up, you can run the following commands from this repo location.

## AccessKey

The Porcupine SDK requires a valid `AccessKey` at initialization. `AccessKey`s act as your credentials when using Porcupine SDKs.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://picovoice.ai/console/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

Once you obtain your `AccessKey`, replace it in [`App.tsx`](App.tsx) file:

```typescript
_accessKey: string ="YOUR_ACCESS_KEY_HERE" // AccessKey obtained from Picovoice Console (https://picovoice.ai/console/)
```

**NOTE**: This will not protect your access key in production builds.

## Usage

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