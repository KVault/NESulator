using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using Avalonia.Threading;
using GUItulator.Utils;
using GUItulator.Views;
using ReactiveUI;
using Serilog;

namespace GUItulator.ViewModels
{
    public class MainWindowViewModel : FPSWindowBase
    {
        private string cpuSpeed;
        public string CPUSpeed
        {
            get {return cpuSpeed;}
            set {this.RaiseAndSetIfChanged(ref cpuSpeed, value);}
        }

        private Thread emulatorThread;
        private WriteableBitmap BackBuffer {get;}
        private Size BackBufferSize {get;}
        public MainWindowViewModel(Action onFrameExecuted, int fps = 60) : base(onFrameExecuted, fps)
        {
            BackBuffer = new WriteableBitmap(new PixelSize(256,240),
                                             new Vector(96,96), PixelFormat.Rgba8888);
            BackBufferSize = BackBuffer.Size;
        }

        public MainWindowViewModel() : base (() => {}, 60){}

        public async void OpenROM()
        {
            var romFile = await new OpenFileDialog()
            {
                AllowMultiple = false
            }.ShowAsync();
            if (romFile != null && romFile.Length > 0)
            {
                StopEmulation();
                StartEmulation(romFile[0]);
            }
        }

        public void HardcodedRun() => StartEmulation(@"C:\dev\NESulator\NESulator\rom\donkey_kong.nes");
        public void OpenPatterntable() => new PatterntableWindow().Show();
        public void OpenNametable() => new NametableWindow().Show();
        public void OpenHexEditor() => new HexEditorWindow().Show();
        public void SetCPUSpeed(int speed) => CWrapper.CPUSpeed(speed);

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
            CPUSpeed = $"CPU Speed: {mhz}MHz";
            var backBuffer = BackBuffer;//So that we can use `ref`
            BitmapUtils.DrawBitmap(CWrapper.BackBuffer(), ref backBuffer, BackBufferSize);
        }
    }
}
