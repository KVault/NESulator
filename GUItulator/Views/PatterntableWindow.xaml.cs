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
        private Image leftPatterntableControl;
        private Image rightPatterntableControl;

        public PatterntableWindow()
        {
            InitializeComponent();
            Closed += (e, args) => viewModel.StopLoop();
#if DEBUG
            this.AttachDevTools();
#endif
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
            var rootPanel = (Grid)Content;
            leftPatterntableControl = (Image)rootPanel.Children[0];
            rightPatterntableControl = (Image)rootPanel.Children[1];
            viewModel = new PatterntableWindowViewModel(() =>
                        Dispatcher.UIThread.InvokeAsync(() =>
                        {
                            leftPatterntableControl.InvalidateVisual();
                            rightPatterntableControl.InvalidateVisual();
                        }).Wait(), 5);
            viewModel.StartLoop();
            DataContext = viewModel;
        }

    }
}
