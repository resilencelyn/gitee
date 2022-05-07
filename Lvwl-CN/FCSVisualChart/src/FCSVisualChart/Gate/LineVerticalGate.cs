using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace FCSVisualChart
{
    /// <summary>
    /// 竖线门
    /// </summary>
    public class LineVerticalGate : BaseGate
    {
        #region 数据
        /// <summary>
        /// 竖线的x真实值
        /// </summary>
        public double X { get; private set; }
        #endregion
        public LineVerticalGate() : base() { }
        public LineVerticalGate(LineVerticalGateModel model) : base(model)
        {
            this.X = model.X;
        }
        /// <summary>
        /// 初始化
        /// </summary>
        protected override void Init()
        {
            this.ShortName = "V";
            this.AreaCount = 2;
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
                    this.X = OwnerChart.XAxis.GetLocationValue(point.X);
                    Draw();
                }
                else if (IsEditing && EditingPath != null && e.LeftButton == MouseButtonState.Pressed)
                {
                    var point = e.GetPosition(panel);
                    this.X = OwnerChart.XAxis.GetLocationValue(point.X);
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
            var tempx = OwnerChart.XAxis.GetValueLocation(X);
            var tempy1 = 0;
            var tempy2 = OwnerChart.ViewPanel.ActualHeight;
            GeometryGroup group = new GeometryGroup();
            #region 图形
            var geometry = new StreamGeometry();
            using (var sgc = geometry.Open())
            {
                sgc.BeginFigure(new Point(tempx, tempy1), false, false);
                sgc.LineTo(new Point(tempx, tempy2), true, true);
            }
            geometry.Freeze();
            group.Children.Add(geometry);
            #endregion
            #region 区域名称
            group.Children.Add(CreateFormattedTextGeometry(Areas[0].Name, tempx - 15, OwnerChart.ViewPanel.ActualHeight / 2, 1d, 0.5d));
            group.Children.Add(CreateFormattedTextGeometry(Areas[1].Name, tempx + 15, OwnerChart.ViewPanel.ActualHeight / 2, 0d, 0.5d));
            #endregion
            group.Freeze();
            return group;
        }
        #endregion

        #region 门控制
        protected override void DrawControl()
        {
            var tempx = OwnerChart.XAxis.GetValueLocation(this.X);
            var tempy = OwnerChart.YAxis.ActualHeight / 2;
            var g = CreateControlGeometry(new Point(tempx, tempy));
            if (ControlPaths.Count == 0)
            {
                ControlPaths.Add(new Path() { Data = g, Cursor = Cursors.SizeWE });
            }
            else
            {
                ControlPaths[0].Data = g;
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
                this.X = OwnerChart.XAxis.GetLocationValue(OwnerChart.XAxis.GetValueLocation(this.X) + x);
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
            return (param is LineVerticalAreaIndexsFuncParam p && x >= p.X) ? param.Areas[1] : param.Areas[0];
        }
        /// <summary>
        /// 获取判断点是否在门区域内的方法的参数
        /// </summary>
        /// <returns></returns>
        public override AreaIndexsFuncParam GetAreaIndexsFuncParam()
        {
            if (Areas == null || Areas.Length != 2) return null;
            return new LineVerticalAreaIndexsFuncParam()
            {
                Areas = this.Areas,
                X = this.X
            };
        }
        #endregion

        /// <summary>
        /// 获取门的数据模型
        /// </summary>
        /// <returns></returns>
        public override BaseGateModel GetModel()
        {
            return new LineVerticalGateModel()
            {
                AreaNames = this.Areas.Select(p => p.Name).ToArray(),
                X = this.X
            };
        }
    }

    public class LineVerticalGateModel : BaseGateModel
    {
        public double X { get; set; }
    }

    public class LineVerticalAreaIndexsFuncParam : AreaIndexsFuncParam
    {
        internal double X { get; set; }
    }
}
