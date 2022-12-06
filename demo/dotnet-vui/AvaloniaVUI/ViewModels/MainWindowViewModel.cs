using System;
using System.Collections.Generic;
using System.Threading.Tasks;

using Avalonia.Media;

using Pv;

using ReactiveUI;

namespace AvaloniaVUI.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        private const string ACCESS_KEY = "{YOUR_ACCESS_KEY_HERE}";

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
                List<BuiltInKeyword> commands = new List<BuiltInKeyword> {
                    BuiltInKeyword.GRAPEFRUIT,
                    BuiltInKeyword.GRASSHOPPER,
                    BuiltInKeyword.BUMBLEBEE,
                    BuiltInKeyword.BLUEBERRY
                };

                using Porcupine porcupine = Porcupine.FromBuiltInKeywords(ACCESS_KEY, commands);

                // create and start recording
                using PvRecorder recorder = PvRecorder.Create(-1, porcupine.FrameLength);
                recorder.Start();

                Console.WriteLine($"Using device: {recorder.SelectedDevice}");
                Console.WriteLine("Listening...");

                while (true)
                {
                    short[] pcm = recorder.Read();
                    int keywordIndex = porcupine.Process(pcm);
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
            });
        }
    }
}