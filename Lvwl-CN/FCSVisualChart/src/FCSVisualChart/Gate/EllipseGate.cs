using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace FCSVisualChart
{
    /// <summary>
    /// 圆形门
    /// </summary>
    public class EllipseGate : BaseGate
    {
        #region 数据
        /// <summary>
        /// 中心点
        /// </summary>
        public Point Center { get; private set; }
        /// <summary>
        /// 宽度
        /// </summary>
        public double HalfWidth { get; private set; }
        /// <summary>
        /// 高度
        /// </summary>
        public double HalfHeight { get; private set; }
        private double angle;
        /// <summary>
        /// 旋转角度
        /// </summary>
        public double Angle
        {
            get { return angle; }
            private set { angle = value; radian = Math.PI * value / 180; }
        }
        private double radian;
        /// <summary>
        /// 旋转弧度
        /// </summary>
        public double Radian
        {
            get { return radian; }
            private set { radian = value; angle = value * 180 / Math.PI; }
        }

        private double ViewHalfWidth { get; set; }
        private double ViewHalfHeight { get; set; }
        #endregion
        public EllipseGate() : base() { }
        public EllipseGate(EllipseGateModel model) : base(model)
        {
            this.Center = model.Center;
            this.HalfWidth = model.HalfWidth;
            this.HalfHeight = model.HalfHeight;
            this.Angle = model.Angle;
        }
        /// <summary>
        /// 初始化
        /// </summary>
        protected override void Init()
        {
            this.ShortName = "E";
            base.Init();
        }

        #region 门创建
        Point CreateStartPoint;
        internal override void Panel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (IsCreating && sender is Panel panel)
            {
                var point = e.GetPosition(panel);
                CreateStartPoint = new Point(OwnerChart.XAxis.GetLocationAxisValue(point.X), OwnerChart.YAxis.GetLocationAxisValue(point.Y));
            }
        }
        internal override void Panel_MouseMove(object sender, MouseEventArgs e)
        {
            if (sender is Panel panel && e.LeftButton == MouseButtonState.Pressed)
            {
                if (IsCreating && CreateStartPoint != default)
                {
                    var point = e.GetPosition(panel);
                    var tempaxisx = OwnerChart.XAxis.GetLocationAxisValue(point.X);
                    var tempaxisy = OwnerChart.YAxis.GetLocationAxisValue(point.Y);
                    var centerx = OwnerChart.XAxis.AxisValueToValue((tempaxisx + CreateStartPoint.X) / 2);
                    var centery = OwnerChart.YAxis.AxisValueToValue((tempaxisy + CreateStartPoint.Y) / 2);
                    Center = new Point(centerx, centery);
                    HalfWidth = Math.Abs((tempaxisx - CreateStartPoint.X) / 2);
                    HalfHeight = Math.Abs((tempaxisy - CreateStartPoint.Y) / 2);
                    Draw();
                }
                else if (IsEditing && EditingPath != null)
                {
                    var point = e.GetPosition(panel);
                    var tempaxisx = OwnerChart.XAxis.GetLocationAxisValue(point.X);
                    var tempaxisy = OwnerChart.YAxis.GetLocationAxisValue(point.Y);
                    var centerx = OwnerChart.XAxis.ValueToAxisValue(Center.X);
                    var centery = OwnerChart.YAxis.ValueToAxisValue(Center.Y);
                    var index = ControlPaths.IndexOf(EditingPath);
                    switch (index)
                    {
                        case 0:
                        case 1:
                            HalfWidth = Math.Sqrt(Math.Pow(tempaxisx - centerx, 2) + Math.Pow(tempaxisy - centery, 2));
                            Radian = Math.Atan((centery - tempaxisy) / (tempaxisx - centerx));
                            break;
                        case 2:
                        case 3:
                            HalfHeight = Math.Sqrt(Math.Pow(tempaxisx - centerx, 2) + Math.Pow(centery - tempaxisy, 2));
                            Radian = Math.Atan((tempaxisx - centerx) / (tempaxisy - centery));
                            break;
                        default:
                            break;
                    }
                    Draw();
                }
                else if (IsMoving)
                {
                    GatePath_MouseMove(GatePath, e);
                }
            }
        }
        internal override void Panel_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (IsCreating)
            {
                IsCreating = false;
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
            var px = OwnerChart.XAxis.GetConvertParam();
            var py = OwnerChart.YAxis.GetConvertParam();
            var tempx = OwnerChart.XAxis.GetValueLocation(Center.X, px);
            var tempy = OwnerChart.YAxis.GetValueLocation(Center.Y, py);
            ViewHalfWidth = Math.Abs(OwnerChart.XAxis.GetAxisValueLocation(OwnerChart.XAxis.ValueToAxisValue(Center.X, px.ValueAxisParam) + HalfWidth, px) - tempx);
            ViewHalfHeight = Math.Abs(OwnerChart.YAxis.GetAxisValueLocation(OwnerChart.YAxis.ValueToAxisValue(Center.Y, py.ValueAxisParam) + HalfHeight, py) - tempy);
            GeometryGroup group = new GeometryGroup();
            #region 图形
            var geometry = new StreamGeometry();
            using (var sgc = geometry.Open())
            {
                sgc.BeginFigure(new Point(tempx - ViewHalfWidth, tempy), true, true);
                sgc.ArcTo(new Point(tempx + ViewHalfWidth, tempy), new Size(ViewHalfWidth, ViewHalfHeight), 180, true, SweepDirection.Counterclockwise, true, true);
                sgc.ArcTo(new Point(tempx - ViewHalfWidth, tempy), new Size(ViewHalfWidth, ViewHalfHeight), 180, true, SweepDirection.Counterclockwise, true, true);
            }
            geometry.Transform = new RotateTransform(this.angle, tempx, tempy);
            geometry.Freeze();
            group.Children.Add(geometry);
            #endregion
            #region 区域名称
            group.Children.Add(CreateFormattedTextGeometry(Areas[0].Name, tempx, tempy));
            #endregion
            group.Freeze();
            return group;
        }
        #endregion

        #region 门控制
        /// <summary>
        /// 创建门的控制按钮
        /// </summary>
        protected override void DrawControl()
        {
            var px = OwnerChart.XAxis.GetConvertParam();
            var py = OwnerChart.YAxis.GetConvertParam();
            var sin = Math.Sin(radian);
            var cos = Math.Cos(radian);
            var tempx = OwnerChart.XAxis.GetValueLocation(Center.X, px);
            var tempy = OwnerChart.YAxis.GetValueLocation(Center.Y, py);
            var temphalfwidth = OwnerChart.XAxis.GetAxisValueLocation(OwnerChart.XAxis.ValueToAxisValue(Center.X, px.ValueAxisParam) + HalfWidth, px) - tempx;
            var temphalfheight = OwnerChart.YAxis.GetAxisValueLocation(OwnerChart.YAxis.ValueToAxisValue(Center.Y, py.ValueAxisParam) + HalfHeight, py) - tempy;
            var subx1 = temphalfwidth * cos;
            var suby1 = temphalfwidth * sin;
            var subx2 = temphalfheight * sin;
            var suby2 = temphalfheight * cos;
            var g1 = CreateControlGeometry(new Point(tempx - subx1, tempy - suby1));
            var g2 = CreateControlGeometry(new Point(tempx + subx1, tempy + suby1));
            var g3 = CreateControlGeometry(new Point(tempx + subx2, tempy - suby2));
            var g4 = CreateControlGeometry(new Point(tempx - subx2, tempy + suby2));
            if (ControlPaths.Count == 0)
            {
                ControlPaths.Add(new Path() { Data = g1, Cursor = Cursors.SizeAll });
                ControlPaths.Add(new Path() { Data = g2, Cursor = Cursors.SizeAll });
                ControlPaths.Add(new Path() { Data = g3, Cursor = Cursors.SizeAll });
                ControlPaths.Add(new Path() { Data = g4, Cursor = Cursors.SizeAll });
            }
            else
            {
                ControlPaths[0].Data = g1;
                ControlPaths[1].Data = g2;
                ControlPaths[2].Data = g3;
                ControlPaths[3].Data = g4;
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
                var tempx = OwnerChart.XAxis.GetLocationValue(OwnerChart.XAxis.GetValueLocation(Center.X) + x);
                var tempy = OwnerChart.YAxis.GetLocationValue(OwnerChart.YAxis.GetValueLocation(Center.Y) + y);
                Center = new Point(tempx, tempy);
            }
            if (GatePath != null && GatePath.RenderTransform is MatrixTransform transform)
            {
                var matrix = transform.Matrix;
                transform.Matrix = new Matrix(1, 0, 0, 1, matrix.OffsetX + x, matrix.OffsetY + y);
            }
            DrawControl();
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
            if (param is EllipseAreaIndexsFuncParam p)
            {
                var tempx = p.XValueLocation(x, p.XParam);
                var tempy = p.YValueLocation(y, p.YParam);
                var subx = tempx - p.ViewCenter.X;
                var suby = p.ViewCenter.Y - tempy;
                tempx = subx * p.CosRadian - suby * p.SinRadian + p.ViewCenter.X;
                tempy = suby * p.CosRadian + subx * p.SinRadian + p.ViewCenter.Y;
                return (Math.Pow(tempx - p.ViewCenter.X, 2) / p.HalfViewWidthPow + Math.Pow(tempy - p.ViewCenter.Y, 2) / p.HalfViewHeightPow) <= 1 ? p.Areas[0] : null;
            }
            else return null;
        }
        /// <summary>
        /// 获取判断点是否在门区域内的方法的参数
        /// </summary>
        /// <returns></returns>
        public override AreaIndexsFuncParam GetAreaIndexsFuncParam()
        {
            if (Areas == null || Areas.Length != 1 || OwnerChart == null || OwnerChart.XAxis == null || OwnerChart.YAxis == null) return null;
            var px = OwnerChart.XAxis.GetConvertParam();
            var py = OwnerChart.YAxis.GetConvertParam();
            Func<double, ValueLocationConvertParam, double> fx = OwnerChart.XAxis.GetValueLocation;
            Func<double, ValueLocationConvertParam, double> fy = OwnerChart.YAxis.GetValueLocation;
            return new EllipseAreaIndexsFuncParam()
            {
                Areas = this.Areas,
                HalfViewWidthPow = Math.Pow(this.ViewHalfWidth, 2),
                HalfViewHeightPow = Math.Pow(this.ViewHalfHeight, 2),
                SinRadian = Math.Sin(this.Radian),
                CosRadian = Math.Cos(this.Radian),
                ViewCenter = new Point(fx(this.Center.X, px), fy(this.Center.Y, py)),
                XParam = px,
                YParam = py,
                XValueLocation = fx,
                YValueLocation = fy
            };
        }
        /// <summary>
        /// 部分图形在轴更换后需要重新绘制和更新区域数据
        /// </summary>
        internal override void AxisChanged()
        {
            if (ViewHalfWidth != 0 && ViewHalfHeight != 0)
            {
                var px = OwnerChart.XAxis.GetConvertParam();
                var py = OwnerChart.YAxis.GetConvertParam();
                var centeraxisx = OwnerChart.XAxis.ValueToAxisValue(Center.X, px.ValueAxisParam);
                var centeraxisy = OwnerChart.YAxis.ValueToAxisValue(Center.Y, py.ValueAxisParam);
                var centerx = OwnerChart.XAxis.GetAxisValueLocation(centeraxisx, px);
                var centery = OwnerChart.YAxis.GetAxisValueLocation(centeraxisy, py);
                HalfWidth = OwnerChart.XAxis.GetLocationAxisValue(centerx + ViewHalfWidth, px) - centeraxisx;
                HalfHeight = OwnerChart.YAxis.GetLocationAxisValue(centery - ViewHalfHeight, py) - centeraxisy;
            }
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
            return new EllipseGateModel()
            {
                AreaNames = this.Areas.Select(p => p.Name).ToArray(),
                AreaColors = this.Areas.Select(p => string.Concat(p.DisplayColor.A.ToString("X2"), p.DisplayColor.R.ToString("X2"), p.DisplayColor.G.ToString("X2"), p.DisplayColor.B.ToString("X2"))).ToArray(),
                Center = this.Center,
                HalfWidth = this.HalfWidth,
                HalfHeight = this.HalfHeight,
                Angle = this.Angle
            };
        }
    }

    public class EllipseGateModel : BaseGateModel
    {
        /// <summary>
        /// 圆中心点，实际值
        /// </summary>
        public Point Center { get; set; }
        /// <summary>
        /// 椭圆长度一半
        /// </summary>
        public double HalfWidth { get; set; }
        /// <summary>
        /// 椭圆高度一半
        /// </summary>
        public double HalfHeight { get; set; }
        /// <summary>
        /// 旋转角度
        /// </summary>
        public double Angle { get; set; }
    }

    public class EllipseAreaIndexsFuncParam : AreaIndexsFuncParam
    {
        /// <summary>
        /// 图中实际坐标点位
        /// </summary>
        internal Point ViewCenter { get; set; }
        /// <summary>
        /// 图中长轴半径平方
        /// </summary>
        internal double HalfViewWidthPow { get; set; }
        /// <summary>
        /// 图中短轴半径平方
        /// </summary>
        internal double HalfViewHeightPow { get; set; }
        internal double SinRadian { get; set; }
        internal double CosRadian { get; set; }

        internal ValueLocationConvertParam XParam { get; set; }
        internal ValueLocationConvertParam YParam { get; set; }

        internal Func<double, ValueLocationConvertParam, double> XValueLocation { get; set; }
        internal Func<double, ValueLocationConvertParam, double> YValueLocation { get; set; }

    }
}
