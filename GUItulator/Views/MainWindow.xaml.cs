﻿using System;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Threading;
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
            Activated += OnInitialized;
        }

        protected override void HandleClosed()
        {
            base.HandleClosed();
            viewModel.StopEmulation();
        }

        private void OnInitialized(object sender, EventArgs args)
        {
            var imageControl= ((DockPanel)Content).FindControl<Image>("BackBuffer");
            viewModel = new MainWindowViewModel(() => imageControl.InvalidateVisual());
            viewModel.Start();
            DataContext = viewModel;
        }
    }
}
