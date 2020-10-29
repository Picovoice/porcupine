const { getDefaultConfig } = require('metro-config')

module.exports = (async () => {
    const {
        resolver: { assetExts },
    } = await getDefaultConfig()
    return {
        transformer: {
            getTransformOptions: async () => ({
                transform: {
                    experimentalImportSupport: false,
                    inlineRequires: false,
                },
            }),
        },
        // resolver: {
        //     assetExts: [...assetExts, 'pv', 'ppn'],
        //     sourceExts: ['ppoo']
        // },
    }
})()