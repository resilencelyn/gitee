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
    /// 四象限门
    /// </summary>
    public class QuadrantGate : BaseGate
    {
        #region 数据
        public Point Center { get; private set; }
        public double Angle1 { get; private set; }
        public double Angle2 { get; private set; }
        public double Angle3 { get; private set; }
        public double Angle4 { get; private set; }
        #endregion

        const double HalfPI = Math.PI / 2;
        const double OnePI = Math.PI;
        const double OneHalfPI = Math.PI * 1.5d;
        const double DoublePI = Math.PI * 2;

        public QuadrantGate() { }
        public QuadrantGate(QuadrantGateModel model) : base(model)
        {
            this.Center = model.Center;
            this.Angle1 = model.Angle1;
            this.Angle2 = model.Angle2;
            this.Angle3 = model.Angle3;
            this.Angle4 = model.Angle4;
        }
        /// <summary>
        /// 初始化
        /// </summary>
        protected override void Init()
        {
            this.ShortName = "Q";
            this.AreaCount = 4;
            base.Init();
        }

        #region 门创建
        internal override void Panel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

        }
        internal override void Panel_MouseMove(object sender, MouseEventArgs e)
        {
            if (sender is Panel panel)
            {
                if (IsCreating)
                {
                    var point = e.GetPosition(panel);
                    var tempx = OwnerChart.XAxis.GetLocationValue(point.X);
                    var tempy = OwnerChart.YAxis.GetLocationValue(point.Y);
                    Center = new Point(tempx, tempy);
                    Draw();
                }
                else if (IsEditing && EditingPath != null && e.LeftButton == MouseButtonState.Pressed)
                {
                    var point = e.GetPosition(panel);
                    var px = OwnerChart.XAxis.GetConvertParam();
                    var py = OwnerChart.YAxis.GetConvertParam();
                    var tempx = OwnerChart.XAxis.GetValueLocation(Center.X, px);
                    var tempy = OwnerChart.YAxis.GetValueLocation(Center.Y, py);
                    var index = ControlPaths.IndexOf(EditingPath);
                    switch (index)
                    {
                        case 0:
                            this.Angle1 = Math.Atan2((tempy - point.Y), (tempx - point.X));
                            break;
                        case 1:
                            this.Angle2 = Math.Atan2((point.X - tempx), (tempy - point.Y));
                            break;
                        case 2:
                            this.Angle3 = Math.Atan2((point.Y - tempy), (point.X - tempx));
                            break;
                        case 3:
                            this.Angle4 = Math.Atan2((tempx - point.X), (point.Y - tempy));
                            break;
                        case 4:
                            Center = new Point(OwnerChart.XAxis.GetLocationValue(point.X, px), OwnerChart.YAxis.GetLocationValue(point.Y, py));
                            break;
                        default:
                            break;
                    }
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
            var subtempx = px.Length - tempx;
            var subtempy = py.Length - tempy;
            GeometryGroup group = new GeometryGroup();
            #region 图形
            var geometry = new StreamGeometry();
            using (var sgc = geometry.Open())
            {
                sgc.BeginFigure(new Point(tempx, tempy), false, false);
                sgc.LineTo(new Point(0, tempy - tempx * Math.Tan(this.Angle1)), true, true);
                sgc.BeginFigure(new Point(tempx, tempy), false, false);
                sgc.LineTo(new Point(tempx + tempy * Math.Tan(this.Angle2), 0), true, true);
                sgc.BeginFigure(new Point(tempx, tempy), false, false);
                sgc.LineTo(new Point(px.Length, tempy + subtempx * Math.Tan(this.Angle3)), true, true);
                sgc.BeginFigure(new Point(tempx, tempy), false, false);
                sgc.LineTo(new Point(tempx - subtempy * Math.Tan(this.Angle4), py.Length), true, true);
            }
            geometry.Freeze();
            group.Children.Add(geometry);
            #endregion
            #region 区域名称
            group.Children.Add(CreateFormattedTextGeometry(Areas[0].Name, 10, 10, 0d, 0d));
            group.Children.Add(CreateFormattedTextGeometry(Areas[1].Name, px.Length - 10, 10, 1d, 0d));
            group.Children.Add(CreateFormattedTextGeometry(Areas[2].Name, px.Length - 10, py.Length - 10, 1d, 1d));
            group.Children.Add(CreateFormattedTextGeometry(Areas[3].Name, 10, py.Length - 10, 0d, 1d));
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
            var tempx = OwnerChart.XAxis.GetValueLocation(Center.X, px);
            var tempy = OwnerChart.YAxis.GetValueLocation(Center.Y, py);
            var subtempx = px.Length - tempx;
            var subtempy = py.Length - tempy;
            var g1 = CreateControlGeometry(new Point(5, tempy - tempx * Math.Tan(this.Angle1)));
            var g2 = CreateControlGeometry(new Point(tempx + tempy * Math.Tan(this.Angle2), 5));
            var g3 = CreateControlGeometry(new Point(px.Length - 5, tempy + subtempx * Math.Tan(this.Angle3)));
            var g4 = CreateControlGeometry(new Point(tempx - subtempy * Math.Tan(this.Angle4), py.Length - 5));
            var g5 = CreateControlGeometry(new Point(tempx, tempy));
            if (ControlPaths.Count == 0)
            {
                ControlPaths.Add(new Path() { Data = g1, Cursor = Cursors.SizeNS });
                ControlPaths.Add(new Path() { Data = g2, Cursor = Cursors.SizeWE });
                ControlPaths.Add(new Path() { Data = g3, Cursor = Cursors.SizeNS });
                ControlPaths.Add(new Path() { Data = g4, Cursor = Cursors.SizeWE });
                ControlPaths.Add(new Path() { Data = g5, Cursor = Cursors.SizeAll });
            }
            else
            {
                ControlPaths[0].Data = g1;
                ControlPaths[1].Data = g2;
                ControlPaths[2].Data = g3;
                ControlPaths[3].Data = g4;
                ControlPaths[4].Data = g5;
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
                var tempx = OwnerChart.XAxis.GetLocationValue(OwnerChart.XAxis.GetValueLocation(Center.X, px) + x, px);
                var tempy = OwnerChart.YAxis.GetLocationValue(OwnerChart.YAxis.GetValueLocation(Center.Y, py) + y, py);
                Center = new Point(tempx, tempy);
            }
            Draw();
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
            if (param is QuadrantAreaIndexsFuncParam p)
            {
                var tempx = p.XValueLocation(x, p.XParam);
                var tempy = p.YValueLocation(y, p.YParam);
                var angle = Math.Atan2(p.AxisCenter.Y - tempy, tempx - p.AxisCenter.X);
                while (angle < 0) angle = DoublePI + angle;
                angle = angle % DoublePI;
                if (angle >= p.Angle2 && angle < p.Angle1) return p.Areas[0];
                else if (angle >= p.Angle1 && angle < p.Angle4) return p.Areas[3];
                else if (angle >= p.Angle4 && angle < p.Angle3 && p.Angle3 > HalfPI) return p.Areas[2];
                else if ((angle >= p.Angle4 || angle < p.Angle3) && p.Angle3 < HalfPI) return p.Areas[2];
                else return p.Areas[1];
            }
            return null;
        }
        /// <summary>
        /// 获取判断点是否在门区域内的方法的参数
        /// </summary>
        /// <returns></returns>
        public override AreaIndexsFuncParam GetAreaIndexsFuncParam()
        {
            if (Areas == null || Areas.Length != 4) return null;
            var param = new QuadrantAreaIndexsFuncParam()
            {
                Areas = this.Areas,
                Angle1 = OnePI - this.Angle1,
                Angle2 = HalfPI - this.Angle2,
                Angle3 = (DoublePI - this.Angle3) % DoublePI,
                Angle4 = OneHalfPI - this.Angle4,
                XParam = OwnerChart.XAxis.GetConvertParam(),
                YParam = OwnerChart.YAxis.GetConvertParam(),
                XValueLocation = OwnerChart.XAxis.GetValueLocation,
                YValueLocation = OwnerChart.YAxis.GetValueLocation
            };
            param.AxisCenter = new Point(param.XValueLocation(Center.X, param.XParam), param.YValueLocation(Center.Y, param.YParam));
            return param;
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
            return new QuadrantGateModel()
            {
                AreaNames = this.Areas.Select(p => p.Name).ToArray(),
                Center = this.Center,
                Angle1 = this.Angle1,
                Angle2 = this.Angle2,
                Angle3 = this.Angle3,
                Angle4 = this.Angle4
            };
        }
    }

    public class QuadrantGateModel : BaseGateModel
    {
        public Point Center { get; set; }
        public double Angle1 { get; set; }
        public double Angle2 { get; set; }
        public double Angle3 { get; set; }
        public double Angle4 { get; set; }
    }

    public class QuadrantAreaIndexsFuncParam : AreaIndexsFuncParam
    {
        internal Point AxisCenter { get; set; }
        internal double Angle1 { get; set; }
        internal double Angle2 { get; set; }
        internal double Angle3 { get; set; }
        internal double Angle4 { get; set; }

        internal ValueLocationConvertParam XParam { get; set; }
        internal ValueLocationConvertParam YParam { get; set; }

        internal Func<double, ValueLocationConvertParam, double> XValueLocation { get; set; }
        internal Func<double, ValueLocationConvertParam, double> YValueLocation { get; set; }
    }
}
