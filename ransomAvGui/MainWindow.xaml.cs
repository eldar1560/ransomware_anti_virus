using System;
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

namespace ransomAvGui
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{

		[DllImport("Setupapi.dll", EntryPoint = "InstallHinfSection", CallingConvention = CallingConvention.StdCall)]
		public static extern void InstallHinfSection(
		[In] IntPtr hwnd,
		[In] IntPtr ModuleHandle,
		[In, MarshalAs(UnmanagedType.LPWStr)] string CmdLineBuffer,
		int nCmdShow);
		DispatcherTimer dt;
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
				InstallHinfSection(IntPtr.Zero, IntPtr.Zero, "my path", 0);
			}
			else
			{
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
