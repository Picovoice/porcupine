let porcupine = null;

window.addEventListener("load", function () {
  let usingBuiltIns = false;
  if (
    porcupineKeywords.length === 0 &&
    porcupineModel.publicPath.endsWith("porcupine_params.pv")
  ) {
    usingBuiltIns = true;
    for (const k in PorcupineWeb.BuiltInKeyword) {
      porcupineKeywords.push(k);
    }
  }

  let select = document.getElementById("keywords");
  for (let i = 0; i < porcupineKeywords.length; i++) {
    let el = document.createElement("option");
    el.textContent = usingBuiltIns
      ? PorcupineWeb.BuiltInKeyword[porcupineKeywords[i]]
      : porcupineKeywords[i].label;
    el.value = `${i}`;
    select.appendChild(el);
  }
});

function writeMessage(message) {
  console.log(message);
  document.getElementById("status").innerHTML = message;
}

function porcupineErrorCallback(error) {
  writeMessage(error);
}

function porcupineKeywordCallback(detection) {
  const time = new Date();
  const message = `keyword detected at ${time.toLocaleTimeString()}: ${detection.label} (index = ${detection.index})`;
  console.log(message);
  document.getElementById("result").innerHTML = message;
}

async function startPorcupine(accessKey, keywordIndex) {
  if (window.WebVoiceProcessor.WebVoiceProcessor.isRecording) {
    await window.WebVoiceProcessor.WebVoiceProcessor.unsubscribe(porcupine);
    await porcupine.terminate();
  }

  writeMessage("Porcupine is loading. Please wait...");
  try {
    porcupine = await PorcupineWeb.PorcupineWorker.create(
      accessKey,
      [porcupineKeywords[keywordIndex]],
      porcupineKeywordCallback,
      porcupineModel,
    );

    writeMessage("Porcupine worker ready!");

    writeMessage(
      "WebVoiceProcessor initializing. Microphone permissions requested ...",
    );
    await window.WebVoiceProcessor.WebVoiceProcessor.subscribe(porcupine);

    writeMessage("WebVoiceProcessor ready and listening!");
  } catch (err) {
    porcupineErrorCallback(err);
  }
}
