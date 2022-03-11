using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

using Pv.Unity;

public class PorcupineDemo : MonoBehaviour {

    private const string ACCESS_KEY = "${YOUR_ACCESS_KEY_HERE}"; // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)

    static List<Porcupine.BuiltInKeyword> _keywords = Enum.GetValues(typeof(Porcupine.BuiltInKeyword)).Cast<Porcupine.BuiltInKeyword>().ToList();
    public Texture[] _imgs;

    Button _startButton;
    RawImage _outputImg;
    Text _textField;
    Color _alphaSubtract = new Color(0, 0, 0, 0.008f);

    private bool _isProcessing;
    PorcupineManager _porcupineManager;
    private bool isError = false;

    void Start() 
    {
        _startButton = gameObject.GetComponentInChildren<Button>();
        _startButton.onClick.AddListener(ToggleProcessing);        
        _outputImg = gameObject.GetComponentInChildren<RawImage>();
        _textField = gameObject.GetComponentInChildren<Text>();
        _keywords.Remove(Porcupine.BuiltInKeyword.HEY_GOOGLE);
        FillKeywords();

        try
        {
            _porcupineManager = PorcupineManager.FromBuiltInKeywords(ACCESS_KEY, _keywords, OnWakeWordDetected, processErrorCallback: ErrorCallback);            
        }
        catch (PorcupineInvalidArgumentException ex)
        {
            SetError($"{ex.Message}\nMake sure your access key '{ACCESS_KEY}' is a valid access key.");
        }
        catch (PorcupineActivationException)
        {
            SetError("AccessKey activation error");
        }
        catch (PorcupineActivationLimitException)
        {
            SetError("AccessKey reached its device limit");
        }
        catch (PorcupineActivationRefusedException)
        {
            SetError("AccessKey refused");
        }
        catch (PorcupineActivationThrottledException)
        {
            SetError("AccessKey has been throttled");
        }
        catch (PorcupineException ex)
        {
            SetError("PorcupineManager was unable to initialize: " + ex.Message);
        }
    }

    private void ToggleProcessing() 
    {		
        if (!_isProcessing)
        {
            StartProcessing();
        }
        else 
        {
            StopProcessing();
        }
    }

    private void StartProcessing()
    {
        (_startButton.targetGraphic as Text).text = "Stop Listening";
        _porcupineManager.Start();
        _isProcessing = true;
    }

    private void StopProcessing() 
    {
        if (_porcupineManager == null) {
            return;
        }

        (_startButton.targetGraphic as Text).text = "Start Listening";
        _porcupineManager.Stop();
        _isProcessing = false;
    }

    private void OnWakeWordDetected(int keywordIndex)
    {
        if (isError)
        {
            return;
        }

        if (keywordIndex >= 0)
        {
            Porcupine.BuiltInKeyword keyword = _keywords[keywordIndex];
            _outputImg.color = Color.white;
            string a = keyword.ToString().ToLower();
            _outputImg.texture = _imgs.First(img => img.name == keyword.ToString().ToLower());
        }
    }

    private void ErrorCallback(Exception e)
    {
        SetError(e.Message);
    }

    private void SetError(string message)
    {
        isError = true;
        _textField.text = message;
        _textField.color = Color.red;
        _startButton.interactable = false;
        StopProcessing();
    }

    void Update () {
        if (isError)
        {
            return;
        }

        if (_outputImg.texture != null)
        {
            _outputImg.color -= _alphaSubtract;
        }
    }

    private void FillKeywords()
    {
        foreach (Porcupine.BuiltInKeyword keyword in _keywords)
        {
            _textField.text = $"{_textField.text}\n- '{keyword.ToString().Replace("_", " ").ToLower()}'";
        }
    }

    void OnApplicationQuit() 
    {
        if (_porcupineManager != null)
        {
            _porcupineManager.Delete();
        }
    }
}
