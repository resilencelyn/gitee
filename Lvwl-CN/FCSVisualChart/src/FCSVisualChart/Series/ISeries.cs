using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Xml.Serialization;

namespace FCSVisualChart
{
    public abstract class ISeries : FrameworkElement
    {
        #region 绘制内容
        protected DrawingVisual DV { get; set; } = new DrawingVisual();
        protected override int VisualChildrenCount => 1;
        protected override Visual GetVisualChild(int index) { return DV; }
        #endregion

        public ISeries()
        {
            this.AddVisualChild(DV);
            this.RenderTransform = new MatrixTransform();
            this.Loaded += (sender, e) => { this.Draw(); };
        }

        public ISeries(BaseSeriesModel model) : this()
        {
            this.ShowFill = model.ShowFill;
            this.ShowStroke = model.ShowStroke;
            this.StrokeThickness = model.StrokeThickness;
            this.FillColorA = model.FillColorA;
            this.StrokeColorA = model.StrokeColorA;
            this.ExcessDisplayAtEdge = model.ExcessDisplayAtEdge;
        }

        /// <summary>
        /// 所属chart控件
        /// </summary>
        public Chart OwnerChart { get; internal set; }

        #region 外观
        /// <summary>
        /// 是否显示填充色
        /// </summary>
        public bool ShowFill
        {
            get { return (bool)GetValue(ShowFillProperty); }
            set { SetValue(ShowFillProperty, value); }
        }
        public static readonly DependencyProperty ShowFillProperty = DependencyProperty.Register("ShowFill", typeof(bool), typeof(ISeries), new PropertyMetadata(true, (d, e) => { if (d is ISeries series) series.Draw(series.ChartDataStreams); }));
        /// <summary>
        /// 是否显示边框色
        /// </summary>
        public bool ShowStroke
        {
            get { return (bool)GetValue(ShowStrokeProperty); }
            set { SetValue(ShowStrokeProperty, value); }
        }
        public static readonly DependencyProperty ShowStrokeProperty = DependencyProperty.Register("ShowStroke", typeof(bool), typeof(ISeries), new PropertyMetadata(true, (d, e) => { if (d is ISeries series) series.Draw(series.ChartDataStreams); }));

        /// <summary>
        /// 边框粗细
        /// </summary>
        public int StrokeThickness
        {
            get { return (int)GetValue(StrokeThicknessProperty); }
            set { SetValue(StrokeThicknessProperty, value); }
        }
        public static readonly DependencyProperty StrokeThicknessProperty = DependencyProperty.Register("StrokeThickness", typeof(int), typeof(ISeries), new PropertyMetadata(1, (d, e) => { if (d is ISeries series) series.Draw(series.ChartDataStreams); }));

        private const byte DefaultA = 0xff;
        /// <summary>
        /// 填充色不透明度
        /// </summary>
        public byte FillColorA
        {
            get { return (byte)GetValue(FillColorAProperty); }
            set { SetValue(FillColorAProperty, value); }
        }
        public static readonly DependencyProperty FillColorAProperty = DependencyProperty.Register("FillColorA", typeof(byte), typeof(ISeries), new PropertyMetadata(DefaultA, (d, e) => { if (d is ISeries series) series.Draw(series.ChartDataStreams); }));
        /// <summary>
        /// 边框色不透明度
        /// </summary>
        public byte StrokeColorA
        {
            get { return (byte)GetValue(StrokeColorAProperty); }
            set { SetValue(StrokeColorAProperty, value); }
        }
        public static readonly DependencyProperty StrokeColorAProperty = DependencyProperty.Register("StrokeColorA", typeof(byte), typeof(ISeries), new PropertyMetadata(DefaultA, (d, e) => { if (d is ISeries series) series.Draw(series.ChartDataStreams); }));

        /// <summary>
        /// 超出界限部分在边缘显示
        /// </summary>
        public bool ExcessDisplayAtEdge
        {
            get { return (bool)GetValue(ExcessDisplayAtEdgeProperty); }
            set { SetValue(ExcessDisplayAtEdgeProperty, value); }
        }
        public static readonly DependencyProperty ExcessDisplayAtEdgeProperty = DependencyProperty.Register("ExcessDisplayAtEdge", typeof(bool), typeof(ISeries), new PropertyMetadata(true, (d, e) => { if (d is ISeries series) series.Draw(); }));
        #endregion

        #region 显示图形
        /// <summary>
        /// 数据源对应的矢量数据流集合
        /// </summary>
        internal Dictionary<ChartDataSource, SeriesDataGeometry> ChartDataStreams { get; } = new Dictionary<ChartDataSource, SeriesDataGeometry>();
        /// <summary>
        /// 绘制图形，全部重新生成矢量数据并绘制
        /// </summary>
        internal virtual void Draw()
        {
            if (!IsLoaded) return;
            if (OwnerChart != null && OwnerChart.DataSource != null && this.OwnerChart.XAxis != null && this.OwnerChart.YAxis != null)
            {
                foreach (var item in OwnerChart.DataSource) this.UpdateChartDataStream(item);
            }
        }
        /// <summary>
        /// 绘制图形显示
        /// </summary>
        /// <param name="chartDataStreams">需要显示的矢量数据流</param>
        internal virtual void Draw(Dictionary<ChartDataSource, SeriesDataGeometry> chartDataStreams)
        {
            if (!this.IsLoaded || OwnerChart == null) return;
            ClearTransform();
            using (var dc = DV.RenderOpen())
            {
                if (OwnerChart.DataSource != null)
                {
                    foreach (var datasource in OwnerChart.DataSource)
                    {
                        if (chartDataStreams.ContainsKey(datasource))
                        {
                            var sdg = chartDataStreams[datasource];
                            DCDraw(datasource.DisplayColor, sdg.MainGeometry, dc);
                            var mygateareas = OwnerChart.Gates.SelectMany(p => p.Areas).ToArray();
                            var othergateareas = sdg.GateAreaGeometries.Keys.ToArray().Where(p => !mygateareas.Contains(p)).ToArray();
                            foreach (var gatearea in othergateareas)
                            {
                                if (!sdg.GateAreaGeometries.ContainsKey(gatearea)) continue;
                                DCDraw(gatearea.DisplayColor, sdg.GateAreaGeometries[gatearea], dc);
                            }
                            foreach (var gatearea in mygateareas)
                            {
                                if (!sdg.GateAreaGeometries.ContainsKey(gatearea)) continue;
                                DCDraw(gatearea.DisplayColor, sdg.GateAreaGeometries[gatearea], dc);
                            }
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 绘制内容
        /// </summary>
        /// <param name="color">基础颜色</param>
        /// <param name="list">矢量数据流</param>
        /// <param name="dc">画布</param>
        internal abstract void DCDraw(Color color, List<StreamGeometry> list, DrawingContext dc);

        /// <summary>
        /// 获取数据源的矢量数据流
        /// </summary>
        /// <param name="dataSource">数据源</param>
        /// <returns></returns>
        protected abstract Task<List<StreamGeometry>> CreateStreamGeometryByChartDataSource(ChartDataSource dataSource, IList<int> indexs, GateArea gateArea = null);
        /// <summary>
        /// 更新数据源的矢量数据流
        /// </summary>
        /// <param name="chartDataSource"></param>
        internal virtual async void UpdateChartDataStream(ChartDataSource chartDataSource)
        {
            if (chartDataSource == null) return;
            try
            {
                if (chartDataSource.CancelTokenSource != null && !chartDataSource.CancelTokenSource.IsCancellationRequested) chartDataSource.CancelTokenSource.Cancel();
                var cancelToken = new CancellationTokenSource();
                chartDataSource.CancelTokenSource = cancelToken;
                var indexs = chartDataSource.DisplayIndexs;
                if (chartDataSource.Indexs != null && chartDataSource.Indexs != indexs) indexs = indexs.Intersect(chartDataSource.Indexs).ToArray();
                var streams = await CreateStreamGeometryByChartDataSource(chartDataSource, indexs);
                if (streams != null)
                {
                    if (ChartDataStreams.ContainsKey(chartDataSource)) ChartDataStreams[chartDataSource].MainGeometry = streams;
                    else ChartDataStreams[chartDataSource] = new SeriesDataGeometry(streams);
                    if (OwnerChart.ShowGateAreaGeometry) await UpdateGateAreaStream(chartDataSource);
                }
                else if (ChartDataStreams.ContainsKey(chartDataSource)) ChartDataStreams.Remove(chartDataSource);
                if (chartDataSource.CancelTokenSource != null && chartDataSource.CancelTokenSource == cancelToken)
                {
                    chartDataSource.CancelTokenSource.Dispose();
                    chartDataSource.CancelTokenSource = null;
                }
                Draw(ChartDataStreams);
            }
            catch (TaskCanceledException) { }
        }
        /// <summary>
        /// 删除数据源的矢量数据流
        /// </summary>
        /// <param name="chartDataSource"></param>
        internal virtual void RemoveChartDataStream(ChartDataSource chartDataSource)
        {
            if (chartDataSource.CancelTokenSource != null && !chartDataSource.CancelTokenSource.IsCancellationRequested) chartDataSource.CancelTokenSource.Cancel();
            this.ChartDataStreams.Remove(chartDataSource);
            Draw(ChartDataStreams);
        }
        #endregion

        #region 门区域划分的数据显示
        /// <summary>
        /// 更新所有门划分区域的显示矢量数据
        /// </summary>
        /// <param name="chartDataSource"></param>
        /// <returns></returns>
        internal virtual async Task UpdateGateAreaStream(ChartDataSource chartDataSource)
        {
            if (chartDataSource == null || !ChartDataStreams.ContainsKey(chartDataSource)) return;
            var gateAreas = chartDataSource.AreaIndexs.Keys.ToArray();
            foreach (var gateArea in gateAreas) await UpdateGateAreaStream(chartDataSource, gateArea);
        }
        /// <summary>
        /// 更新门划分区域的显示矢量数据
        /// </summary>
        /// <param name="chartDataSource"></param>
        /// <param name="gateArea"></param>
        /// <returns></returns>
        internal virtual async Task UpdateGateAreaStream(ChartDataSource chartDataSource, GateArea gateArea)
        {
            if (chartDataSource == null || !ChartDataStreams.ContainsKey(chartDataSource) || ChartDataStreams[chartDataSource] == null || !chartDataSource.AreaIndexs.ContainsKey(gateArea) || chartDataSource.AreaIndexs[gateArea] == null) return;
            var sdg = ChartDataStreams[chartDataSource];
            var indexs = chartDataSource.AreaIndexs[gateArea];
            if (chartDataSource.DisplayIndexs != null) indexs = indexs.Intersect(chartDataSource.DisplayIndexs).ToArray();
            if (chartDataSource.Indexs != chartDataSource.DisplayIndexs && chartDataSource.Indexs != null) indexs = indexs.Intersect(chartDataSource.Indexs).ToArray();
            if (chartDataSource.GateAreaCancelTokenSource.ContainsKey(gateArea) && chartDataSource.GateAreaCancelTokenSource[gateArea] != null) chartDataSource.GateAreaCancelTokenSource[gateArea].Cancel();
            var token = new CancellationTokenSource();
            chartDataSource.GateAreaCancelTokenSource[gateArea] = token;
            var streams = await CreateStreamGeometryByChartDataSource(chartDataSource, indexs, gateArea);
            if (chartDataSource.GateAreaCancelTokenSource.ContainsKey(gateArea) && chartDataSource.GateAreaCancelTokenSource[gateArea] == token)
            {
                chartDataSource.GateAreaCancelTokenSource[gateArea].Dispose();
                chartDataSource.GateAreaCancelTokenSource[gateArea] = null;
            }
            if (streams != null) sdg.GateAreaGeometries[gateArea] = streams;
            else if (sdg.GateAreaGeometries.ContainsKey(gateArea)) sdg.GateAreaGeometries.Remove(gateArea);
        }
        /// <summary>
        /// 删除门划分区域的矢量数据
        /// </summary>
        /// <param name="chartDataSource"></param>
        /// <param name="gateArea"></param>
        internal virtual void RemoveGateAreaStream(ChartDataSource chartDataSource, GateArea gateArea)
        {
            if (!ChartDataStreams.ContainsKey(chartDataSource) || !ChartDataStreams[chartDataSource].GateAreaGeometries.ContainsKey(gateArea)) return;
            ChartDataStreams[chartDataSource].GateAreaGeometries.Remove(gateArea);
        }
        /// <summary>
        /// 清除门划分区域的矢量数据
        /// </summary>
        /// <param name="chartDataSource"></param>
        internal virtual void ClearGateAreaStream(ChartDataSource chartDataSource)
        {
            if (!ChartDataStreams.ContainsKey(chartDataSource)) return;
            ChartDataStreams[chartDataSource].GateAreaGeometries.Clear();
        }

        #endregion

        #region 计算
        /// <summary>
        /// 计算图形的for循环时允许的最大并发线程数量
        /// </summary>
        public int MaxDegreeOfParallelism
        {
            get { return (int)GetValue(MaxDegreeOfParallelismProperty); }
            set { SetValue(MaxDegreeOfParallelismProperty, value); }
        }
        public static readonly DependencyProperty MaxDegreeOfParallelismProperty = DependencyProperty.Register("MaxDegreeOfParallelism", typeof(int), typeof(ISeries), new PropertyMetadata(4));

        /// <summary>
        /// 统计点位数量信息
        /// </summary>
        /// <param name="dataSource">数据源</param>
        /// <param name="pointLength">点大小</param>
        /// <param name="width">显示区域宽度</param>
        /// <param name="height">显示区域高度</param>
        /// <param name="maxDegreeOfParallelism">最大线程数</param>
        /// <param name="xParam">x轴数据转换参数</param>
        /// <param name="yParam">y轴数据转换参数</param>
        /// <param name="xGetLocation">x轴获取数据位置方法</param>
        /// <param name="yGetLocation">y轴获取数据位置方法</param>
        /// <param name="excessDisplayAtEdge">超出界限是否压边显示</param>
        /// <returns></returns>
        internal virtual ConcurrentDictionary<int, ConcurrentDictionary<int, int>> PointCountStatistics(CancellationToken token, IList xSource, IList ySource, IList<int> indexs, Func<object, double> xConverter, Func<object, double> yConverter, int pointLength, int maxDegreeOfParallelism, ValueLocationConvertParam xParam, ValueLocationConvertParam yParam, Func<double, ValueLocationConvertParam, double> xGetLocation, Func<double, ValueLocationConvertParam, double> yGetLocation, bool excessDisplayAtEdge)
        {
            if (xSource == null || ySource == null || token.IsCancellationRequested) return null;
            ConcurrentDictionary<int, ConcurrentDictionary<int, int>> data = new ConcurrentDictionary<int, ConcurrentDictionary<int, int>>();
            var width = xParam.Length - pointLength;
            var height = yParam.Length - pointLength;
            int maxwidth = 0;
            int maxheight = 0;
            for (int i = 0; i < width; i += pointLength)
            {
                maxwidth = i;
                if (token.IsCancellationRequested) return null;
                data[i] = new ConcurrentDictionary<int, int>();
                for (int j = 0; j < height; j += pointLength)
                {
                    maxheight = j;
                    if (token.IsCancellationRequested) return null;
                    data[i][j] = 0;
                }
            }
            if (indexs == null)
            {
                try
                {
                    var maxcount = Math.Min(xSource.Count, ySource.Count);
                    var result = Parallel.For(0, maxcount, new ParallelOptions() { CancellationToken = token, MaxDegreeOfParallelism = maxDegreeOfParallelism }, (i, loop) =>
                    {
                        if (loop.IsStopped) return;
                        if (token.IsCancellationRequested) loop.Stop();
                        if (xSource.Count > i && ySource.Count > i)
                        {
                            var x = xGetLocation(xConverter(xSource[i]), xParam);
                            var y = yGetLocation(yConverter(ySource[i]), yParam);
                            if (excessDisplayAtEdge)
                            {
                                if (x < 0) x = 0; else if (x > maxwidth) x = maxwidth;
                                if (y < 0) y = 0; else if (y > maxheight) y = maxheight;
                            }
                            if (!double.IsNaN(x) && !double.IsInfinity(x) && !double.IsNaN(y) && !double.IsInfinity(y))
                            {
                                var xi = Convert.ToInt32(Math.Floor(x)) / pointLength * pointLength;
                                var yi = Convert.ToInt32(Math.Floor(y)) / pointLength * pointLength;
                                if (data.ContainsKey(xi) && data[xi].ContainsKey(yi)) data[xi][yi] += 1;
                            }
                        }
                    });
                    if (!result.IsCompleted) return null;
                }
                catch (OperationCanceledException) { return null; }
            }
            else
            {
                try
                {
                    var result = Parallel.ForEach(indexs, new ParallelOptions() { CancellationToken = token, MaxDegreeOfParallelism = maxDegreeOfParallelism }, (i, loop) =>
                    {
                        if (loop.IsStopped) return;
                        if (token.IsCancellationRequested) loop.Stop();
                        if (xSource.Count > i && ySource.Count > i)
                        {
                            var x = xGetLocation(xConverter(xSource[i]), xParam);
                            var y = yGetLocation(yConverter(ySource[i]), yParam);
                            if (excessDisplayAtEdge)
                            {
                                if (x < 0) x = 0; else if (x > maxwidth) x = maxwidth;
                                if (y < 0) y = 0; else if (y > maxheight) y = maxheight;
                            }
                            if (!double.IsNaN(x) && !double.IsInfinity(x) && !double.IsNaN(y) && !double.IsInfinity(y))
                            {
                                var xi = Convert.ToInt32(Math.Floor(x)) / pointLength * pointLength;
                                var yi = Convert.ToInt32(Math.Floor(y)) / pointLength * pointLength;
                                if (data.ContainsKey(xi) && data[xi].ContainsKey(yi)) data[xi][yi] += 1;
                            }
                        }
                    });
                    if (!result.IsCompleted) return null;
                }
                catch (OperationCanceledException) { return null; }
            }
            return data;
        }
        #endregion

        #region 变形
        /// <summary>
        /// 移动图形
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        internal virtual void Move(double x, double y)
        {
            if (this.RenderTransform is MatrixTransform matrixTransform)
            {
                var matrix = matrixTransform.Matrix;
                matrixTransform.Matrix = new Matrix(1, 0, 0, 1, matrix.OffsetX + x, matrix.OffsetY + y);
            }
        }
        /// <summary>
        /// 清除图形转换
        /// </summary>
        internal virtual void ClearTransform()
        {
            if (this.RenderTransform is MatrixTransform matrixTransform && matrixTransform.Matrix != default)
            {
                matrixTransform.Matrix = new Matrix();
            }
        }
        #endregion

        /// <summary>
        /// 获取伪彩渐变色
        /// </summary>
        /// <param name="i">当前梯度值</param>
        /// <param name="max">最大梯度量</param>
        /// <param name="baseColor">基础颜色</param>
        /// <returns></returns>
        public static Color GetGradientColor(long i, long max, Color baseColor)
        {
            var tempv = 1536 * i / max;
            var r = baseColor.R;
            var g = baseColor.G;
            var b = baseColor.B;
            var vs = new byte[] { r, g, b }.OrderByDescending(p => p).ToArray();
            var sub0 = 255 - vs[0];
            var sub1 = 255 - vs[1];
            var sub2 = 255 - vs[2];
            var sumsub = sub0 + sub1 + sub2;
            if (tempv < sub0)
            {
                vs[0] += Convert.ToByte(tempv);
            }
            else if (tempv < sub0 + sub1)
            {
                vs[0] = 0xff;
                vs[1] += Convert.ToByte(tempv - sub0);
            }
            else if (tempv < sub0 + sub1 + 255)
            {
                vs[0] = Convert.ToByte(255 + sub0 + sub1 - tempv);
                vs[1] = 0xff;
            }
            else if (tempv < sub0 + sub1 + 255 + sub2)
            {
                vs[0] = 0x00;
                vs[1] = 0xff;
                vs[2] += Convert.ToByte(tempv - sub0 - sub1 - 255);
            }
            else if (tempv < sumsub + 510)
            {
                vs[0] = 0x00;
                vs[1] = Convert.ToByte(sumsub + 510 - tempv);
                vs[2] = 0xff;
            }
            else if (tempv < sumsub + 765)
            {
                vs[0] = Convert.ToByte(tempv - sumsub - 510);
                vs[1] = 0x00;
                vs[2] = 0xff;
            }
            else if (tempv < sumsub + 1020)
            {
                vs[0] = 0xff;
                vs[1] = 0x00;
                vs[2] = Convert.ToByte(sumsub + 1020 - tempv);
            }
            else if (tempv < sumsub + 1275)
            {
                vs[0] = 0xff;
                vs[1] = Convert.ToByte(tempv - sumsub - 1020);
                vs[2] = 0x00;
            }
            else if (tempv < sumsub + 1530)
            {
                vs[0] = Convert.ToByte(sumsub + 1530 - tempv);
                vs[1] = 0xff;
                vs[2] = 0x00;
            }

            if (r > g && r > b)
            {
                if (g > b) return Color.FromArgb(baseColor.A, vs[0], vs[1], vs[2]);
                else return Color.FromArgb(baseColor.A, vs[0], vs[2], vs[1]);
            }
            else if (g > r && g > b)
            {
                if (r > b) return Color.FromArgb(baseColor.A, vs[1], vs[0], vs[2]);
                else return Color.FromArgb(baseColor.A, vs[2], vs[0], vs[1]);
            }
            else
            {
                if (r > g) return Color.FromArgb(baseColor.A, vs[1], vs[2], vs[0]);
                else return Color.FromArgb(baseColor.A, vs[2], vs[1], vs[0]);
            }
        }

        #region 数据模型
        /// <summary>
        /// 获取数据模型
        /// </summary>
        /// <returns></returns>
        public abstract BaseSeriesModel GetModel();

        /// <summary>
        /// 根据模型创建对象
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public static ISeries CreateByModel(BaseSeriesModel model)
        {
            if (model is ContourSeriesModel cm) return new ContourSeries(cm);
            else if (model is DensitySeriesModel dm) return new DensitySeries(dm);
            else if (model is HistogramXStatisticsSeriesModel hm) return new HistogramXStatisticsSeries(hm);
            else if (model is PseudoColorSeriesModel pm) return new PseudoColorSeries(pm);
            else if (model is ScatterSeriesModel sm) return new ScatterSeries(sm);
            return new ScatterSeries();
        }
        #endregion
    }

    [XmlInclude(typeof(ContourSeriesModel))]
    [XmlInclude(typeof(DensitySeriesModel))]
    [XmlInclude(typeof(HistogramXStatisticsSeriesModel))]
    [XmlInclude(typeof(PseudoColorSeriesModel))]
    [XmlInclude(typeof(ScatterSeriesModel))]
    public abstract class BaseSeriesModel
    {
        public bool ShowFill { get; set; }
        public bool ShowStroke { get; set; }
        public int StrokeThickness { get; set; }
        public byte FillColorA { get; set; }
        public byte StrokeColorA { get; set; }
        public bool ExcessDisplayAtEdge { get; set; }
    }

    /// <summary>
    /// 矢量数据流
    /// </summary>
    internal class SeriesDataGeometry
    {
        /// <summary>
        /// 主要的显示图形的矢量数据流
        /// </summary>
        internal List<StreamGeometry> MainGeometry { get; set; }
        /// <summary>
        /// 门划分区域内数据的显示图形的矢量数据流
        /// </summary>
        internal Dictionary<GateArea, List<StreamGeometry>> GateAreaGeometries { get; } = new Dictionary<GateArea, List<StreamGeometry>>();

        internal SeriesDataGeometry() { }
        internal SeriesDataGeometry(List<StreamGeometry> streamGeometries)
        {
            this.MainGeometry = streamGeometries;
        }
    }
}
