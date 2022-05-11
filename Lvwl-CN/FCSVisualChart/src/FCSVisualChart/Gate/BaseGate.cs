using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Xml.Serialization;

namespace FCSVisualChart
{
    public abstract class BaseGate : NotifyPropertyChanged, IDisposable
    {
        #region 外观
        private Brush fill = Brushes.Transparent;
        /// <summary>
        /// 门的填充颜色
        /// </summary>
        public Brush Fill
        {
            get { return fill; }
            set { fill = value; OnPropertyChanged(); }
        }
        private Brush stroke = Brushes.Black;
        /// <summary>
        /// 门的线条颜色
        /// </summary>
        public Brush Stroke
        {
            get { return stroke; }
            set { stroke = value; OnPropertyChanged(); }
        }
        private double strokeThickness = 1;
        /// <summary>
        /// 门的线条粗细
        /// </summary>
        public double StrokeThickness
        {
            get { return strokeThickness; }
            set { strokeThickness = value; OnPropertyChanged(); }
        }
        private double areaNameFontSize = 14d;
        /// <summary>
        /// 区域名称文字大小
        /// </summary>
        public double AreaNameFontSize
        {
            get { return areaNameFontSize; }
            set { areaNameFontSize = value; Draw(); }
        }
        private Typeface areaNameTypeface = new Typeface("Segoe UI");
        /// <summary>
        /// 区域名称文字字体
        /// </summary>
        public Typeface AreaNameTypeface
        {
            get { return areaNameTypeface; }
            set { areaNameTypeface = value; Draw(); }
        }
        #endregion

        /// <summary>
        /// 门划分的区域名称前缀
        /// </summary>
        public string ShortName { get; protected set; } = "G";
        /// <summary>
        /// 门所划分的区域数量
        /// </summary>
        public int AreaCount { get; protected set; } = 1;

        public BaseGate()
        {
            IsCreating = true;
            Init();
            Areas = new GateArea[AreaCount];
            for (int i = 0; i < AreaCount; i++)
            {
                var area = new GateArea() { OwnerGate = this };
                area.PropertyChanged += Area_PropertyChanged;
                Areas[i] = area;
            }
        }

        public BaseGate(BaseGateModel model)
        {
            Init();
            if (model.AreaNames != null && model.AreaNames.Length == AreaCount)
            {
                Areas = new GateArea[AreaCount];
                for (int i = 0; i < AreaCount; i++)
                {
                    Color color = ChartDataSource.RandomColor();
                    if (model.AreaColors != null && model.AreaColors.Length > i)
                    {
                        var colorString = model.AreaColors[i];
                        color = new Color() { A = Convert.ToByte(colorString.Substring(0, 2), 16), R = Convert.ToByte(colorString.Substring(2, 2), 16), G = Convert.ToByte(colorString.Substring(4, 2), 16), B = Convert.ToByte(colorString.Substring(6, 2), 16) };
                    }
                    Areas[i] = new GateArea() { Name = model.AreaNames[i], DisplayColor = color, OwnerGate = this };
                }
                Chart.AddExistedGraphicalName(this.ShortName, model.AreaNames.ToArray());
            }
            else
            {
                Areas = new GateArea[AreaCount];
                for (int i = 0; i < AreaCount; i++) Areas[i] = new GateArea() { OwnerGate = this };
            }
            //区域名称发生变化时，需要重新绘制门图形
            foreach (var area in Areas) area.PropertyChanged += Area_PropertyChanged;
        }

        /// <summary>
        /// 区域名称改变，重新绘制门图形
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Area_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (nameof(GateArea.Name).Equals(e.PropertyName)) Draw();
        }

        /// <summary>
        /// 初始化
        /// </summary>
        protected virtual void Init() { }
        private Chart ownerChart;
        /// <summary>
        /// 所属chart控件
        /// </summary>
        public Chart OwnerChart
        {
            get { return ownerChart; }
            internal set
            {
                ownerChart = value;
                if (value != null && Areas != null)
                {
                    foreach (var area in Areas) if (string.IsNullOrEmpty(area.Name)) area.Name = value.CreateNewAreaNameFunction(this);
                }
            }
        }
        /// <summary>
        /// 门划分的区域集合
        /// </summary>
        public GateArea[] Areas { get; internal set; }

        #region 容器鼠标事件-用于门的创建
        private bool isCreating;
        /// <summary>
        /// 是否正在创建中
        /// </summary>
        public bool IsCreating
        {
            get { return isCreating; }
            protected set
            {
                isCreating = value;
                OnPropertyChanged();
                if (!value)
                {
                    if (OwnerChart != null) GatePath.Focus();
                    DrawControl();
                    RefreshAreaIndexs();
                }
            }
        }
        private Path gatePath;
        /// <summary>
        /// 门的显示图形
        /// </summary>
        protected Path GatePath
        {
            get { return gatePath; }
            set
            {
                gatePath = value;
                if (value != null) OwnerChart.ViewPanel.Children.Add(value);
            }
        }
        /// <summary>
        /// 容器鼠标按下
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        internal abstract void Panel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e);
        /// <summary>
        /// 容器鼠标移动
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        internal abstract void Panel_MouseMove(object sender, MouseEventArgs e);
        /// <summary>
        /// 容器鼠标抬起
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        internal abstract void Panel_MouseLeftButtonUp(object sender, MouseButtonEventArgs e);
        #endregion

        #region 门绘制
        /// <summary>
        /// 创建门的矢量数据
        /// </summary>
        /// <returns></returns>
        protected abstract Geometry CreateGatePathData();
        /// <summary>
        /// 绘制门图形
        /// </summary>
        internal virtual void Draw()
        {
            if (OwnerChart == null || !OwnerChart.IsLoaded || OwnerChart.XAxis == null || !OwnerChart.XAxis.IsLoaded || OwnerChart.XAxis.ActualWidth == 0 || OwnerChart.YAxis == null || !OwnerChart.YAxis.IsLoaded || OwnerChart.YAxis.ActualHeight == 0) return;
            if (GatePath == null)
            {
                var temp = new Path() { Cursor = Cursors.Hand, RenderTransform = new MatrixTransform(), Focusable = true };
                temp.SetBinding(Shape.FillProperty, new Binding("Fill") { Source = this, Mode = BindingMode.OneWay });
                temp.SetBinding(Shape.StrokeProperty, new Binding("Stroke") { Source = this, Mode = BindingMode.OneWay });
                temp.SetBinding(Shape.StrokeThicknessProperty, new Binding("StrokeThickness") { Source = this, Mode = BindingMode.OneWay });
                //temp.SetBinding(UIElement.FocusableProperty, new Binding("CanChangeGate") { Source = OwnerChart, Mode = BindingMode.OneWay });
                temp.MouseLeftButtonDown += GatePath_MouseLeftButtonDown;
                temp.MouseMove += GatePath_MouseMove;
                temp.MouseLeftButtonUp += GatePath_MouseLeftButtonUp;
                temp.GotFocus += GatePath_GotFocus;
                temp.LostFocus += GatePath_LostFocus;
                GatePath = temp;
            }
            if (GatePath.RenderTransform is MatrixTransform transform) transform.Matrix = new Matrix();
            GatePath.Data = CreateGatePathData();
            if (!IsCreating) DrawControl();
        }

        private void GatePath_GotFocus(object sender, RoutedEventArgs e)
        {
            if (OwnerChart.GateGotFocusCommand != null && OwnerChart.GateGotFocusCommand.CanExecute(this)) OwnerChart.GateGotFocusCommand.Execute(this);
        }

        private void GatePath_LostFocus(object sender, RoutedEventArgs e)
        {
            if (OwnerChart.GateLostFocusCommand != null && OwnerChart.GateLostFocusCommand.CanExecute(this)) OwnerChart.GateLostFocusCommand.Execute(this);
        }

        /// <summary>
        /// 创建文本矢量数据流
        /// </summary>
        /// <param name="str">文本字符串</param>
        /// <param name="offsetX">x轴偏移量</param>
        /// <param name="offsetY">y轴偏移量</param>
        /// <param name="ftwp">文本反向偏移的文本宽度的倍数</param>
        /// <param name="fthp">文本反向偏移的文本高度的倍数</param>
        /// <returns></returns>
        protected Geometry CreateFormattedTextGeometry(string str, double offsetX, double offsetY, double ftwp = 0.5d, double fthp = 0.5d)
        {
            var ft = new FormattedText(str, System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, AreaNameTypeface, AreaNameFontSize, Brushes.Black, null, VisualTreeHelper.GetDpi(OwnerChart).PixelsPerDip);
            var ftg = ft.BuildGeometry(new Point(0, 0));
            ftg.Transform = new MatrixTransform(1, 0, 0, 1, offsetX - ft.Width * ftwp, offsetY - ft.Height * fthp);
            ftg.Freeze();
            return ftg;
        }
        /// <summary>
        /// 删除门
        /// </summary>
        public void Dispose()
        {
            if (OwnerChart != null && OwnerChart.ViewPanel != null)
            {
                if (GatePath != null && OwnerChart.ViewPanel.Children.Contains(GatePath)) { OwnerChart.ViewPanel.Children.Remove(GatePath); GatePath = null; }
                if (ControlPaths.Count > 0)
                {
                    foreach (var path in ControlPaths)
                    {
                        if (OwnerChart.ViewPanel.Children.Contains(path)) OwnerChart.ViewPanel.Children.Remove(path);
                    }
                }
                if (OwnerChart.Gates.Contains(this)) OwnerChart.Gates.Remove(this);
                if (Areas != null && Areas.Length > 0)
                {
                    foreach (var area in Areas)
                    {
                        if (area == null) continue;
                        if (OwnerChart.DataSource != null)
                            foreach (var item in OwnerChart.DataSource)
                            {
                                if (item.AreaIndexs != null && item.AreaIndexs.ContainsKey(area)) item.AreaIndexs.Remove(area);
                            }
                    }
                }
            }
        }
        #endregion

        #region 门控制
        /// <summary>
        /// 是否正在编辑中
        /// </summary>
        internal bool IsEditing { get; set; }
        private ObservableCollection<Path> controlPaths;
        /// <summary>
        /// 控制图形的点位图形
        /// </summary>
        protected ObservableCollection<Path> ControlPaths
        {
            get
            {
                if (controlPaths == null)
                {
                    controlPaths = new ObservableCollection<Path>();
                    controlPaths.CollectionChanged += (sender, e) =>
                    {
                        switch (e.Action)
                        {
                            case System.Collections.Specialized.NotifyCollectionChangedAction.Add:
                                foreach (var item in e.NewItems)
                                {
                                    if (item is Path path)
                                    {
                                        path.SetBinding(Shape.FillProperty, new Binding("Stroke") { Source = this, Mode = BindingMode.OneWay });
                                        path.SetBinding(UIElement.VisibilityProperty, new Binding("IsFocused") { Source = this.GatePath, Converter = Converter.BoolToVisibilityConverter.Converter });
                                        path.MouseLeftButtonDown += ControlPath_MouseLeftButtonDown;
                                        path.MouseLeftButtonUp += ControlPath_MouseLeftButtonUp;
                                        OwnerChart.ViewPanel.Children.Add(path);
                                    }
                                }
                                break;
                            case System.Collections.Specialized.NotifyCollectionChangedAction.Remove:
                                foreach (var item in e.OldItems)
                                {
                                    if (item is Path path)
                                    {
                                        path.MouseLeftButtonDown -= ControlPath_MouseLeftButtonDown;
                                        path.MouseLeftButtonUp -= ControlPath_MouseLeftButtonUp;
                                        OwnerChart.ViewPanel.Children.Remove(path);
                                    }
                                }
                                break;
                            case System.Collections.Specialized.NotifyCollectionChangedAction.Replace:
                                break;
                            case System.Collections.Specialized.NotifyCollectionChangedAction.Move:
                                break;
                            case System.Collections.Specialized.NotifyCollectionChangedAction.Reset:
                                break;
                            default:
                                break;
                        }
                    };
                }
                return controlPaths;
            }
        }
        /// <summary>
        /// 绘制控制点
        /// </summary>
        protected abstract void DrawControl();
        /// <summary>
        /// 编辑中的控制点
        /// </summary>
        internal Path EditingPath { get; set; }
        /// <summary>
        /// 门控制点按下
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void ControlPath_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (OwnerChart.CanChangeGate)
            {
                GateMoveStartPoint = default;
                IsEditing = true;
                IsMoving = false;
                EditingPath = sender as Path;
                e.Handled = true;
            }
        }
        /// <summary>
        /// 门控制按钮鼠标抬起
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void ControlPath_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (OwnerChart.CanChangeGate)
            {
                IsEditing = false;
                EditingPath = null;
                e.Handled = true;
                RefreshAreaIndexs();
            }
        }
        /// <summary>
        /// 创建控制点的矢量数据流
        /// </summary>
        /// <param name="point"></param>
        /// <returns></returns>
        protected virtual Geometry CreateControlGeometry(Point point)
        {
            return new EllipseGeometry(point, 5, 5);
        }
        #endregion

        #region 门移动
        /// <summary>
        /// 移动门
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="changeAxisValue">是否变更了坐标值</param>
        internal abstract void Move(double x, double y, bool changeAxisValue = false);
        /// <summary>
        /// 门移动的起始点
        /// </summary>
        protected Point GateMoveStartPoint { get; set; }
        /// <summary>
        /// 是否正在移动门
        /// </summary>
        internal bool IsMoving { get; set; }
        protected virtual void GatePath_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            GatePath.Focus();
            if (OwnerChart != null && OwnerChart.CanChangeGate)
            {
                GateMoveStartPoint = e.GetPosition(OwnerChart.ViewPanel);
                IsMoving = true;
            }
            e.Handled = true;
        }
        protected virtual void GatePath_MouseMove(object sender, MouseEventArgs e)
        {
            if (IsMoving && e.LeftButton == MouseButtonState.Pressed)
            {
                if (GateMoveStartPoint != default)
                {
                    var point = e.GetPosition(OwnerChart.ViewPanel);
                    var x = point.X - GateMoveStartPoint.X;
                    var y = point.Y - GateMoveStartPoint.Y;
                    GateMoveStartPoint = point;
                    Move(x, y, true);
                    e.Handled = true;
                }
            }
            else GateMoveStartPoint = default;
        }
        protected virtual void GatePath_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (IsMoving)
            {
                IsMoving = false;
                GateMoveStartPoint = default;
                RefreshAreaIndexs();
            }
        }
        #endregion

        #region 区域数据过滤
        internal virtual void RefreshAreaIndexs()
        {
            if (OwnerChart != null)
            {
                if (OwnerChart.DataSource != null) foreach (var item in OwnerChart.DataSource) RefreshAreaIndexs(item);
                if (OwnerChart.GateChangedCommand != null && OwnerChart.GateChangedCommand.CanExecute(this)) OwnerChart.GateChangedCommand.Execute(this);
            }
        }
        /// <summary>
        /// 数据源过滤token集合
        /// </summary>
        protected Dictionary<ChartDataSource, CancellationTokenSource> RefreshAreaCancellationTokenSources { get; } = new Dictionary<ChartDataSource, CancellationTokenSource>();
        /// <summary>
        /// 刷新门内区域数据
        /// </summary>
        /// <param name="chartDataSource"></param>
        internal virtual async void RefreshAreaIndexs(ChartDataSource chartDataSource)
        {
            if (RefreshAreaCancellationTokenSources.ContainsKey(chartDataSource) && RefreshAreaCancellationTokenSources[chartDataSource] != null && !RefreshAreaCancellationTokenSources[chartDataSource].IsCancellationRequested) RefreshAreaCancellationTokenSources[chartDataSource].Cancel();
            var token = new CancellationTokenSource();
            RefreshAreaCancellationTokenSources[chartDataSource] = token;
            await RefreshAreaIndexs(chartDataSource, token.Token, OwnerChart.Series.MaxDegreeOfParallelism);
            token.Dispose();
            if (RefreshAreaCancellationTokenSources.ContainsKey(chartDataSource) && RefreshAreaCancellationTokenSources[chartDataSource] == token) RefreshAreaCancellationTokenSources.Remove(chartDataSource);
        }
        protected virtual async Task<bool> RefreshAreaIndexs(ChartDataSource chartDataSource, CancellationToken token, int maxDegreeOfParallelism)
        {
            var p = GetAreaIndexsFuncParam();
            ConcurrentDictionary<GateArea, ConcurrentBag<int>> AreaIndexs = new ConcurrentDictionary<GateArea, ConcurrentBag<int>>();
            foreach (var area in p.Areas) AreaIndexs[area] = new ConcurrentBag<int>();
            try
            {
                var response = await Task.Run(() =>
                {
                    if (chartDataSource == null || chartDataSource.XSource == null || p == null) return false;
                    if (chartDataSource.Indexs == null)
                    {
                        try
                        {
                            var count = chartDataSource.XSource.Count;
                            var result = Parallel.For(0, count, new ParallelOptions() { CancellationToken = token, MaxDegreeOfParallelism = maxDegreeOfParallelism }, (i, loop) =>
                            {
                                if (loop.IsStopped) return;
                                if (token.IsCancellationRequested) loop.Stop();
                                if (chartDataSource.XSource.Count > i)
                                {
                                    var x = chartDataSource.XSourceConverter(chartDataSource.XSource[i]);
                                    var y = (chartDataSource.YSource == null || chartDataSource.YSource.Count <= i) ? double.NaN : chartDataSource.YSourceConverter(chartDataSource.YSource[i]);
                                    var area = DataInArea(x, y, p);
                                    if (area != null && AreaIndexs.ContainsKey(area)) AreaIndexs[area].Add(i);
                                }
                            });
                            if (result.IsCompleted) { return true; }
                        }
                        catch (OperationCanceledException) { }
                    }
                    else
                    {
                        try
                        {
                            var result = Parallel.ForEach(chartDataSource.Indexs, new ParallelOptions() { CancellationToken = token, MaxDegreeOfParallelism = maxDegreeOfParallelism }, (i, loop) =>
                            {
                                if (loop.IsStopped) return;
                                if (token.IsCancellationRequested) loop.Stop();
                                if (chartDataSource.XSource.Count > i)
                                {
                                    var x = chartDataSource.XSourceConverter(chartDataSource.XSource[i]);
                                    var y = (chartDataSource.YSource == null || chartDataSource.YSource.Count <= i) ? double.NaN : chartDataSource.YSourceConverter(chartDataSource.YSource[i]);
                                    var area = DataInArea(x, y, p);
                                    if (area != null && AreaIndexs.ContainsKey(area)) AreaIndexs[area].Add(i);
                                }
                            });
                            if (result.IsCompleted) { return true; }
                        }
                        catch (OperationCanceledException) { }
                    }
                    return false;
                }, token);
                if (response)
                {
                    foreach (var area in AreaIndexs.Keys) chartDataSource.AreaIndexs[area] = AreaIndexs[area].ToArray();
                }
                else
                {
                    //foreach (var area in AreaIndexs.Keys) if (chartDataSource.AreaIndexs.ContainsKey(area)) chartDataSource.AreaIndexs.Remove(area);
                }
                return response;
            }
            catch (TaskCanceledException)
            {
                //foreach (var area in AreaIndexs.Keys) if (chartDataSource.AreaIndexs.ContainsKey(area)) chartDataSource.AreaIndexs.Remove(area);
                return false;
            }
        }
        /// <summary>
        /// 数据在门的哪个区域内
        /// </summary>
        /// <param name="x">x轴真实值</param>
        /// <param name="y">y轴真实值</param>
        /// <param name="param">数据在区域内判断方法的参数</param>
        /// <returns>null表示不在任何区域内</returns>
        public abstract GateArea DataInArea(double x, double y, AreaIndexsFuncParam param);
        /// <summary>
        /// 获取判断点是否在门区域内的方法的参数
        /// </summary>
        /// <returns></returns>
        public abstract AreaIndexsFuncParam GetAreaIndexsFuncParam();
        /// <summary>
        /// 部分图形在轴更换后需要重新绘制和更新区域数据
        /// </summary>
        internal virtual void AxisChanged()
        {
            Draw();
        }
        #endregion

        #region 数据模型
        /// <summary>
        /// 获取门的模型
        /// </summary>
        /// <returns></returns>
        public abstract BaseGateModel GetModel();
        /// <summary>
        /// 根据模型创建门对象
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public static BaseGate CreateByModel(BaseGateModel model)
        {
            if (model is SegmentXGateModel sm) return new SegmentXGate(sm);
            else if (model is EllipseGateModel em) return new EllipseGate(em);
            else if (model is LineHorizontalGateModel hm) return new LineHorizontalGate(hm);
            else if (model is LineVerticalGateModel vm) return new LineVerticalGate(vm);
            else if (model is PolygonGateModel pm) return new PolygonGate(pm);
            else if (model is QuadrantGateModel qm) return new QuadrantGate(qm);
            else if (model is RectangleGateModel rm) return new RectangleGate(rm);
            return null;
        }
        #endregion
    }


    /// <summary>
    /// 门的数据模型
    /// </summary>
    [XmlInclude(typeof(SegmentXGateModel))]
    [XmlInclude(typeof(EllipseGateModel))]
    [XmlInclude(typeof(LineHorizontalGateModel))]
    [XmlInclude(typeof(LineVerticalGateModel))]
    [XmlInclude(typeof(PolygonGateModel))]
    [XmlInclude(typeof(QuadrantGateModel))]
    [XmlInclude(typeof(RectangleGateModel))]
    public abstract class BaseGateModel
    {
        /// <summary>
        /// 区域名称
        /// </summary>
        public string[] AreaNames { get; set; }
        /// <summary>
        /// 区域颜色
        /// </summary>
        public string[] AreaColors { get; set; }
    }

    /// <summary>
    /// 判断点是否在门的区域内的判断方法参数
    /// </summary>
    public class AreaIndexsFuncParam
    {
        internal GateArea[] Areas { get; set; }
    }
}
