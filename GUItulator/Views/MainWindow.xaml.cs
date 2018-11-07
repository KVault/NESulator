using System;
using System.Threading;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using GUItulator.ViewModels;

namespace GUItulator.Views
{
    public class MainWindow : Window
    {
        private MainWindowViewModel viewModel;
        public MainWindow()
        {
            InitializeComponent();
#if DEBUG
            this.AttachDevTools();
#endif
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
            this.Activated += OnInitialized;
        }

        private void OnInitialized(object sender, EventArgs args)
        {
            viewModel = DataContext as MainWindowViewModel;
            viewModel.StartPollCPUSpeed();

            new Thread(() => CWrapper.StartEmulation(@"C:\dev\NESulator\NESulator\rom\donkey_kong.nes")).Start();
        }
    }
}
