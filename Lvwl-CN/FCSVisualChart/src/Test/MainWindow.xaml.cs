using FCSVisualChart;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Xml;
using System.Xml.Serialization;
using System.Linq;
using System.ComponentModel;
using System.Windows.Input;
using System.Diagnostics;
using System.Windows.Media;

namespace Test
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            this.chart.DataContext = new ChartDataContext();
            this.Loaded += MainWindow_Loaded;
        }

        Random random = new Random();

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void LoadData_Click(object sender, RoutedEventArgs e)
        {
            List<double> xs = new List<double>();
            List<double> ys = new List<double>();
            for (int i = 0; i < 50000; i++)
            {
                xs.Add(random.Next(1000, 50000));
                ys.Add(random.Next(1000, 50000));
            }
            ChartDataSource data = new ChartDataSource() { XSource = xs, YSource = ys };
            chart.DataSource = new System.Collections.ObjectModel.ObservableCollection<ChartDataSource>() { data };
        }

        private void ReadFCS_Click(object sender, RoutedEventArgs e)
        {
            var fcs = FCS.Factory.ReadFileOneDataset(@"D:\CasCyte\Data\CD483\Sample_003.fcs");
            ChartDataSource data1 = new ChartDataSource() { XSource = fcs.Measurements[0].ChannelValues, YSource = fcs.Measurements[3].ChannelValues };
            data1.DisplayColor = Color.FromArgb(0xFF, data1.DisplayColor.R, data1.DisplayColor.G, data1.DisplayColor.B);
            ChartDataSource data2 = new ChartDataSource() { XSource = fcs.Measurements[12].ChannelValues, YSource = fcs.Measurements[15].ChannelValues };
            data2.DisplayColor = Color.FromArgb(0xFF, data2.DisplayColor.R, data2.DisplayColor.G, data2.DisplayColor.B);
            chart.DataSource = new System.Collections.ObjectModel.ObservableCollection<ChartDataSource>() { data1, data2 };
        }

        private void Gate_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button b && b.CommandParameter is string str)
            {
                switch (str)
                {
                    case "SegmentX":
                        chart.Gates.Add(new SegmentXGate());
                        break;
                    case "Ellipse":
                        chart.Gates.Add(new EllipseGate());
                        break;
                    case "Polygon":
                        chart.Gates.Add(new PolygonGate());
                        break;
                    case "Rectangle":
                        chart.Gates.Add(new RectangleGate());
                        break;
                    case "Quadrant":
                        chart.Gates.Add(new QuadrantGate());
                        break;
                    case "LineHorizontal":
                        chart.Gates.Add(new LineHorizontalGate());
                        break;
                    case "LineVertical":
                        chart.Gates.Add(new LineVerticalGate());
                        break;
                    default:
                        break;
                }
            }
        }

        private void Axis_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button b && b.CommandParameter is string str)
            {
                switch (str)
                {
                    case "LineX":
                        chart.XAxis = new LinearAxis();
                        break;
                    case "LogX":
                        chart.XAxis = new NegativeLogAxis();
                        break;
                    case "LineY":
                        chart.YAxis = new LinearAxis();
                        break;
                    case "LogY":
                        chart.YAxis = new NegativeLogAxis();
                        break;
                    default:
                        break;
                }
            }
        }

        private void Series_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button b && b.CommandParameter is string str)
            {
                switch (str)
                {
                    case "Scatter":
                        chart.Series = new ScatterSeries();
                        break;
                    case "HistogramX":
                        chart.Series = new HistogramXStatisticsSeries() { FillColorA = 0x20, Smooth = true };
                        break;
                    case "Density":
                        chart.Series = new DensitySeries();
                        break;
                    case "PseudoColor":
                        chart.Series = new PseudoColorSeries();
                        break;
                    case "Contour":
                        chart.Series = new ContourSeries() { ShowFill = false };
                        break;
                    default:
                        break;
                }
            }
        }

        private void GateModel_Click(object sender, RoutedEventArgs e)
        {
            var gatexmls = this.chart.Gates.ToList().Select(p => SerializeToXml(p.GetModel())).ToArray();
            foreach (var gatexml in gatexmls)
            {
                MessageBox.Show(gatexml);
            }
        }


        public static string SerializeToXml<T>(T myObject) where T : class
        {
            if (myObject != null)
            {
                XmlSerializer xs = new XmlSerializer(typeof(T));
                using (MemoryStream stream = new MemoryStream())
                {
                    using (XmlTextWriter writer = new XmlTextWriter(stream, Encoding.UTF8) { Formatting = Formatting.None })
                    {
                        xs.Serialize(writer, myObject);
                        stream.Position = 0;
                        StringBuilder sb = new StringBuilder();
                        using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
                        {
                            string? line;
                            while ((line = reader.ReadLine()) != null) sb.Append(line);
                        }
                        writer.Close();
                        return sb.ToString();
                    }
                }
            }
            return string.Empty;
        }

        public static T? DeserializeToObject<T>(string tempString) where T : class
        {
            try
            {
                string xmlString = tempString;
                if (File.Exists(tempString))
                {
                    XmlDocument xmlDoc = new XmlDocument();
                    xmlDoc.Load(tempString);
                    xmlString = xmlDoc.InnerXml;
                }
                XmlSerializer serializer = new XmlSerializer(typeof(T));
                using (StringReader reader = new StringReader(xmlString))
                {
                    return (T?)serializer.Deserialize(reader);
                }
            }
            catch (Exception e)
            {

                return default;
            }
        }
    }

    public class ChartDataContext : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;

        public DelegateCommand GateChangedCommand { get; set; }
        public DelegateCommand GateFocusedCommand { get; set; }
        public DelegateCommand GateLostFocusedCommand { get; set; }

        public ChartDataContext()
        {
            GateChangedCommand = new DelegateCommand(GateChanged);
            GateFocusedCommand = new DelegateCommand(GateFocused);
            GateLostFocusedCommand = new DelegateCommand(GateLostFocused);
        }

        private void GateChanged(object? obj)
        {
            //Debug.WriteLine("门发生变化");
        }

        private void GateFocused(object? obj)
        {
            //Debug.WriteLine("门获取焦点");
        }

        private void GateLostFocused(object? obj)
        {
            //Debug.WriteLine("门失去焦点");
        }
    }

    public class DelegateCommand : ICommand
    {
        public event EventHandler? CanExecuteChanged;
        Action<object?> ExecuteAction;
        public DelegateCommand(Action<object?> executeAction)
        {
            this.ExecuteAction = executeAction;
        }
        public bool CanExecute(object? parameter)
        {
            return true;
        }

        public void Execute(object? parameter)
        {
            ExecuteAction?.Invoke(parameter);
        }
    }
}
