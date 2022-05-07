using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace FCSVisualChart
{
    /// <summary>
    /// x轴统计直方图
    /// </summary>
    public class HistogramXStatisticsSeries : ISeries
    {
        /// <summary>
        /// 直方图等份数量
        /// </summary>
        public int Count
        {
            get { return (int)GetValue(CountProperty); }
            set { SetValue(CountProperty, value); }
        }
        public static readonly DependencyProperty CountProperty = DependencyProperty.Register("Count", typeof(int), typeof(HistogramXStatisticsSeries), new PropertyMetadata(256, (d, e) => { if (d is ISeries series) series.Draw(); }));

        /// <summary>
        /// 是否平滑
        /// </summary>
        public bool Smooth
        {
            get { return (bool)GetValue(SmoothProperty); }
            set { SetValue(SmoothProperty, value); }
        }
        public static readonly DependencyProperty SmoothProperty = DependencyProperty.Register("Smooth", typeof(bool), typeof(HistogramXStatisticsSeries), new PropertyMetadata(false, (d, e) => { if (d is ISeries series) series.Draw(); }));

        /// <summary>
        /// 简单平滑算法的点数
        /// </summary>
        public int SmoothPointCount
        {
            get { return (int)GetValue(SmoothPointCountProperty); }
            set { SetValue(SmoothPointCountProperty, value); }
        }
        public static readonly DependencyProperty SmoothPointCountProperty = DependencyProperty.Register("SmoothPointCount", typeof(int), typeof(HistogramXStatisticsSeries), new PropertyMetadata(2, (d, e) => { if (d is ISeries series) series.Draw(); }));

        #region 偏移
        const ushort DefaultOffset = 0;
        /// <summary>
        /// x轴偏移量
        /// </summary>
        public ushort OffsetX
        {
            get { return (ushort)GetValue(OffsetXProperty); }
            set { SetValue(OffsetXProperty, value); }
        }
        public static readonly DependencyProperty OffsetXProperty = DependencyProperty.Register("OffsetX", typeof(ushort), typeof(HistogramXStatisticsSeries), new PropertyMetadata(DefaultOffset, (d, e) => { if (d is ISeries series) series.Draw(); }));

        /// <summary>
        /// y轴偏移量
        /// </summary>
        public ushort OffsetY
        {
            get { return (ushort)GetValue(OffsetYProperty); }
            set { SetValue(OffsetYProperty, value); }
        }
        public static readonly DependencyProperty OffsetYProperty = DependencyProperty.Register("OffsetY", typeof(ushort), typeof(HistogramXStatisticsSeries), new PropertyMetadata(DefaultOffset, (d, e) => { if (d is ISeries series) series.Draw(); }));
        #endregion

        public HistogramXStatisticsSeries() : base() { }
        public HistogramXStatisticsSeries(HistogramXStatisticsSeriesModel model) : base(model)
        {
            this.Count = model.Count;
            this.Smooth = model.Smooth;
            this.SmoothPointCount = model.SmoothPointCount;
            this.OffsetX = model.OffsetX;
            this.OffsetY = model.OffsetY;
        }

        /// <summary>
        /// 直方图统计信息
        /// </summary>
        ConcurrentDictionary<ChartDataSource, SeriesDataStatistics> Statistics = new ConcurrentDictionary<ChartDataSource, SeriesDataStatistics>();

        internal override void Draw(Dictionary<ChartDataSource, SeriesDataGeometry> chartDataStreams)
        {
            if (!this.IsLoaded || OwnerChart == null) return;
            ClearTransform();
            using (var dc = DV.RenderOpen())
            {
                if (OffsetX != 0u && OffsetY != 0u)
                {
                    var countsub1 = OwnerChart.DataSource.Count - 1;
                    var xlength = countsub1 * OffsetX;
                    var ylength = countsub1 * OffsetY;
                    var pen = new Pen(OwnerChart.XAxis.Foreground, 1);
                    dc.DrawLine(pen, new Point(0, OwnerChart.ViewPanel.ActualHeight), new Point(xlength, OwnerChart.ViewPanel.ActualHeight - ylength));
                    dc.DrawLine(pen, new Point(OwnerChart.ViewPanel.ActualWidth - xlength, OwnerChart.ViewPanel.ActualHeight), new Point(OwnerChart.ViewPanel.ActualWidth, OwnerChart.ViewPanel.ActualHeight - ylength));
                }
                var sources = OwnerChart.DataSource.Reverse();
                foreach (var datasource in sources)
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

        /// <summary>
        /// 绘制内容
        /// </summary>
        /// <param name="color">基础颜色</param>
        /// <param name="list">矢量数据流</param>
        /// <param name="dc">画布</param>
        internal override void DCDraw(Color color, List<StreamGeometry> list, DrawingContext dc)
        {
            var pa = color.A / 255d;
            var fillbrush = new SolidColorBrush(Color.FromArgb(Convert.ToByte(FillColorA * pa), color.R, color.G, color.B));
            fillbrush.Freeze();
            var strokebrush = new SolidColorBrush(Color.FromArgb(Convert.ToByte(StrokeColorA * pa), color.R, color.G, color.B));
            strokebrush.Freeze();
            Brush fill = ShowFill ? fillbrush : null;
            Pen pen = ShowStroke ? new Pen(strokebrush, StrokeThickness) : null;
            foreach (var stream in list)
            {
                if (stream.IsEmpty()) continue;
                dc.DrawGeometry(fill, pen, stream);
            }
        }

        protected override Task<List<StreamGeometry>> CreateStreamGeometryByChartDataSource(ChartDataSource dataSource, IList<int> indexs, GateArea gateArea = null)
        {
            if (!this.IsLoaded || this.OwnerChart == null || this.OwnerChart.XAxis == null || this.OwnerChart.YAxis == null || !Statistics.ContainsKey(dataSource) || Statistics[dataSource] == null || (gateArea != null && !Statistics[dataSource].GateAreaStatistics.ContainsKey(gateArea)) || (gateArea == null && (dataSource.CancelTokenSource == null || dataSource.CancelTokenSource.IsCancellationRequested)) || (gateArea != null && (!dataSource.GateAreaCancelTokenSource.ContainsKey(gateArea) || dataSource.GateAreaCancelTokenSource[gateArea] == null || dataSource.GateAreaCancelTokenSource[gateArea].IsCancellationRequested))) return Task.FromResult<List<StreamGeometry>>(null);
            var token = gateArea == null ? dataSource.CancelTokenSource.Token : dataSource.GateAreaCancelTokenSource[gateArea].Token;
            var xParam = this.OwnerChart.XAxis.GetConvertParam();
            var yParam = this.OwnerChart.YAxis.GetConvertParam();
            Func<double, ValueLocationConvertParam, double> yGetLocation = this.OwnerChart.YAxis.GetValueLocation;
            ConcurrentDictionary<int, double> data = gateArea == null ? Statistics[dataSource].MainStatistics : Statistics[dataSource].GateAreaStatistics[gateArea];
            var range = xParam.Length / data.Count;
            var i = this.OwnerChart.DataSource.IndexOf(dataSource);
            if (i < 0) i = 0;
            double offsetX = this.OffsetX * i;
            double offsetY = -this.OffsetY * i;
            if (offsetY < -yParam.Length) offsetY = -yParam.Length * i / this.OwnerChart.DataSource.Count;
            return Task.Factory.StartNew<List<StreamGeometry>>(() =>
            {
                StreamGeometry stream = new StreamGeometry();
                using (StreamGeometryContext sgc = stream.Open())
                {
                    sgc.BeginFigure(new Point(0, yParam.Length), true, true);
                    foreach (var item in data)
                    {
                        var x = item.Key * range;
                        var y = yGetLocation(item.Value, yParam);
                        sgc.LineTo(new Point(x, y), true, true);
                        sgc.LineTo(new Point(x + range, y), true, true);
                    }
                    sgc.LineTo(new Point(xParam.Length, yParam.Length), true, true);
                    sgc.LineTo(new Point(0, yParam.Length), true, true);
                }
                stream.Transform = new MatrixTransform(1, 0, 0, 1, offsetX, offsetY);
                stream.Freeze();
                return new List<StreamGeometry>() { stream };
            }, token);
        }

        /// <summary>
        /// 更新矢量数据流
        /// </summary>
        /// <param name="chartDataSource"></param>
        internal override async void UpdateChartDataStream(ChartDataSource chartDataSource)
        {
            if (chartDataSource == null) return;
            try
            {
                if (chartDataSource.CancelTokenSource != null && !chartDataSource.CancelTokenSource.IsCancellationRequested) chartDataSource.CancelTokenSource.Cancel();
                var cancelToken = new CancellationTokenSource();
                chartDataSource.CancelTokenSource = cancelToken;
                var indexs = chartDataSource.DisplayIndexs;
                if (chartDataSource.Indexs != null && chartDataSource.Indexs != chartDataSource.DisplayIndexs) indexs = indexs.Intersect(chartDataSource.Indexs).ToArray();
                if (UpdateStatisticsAndYAxis(chartDataSource, indexs))
                {
                    var dataSources = Statistics.Keys.ToArray();
                    foreach (var dataSource in dataSources)
                    {
                        if (dataSource.CancelTokenSource != null && !dataSource.CancelTokenSource.IsCancellationRequested) dataSource.CancelTokenSource.Cancel();
                        var token = new CancellationTokenSource();
                        dataSource.CancelTokenSource = token;
                        var streams = await CreateStreamGeometryByChartDataSource(dataSource, null);
                        if (streams != null)
                        {
                            if (ChartDataStreams.ContainsKey(dataSource)) ChartDataStreams[dataSource].MainGeometry = streams;
                            else ChartDataStreams[dataSource] = new SeriesDataGeometry(streams);
                            if (OwnerChart.ShowGateAreaGeometry) await UpdateGateAreaStream(dataSource);
                        }
                        else if (ChartDataStreams.ContainsKey(dataSource)) ChartDataStreams.Remove(dataSource);
                        if (dataSource.CancelTokenSource != null && dataSource.CancelTokenSource == token)
                        {
                            dataSource.CancelTokenSource.Dispose();
                            dataSource.CancelTokenSource = null;
                        }
                    }
                }
                else
                {
                    var streams = await CreateStreamGeometryByChartDataSource(chartDataSource, null);
                    if (streams != null)
                    {
                        if (ChartDataStreams.ContainsKey(chartDataSource)) ChartDataStreams[chartDataSource].MainGeometry = streams;
                        else ChartDataStreams[chartDataSource] = new SeriesDataGeometry(streams);
                        if (OwnerChart.ShowGateAreaGeometry) await UpdateGateAreaStream(chartDataSource);
                    }
                    else if (ChartDataStreams.ContainsKey(chartDataSource)) ChartDataStreams.Remove(chartDataSource);
                }
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
        /// 更新统计数据并更新y轴的最大最小值
        /// </summary>
        /// <param name="chartDataSource"></param>
        /// <returns>返回是否更改了y轴坐标大小</returns>
        private bool UpdateStatisticsAndYAxis(ChartDataSource chartDataSource, IList<int> indexs)
        {
            if (!IsLoaded || this.OwnerChart == null || this.OwnerChart.XAxis == null || this.OwnerChart.YAxis == null || OwnerChart.DataSource == null) return false;
            var maxDegreeOfParallelism = this.MaxDegreeOfParallelism;
            var xParam = this.OwnerChart.XAxis.GetConvertParam();
            var yParam = this.OwnerChart.YAxis.GetConvertParam();
            Func<double, ValueAxisConvertParamBase, double> xValutAxisConvert = this.OwnerChart.XAxis.ValueToAxisValue;
            var excessDisplayAtEdge = this.ExcessDisplayAtEdge;
            var minAxis = xValutAxisConvert(this.OwnerChart.XAxis.Min, xParam.ValueAxisParam);
            var maxAxis = xValutAxisConvert(this.OwnerChart.XAxis.Max, xParam.ValueAxisParam);
            var temp = CountStatistics(chartDataSource.CancelTokenSource.Token, chartDataSource.XSource, indexs, chartDataSource.XSourceConverter, minAxis, maxAxis, this.Count, this.Smooth, this.SmoothPointCount, maxDegreeOfParallelism, xParam.ValueAxisParam, xValutAxisConvert, excessDisplayAtEdge);

            if (Statistics.ContainsKey(chartDataSource)) Statistics[chartDataSource].MainStatistics = temp;
            else Statistics[chartDataSource] = new SeriesDataStatistics(temp);
            var max = Statistics.Count > 0 ? Statistics.Max(p => p.Value == null ? 0d : p.Value.MainStatistics.Max(k => k.Value)) : 100000d; ;
            if (max <= 0) max = 100000d;
            if (OffsetY != 0u)
            {
                var length = OffsetY * OwnerChart.DataSource.Count;
                if (length >= yParam.Length) max = max * OwnerChart.DataSource.Count;
                else max = max * yParam.Length / (yParam.Length - length);
            }
            max = max * 1.2;
            if (this.OwnerChart.YAxis.Max != max || this.OwnerChart.YAxis.Min != 0)
            {
                this.OwnerChart.YAxis.Max = max;
                this.OwnerChart.YAxis.Min = 0;
                this.OwnerChart.YAxis.Draw();
                return true;
            }
            return false;
        }

        /// <summary>
        /// 删除一个数据源
        /// </summary>
        /// <param name="chartDataSource"></param>
        internal override void RemoveChartDataStream(ChartDataSource chartDataSource)
        {
            base.RemoveChartDataStream(chartDataSource);
            if (Statistics.ContainsKey(chartDataSource)) Statistics.TryRemove(chartDataSource, out SeriesDataStatistics d);
        }

        internal override async Task UpdateGateAreaStream(ChartDataSource chartDataSource, GateArea gateArea)
        {
            if (!IsLoaded || this.OwnerChart == null || this.OwnerChart.XAxis == null || this.OwnerChart.YAxis == null || chartDataSource == null || !ChartDataStreams.ContainsKey(chartDataSource) || !chartDataSource.AreaIndexs.ContainsKey(gateArea) || chartDataSource.AreaIndexs[gateArea] == null) return;
            var indexs = chartDataSource.AreaIndexs[gateArea];
            if (chartDataSource.DisplayIndexs != null) indexs = indexs.Intersect(chartDataSource.DisplayIndexs).ToArray();
            if (chartDataSource.Indexs != chartDataSource.DisplayIndexs && chartDataSource.Indexs != null) indexs = indexs.Intersect(chartDataSource.Indexs).ToArray();

            var maxDegreeOfParallelism = this.MaxDegreeOfParallelism;
            var xParam = this.OwnerChart.XAxis.GetConvertParam();
            Func<double, ValueAxisConvertParamBase, double> xValutAxisConvert = this.OwnerChart.XAxis.ValueToAxisValue;
            var excessDisplayAtEdge = this.ExcessDisplayAtEdge;
            var minAxis = xValutAxisConvert(this.OwnerChart.XAxis.Min, xParam.ValueAxisParam);
            var maxAxis = xValutAxisConvert(this.OwnerChart.XAxis.Max, xParam.ValueAxisParam);

            if (chartDataSource.GateAreaCancelTokenSource.ContainsKey(gateArea) && chartDataSource.GateAreaCancelTokenSource[gateArea] != null) chartDataSource.GateAreaCancelTokenSource[gateArea].Cancel();
            var token = new CancellationTokenSource();
            chartDataSource.GateAreaCancelTokenSource[gateArea] = token;

            Statistics[chartDataSource].GateAreaStatistics[gateArea] = CountStatistics(token.Token, chartDataSource.XSource, indexs, chartDataSource.XSourceConverter, minAxis, maxAxis, this.Count, this.Smooth, this.SmoothPointCount, maxDegreeOfParallelism, xParam.ValueAxisParam, xValutAxisConvert, excessDisplayAtEdge);
            var streams = await CreateStreamGeometryByChartDataSource(chartDataSource, null, gateArea);

            if (chartDataSource.GateAreaCancelTokenSource.ContainsKey(gateArea) && chartDataSource.GateAreaCancelTokenSource[gateArea] == token)
            {
                chartDataSource.GateAreaCancelTokenSource[gateArea].Dispose();
                chartDataSource.GateAreaCancelTokenSource[gateArea] = null;
            }
            if (streams != null) ChartDataStreams[chartDataSource].GateAreaGeometries[gateArea] = streams;
            else ChartDataStreams[chartDataSource].GateAreaGeometries.Remove(gateArea);
        }

        internal override void RemoveGateAreaStream(ChartDataSource chartDataSource, GateArea gateArea)
        {
            base.RemoveGateAreaStream(chartDataSource, gateArea);
            if (!Statistics.ContainsKey(chartDataSource) || !Statistics[chartDataSource].GateAreaStatistics.ContainsKey(gateArea)) return;
            Statistics[chartDataSource].GateAreaStatistics.Remove(gateArea);
        }

        internal override void ClearGateAreaStream(ChartDataSource chartDataSource)
        {
            base.ClearGateAreaStream(chartDataSource);
            if (!Statistics.ContainsKey(chartDataSource)) return;
            Statistics[chartDataSource].GateAreaStatistics.Clear();
        }

        /// <summary>
        /// x轴统计直方图
        /// </summary>
        /// <param name="dataSource">数据源</param>
        /// <param name="minAxis">x轴最小值</param>
        /// <param name="maxAxis">x轴最大值</param>
        /// <param name="count">分割数量</param>
        /// <param name="smooth">是否启用平滑</param>
        /// <param name="smoothCount">简单平滑算法数据点数</param>
        /// <param name="maxDegreeOfParallelism">最大线程数</param>
        /// <param name="param">实际值转坐标值参数</param>
        /// <param name="valueToAxisValue">实际值转坐标值方法</param>
        /// <param name="excessDisplayAtEdge">是否压边显示</param>
        /// <returns></returns>
        internal ConcurrentDictionary<int, double> CountStatistics(CancellationToken token, IList source, IList<int> indexs, Func<object, double> converter, double minAxis, double maxAxis, int count, bool smooth, int smoothCount, int maxDegreeOfParallelism, ValueAxisConvertParamBase param, Func<double, ValueAxisConvertParamBase, double> valueToAxisValue, bool excessDisplayAtEdge)
        {
            if (source == null || token.IsCancellationRequested) return null;
            ConcurrentDictionary<int, double> data = new ConcurrentDictionary<int, double>();
            for (int i = 0; i < count; i++) data[i] = 0;
            var range = (maxAxis - minAxis) / count;
            if (indexs == null)
            {
                try
                {
                    var maxcount = source.Count;
                    var result = Parallel.For(0, maxcount, new ParallelOptions() { CancellationToken = token, MaxDegreeOfParallelism = maxDegreeOfParallelism }, (i, loop) =>
                    {
                        if (loop.IsStopped) return;
                        if (token.IsCancellationRequested) loop.Stop();
                        if (source.Count > i)
                        {
                            var x = valueToAxisValue(converter(source[i]), param);
                            if (excessDisplayAtEdge)
                            {
                                if (x <= minAxis)
                                {
                                    data[0] += 1;
                                    return;
                                }
                                else if (x >= maxAxis)
                                {
                                    data[count - 1] += 1;
                                    return;
                                }
                            }
                            if (!double.IsNaN(x) && !double.IsInfinity(x))
                            {
                                var xi = Convert.ToInt32(Math.Floor((x - minAxis) / range));
                                if (data.ContainsKey(xi)) data[xi] += 1;
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
                        if (source.Count > i)
                        {
                            var x = valueToAxisValue(converter(source[i]), param);
                            if (excessDisplayAtEdge)
                            {
                                if (x <= minAxis)
                                {
                                    data[0] += 1;
                                    return;
                                }
                                else if (x >= maxAxis)
                                {
                                    data[count - 1] += 1;
                                    return;
                                }
                            }
                            if (!double.IsNaN(x) && !double.IsInfinity(x))
                            {
                                var xi = Convert.ToInt32(Math.Floor((x - minAxis) / range));
                                if (data.ContainsKey(xi)) data[xi] += 1;
                            }
                        }
                    });
                    if (!result.IsCompleted) return null;
                }
                catch (OperationCanceledException) { return null; }
            }
            if (smooth)
            {
                List<double> temps = new List<double>();
                for (int i = 0; i < count; i++)
                {
                    temps.Add(data[i]);
                    if (temps.Count > smoothCount) temps.RemoveAt(0);
                    data[i] = temps.Average();
                }
            }
            return data;
        }

        public override BaseSeriesModel GetModel()
        {
            return new HistogramXStatisticsSeriesModel()
            {
                ShowFill = this.ShowFill,
                ShowStroke = this.ShowStroke,
                StrokeThickness = this.StrokeThickness,
                FillColorA = this.FillColorA,
                StrokeColorA = this.StrokeColorA,
                ExcessDisplayAtEdge = this.ExcessDisplayAtEdge,
                Count = this.Count,
                Smooth = this.Smooth,
                SmoothPointCount = this.SmoothPointCount,
                OffsetX = this.OffsetX,
                OffsetY = this.OffsetY
            };
        }
    }

    public class HistogramXStatisticsSeriesModel : BaseSeriesModel
    {
        public int Count { get; set; }
        public bool Smooth { get; set; }
        public int SmoothPointCount { get; set; }
        public ushort OffsetX { get; set; }
        public ushort OffsetY { get; set; }
    }

    /// <summary>
    /// 直方图统计
    /// </summary>
    internal class SeriesDataStatistics
    {
        /// <summary>
        /// 主要的统计
        /// </summary>
        internal ConcurrentDictionary<int, double> MainStatistics { get; set; }
        /// <summary>
        /// 门划分区域内数据的统计
        /// </summary>
        internal Dictionary<GateArea, ConcurrentDictionary<int, double>> GateAreaStatistics { get; } = new Dictionary<GateArea, ConcurrentDictionary<int, double>>();

        internal SeriesDataStatistics() { }
        internal SeriesDataStatistics(ConcurrentDictionary<int, double> statistics)
        {
            this.MainStatistics = statistics;
        }
    }
}
