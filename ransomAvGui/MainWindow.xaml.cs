﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.InteropServices;
using System.Windows.Threading;
using System.IO;
using System.Diagnostics;
using System.ServiceProcess;
using System.Configuration.Install;

namespace ransomAvGui
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		private DispatcherTimer dt;
		public MainWindow()
		{
			InitializeComponent();

			dt = new DispatcherTimer();
			dt.Tick += new EventHandler(logReader_Tick);
			dt.Interval = new TimeSpan(0, 0, 0, 0, 250); // execute every second
			dt.Start();

		}
		 
		private void logReader_Tick(object sender, EventArgs e)
		{			
			string filename = "c:\\windows\\temp\\ransomav.log";
			if (File.Exists(filename))
			{
				Log.Text = System.Text.Encoding.UTF8.GetString((File.ReadAllBytes(filename)));
			}

		}

		private void DriverDetector_Clicked(object sender, RoutedEventArgs e)
		{
			bool is_checked = DriverDetector.IsChecked ?? false;

			if (is_checked)
			{				
				var process = new Process();
				process.StartInfo.UseShellExecute = false;
				process.StartInfo.CreateNoWindow = true;
				process.StartInfo.RedirectStandardOutput = true;
				process.StartInfo.RedirectStandardError = true;
				process.StartInfo.FileName = "cmd.exe";
				process.StartInfo.Arguments = "/c C:\\Windows\\System32\\InfDefaultInstall.exe C:\\Users\\user\\Desktop\\ransom_detector\\net6.0-windows\\ransom_detector.inf"; // where driverPath is path of .inf file
				process.Start();
				process.WaitForExit();
				process.Dispose();

				ServiceController controller = new ServiceController("ransom_detector");
				controller.Start();
			}
			else
			{
				
				ServiceInstaller ServiceInstallerObj = new ServiceInstaller();
				InstallContext Context = new InstallContext("log_file.txt", null);
				ServiceInstallerObj.Context = Context;
				ServiceInstallerObj.ServiceName = "ransom_detector";
				ServiceInstallerObj.Uninstall(null);				
			}
			
		}

		private void Hooker_Clicked(object sender, RoutedEventArgs e)
		{
			bool is_checked = Hooker.IsChecked ?? false;

			if (is_checked)
			{
			}
			else
			{
			}
		}
	}
}
