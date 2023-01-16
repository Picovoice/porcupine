const testData = require('../test_data.json');

describe('Porcupine', () => {
  beforeAll(async () => {
    await device.launchApp();
  });

  beforeEach(async () => {
    await device.reloadReactNative();
  });

  it('should pass all tests', async () => {
    await element(by.id('runTests')).tap();

    await waitFor(element(by.id('testStatus')))
      .not.toExist()
      .withTimeout(12 * 60 * 1000);

    const numTestCases =
      testData.tests.singleKeyword.length +
      testData.tests.multipleKeyword.length;
    for (let i = 0; i < numTestCases; i += 1) {
      await expect(element(by.id('testResult')).atIndex(i)).toHaveText('true');
    }
  });
});
