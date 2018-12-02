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
        private IControl leftPatterntableControl;
        private IControl rightPatterntableControl;

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
            var rootPanel = (Grid)Content;
            leftPatterntableControl = rootPanel.Children[0];
            rightPatterntableControl = rootPanel.Children[1];
            viewModel = new PatterntableWindowViewModel(() =>
                        Dispatcher.UIThread.InvokeAsync(() =>
                        {
                            leftPatterntableControl.InvalidateVisual();
                            rightPatterntableControl.InvalidateVisual();
                        }).Wait());
            viewModel.FramesRunning = true;
            DataContext = viewModel;
        }

    }
}
