// metro.config.js
// with workaround solutions

const path = require('path')

module.exports = {
  // workaround for issue with symlinks encountered starting with
  // metro@0.55 / React Native 0.61
  // (not needed with React Native 0.60 / metro@0.54)
  resolver: {
    extraNodeModules: new Proxy(
      {},
      { get: (_, name) => path.resolve('.', 'node_modules', name) }
    )
  },

  // quick workaround solution for issue with symlinked modules ref:
  // https://github.com/brodybits/create-react-native-module/issues/232
  watchFolders: [path.resolve('.'), path.resolve('../../../binding/react-native/react-native-porcupine')]
}
