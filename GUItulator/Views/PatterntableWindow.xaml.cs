using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Threading;
using GUItulator.ViewModels;

namespace GUItulator.Views
{
    public class PatterntableWindow : Window
    {
        private PatterntableWindowViewModel viewModel;
        private IControl patterntableControl;

        public PatterntableWindow()
        {
            InitializeComponent();
            Closed += (e,args) => viewModel.FramesRunning = false;
#if DEBUG
            this.AttachDevTools();
#endif
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
            patterntableControl = (Image) Content;
            viewModel = new PatterntableWindowViewModel(() =>
                        Dispatcher.UIThread.InvokeAsync(() => patterntableControl.InvalidateVisual()).Wait());
            viewModel.FramesRunning = true;
            DataContext = viewModel;
        }

    }
}
