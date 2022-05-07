using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace FCSVisualChart
{
    /// <summary>
    /// 等高线图
    /// </summary>
    public class ContourSeries : ISeries
    {
        /// <summary>
        /// 点的宽度
        /// </summary>
        public int PointLength
        {
            get { return (int)GetValue(PointLengthProperty); }
            set { SetValue(PointLengthProperty, value); }
        }
        public static readonly DependencyProperty PointLengthProperty = DependencyProperty.Register("PointLength", typeof(int), typeof(ContourSeries), new PropertyMetadata(4, (d, e) => { if (d is ISeries series) series.Draw(); }));

        /// <summary>
        /// 等高线分级基数
        /// </summary>
        public int GradeBase
        {
            get { return (int)GetValue(GradeBaseProperty); }
            set { SetValue(GradeBaseProperty, value); }
        }
        public static readonly DependencyProperty GradeBaseProperty = DependencyProperty.Register("GradeBase", typeof(int), typeof(ContourSeries), new PropertyMetadata(5, (d, e) => { if (d is ISeries series) series.Draw(); }));

        /// <summary>
        /// 分级方式
        /// </summary>
        public GradeType GradeType
        {
            get { return (GradeType)GetValue(GradeTypeProperty); }
            set { SetValue(GradeTypeProperty, value); }
        }
        public static readonly DependencyProperty GradeTypeProperty = DependencyProperty.Register("GradeType", typeof(GradeType), typeof(ContourSeries), new PropertyMetadata(GradeType.Log, (d, e) => { if (d is ISeries series) series.Draw(); }));

        public ContourSeries() : base() { }
        public ContourSeries(ContourSeriesModel model) : base(model)
        {
            this.PointLength = model.PointLength;
            this.GradeBase = model.GradeBase;
            this.GradeType = model.GradeType;
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

        /// <summary>
        /// 获取数据源的矢量数据流
        /// </summary>
        /// <param name="dataSource">数据源</param>
        /// <returns></returns>
        protected override Task<List<StreamGeometry>> CreateStreamGeometryByChartDataSource(ChartDataSource dataSource, IList<int> indexs, GateArea gateArea = null)
        {
            if (!this.IsLoaded || this.OwnerChart == null || this.OwnerChart.XAxis == null || this.OwnerChart.YAxis == null || (gateArea == null && (dataSource.CancelTokenSource == null || dataSource.CancelTokenSource.IsCancellationRequested)) || (gateArea != null && (!dataSource.GateAreaCancelTokenSource.ContainsKey(gateArea) || dataSource.GateAreaCancelTokenSource[gateArea] == null || dataSource.GateAreaCancelTokenSource[gateArea].IsCancellationRequested))) return Task.FromResult<List<StreamGeometry>>(null);
            var token = gateArea == null ? dataSource.CancelTokenSource.Token : dataSource.GateAreaCancelTokenSource[gateArea].Token;
            var pointLength = PointLength;
            var halfPointLength = pointLength / 2d;
            var maxDegreeOfParallelism = this.MaxDegreeOfParallelism;
            var xParam = this.OwnerChart.XAxis.GetConvertParam();
            var yParam = this.OwnerChart.YAxis.GetConvertParam();
            Func<double, ValueLocationConvertParam, double> xGetLocation = this.OwnerChart.XAxis.GetValueLocation;
            Func<double, ValueLocationConvertParam, double> yGetLocation = this.OwnerChart.YAxis.GetValueLocation;
            var excessDisplayAtEdge = this.ExcessDisplayAtEdge;
            var gradeBase = this.GradeBase;
            var gradeType = this.GradeType;
            return Task.Factory.StartNew<List<StreamGeometry>>(() =>
            {
                var points = PointCountStatistics(token, dataSource.XSource, dataSource.YSource, indexs, dataSource.XSourceConverter, dataSource.YSourceConverter, pointLength, maxDegreeOfParallelism, xParam, yParam, xGetLocation, yGetLocation, excessDisplayAtEdge);
                if (points == null) return null;
                var maxcount = points.Max(p => p.Value.Max(k => k.Value));
                if (maxcount <= 0) return null;
                var widthcount = points.Count;
                var heightcount = points.Max(p => p.Value.Count);
                var streamCount = 0;
                switch (gradeType)
                {
                    case GradeType.Line:
                        streamCount = maxcount / gradeBase + 1;
                        break;
                    case GradeType.Log:
                        streamCount = Convert.ToInt32(Math.Log(maxcount, gradeBase)) + 1;
                        break;
                    default:
                        break;
                }
                Dictionary<int, List<LinePoints>> streamLinePoints = new Dictionary<int, List<LinePoints>>();
                for (int i = 0; i < streamCount; i++)
                {
                    streamLinePoints[i] = new List<LinePoints>();
                }
                for (int w = 0; w < widthcount - 1; w++)
                {
                    for (int h = 0; h < heightcount - 1; h++)
                    {
                        var wb = w * pointLength;
                        var hb = h * pointLength;
                        var c1 = points[wb][hb];
                        var c2 = points[wb + pointLength][hb];
                        var c3 = points[wb + pointLength][hb + pointLength];
                        var c4 = points[wb][hb + pointLength];
                        for (int i = 1; i < streamCount; i++)
                        {
                            var ps = streamLinePoints[i];
                            var basecount = 0;
                            switch (gradeType)
                            {
                                case GradeType.Line:
                                    basecount = i * gradeBase;
                                    break;
                                case GradeType.Log:
                                    basecount = Convert.ToInt32(Math.Pow(gradeBase, i));
                                    break;
                                default:
                                    break;
                            }
                            var num = ((c1 >= basecount ? 1 : 0) << 3) + ((c2 >= basecount ? 1 : 0) << 2) + ((c3 >= basecount ? 1 : 0) << 1) + (c4 >= basecount ? 1 : 0);
                            switch (num)
                            {
                                case 0:
                                    if (i == 1 && c1 > 0) streamLinePoints[0].Add(new LinePoints() { Begin = new Point(w * pointLength + halfPointLength - 1, h * pointLength + halfPointLength - 1) });
                                    break;
                                case 1:
                                    ps.Add(new LinePoints() { Begin = new Point(wb, hb + halfPointLength), End = new Point(wb + halfPointLength, hb + pointLength) });
                                    break;
                                case 2:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + halfPointLength, hb + pointLength), End = new Point(wb + pointLength, hb + halfPointLength) });
                                    break;
                                case 3:
                                    ps.Add(new LinePoints() { Begin = new Point(wb, hb + halfPointLength), End = new Point(wb + pointLength, hb + halfPointLength) });
                                    break;
                                case 4:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + pointLength, hb + halfPointLength), End = new Point(wb + halfPointLength, hb) });
                                    break;
                                case 5:
                                    ps.Add(new LinePoints() { Begin = new Point(wb, hb + halfPointLength), End = new Point(wb + halfPointLength, hb) });
                                    ps.Add(new LinePoints() { Begin = new Point(wb + pointLength, hb + halfPointLength), End = new Point(wb + halfPointLength, hb + pointLength) });
                                    break;
                                case 6:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + halfPointLength, hb + pointLength), End = new Point(wb + halfPointLength, hb) });
                                    break;
                                case 7:
                                    ps.Add(new LinePoints() { Begin = new Point(wb, hb + halfPointLength), End = new Point(wb + halfPointLength, hb) });
                                    break;
                                case 8:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + halfPointLength, hb), End = new Point(wb, hb + halfPointLength) });
                                    break;
                                case 9:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + halfPointLength, hb), End = new Point(wb + halfPointLength, hb + pointLength) });
                                    break;
                                case 10:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + halfPointLength, hb), End = new Point(wb + pointLength, hb + halfPointLength) });
                                    ps.Add(new LinePoints() { Begin = new Point(wb + halfPointLength, hb + pointLength), End = new Point(wb, hb + halfPointLength) });
                                    break;
                                case 11:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + halfPointLength, hb), End = new Point(wb + pointLength, hb + halfPointLength) });
                                    break;
                                case 12:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + pointLength, hb + halfPointLength), End = new Point(wb, hb + halfPointLength) });
                                    break;
                                case 13:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + pointLength, hb + halfPointLength), End = new Point(wb + halfPointLength, hb + pointLength) });
                                    break;
                                case 14:
                                    ps.Add(new LinePoints() { Begin = new Point(wb + halfPointLength, hb + pointLength), End = new Point(wb, hb + halfPointLength) });
                                    break;
                                case 15:
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }
                List<StreamGeometry> streams = new List<StreamGeometry>();
                for (int i = 0; i < streamCount; i++)
                {
                    var stream = new StreamGeometry();
                    streams.Add(stream);
                    var ps = streamLinePoints[i];
                    using (var sgc = stream.Open())
                    {
                        if (i == 0)
                        {
                            foreach (var item in ps)
                            {
                                sgc.BeginFigure(item.Begin, true, true);
                                sgc.PolyLineTo(new Point[] { new Point(item.Begin.X + 1, item.Begin.Y), new Point(item.Begin.X + 1, item.Begin.Y + 1), new Point(item.Begin.X, item.Begin.Y + 1) }, true, true);
                            }
                        }
                        else
                        {
                            while (ps.Count > 0)
                            {
                                List<Point> temps = new List<Point>();
                                var begin = ps[0].Begin;
                                var end = ps[0].End;
                                temps.Add(begin);
                                temps.Add(end);
                                ps.RemoveAt(0);
                                var last = ps.FirstOrDefault(p => p.End == begin);
                                while (last != null)
                                {
                                    ps.Remove(last);
                                    begin = last.Begin;
                                    temps.Insert(0, begin);
                                    last = ps.FirstOrDefault(p => p.End == begin);
                                }
                                var next = ps.FirstOrDefault(p => p.Begin == end);
                                while (next != null)
                                {
                                    ps.Remove(next);
                                    end = next.End;
                                    temps.Add(end);
                                    next = ps.FirstOrDefault(p => p.Begin == end);
                                }
                                sgc.BeginFigure(temps[0], true, true);
                                temps.RemoveAt(0);
                                sgc.PolyLineTo(temps, true, true);
                            }
                        }
                    }
                    stream.Freeze();
                }
                return streams;
            }, token);
        }

        public override BaseSeriesModel GetModel()
        {
            return new ContourSeriesModel()
            {
                ShowFill = this.ShowFill,
                ShowStroke = this.ShowStroke,
                StrokeThickness = this.StrokeThickness,
                FillColorA = this.FillColorA,
                StrokeColorA = this.StrokeColorA,
                ExcessDisplayAtEdge = this.ExcessDisplayAtEdge,
                PointLength = this.PointLength,
                GradeBase = this.GradeBase,
                GradeType = this.GradeType
            };
        }
    }

    /// <summary>
    /// 分级方式
    /// </summary>
    public enum GradeType
    {
        /// <summary>
        /// 线性
        /// </summary>
        Line = 0,
        /// <summary>
        /// 对数
        /// </summary>
        Log = 1,
    }

    internal class LinePoints
    {
        public Point Begin { get; set; }
        public Point End { get; set; }

    }

    public class ContourSeriesModel : BaseSeriesModel
    {
        public int PointLength { get; set; }
        public int GradeBase { get; set; }
        public GradeType GradeType { get; set; }
    }
}
