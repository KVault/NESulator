using System.Runtime.CompilerServices;
using System.Threading;
using Avalonia;
using Avalonia.Threading;
using ReactiveUI;

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

        /// <summary>
        /// Sets up a coroutine that runs once a second. It polls the CPU speed from the backend
        /// </summary>
        public void StartPollCPUSpeed()
        {
            new Thread(()=>
            {
                while (true)
                {
                    Thread.Sleep(1000);
                    var mhz = (CWrapper.CPUSpeed() / 1000000.0f).ToString("#.##");
                    Greeting = $"CPU Speed: {mhz}MHz";
                }

            }).Start();
        }
    }
}
