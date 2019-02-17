using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Threading;
using GUItulator.ViewModels;

namespace GUItulator.Views
{
    public partial class NametableWindow : Window
    {
        private NametableWindowViewModel viewModel;
        private Image[] nametables;

        public NametableWindow()
        {
            nametables = new Image[4];
            InitializeComponent();
            Closed += (e, args) => viewModel.Stop();
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
            var rootPanel = (Grid)Content;
            for (var i = 0; i < nametables.Length; i++)
            {
                nametables[i] =(Image)rootPanel.Children[i];
            }

            viewModel = new NametableWindowViewModel(() =>
                                Dispatcher.UIThread.InvokeAsync(() =>
                                {
                                    for (var i = 0; i < nametables.Length; i++)
                                    {
                                        nametables[i].InvalidateVisual();
                                    }
                                }).Wait(),20);
            viewModel.Start();
            DataContext = viewModel;
        }
    }
}
