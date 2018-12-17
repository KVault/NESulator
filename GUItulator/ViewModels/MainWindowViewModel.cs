using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Threading;
using GUItulator.Views;
using ReactiveUI;
using Serilog;

namespace GUItulator.ViewModels
{
    public class MainWindowViewModel : FPSWindowBase
    {
        private string greeting;
        public string Greeting
        {
            get {return greeting;}
            set {this.RaiseAndSetIfChanged(ref greeting, value);}
        }

        private Thread emulatorThread;

        public MainWindowViewModel(Action onFrameExecuted, int fps = 60) : base(onFrameExecuted, fps){ }
        public MainWindowViewModel() : base (() => {}, 60){}

        public async void OpenROM()
        {
            var romFile = await new OpenFileDialog()
            {
                AllowMultiple = false,
                Filters = new List<FileDialogFilter>()
                {
                    new FileDialogFilter() {Extensions = {"nes"}}
                }
            }.ShowAsync(null);
            if (romFile != null && romFile.Length > 0)
            {
                StartEmulation(romFile[0]);
            }
        }

        public void HardcodedRun() => StartEmulation(@"C:\dev\NESulator\NESulator\rom\donkey_kong.nes");
        public void OpenPatterntable() => new PatterntableWindow().Show();
        public void OpenNametable() => new NametableWindow().Show();
        public void OpenHexEditor() => new HexEditorWindow().Show();

        public void StartEmulation(string fileName)
        {
            if (emulatorThread == null)
            {
                emulatorThread = new Thread(() => CWrapper.StartEmulation(fileName));
                emulatorThread.Start();
            }
        }

        public void StopEmulation()
        {
            try
            {
                CWrapper.StopEmulation();
            }
            catch (Exception e)
            {
                Log.Error(e, e.Message);
            }
        }

        protected override void Update()
        {
            var mhz = (CWrapper.CPUSpeed() / 1000000.0f).ToString("#.##");
            Greeting = $"CPU Speed: {mhz}MHz";
        }
    }
}
