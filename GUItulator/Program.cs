using GUItulator.ViewModels;
using GUItulator.Views;
using Avalonia;
using Avalonia.Logging.Serilog;

namespace GUItulator
{
    class Program
    {
        static void Main(string[] args)
        {
            BuildAvaloniaApp().Start<MainWindow>(() => new MainWindowViewModel());
        }

        public static AppBuilder BuildAvaloniaApp()
            => AppBuilder.Configure<App>()
                .UsePlatformDetect()
                .UseReactiveUI();
    }
}
