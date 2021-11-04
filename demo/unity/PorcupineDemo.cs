using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

using Pv.Unity;

public class PorcupineDemo : MonoBehaviour {

    private const string ACCESS_KEY = "${YOUR_ACCESS_KEY_HERE}"; // AccessKey obtained from Picovoice Console (https://picovoice.ai/console/)

    static List<Porcupine.BuiltInKeyword> _keywords = Enum.GetValues(typeof(Porcupine.BuiltInKeyword)).Cast<Porcupine.BuiltInKeyword>().ToList();
    public Texture[] _imgs;

    Button _startButton;
    RawImage _outputImg;
    Text _instructions;
    Color _alphaSubtract = new Color(0, 0, 0, 0.008f);

    private bool _isProcessing;
    PorcupineManager _porcupineManager;

    void Start() 
    {
        _startButton = gameObject.GetComponentInChildren<Button>();
        _startButton.onClick.AddListener(ToggleProcessing);        
        _outputImg = gameObject.GetComponentInChildren<RawImage>();
        _instructions = gameObject.GetComponentInChildren<Text>();
        FillKeywords();

        try
        {
            _porcupineManager = PorcupineManager.FromBuiltInKeywords(ACCESS_KEY, _keywords, OnWakeWordDetected);            
        }
        catch (Exception ex)
        {
            Debug.LogError("PorcupineManager was unable to initialize: " + ex.ToString());
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
        (_startButton.targetGraphic as Text).text = "Start Listening";
        _porcupineManager.Stop();
        _isProcessing = false;
    }

    private void OnWakeWordDetected(int keywordIndex) 
    {		
        if (keywordIndex >= 0) {
            Porcupine.BuiltInKeyword keyword = _keywords[keywordIndex];			
            _outputImg.color = Color.white;
            _outputImg.texture = _imgs.First(img => img.name == keyword.ToString().Replace("_", " ").ToLower());
        }
    }	
    void Update () {
        if (_outputImg.texture != null)
        {
            _outputImg.color -= _alphaSubtract;
        }
    }

    private void FillKeywords()
    {
        foreach (string name in Enum.GetNames(typeof(Porcupine.BuiltInKeyword)))
        {
            _instructions.text = $"{_instructions.text}\n- '{name.Replace("_", " ").ToLower()}'";
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
