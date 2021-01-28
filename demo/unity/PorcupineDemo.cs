using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

using Pv.Unity;

public class PorcupineDemo : MonoBehaviour {
    
    public Texture[] _imgs = new Texture[7];
    List<string> _keywords = new List<string> { "americano", "grasshopper", "grapefruit", "computer", "blueberry", "bumblebee", "porcupine" };
    
    Button _startButton;
    RawImage _outputImg;
    Color _alphaSubtract = new Color(0, 0, 0, 0.008f);

    private bool _isProcessing;
    PorcupineManager _porcupineManager;
    
    void Start() 
    {
        _startButton = gameObject.GetComponentInChildren<Button>();
        _startButton.onClick.AddListener(ToggleProcessing);        
        _outputImg = gameObject.GetComponentInChildren<RawImage>();

        try
        {
            _porcupineManager = PorcupineManager.FromKeywords(_keywords, OnWakeWordDetected);            
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
            string keyword = _keywords[keywordIndex];			
            _outputImg.color = Color.white;
            _outputImg.texture = _imgs.First(img => img.name == keyword);
            
        }	
    }	
    void Update () {
        if (_outputImg.texture != null)
        {
            _outputImg.color -= _alphaSubtract;
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
