using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace FCSVisualChart
{
    /// <summary>
    /// 多边形门
    /// </summary>
    public class PolygonGate : BaseGate
    {
        #region 数据
        /// <summary>
        /// 数据点位
        /// </summary>
        public List<Point> Points { get; private set; }
        #endregion
        public PolygonGate() : base() { Points = new List<Point>(); }
        public PolygonGate(PolygonGateModel model) : base(model)
        {
            this.Points = model.Points;
            if (this.Points == null) Points = new List<Point>();
        }
        /// <summary>
        /// 初始化
        /// </summary>
        protected override void Init()
        {
            this.ShortName = "P";
            base.Init();
        }

        #region 门创建
        internal override void Panel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (IsCreating && sender is Panel panel && Points.Count <= 0)
            {
                var point = e.GetPosition(panel);
                var x = OwnerChart.XAxis.GetLocationValue(point.X);
                var y = OwnerChart.YAxis.GetLocationValue(point.Y);
                Points.Add(new Point(x, y));
            }
        }
        internal override void Panel_MouseMove(object sender, MouseEventArgs e)
        {
            if (sender is Panel panel)
            {
                if (IsCreating && Points.Count > 0)
                {
                    var point = e.GetPosition(panel);
                    var x = OwnerChart.XAxis.GetLocationValue(point.X);
                    var y = OwnerChart.YAxis.GetLocationValue(point.Y);
                    Points[0] = new Point(x, y);
                    Draw();
                }
                else if (IsEditing && EditingPath != null && e.LeftButton == MouseButtonState.Pressed)
                {
                    var point = e.GetPosition(panel);
                    var x = OwnerChart.XAxis.GetLocationValue(point.X);
                    var y = OwnerChart.YAxis.GetLocationValue(point.Y);
                    var index = ControlPaths.IndexOf(EditingPath);
                    Points[index] = new Point(x, y);
                    Draw();
                }
                else if (IsMoving && e.LeftButton == MouseButtonState.Pressed)
                {
                    GatePath_MouseMove(GatePath, e);
                }
            }
        }
        internal override void Panel_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (IsCreating && sender is Panel panel)
            {
                var point = e.GetPosition(panel);
                var x = OwnerChart.XAxis.GetLocationValue(point.X);
                var y = OwnerChart.YAxis.GetLocationValue(point.Y);
                var temp = new Point(x, y);
                if (Points.Count > 3 && Points[Points.Count - 1].Equals(temp))
                {
                    Points.RemoveAt(0);
                    IsCreating = false;
                }
                else
                {
                    Points.Add(temp);
                }
                Draw();
            }
            else if (IsEditing)
            {
                IsEditing = false;
                EditingPath = null;
                RefreshAreaIndexs();
            }
            else if (IsMoving)
            {
                IsMoving = false;
                GateMoveStartPoint = default;
                Draw();
            }
        }
        #endregion

        #region 门绘制
        /// <summary>
        /// 创建门的矢量数据
        /// </summary>
        /// <returns></returns>
        protected override Geometry CreateGatePathData()
        {
            if (Points == null || Points.Count <= 0) return null;
            var px = OwnerChart.XAxis.GetConvertParam();
            var py = OwnerChart.YAxis.GetConvertParam();
            Func<double, ValueLocationConvertParam, double> fx = OwnerChart.XAxis.GetValueLocation;
            Func<double, ValueLocationConvertParam, double> fy = OwnerChart.YAxis.GetValueLocation;
            List<Point> ps = Points.Select(p => new Point(fx(p.X, px), fy(p.Y, py))).ToList();
            var centerX = ps.Average(p => p.X);
            var centerY = ps.Average(p => p.Y);
            GeometryGroup group = new GeometryGroup();
            #region 图形
            var geometry = new StreamGeometry();
            using (var sgc = geometry.Open())
            {
                var p1 = ps[0];
                ps.RemoveAt(0);
                sgc.BeginFigure(p1, true, true);
                sgc.PolyLineTo(ps, true, true);
            }
            geometry.Freeze();
            group.Children.Add(geometry);
            #endregion
            #region 区域名称
            if (!IsCreating) group.Children.Add(CreateFormattedTextGeometry(Areas[0].Name, centerX, centerY));
            #endregion
            group.Freeze();
            return group;
        }
        #endregion

        #region 门控制
        protected override void DrawControl()
        {
            var px = OwnerChart.XAxis.GetConvertParam();
            var py = OwnerChart.YAxis.GetConvertParam();
            Func<double, ValueLocationConvertParam, double> fx = OwnerChart.XAxis.GetValueLocation;
            Func<double, ValueLocationConvertParam, double> fy = OwnerChart.YAxis.GetValueLocation;
            if (ControlPaths.Count == 0)
            {
                foreach (var p in Points)
                {
                    ControlPaths.Add(new Path() { Data = CreateControlGeometry(new Point(fx(p.X, px), fy(p.Y, py))), Cursor = Cursors.SizeAll });
                }
            }
            else
            {
                for (int i = 0; i < Points.Count; i++)
                {
                    var p = Points[i];
                    ControlPaths[i].Data = CreateControlGeometry(new Point(fx(p.X, px), fy(p.Y, py)));
                }
            }
        }
        #endregion

        #region 门移动
        /// <summary>
        /// 移动门位置
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="changeAxisValue">是否变更了坐标值</param>
        internal override void Move(double x, double y, bool changeAxisValue = false)
        {
            if (changeAxisValue)
            {
                var px = OwnerChart.XAxis.GetConvertParam();
                var py = OwnerChart.YAxis.GetConvertParam();
                Func<double, ValueLocationConvertParam, double> fx = OwnerChart.XAxis.GetValueLocation;
                Func<double, ValueLocationConvertParam, double> fy = OwnerChart.YAxis.GetValueLocation;
                Func<double, ValueLocationConvertParam, double> fcx = OwnerChart.XAxis.GetLocationValue;
                Func<double, ValueLocationConvertParam, double> fcy = OwnerChart.YAxis.GetLocationValue;
                Points = Points.Select(p => new Point(fcx(fx(p.X, px) + x, px), fcy(fy(p.Y, py) + y, py))).ToList();
            }
            if (GatePath != null && GatePath.RenderTransform is MatrixTransform transform)
            {
                var matrix = transform.Matrix;
                transform.Matrix = new Matrix(1, 0, 0, 1, matrix.OffsetX + x, matrix.OffsetY + y);
            }
            if (!IsCreating) DrawControl();
        }
        #endregion

        #region 判断数据是否在区域内
        /// <summary>
        /// 数据在门的哪个区域内
        /// </summary>
        /// <param name="x">x轴真实值</param>
        /// <param name="y">y轴真实值</param>
        /// <param name="param">数据在区域内判断方法的参数</param>
        /// <returns>null表示不在任何区域内</returns>
        public override GateArea DataInArea(double x, double y, AreaIndexsFuncParam param)
        {
            if (param is PolygonAreaIndexsFuncParam p)
            {
                if (x < p.MinX || x > p.MaxX || y < p.MinY || y > p.MaxY) return null;
                var tempx = p.XAxisValue(x, p.XParam);
                var tempy = p.YAxisValue(y, p.YParam);
                if (OnePointIsInPolygon(tempx, tempy, p.AxisPoints.Count, p.AxisPoints)) return p.Areas[0];
                return null;
            }
            else return null;
        }
        /// <summary>
        /// 获取判断点是否在门区域内的方法的参数
        /// </summary>
        /// <returns></returns>
        public override AreaIndexsFuncParam GetAreaIndexsFuncParam()
        {
            if (Areas == null || Areas.Length != 1) return null;
            var px = OwnerChart.XAxis.GetValueAxisConvertParam();
            var py = OwnerChart.YAxis.GetValueAxisConvertParam();
            Func<double, ValueAxisConvertParamBase, double> fx = OwnerChart.XAxis.ValueToAxisValue;
            Func<double, ValueAxisConvertParamBase, double> fy = OwnerChart.YAxis.ValueToAxisValue;
            var ps = Points.Select(p => new Point(fx(p.X, px), fy(p.Y, py))).ToList();
            return new PolygonAreaIndexsFuncParam()
            {
                Areas = this.Areas,
                AxisPoints = ps,
                MinX = Points.Min(p => p.X),
                MaxX = Points.Max(p => p.X),
                MinY = Points.Min(p => p.Y),
                MaxY = Points.Max(p => p.Y),
                XParam = px,
                YParam = py,
                XAxisValue = fx,
                YAxisValue = fy
            };
        }

        /// <summary>
        /// 判断某个点是不是在多边形内
        /// </summary>
        /// <param name="tempx">点x值</param>
        /// <param name="tempy">点y值</param>
        /// <param name="nvert">多边形角数量</param>
        /// <param name="areaPoints">多边形点位置</param>
        /// <returns></returns>
        private bool OnePointIsInPolygon(double tempx, double tempy, int nvert, List<Point> areaPoints)
        {
            int i, j, c = 0;
            for (i = 0, j = nvert - 1; i < nvert; j = i++)
            {
                var pointI = areaPoints[i];
                var pointJ = areaPoints[j];
                if (((pointI.Y > tempy) != (pointJ.Y > tempy)) && (tempx < (pointJ.X - pointI.X) * (tempy - pointI.Y) / (pointJ.Y - pointI.Y) + pointI.X))
                {
                    c = 1 + c;
                }
            }
            return (c % 2 != 0);
        }
        /// <summary>
        /// 部分图形在轴更换后需要重新绘制和更新区域数据
        /// </summary>
        internal override void AxisChanged()
        {
            base.AxisChanged();
            RefreshAreaIndexs();
        }
        #endregion

        /// <summary>
        /// 获取门的数据模型
        /// </summary>
        /// <returns></returns>
        public override BaseGateModel GetModel()
        {
            return new PolygonGateModel()
            {
                AreaNames = this.Areas.Select(p => p.Name).ToArray(),
                Points = this.Points.ToList()
            };
        }
    }

    public class PolygonGateModel : BaseGateModel
    {
        public List<Point> Points { get; set; }
    }

    public class PolygonAreaIndexsFuncParam : AreaIndexsFuncParam
    {
        internal List<Point> AxisPoints { get; set; }
        internal double MinX { get; set; }
        internal double MaxX { get; set; }
        internal double MinY { get; set; }
        internal double MaxY { get; set; }

        internal ValueAxisConvertParamBase XParam { get; set; }
        internal ValueAxisConvertParamBase YParam { get; set; }

        internal Func<double, ValueAxisConvertParamBase, double> XAxisValue { get; set; }
        internal Func<double, ValueAxisConvertParamBase, double> YAxisValue { get; set; }
    }
}
