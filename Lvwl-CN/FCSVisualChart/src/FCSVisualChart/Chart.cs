using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace FCSVisualChart
{
    [TemplatePart(Name = "Part_Panel", Type = typeof(Panel))]
    [TemplatePart(Name = "Part_Threshold", Type = typeof(Path))]
    public class Chart : Control
    {
        static Chart()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(Chart), new FrameworkPropertyMetadata(typeof(Chart)));
        }

        public Chart()
        {
            this.Loaded += (sender, e) =>
            {
                if (Gates == null) Gates = new ObservableCollection<BaseGate>();
                else foreach (var gate in Gates) gate.Draw();
                DrawThresholdGeometry();
            };
        }

        public Chart(ChartModel model) : this()
        {
            this.MoveType = model.MoveType;
            this.ZoomType = model.ZoomType;
            this.XAxis = IAxis.CreateByModel(model.XAxis);
            this.YAxis = IAxis.CreateByModel(model.YAxis);
            this.Series = ISeries.CreateByModel(model.Series);
            if (model.Gates != null) this.Gates = new ObservableCollection<BaseGate>(model.Gates.Select(p => BaseGate.CreateByModel(p)).Where(p => p != null));
        }

        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);
            if (IsLoaded)
            {
                if (XAxis != null) { XAxis.Draw(); XAxis.DrawGain(); }
                if (YAxis != null) { YAxis.Draw(); YAxis.DrawGain(); }
                if (Series != null) Series.Draw();
                if (Gates != null) foreach (var gate in Gates) gate.Draw();
                DrawThresholdGeometry();
            }
        }

        public Panel ViewPanel { get; internal set; }
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            var temp = GetTemplateChild("Part_Panel");
            if (temp != null && temp is Panel p)
            {
                ViewPanel = p;
                ViewPanel.MouseLeftButtonDown += Panel_MouseLeftButtonDown;
                ViewPanel.MouseLeftButtonUp += Panel_MouseLeftButtonUp;
                ViewPanel.MouseMove += Panel_MouseMove;
                ViewPanel.MouseWheel += Panel_MouseWheel;
            }
            var threshold = GetTemplateChild("Part_Threshold");
            if (threshold != null && threshold is Path thresholdPath)
            {
                thresholdPath.MouseLeftButtonDown += (sender, e) => { ThresholdIsMoving = true; };
            }
        }

        #region 鼠标控制
        /// <summary>
        /// 移动方式
        /// </summary>
        public ChangeType MoveType
        {
            get { return (ChangeType)GetValue(MoveTypeProperty); }
            set { SetValue(MoveTypeProperty, value); }
        }
        public static readonly DependencyProperty MoveTypeProperty = DependencyProperty.Register("MoveType", typeof(ChangeType), typeof(Chart), new PropertyMetadata(ChangeType.XY));

        /// <summary>
        /// 缩放方式
        /// </summary>
        public ChangeType ZoomType
        {
            get { return (ChangeType)GetValue(ZoomTypeProperty); }
            set { SetValue(ZoomTypeProperty, value); }
        }
        public static readonly DependencyProperty ZoomTypeProperty = DependencyProperty.Register("ZoomType", typeof(ChangeType), typeof(Chart), new PropertyMetadata(ChangeType.XY));

        /// <summary>
        /// 移动图形的起始点
        /// </summary>
        protected Point MoveStartPoint { get; set; }
        /// <summary>
        /// 鼠标左键按下
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void Panel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            var creatingGate = Gates.FirstOrDefault(p => p.IsCreating);
            if (creatingGate != null)
            {
                creatingGate.Panel_MouseLeftButtonDown(sender, e);
                e.Handled = true;
                return;
            }
            MoveStartPoint = e.GetPosition(ViewPanel);
            this.Focus();
        }
        /// <summary>
        /// 鼠标移动
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void Panel_MouseMove(object sender, MouseEventArgs e)
        {
            var editingGate = CanChangeGate ? Gates.FirstOrDefault(p => p.IsCreating || p.IsEditing || p.IsMoving) : Gates.FirstOrDefault(p => p.IsCreating);
            if (editingGate != null)
            {
                editingGate.Panel_MouseMove(sender, e);
                e.Handled = true;
                return;
            }
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                if (ThresholdIsMoving)
                {
                    var point = e.GetPosition(ViewPanel);
                    var value = XAxis.GetLocationValue(point.X);
                    if (value >= ThresholdMinValue && value <= ThresholdMaxValue) ThresholdValue = value;
                    e.Handled = true;
                }
                else if (MoveStartPoint != default)
                {
                    var point = e.GetPosition(ViewPanel);
                    var x = point.X - MoveStartPoint.X;
                    var y = point.Y - MoveStartPoint.Y;
                    MoveStartPoint = point;
                    Move(x, y, MoveType);
                }
            }
            else
            {
                MoveStartPoint = default;
                ThresholdIsMoving = false;
            }
        }
        /// <summary>
        /// 鼠标抬起
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void Panel_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            var editingGate = CanChangeGate ? Gates.FirstOrDefault(p => p.IsCreating || p.IsEditing || p.IsMoving) : Gates.FirstOrDefault(p => p.IsCreating);
            if (editingGate != null)
            {
                editingGate.Panel_MouseLeftButtonUp(sender, e);
                e.Handled = true;
                return;
            }
            if (ThresholdIsMoving)
            {
                ThresholdIsMoving = false;
                if (ThresholdValueChangedCommand != null && ThresholdValueChangedCommand.CanExecute(ThresholdValue)) ThresholdValueChangedCommand.Execute(ThresholdValue);
            }
            else if (MoveStartPoint != default)
            {
                Series?.Draw();
                MoveStartPoint = default;
            }
        }
        /// <summary>
        /// 鼠标滚动
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void Panel_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (ZoomType == ChangeType.None) return;
            var point = e.GetPosition(ViewPanel);
            var percent = e.Delta > 0 ? 1.1d : 0.9d;
            Zoom(point, percent, ZoomType);
            e.Handled = true;
        }

        /// <summary>
        /// 移动图形
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="moveType"></param>
        internal virtual void Move(double x, double y, ChangeType moveType)
        {
            if (moveType == ChangeType.None) return;
            switch (moveType)
            {
                case ChangeType.X:
                    XAxis?.Move(x, 0);
                    y = 0;
                    break;
                case ChangeType.Y:
                    YAxis?.Move(0, y);
                    x = 0;
                    break;
                case ChangeType.XY:
                    XAxis?.Move(x, y);
                    YAxis?.Move(x, y);
                    break;
                default:
                    break;
            }
            Series?.Move(x, y);
            foreach (var gate in Gates) gate.Move(x, y);
            DrawThresholdGeometry();
        }

        /// <summary>
        /// 缩放图形
        /// </summary>
        /// <param name="point"></param>
        /// <param name="percent"></param>
        /// <param name="zoomType"></param>
        internal virtual void Zoom(Point point, double percent, ChangeType zoomType)
        {
            if (zoomType == ChangeType.None) return;
            switch (zoomType)
            {
                case ChangeType.X:
                    XAxis?.Zoom(percent, point);
                    break;
                case ChangeType.Y:
                    YAxis?.Zoom(percent, point);
                    break;
                case ChangeType.XY:
                    XAxis?.Zoom(percent, point);
                    YAxis?.Zoom(percent, point);
                    break;
                default:
                    break;
            }
            Series?.Draw();
            foreach (var gate in Gates) gate.Draw();
            DrawThresholdGeometry();
        }
        #endregion

        #region 数据源
        /// <summary>
        /// 数据源，多数据源集合
        /// </summary>
        public ObservableCollection<ChartDataSource> DataSource
        {
            get { return (ObservableCollection<ChartDataSource>)GetValue(DataSourceProperty); }
            set { SetValue(DataSourceProperty, value); }
        }
        public static readonly DependencyProperty DataSourceProperty = DependencyProperty.Register("DataSource", typeof(ObservableCollection<ChartDataSource>), typeof(Chart), new PropertyMetadata((d, e) =>
        {
            if (d is Chart chart)
            {
                if (e.OldValue is ObservableCollection<ChartDataSource> olddatasources)
                {
                    olddatasources.CollectionChanged -= chart.DataSources_CollectionChanged;
                    foreach (var dataSource in olddatasources)
                    {
                        dataSource.PropertyChanged -= chart.DataSource_PropertyChanged;
                        dataSource.AreaIndexs.CollectionChanged -= chart.AreaIndexs_CollectionChanged;
                        if (chart.Series != null) chart.Series.RemoveChartDataStream(dataSource);
                    }
                }
                if (e.NewValue is ObservableCollection<ChartDataSource> newdatasources)
                {
                    newdatasources.CollectionChanged += chart.DataSources_CollectionChanged;
                    foreach (var dataSource in newdatasources)
                    {
                        dataSource.PropertyChanged += chart.DataSource_PropertyChanged;
                        dataSource.AreaIndexs.CollectionChanged += chart.AreaIndexs_CollectionChanged;
                        if (chart.Series != null) chart.Series.UpdateChartDataStream(dataSource);
                        if (chart.Gates != null && chart.Gates.Count > 0) foreach (var gate in chart.Gates) gate.RefreshAreaIndexs(dataSource);
                    }
                }
            }
        }));

        private void DataSources_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (this.Series != null)
            {
                switch (e.Action)
                {
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Add:
                        foreach (var item in e.NewItems)
                        {
                            if (item is ChartDataSource dataSource)
                            {
                                dataSource.PropertyChanged += DataSource_PropertyChanged;
                                dataSource.AreaIndexs.CollectionChanged += AreaIndexs_CollectionChanged;
                                this.Series.UpdateChartDataStream(dataSource);
                                if (Gates != null && Gates.Count > 0) foreach (var gate in Gates) gate.RefreshAreaIndexs(dataSource);
                            }
                        }
                        break;
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Remove:
                        foreach (var item in e.OldItems)
                        {
                            if (item is ChartDataSource dataSource)
                            {
                                dataSource.PropertyChanged -= DataSource_PropertyChanged;
                                dataSource.AreaIndexs.CollectionChanged -= AreaIndexs_CollectionChanged;
                                this.Series.RemoveChartDataStream(dataSource);
                            }
                        }
                        break;
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Replace:
                        foreach (var item in e.OldItems)
                        {
                            if (item is ChartDataSource dataSource)
                            {
                                dataSource.PropertyChanged -= DataSource_PropertyChanged;
                                dataSource.AreaIndexs.CollectionChanged -= AreaIndexs_CollectionChanged;
                                this.Series.RemoveChartDataStream(dataSource);
                            }
                        }
                        foreach (var item in e.NewItems)
                        {
                            if (item is ChartDataSource dataSource)
                            {
                                dataSource.PropertyChanged += DataSource_PropertyChanged;
                                dataSource.AreaIndexs.CollectionChanged += AreaIndexs_CollectionChanged;
                                this.Series.UpdateChartDataStream(dataSource);
                                if (Gates != null && Gates.Count > 0) foreach (var gate in Gates) gate.RefreshAreaIndexs(dataSource);
                            }
                        }
                        break;
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Move:
                        this.Series.Draw(this.Series.ChartDataStreams);
                        break;
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Reset:
                        var temps = this.Series.ChartDataStreams.Keys.ToArray();
                        foreach (var dataSource in temps)
                        {
                            dataSource.PropertyChanged -= DataSource_PropertyChanged;
                            dataSource.AreaIndexs.CollectionChanged -= AreaIndexs_CollectionChanged;
                            this.Series.RemoveChartDataStream(dataSource);
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        private void DataSource_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                if (sender is ChartDataSource dataSource && this.Series != null && this.Series.ChartDataStreams.ContainsKey(dataSource))
                {
                    switch (e.PropertyName)
                    {
                        case "XSource":
                        case "YSource":
                        case "Indexs":
                        case "XSourceConverter":
                        case "YSourceConverter":
                            this.Series.UpdateChartDataStream(dataSource);
                            if (Gates != null && Gates.Count > 0) foreach (var gate in Gates) gate.RefreshAreaIndexs(dataSource);
                            break;
                        case "DisplayIndexs":
                            this.Series.UpdateChartDataStream(dataSource);
                            break;
                        case "DisplayColor":
                            this.Series.Draw(this.Series.ChartDataStreams);
                            break;
                        case "AreaIndexs":
                            dataSource.AreaIndexs.CollectionChanged += AreaIndexs_CollectionChanged;
                            break;
                        default:
                            break;
                    }
                }
            });
        }

        private async void AreaIndexs_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (this.ShowGateAreaGeometry && this.Series != null && sender is ObservableDictionary<GateArea, IList<int>> gateAreas)
            {
                var dataSource = DataSource.FirstOrDefault(p => p.AreaIndexs == gateAreas);
                if (dataSource == null) return;
                switch (e.Action)
                {
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Add:
                        {
                            foreach (var item in e.NewItems) if (item is KeyValuePair<GateArea, IList<int>> kv) await this.Series.UpdateGateAreaStream(dataSource, kv.Key);
                            this.Series.Draw(this.Series.ChartDataStreams);
                        }
                        break;
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Remove:
                        {
                            foreach (var item in e.OldItems) if (item is KeyValuePair<GateArea, IList<int>> kv) this.Series.RemoveGateAreaStream(dataSource, kv.Key);
                            this.Series.Draw(this.Series.ChartDataStreams);
                        }
                        break;
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Replace:
                        {
                            foreach (var item in e.NewItems) if (item is KeyValuePair<GateArea, IList<int>> kv) await this.Series.UpdateGateAreaStream(dataSource, kv.Key);
                            this.Series.Draw(this.Series.ChartDataStreams);
                        }
                        break;
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Move:
                        break;
                    case System.Collections.Specialized.NotifyCollectionChangedAction.Reset:
                        this.Series.ClearGateAreaStream(dataSource);
                        this.Series.Draw(this.Series.ChartDataStreams);
                        break;
                    default:
                        break;
                }
            }
        }

        #endregion

        #region 图的轴和显示内容
        /// <summary>
        /// x轴
        /// </summary>
        public IAxis XAxis
        {
            get { return (IAxis)GetValue(XAxisProperty); }
            set { SetValue(XAxisProperty, value); }
        }
        public static readonly DependencyProperty XAxisProperty = DependencyProperty.Register("XAxis", typeof(IAxis), typeof(Chart), new PropertyMetadata((d, e) => { if (e.NewValue is IAxis axis && d is Chart chart) { axis.AxisType = AxisType.X; axis.OwnerChart = chart; } }));
        /// <summary>
        /// y轴
        /// </summary>
        public IAxis YAxis
        {
            get { return (IAxis)GetValue(YAxisProperty); }
            set { SetValue(YAxisProperty, value); }
        }
        public static readonly DependencyProperty YAxisProperty = DependencyProperty.Register("YAxis", typeof(IAxis), typeof(Chart), new PropertyMetadata((d, e) => { if (e.NewValue is IAxis axis && d is Chart chart) { axis.AxisType = AxisType.Y; axis.OwnerChart = chart; } }));

        /// <summary>
        /// 显示的图
        /// </summary>
        public ISeries Series
        {
            get { return (ISeries)GetValue(SeriesProperty); }
            set { SetValue(SeriesProperty, value); }
        }
        public static readonly DependencyProperty SeriesProperty = DependencyProperty.Register("Series", typeof(ISeries), typeof(Chart), new PropertyMetadata((d, e) => { if (e.NewValue is ISeries series && d is Chart chart) series.OwnerChart = chart; }));
        #endregion

        #region 门
        /// <summary>
        /// 门的集合
        /// </summary>
        public ObservableCollection<BaseGate> Gates
        {
            get { return (ObservableCollection<BaseGate>)GetValue(GatesProperty); }
            set { SetValue(GatesProperty, value); }
        }
        public static readonly DependencyProperty GatesProperty = DependencyProperty.Register("Gates", typeof(ObservableCollection<BaseGate>), typeof(Chart), new PropertyMetadata((d, e) =>
        {
            if (d is Chart chart)
            {
                if (e.OldValue is ObservableCollection<BaseGate> olds)
                {
                    olds.CollectionChanged -= chart.Gates_CollectionChanged;
                    foreach (var item in olds) item.Dispose();
                }
                if (e.NewValue is ObservableCollection<BaseGate> news)
                {
                    news.CollectionChanged += chart.Gates_CollectionChanged;
                    foreach (var item in news)
                    {
                        item.OwnerChart = chart;
                        if (chart.IsLoaded && !item.IsCreating) item.Draw();
                    }
                }
            }
        }));
        private void Gates_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            switch (e.Action)
            {
                case System.Collections.Specialized.NotifyCollectionChangedAction.Add:
                    foreach (var item in e.NewItems)
                        if (item is BaseGate bg)
                        {
                            bg.OwnerChart = this;
                            if (this.IsLoaded && !bg.IsCreating) bg.Draw();
                        }
                    break;
                case System.Collections.Specialized.NotifyCollectionChangedAction.Remove:
                    foreach (var item in e.OldItems) if (item is BaseGate bg) bg.Dispose();
                    break;
                case System.Collections.Specialized.NotifyCollectionChangedAction.Replace:
                    foreach (var item in e.OldItems) if (item is BaseGate bg) bg.Dispose();
                    foreach (var item in e.NewItems)
                        if (item is BaseGate bg)
                        {
                            bg.OwnerChart = this;
                            if (this.IsLoaded && !bg.IsCreating) bg.Draw();
                        }
                    break;
                case System.Collections.Specialized.NotifyCollectionChangedAction.Move:
                    break;
                case System.Collections.Specialized.NotifyCollectionChangedAction.Reset:
                    if (e.OldItems != null) foreach (var item in e.OldItems) if (item is BaseGate bg) bg.Dispose();
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// 是否可以更改门
        /// </summary>
        public bool CanChangeGate
        {
            get { return (bool)GetValue(CanChangeGateProperty); }
            set { SetValue(CanChangeGateProperty, value); }
        }
        public static readonly DependencyProperty CanChangeGateProperty = DependencyProperty.Register("CanChangeGate", typeof(bool), typeof(Chart), new PropertyMetadata(true));

        /// <summary>
        /// 门发生变化命令
        /// CommandParameter是门对象
        /// </summary>
        public ICommand GateChangedCommand
        {
            internal get { return (ICommand)GetValue(GateChangedCommandProperty); }
            set { SetValue(GateChangedCommandProperty, value); }
        }
        public static readonly DependencyProperty GateChangedCommandProperty = DependencyProperty.Register("GateChangedCommand", typeof(ICommand), typeof(Chart), new PropertyMetadata(null));

        /// <summary>
        /// 门获取焦点命令
        /// CommandParam是门对象
        /// </summary>
        public ICommand GateGotFocusCommand
        {
            internal get { return (ICommand)GetValue(GateGotFocusCommandProperty); }
            set { SetValue(GateGotFocusCommandProperty, value); }
        }
        public static readonly DependencyProperty GateGotFocusCommandProperty = DependencyProperty.Register("GateGotFocusCommand", typeof(ICommand), typeof(Chart), new PropertyMetadata(null));

        /// <summary>
        /// 门失去焦点命令
        /// </summary>
        public ICommand GateLostFocusCommand
        {
            internal get { return (ICommand)GetValue(GateLostFocusCommandProperty); }
            set { SetValue(GateLostFocusCommandProperty, value); }
        }
        public static readonly DependencyProperty GateLostFocusCommandProperty = DependencyProperty.Register("GateLostFocusCommand", typeof(ICommand), typeof(Chart), new PropertyMetadata(null));

        #region 门划分的区域名称
        /// <summary>
        /// 门划分的区域的命名方法
        /// </summary>
        public Func<BaseGate, string> CreateNewAreaNameFunction { get; set; } = CreateNewNameFaction;

        /// <summary>
        /// 已使用的区域名称集合，默认命名方法使用
        /// </summary>
        public static Dictionary<string, int> ExistedGateAreaNames { get; } = new Dictionary<string, int>();
        /// <summary>
        /// 创建新门的名称
        /// </summary>
        /// <param name="gate"></param>
        /// <returns></returns>
        internal static string CreateNewNameFaction(BaseGate gate)
        {
            if (!ExistedGateAreaNames.ContainsKey(gate.ShortName)) ExistedGateAreaNames[gate.ShortName] = 0;
            ExistedGateAreaNames[gate.ShortName]++;
            return string.Concat(gate.ShortName, ExistedGateAreaNames[gate.ShortName]);
        }

        /// <summary>
        /// 添加已存在的区域名称
        /// </summary>
        /// <param name="shortName">前缀</param>
        /// <param name="areaNames">区域名称</param>
        internal static void AddExistedGraphicalName(string shortName, params string[] areaNames)
        {
            if (string.IsNullOrEmpty(shortName) || areaNames == null || areaNames.Length <= 0) return;
            foreach (var areaName in areaNames)
            {
                if (string.IsNullOrEmpty(areaName) || !areaName.StartsWith(shortName)) continue;
                var number = areaName.Substring(shortName.Length);
                if (int.TryParse(number, out int n))
                {
                    if (ExistedGateAreaNames.ContainsKey(shortName) && ExistedGateAreaNames[shortName] <= n) ExistedGateAreaNames[shortName] = n;
                    else if (!ExistedGateAreaNames.ContainsKey(shortName)) ExistedGateAreaNames[shortName] = n;
                }
            }
        }
        #endregion

        /// <summary>
        /// 显示门划分区域的数据图形
        /// </summary>
        public bool ShowGateAreaGeometry
        {
            get { return (bool)GetValue(ShowGateAreaGeometryProperty); }
            set { SetValue(ShowGateAreaGeometryProperty, value); }
        }
        public static readonly DependencyProperty ShowGateAreaGeometryProperty = DependencyProperty.Register("ShowGateAreaGeometry", typeof(bool), typeof(Chart), new PropertyMetadata(false, async (d, e) =>
        {
            if (d is Chart chart && chart.Series != null && chart.DataSource != null && e.NewValue is bool b)
            {
                foreach (var dataSource in chart.DataSource)
                {
                    if (dataSource == null) continue;
                    if (b) await chart.Series.UpdateGateAreaStream(dataSource);
                    else chart.Series.ClearGateAreaStream(dataSource);
                }
                chart.Series.Draw(chart.Series.ChartDataStreams);
            }
        }));

        #endregion

        #region 增益控制
        /// <summary>
        /// 增益最小值
        /// </summary>
        public double GainMinValue
        {
            get { return (double)GetValue(GainMinValueProperty); }
            set { SetValue(GainMinValueProperty, value); }
        }
        public static readonly DependencyProperty GainMinValueProperty = DependencyProperty.Register("GainMinValue", typeof(double), typeof(Chart), new PropertyMetadata(0d, (d, e) => { if (d is Chart c) { if (c.XAxis != null) c.XAxis.DrawGain(); if (c.YAxis != null) c.YAxis.DrawGain(); } }));
        /// <summary>
        /// 增益最大值
        /// </summary>
        public double GainMaxValue
        {
            get { return (double)GetValue(GainMaxValueProperty); }
            set { SetValue(GainMaxValueProperty, value); }
        }
        public static readonly DependencyProperty GainMaxValueProperty = DependencyProperty.Register("GainMaxValue", typeof(double), typeof(Chart), new PropertyMetadata(4096d, (d, e) => { if (d is Chart c) { if (c.XAxis != null) c.XAxis.DrawGain(); if (c.YAxis != null) c.YAxis.DrawGain(); } }));
        /// <summary>
        /// x轴增益值
        /// </summary>
        public double GainXValue
        {
            get { return (double)GetValue(GainXValueProperty); }
            set { SetValue(GainXValueProperty, value); }
        }
        public static readonly DependencyProperty GainXValueProperty = DependencyProperty.Register("GainXValue", typeof(double), typeof(Chart), new PropertyMetadata(0d, (d, e) => { if (d is Chart c) if (c.XAxis != null) c.XAxis.DrawGain(); }));
        /// <summary>
        /// y轴增益值
        /// </summary>
        public double GainYValue
        {
            get { return (double)GetValue(GainYValueProperty); }
            set { SetValue(GainYValueProperty, value); }
        }
        public static readonly DependencyProperty GainYValueProperty = DependencyProperty.Register("GainYValue", typeof(double), typeof(Chart), new PropertyMetadata(0d, (d, e) => { if (d is Chart c) if (c.YAxis != null) c.YAxis.DrawGain(); }));
        /// <summary>
        /// x轴的增益是否可编辑
        /// </summary>
        public bool GainXCanChange
        {
            get { return (bool)GetValue(GainXCanChangeProperty); }
            set { SetValue(GainXCanChangeProperty, value); }
        }
        public static readonly DependencyProperty GainXCanChangeProperty = DependencyProperty.Register("GainXCanChange", typeof(bool), typeof(Chart), new PropertyMetadata(false, (d, e) => { if (d is Chart c) if (c.XAxis != null) c.XAxis.DrawGain(); }));
        /// <summary>
        /// y轴的增益是否可编辑
        /// </summary>
        public bool GainYCanChange
        {
            get { return (bool)GetValue(GainYCanChangeProperty); }
            set { SetValue(GainYCanChangeProperty, value); }
        }
        public static readonly DependencyProperty GainYCanChangeProperty = DependencyProperty.Register("GainYCanChange", typeof(bool), typeof(Chart), new PropertyMetadata(false, (d, e) => { if (d is Chart c) if (c.YAxis != null) c.YAxis.DrawGain(); }));
        /// <summary>
        /// x轴增益更改完成命令
        /// </summary>
        public ICommand GainXValueChangedCommand
        {
            internal get { return (ICommand)GetValue(GainXValueChangedCommandProperty); }
            set { SetValue(GainXValueChangedCommandProperty, value); }
        }
        public static readonly DependencyProperty GainXValueChangedCommandProperty = DependencyProperty.Register("GainXValueChangedCommand", typeof(ICommand), typeof(Chart), new PropertyMetadata(null));
        /// <summary>
        /// y轴增益更改完成命令
        /// </summary>
        public ICommand GainYValueChangedCommand
        {
            internal get { return (ICommand)GetValue(GainYValueChangedCommandProperty); }
            set { SetValue(GainYValueChangedCommandProperty, value); }
        }
        public static readonly DependencyProperty GainYValueChangedCommandProperty = DependencyProperty.Register("GainYValueChangedCommand", typeof(ICommand), typeof(Chart), new PropertyMetadata(null));

        #region 外观
        /// <summary>
        /// 增益图标边框粗细
        /// </summary>
        public double GainStrokeThickness
        {
            get { return (double)GetValue(GainStrokeThicknessProperty); }
            set { SetValue(GainStrokeThicknessProperty, value); }
        }
        public static readonly DependencyProperty GainStrokeThicknessProperty = DependencyProperty.Register("GainStrokeThickness", typeof(double), typeof(Chart), new PropertyMetadata(0d, (d, e) => { if (d is Chart c) { if (c.XAxis != null) c.XAxis.DrawGain(); if (c.YAxis != null) c.YAxis.DrawGain(); } }));
        /// <summary>
        /// 增益图标边框色
        /// </summary>
        public Brush GainStroke
        {
            get { return (Brush)GetValue(GainStrokeProperty); }
            set { SetValue(GainStrokeProperty, value); }
        }
        public static readonly DependencyProperty GainStrokeProperty = DependencyProperty.Register("GainStroke", typeof(Brush), typeof(Chart), new PropertyMetadata(Brushes.Red, (d, e) => { if (d is Chart c) { if (c.XAxis != null) c.XAxis.DrawGain(); if (c.YAxis != null) c.YAxis.DrawGain(); } }));
        static readonly Brush DefaultGainFill = new SolidColorBrush(Color.FromArgb(0x90, 0xFF, 0x00, 0x00));
        /// <summary>
        /// 增益图标填充色
        /// </summary>
        public Brush GainFill
        {
            get { return (Brush)GetValue(GainFillProperty); }
            set { SetValue(GainFillProperty, value); }
        }
        public static readonly DependencyProperty GainFillProperty = DependencyProperty.Register("GainFill", typeof(Brush), typeof(Chart), new PropertyMetadata(DefaultGainFill, (d, e) => { if (d is Chart c) { if (c.XAxis != null) c.XAxis.DrawGain(); if (c.YAxis != null) c.YAxis.DrawGain(); } }));
        #endregion
        #endregion

        #region 阈值控制
        /// <summary>
        /// 是否显示阈值控制线
        /// </summary>
        public Visibility ThresholdVisibility
        {
            get { return (Visibility)GetValue(ThresholdVisibilityProperty); }
            set { SetValue(ThresholdVisibilityProperty, value); }
        }
        public static readonly DependencyProperty ThresholdVisibilityProperty = DependencyProperty.Register("ThresholdVisibility", typeof(Visibility), typeof(Chart), new PropertyMetadata(Visibility.Collapsed, (d, e) => { if (d is Chart c) c.DrawThresholdGeometry(); }));
        /// <summary>
        /// 阈值最小值
        /// </summary>
        public double ThresholdMinValue
        {
            get { return (double)GetValue(ThresholdMinValueProperty); }
            set { SetValue(ThresholdMinValueProperty, value); }
        }
        public static readonly DependencyProperty ThresholdMinValueProperty = DependencyProperty.Register("ThresholdMinValue", typeof(double), typeof(Chart), new PropertyMetadata(0d));
        /// <summary>
        /// 阈值最大值
        /// </summary>
        public double ThresholdMaxValue
        {
            get { return (double)GetValue(ThresholdMaxValueProperty); }
            set { SetValue(ThresholdMaxValueProperty, value); }
        }
        public static readonly DependencyProperty ThresholdMaxValueProperty = DependencyProperty.Register("ThresholdMaxValue", typeof(double), typeof(Chart), new PropertyMetadata(65535d));
        /// <summary>
        /// 阈值
        /// </summary>
        public double ThresholdValue
        {
            get { return (double)GetValue(ThresholdValueProperty); }
            set { SetValue(ThresholdValueProperty, value); }
        }
        public static readonly DependencyProperty ThresholdValueProperty = DependencyProperty.Register("ThresholdValue", typeof(double), typeof(Chart), new PropertyMetadata(0d, (d, e) => { if (d is Chart c) c.DrawThresholdGeometry(); }));
        /// <summary>
        /// 阈值变化命令
        /// </summary>
        public ICommand ThresholdValueChangedCommand
        {
            internal get { return (ICommand)GetValue(ThresholdValueChangedCommandProperty); }
            set { SetValue(ThresholdValueChangedCommandProperty, value); }
        }
        public static readonly DependencyProperty ThresholdValueChangedCommandProperty = DependencyProperty.Register("ThresholdValueChangedCommand", typeof(ICommand), typeof(Chart), new PropertyMetadata(null));
        /// <summary>
        /// 是否正在移动阈值控制线
        /// </summary>
        protected bool ThresholdIsMoving { get; set; }

        #region 外观
        static readonly Thickness DefaultThresholdStrokeThickness = new Thickness(0.1d);
        /// <summary>
        /// 阈值控制线条宽度
        /// </summary>
        public Thickness ThresholdStrokeThickness
        {
            get { return (Thickness)GetValue(ThresholdStrokeThicknessProperty); }
            set { SetValue(ThresholdStrokeThicknessProperty, value); }
        }
        public static readonly DependencyProperty ThresholdStrokeThicknessProperty = DependencyProperty.Register("ThresholdStrokeThickness", typeof(Thickness), typeof(Chart), new PropertyMetadata(DefaultThresholdStrokeThickness));
        /// <summary>
        /// 阈值控制线填充色
        /// </summary>
        public Brush ThresholdFill
        {
            get { return (Brush)GetValue(ThresholdFillProperty); }
            set { SetValue(ThresholdFillProperty, value); }
        }
        public static readonly DependencyProperty ThresholdFillProperty = DependencyProperty.Register("ThresholdFill", typeof(Brush), typeof(Chart), new PropertyMetadata(Brushes.Black));
        /// <summary>
        /// 阈值控制线边框色
        /// </summary>
        public Brush ThresholdStroke
        {
            get { return (Brush)GetValue(ThresholdStrokeProperty); }
            set { SetValue(ThresholdStrokeProperty, value); }
        }
        public static readonly DependencyProperty ThresholdStrokeProperty = DependencyProperty.Register("ThresholdStroke", typeof(Brush), typeof(Chart), new PropertyMetadata(Brushes.Black));
        /// <summary>
        /// 阈值控制线条矢量数据
        /// </summary>
        public Geometry ThresholdGeometry
        {
            get { return (Geometry)GetValue(ThresholdGeometryProperty); }
            internal set { SetValue(ThresholdGeometryProperty, value); }
        }
        public static readonly DependencyProperty ThresholdGeometryProperty = DependencyProperty.Register("ThresholdGeometry", typeof(Geometry), typeof(Chart), new PropertyMetadata(null));
        static readonly byte DefaultThresholdCenterPointSize = 0x06;
        /// <summary>
        /// 阈值控制线中心点的大小
        /// </summary>
        public byte ThresholdCenterPointSize
        {
            get { return (byte)GetValue(ThresholdCenterPointSizeProperty); }
            set { SetValue(ThresholdCenterPointSizeProperty, value); }
        }
        public static readonly DependencyProperty ThresholdCenterPointSizeProperty = DependencyProperty.Register("ThresholdCenterPointSize", typeof(byte), typeof(Chart), new PropertyMetadata(DefaultThresholdCenterPointSize, (d, e) => { if (d is Chart c) c.DrawThresholdGeometry(); }));

        /// <summary>
        /// 绘制阈值控制线条矢量数据
        /// </summary>
        protected virtual void DrawThresholdGeometry()
        {
            if (ThresholdVisibility == Visibility.Visible && XAxis != null && !double.IsNaN(ThresholdValue) && !double.IsInfinity(ThresholdValue) && ViewPanel != null && ViewPanel.IsLoaded)
            {
                var x = XAxis.GetValueLocation(ThresholdValue);
                if (!double.IsNaN(x) && !double.IsInfinity(x))
                {
                    var height = ViewPanel.ActualHeight;
                    var halfheight = height / 2;
                    var points = new Point[] { new Point(x, halfheight - ThresholdCenterPointSize * 2), new Point(x + ThresholdCenterPointSize, halfheight), new Point(x, halfheight + ThresholdCenterPointSize * 2), new Point(x, height), new Point(x, halfheight + ThresholdCenterPointSize * 2), new Point(x - ThresholdCenterPointSize, halfheight), new Point(x, halfheight - ThresholdCenterPointSize * 2), new Point(x, 0) };
                    StreamGeometry geometry = new StreamGeometry();
                    using (StreamGeometryContext sgc = geometry.Open())
                    {
                        sgc.BeginFigure(new Point(x, 0), true, true);
                        sgc.PolyLineTo(points, true, true);
                        sgc.Close();
                    }
                    geometry.Freeze();

                    var ft = new FormattedText(ThresholdValue.ToString("n0"), System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, new Typeface("Segoe UI"), XAxis.FontSize, Brushes.Black, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                    var ftg = ft.BuildGeometry(new Point(x + ThresholdCenterPointSize * 2, halfheight));
                    ftg.Freeze();
                    GeometryGroup group = new GeometryGroup();
                    group.Children.Add(geometry);
                    group.Children.Add(ftg);
                    group.Freeze();

                    ThresholdGeometry = group;
                }
            }
        }
        #endregion
        #endregion

        /// <summary>
        /// 获取数据模型
        /// </summary>
        /// <returns></returns>
        public ChartModel GetModel()
        {
            return new ChartModel()
            {
                MoveType = this.MoveType,
                ZoomType = this.ZoomType,
                XAxis = this.XAxis == null ? null : this.XAxis.GetModel(),
                YAxis = this.YAxis == null ? null : this.YAxis.GetModel(),
                Series = this.Series == null ? null : this.Series.GetModel(),
                Gates = this.Gates.Select(p => p.GetModel()).ToList(),
                CanChangeGate = this.CanChangeGate
            };
        }
    }

    /// <summary>
    /// 图表移动和放大缩小方式
    /// </summary>
    public enum ChangeType
    {
        /// <summary>
        /// 不改变
        /// </summary>
        None = 0,
        /// <summary>
        /// 只改变X轴
        /// </summary>
        X = 1,
        /// <summary>
        /// 只改变Y轴
        /// </summary>
        Y = 2,
        /// <summary>
        /// 改变XY轴
        /// </summary>
        XY = 3
    }

    public class ChartModel
    {
        public ChangeType MoveType { get; set; }
        public ChangeType ZoomType { get; set; }
        public BaseAxisModel XAxis { get; set; }
        public BaseAxisModel YAxis { get; set; }
        public BaseSeriesModel Series { get; set; }
        public List<BaseGateModel> Gates { get; set; }
        public bool CanChangeGate { get; set; }
    }
}
