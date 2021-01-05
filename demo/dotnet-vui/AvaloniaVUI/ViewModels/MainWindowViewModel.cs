using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

using ReactiveUI;
using Avalonia.Media;

using Pv;
using OpenTK.Audio.OpenAL;

namespace AvaloniaVUI.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        private Color _backgroundColor = Colors.LightGray;
        public Color BackgroundColor
        {
            get => _backgroundColor;
            set => this.RaiseAndSetIfChanged(ref _backgroundColor, value);
        }

        private readonly List<Color> _bgColors = new List<Color>
        {
            Colors.LightPink,
            Colors.LawnGreen,
            Colors.Yellow,
            Colors.BlueViolet
        };

        private bool _isGrapefruit;
        public bool IsGrapefruit
        {
            get => _isGrapefruit;
            set
            {
                this.RaiseAndSetIfChanged(ref _isGrapefruit, value);
                BackgroundColor = _bgColors[0];
            }
        }

        private bool _isGrasshopper;
        public bool IsGrasshopper
        {
            get => _isGrasshopper;
            set
            {
                this.RaiseAndSetIfChanged(ref _isGrasshopper, value);
                BackgroundColor = _bgColors[1];
            }
        }

        private bool _isBumblebee;
        public bool IsBumblebee
        {
            get => _isBumblebee;
            set
            {
                this.RaiseAndSetIfChanged(ref _isBumblebee, value);
                BackgroundColor = _bgColors[2];
            }
        }

        private bool _isBlueberry;
        public bool IsBlueberry
        {
            get => _isBlueberry;
            set
            {
                this.RaiseAndSetIfChanged(ref _isBlueberry, value);
                BackgroundColor = _bgColors[3];
            }
        }

        public MainWindowViewModel()
        {
            Task.Factory.StartNew(() =>
            {
                List<string> commands = new List<string> { "grapefruit", "grasshopper", "bumblebee", "blueberry" };
                using Porcupine porcupine = Porcupine.Create(keywords: commands);

                short[] frameBuffer = new short[porcupine.FrameLength];
                ALCaptureDevice captureDevice = ALC.CaptureOpenDevice(null, porcupine.SampleRate, ALFormat.Mono16, porcupine.FrameLength * 2);
                {
                    ALC.CaptureStart(captureDevice);
                    while (true)
                    {
                        int samplesAvailable = ALC.GetAvailableSamples(captureDevice);
                        if (samplesAvailable > porcupine.FrameLength)
                        {
                            ALC.CaptureSamples(captureDevice, ref frameBuffer[0], porcupine.FrameLength);
                            int keywordIndex = porcupine.Process(frameBuffer);

                            if (keywordIndex >= 0)
                            {
                                switch (keywordIndex)
                                {
                                    case 0:
                                        IsGrapefruit = true;
                                        break;
                                    case 1:
                                        IsGrasshopper = true;
                                        break;
                                    case 2:
                                        IsBumblebee = true;
                                        break;
                                    case 3:
                                        IsBlueberry = true;
                                        break;
                                }
                            }
                        }
                    }
                }

            });
        }
    }
}
