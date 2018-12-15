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
    public class MainWindowViewModel : ViewModelBase
    {
        private string greeting;
        public string Greeting
        {
            get {return greeting;}
            set {this.RaiseAndSetIfChanged(ref greeting, value);}
        }

        private Thread emulatorThread;
        private bool isCPUPooling;

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

        /// <summary>
        /// Sets up a coroutine that runs once a second. It polls the CPU speed from the backend
        /// </summary>
        public void StartPollCPUSpeed()
        {
            isCPUPooling = true;
            new Thread(() =>
            {
                while (isCPUPooling)
                {
                    var mhz = (CWrapper.CPUSpeed() / 1000000.0f).ToString("#.##");
                    Greeting = $"CPU Speed: {mhz}MHz";
                    Thread.Sleep(1000);
                }

            }).Start();
        }

        /// <summary>
        /// Simply stops the thread. Will be done in a cleanup area so that no threads persists after the
        /// main app is killed
        /// </summary>
        public void StopPollCPUSpeed()
        {
            isCPUPooling = false;
        }

        public void StartEmulation(string fileName)
        {
            emulatorThread = new Thread(() => CWrapper.StartEmulation(fileName));
            emulatorThread.Start();
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
    }
}
