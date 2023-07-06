using DieEdit;
using DieEdit.Controls;
using DieEdit.Other;
using DieEdit.Source;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using static DieEdit.Other.RoundControl;

namespace UserCtrl
{
    public partial class LayerView : UserControl
    {
        public string _Name { get; set; } = string.Empty;
        public bool? _Visible { get; set; } = false;
        Bitmap _Image { get; set; }
        public int _Opacity { get; set; } = 100;
        bool bFlagCross = false;
        bool bFlagResize = false;
        bool ObjectControlMode_MouseCursor = false;
        bool MouseCursorSize = false;
        bool bOnly = false;

        RectangleF Image_Rect;
        double Ratio = 1;

        int nCursor = 0;
        Point ImageSize;
        RectangleF RectPosTemp;
        RectangleF pRectPosTemp;
        RectangleF MouseRectTemp;
        RectangleF MouseCursor_;
        RectangleF ControlRectTemp;

        RectangleF ScrollTemp;
        //
        // Select Object
        RectangleF ObjectSelectArea;
        RectangleF AreaEnlargeArea;
        // Current Mouse
        PointF pntPosTemp = new PointF();
        PointF pntLastPosTemp = new PointF();

        //11.14
        RectangleF pntDownAreaZoomIn = new RectangleF();
        RectangleF pntUpAreaZoomIn = new RectangleF();


        PointF pntImagePosTemp = new PointF();
        PointF pntImageLastPosTemp = new PointF();
        List<PointF> pntsPolygon = new List<PointF>();
        List<PointF> pntsImagePolygon = new List<PointF>();
        bool bRectBtnDown = false;
        bool bSelectedRegion = false;
        int nSelectedGuideRect = -1;
        int nSelectedObject = -1;
        int nSelectedPolyPos = -1;

        List<PointF> ShapeMoveGuideLine = new List<PointF>();

        SmartList<Shape_Obj> ObjectTemp = new SmartList<Shape_Obj>();
        Shape_Obj ResizeObject;

        Pen DrawLineColor;
        object Locker = new object();

        List<LayerVisible> LayerVisibleTemp = new List<LayerVisible>();
        List<LayerTransparent> LayerTransparentTemp = new List<LayerTransparent>();





        // Func Flag
        #region Flag
        public bool PrintCoord { get; set; } = true;
        public bool MouseClickEvent { get; set; } = true;
        public bool MouseMoveEvent { get; set; } = true;

        public bool MouseUpEvent { get; set; } = true;
        public bool MouseDownEvent { get; set; } = true;
        public bool MouseEnterEvent { get; set; } = true;

        public bool MouseLeaveEvent { get; set; } = true;
        public bool MouseWheelEvent { get; set; } = true;
        #endregion
        #region Delegate Event
        // Delegate Event
        public DelegateEvent.delZoomInChain ZommInChain { get; set; }
        public DelegateEvent.delZoomOutChain ZoomOutChain { get; set; }
        public DelegateEvent.delMouseMoveChain MouseMoveChain { get; set; }
        public DelegateEvent.delMouseDownChain MouseDownChain { get; set; }
        public DelegateEvent.delMouseEnterChain MouseEnterChain { get; set; }
        public DelegateEvent.delMouseLeaveChain MouseLeaveChain { get; set; }
        public DelegateEvent.delMouseClickChain MouseClickChain { get; set; }
        public DelegateEvent.delMouseWheelChain MouseWheelChain { get; set; }
        public DelegateEvent.delMouseWheelChain MouseWheelChainCb { get; set; }
        #endregion
        public void SetPosTemp(PointF p)
        {
            pntPosTemp = p;
        }
        public void SetLastPosTemp(PointF p)
        {
            pntLastPosTemp = p;
        }
        public PictureBox GetPictureBox()
        {
            return Pic_LayerView;
        }

        public void PictureBoxDispose()
        {
            Image Temp = Pic_LayerView.Image;
            Pic_LayerView.Image = new Bitmap(100, 100);
            _Image.Dispose();
            Temp.Dispose();

        }
        public void SetDrawPen(Color color)
        {
            DrawLineColor = new Pen(Color.Red);
        }
        public LayerView(string name, bool? visible, Control ctrl)
        {
            try
            {
                InitializeComponent();
                _Name = name;
                _Visible = visible;
                Parent = ctrl;
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        public void MakeTransparent(int a, int r, int g, int b)
        {
            _Image.MakeTransparent(Color.FromArgb(a, r, g, b));
        }

        public void CombineMouseWheelEvent(ref DelegateEvent.delMouseWheelChain delMouseChain)
        {
            try
            {
                MouseWheelChainCb = (DelegateEvent.delMouseWheelChain)Delegate.Combine(delMouseChain, new DelegateEvent.delMouseWheelChain(Pic_LayerView_MouseWheel_Event));
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public void ZoomIn(int direction)
        {
            try
            {
                Program.frm_Layers.UnSelectedObject();
                //double dRatioTemp = Ratio;
                //Ratio = Program.frm_Main.m_UcRcpTeachPic2.GetRatio();
                Image_Rect = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();

                pntPosTemp.X = Util.fnCalCoordinate(0, pntImagePosTemp.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                pntPosTemp.Y = Util.fnCalCoordinate(0, pntImagePosTemp.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                pntLastPosTemp.X = Util.fnCalCoordinate(0, pntImageLastPosTemp.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                pntLastPosTemp.Y = Util.fnCalCoordinate(0, pntImageLastPosTemp.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                Pic_LayerView.Invalidate();
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        public void ZoomOut(int direction)
        {
            try
            {
                Program.frm_Layers.UnSelectedObject();
                double dRatioTemp = Ratio;
                Ratio = Program.frm_Main.m_UcRcpTeachPic2.GetRatio();
                Image_Rect = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();

                pntPosTemp.X = Util.fnCalCoordinate(0, pntImagePosTemp.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                pntPosTemp.Y = Util.fnCalCoordinate(0, pntImagePosTemp.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                pntLastPosTemp.X = Util.fnCalCoordinate(0, pntImageLastPosTemp.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                pntLastPosTemp.Y = Util.fnCalCoordinate(0, pntImageLastPosTemp.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());

                Pic_LayerView.Invalidate();
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        public LayerView()
        {
            InitializeComponent();

            //Image_Rect = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();
        }
        public void Initialize()
        {

        }

        public void SetImageRect(RectangleF rect)
        {
            Image_Rect = rect;
        }

        public void LinkImage(ref Bitmap image)
        {
            _Image = image;
            Pic_LayerView.Image = _Image;
            ImageSize.X = _Image.Width;
            ImageSize.Y = _Image.Height;
        }

        public void SetPictureBoxSize(Point start_pos, Size size)
        {
            Location = start_pos;
            Size = size;
        }
        private void Pic_LayerView_Paint(object sender, PaintEventArgs e)
        {
            try
            {
                // 지점간 측정 기능 :: 점선 그리기 및 거리 측정 표시
                if (Frm_Main.MeasureLengthMode)
                {
                    if (pntImagePosTemp.X != 0 && pntImagePosTemp.Y != 0)
                    {
                        using (Pen pen = new System.Drawing.Pen(System.Drawing.Color.Black))
                        {
                            pen.DashStyle = System.Drawing.Drawing2D.DashStyle.DashDot;
                            pen.Width = 3;

                            pntPosTemp.X = Util.fnCalCoordinate(0, pntImagePosTemp.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                            pntPosTemp.Y = Util.fnCalCoordinate(0, pntImagePosTemp.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                            pntLastPosTemp.X = Util.fnCalCoordinate(0, pntImageLastPosTemp.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                            pntLastPosTemp.Y = Util.fnCalCoordinate(0, pntImageLastPosTemp.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());

                            e.Graphics.DrawLine(pen, pntPosTemp, pntLastPosTemp);

                            double distX = Math.Abs(pntImageLastPosTemp.X - pntImagePosTemp.X);
                            double distY = Math.Abs(pntImageLastPosTemp.Y - pntImagePosTemp.Y);
                            e.Graphics.DrawString(Math.Sqrt((Math.Pow(distX, 2) + Math.Pow(distY, 2))).ToString("0"), new Font("Arial", 20), Brushes.Black, new PointF((float)pntLastPosTemp.X, (float)pntLastPosTemp.Y));
                        }
                    }
                }
                if (Frm_ObjectDrawExtend.RectMode && !Frm_Main.ObjectControlMode)
                {
                    using (Pen pen = new System.Drawing.Pen(System.Drawing.Color.FromArgb(255, 50, 50)))
                    {
                        pen.DashStyle = System.Drawing.Drawing2D.DashStyle.DashDot;
                        pen.Width = 3;
                        Util.AdjustCoordination(ref MouseRectTemp);
                        e.Graphics.DrawRectangle(pen, MouseRectTemp.X, MouseRectTemp.Y, MouseRectTemp.Width, MouseRectTemp.Height);
                    }
                }
                else if (Frm_ObjectDrawExtend.CircleMode && !Frm_Main.ObjectControlMode)
                {
                    using (Pen pen = new System.Drawing.Pen(System.Drawing.Color.FromArgb(255, 50, 50)))
                    {
                        pen.DashStyle = System.Drawing.Drawing2D.DashStyle.DashDot;
                        pen.Width = 3;
                        Util.AdjustCoordination(ref MouseRectTemp);
                        e.Graphics.DrawEllipse(pen, MouseRectTemp);
                    }
                }
                else if (Frm_ObjectDrawExtend.PolygonMode && !Frm_Main.ObjectControlMode)
                {
                    int nCnt = pntsPolygon.Count;
                    if (nCnt > 0)
                    {
                        using (Pen pen = new System.Drawing.Pen(System.Drawing.Color.FromArgb(255, 50, 50)))
                        {
                            pen.DashStyle = System.Drawing.Drawing2D.DashStyle.DashDot;
                            pen.Width = 3;

                            PointF pnt = new PointF(MouseRectTemp.X, MouseRectTemp.Y);
                            for (int i = 1; i < nCnt; i++)
                                e.Graphics.DrawLine(pen, pntsPolygon[i - 1], pntsPolygon[i]);
                            e.Graphics.DrawLine(pen, pntsPolygon[nCnt - 1], pnt);
                        }
                    }
                }
                if (Frm_Main.NormalCursorMode)
                {
                    using (Pen pen = new System.Drawing.Pen(System.Drawing.Color.FromArgb(0, 255, 0)))
                    {
                        pen.DashStyle = System.Drawing.Drawing2D.DashStyle.DashDot;
                        pen.Width = 3;
                        Util.AdjustCoordination(ref ObjectSelectArea);
                        e.Graphics.DrawRectangle(pen, ObjectSelectArea.X, ObjectSelectArea.Y, ObjectSelectArea.Width, ObjectSelectArea.Height);
                    }
                }
                //특정 ROI 확대/축소
                if (Frm_Main.AreaEnlargementMode)
                {
                    using (Pen pen = new System.Drawing.Pen(System.Drawing.Color.FromArgb(255, 125, 173)))
                    {
                        pen.DashStyle = System.Drawing.Drawing2D.DashStyle.DashDot;
                        pen.Width = 3;
                        Util.AdjustCoordination(ref AreaEnlargeArea);
                        e.Graphics.DrawRectangle(pen, AreaEnlargeArea.X, AreaEnlargeArea.Y, AreaEnlargeArea.Width, AreaEnlargeArea.Height);
                    }
                }


                BeginInvoke(new Action(() =>
                {
                    Program.frm_Layers.DrawObject();
                }));
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void Pic_LayerView_MouseMove(object sender, MouseEventArgs e)
        {
            try
            {
                this.BeginInvoke(new Action(() =>
                {
                    if (MouseMoveChain != null)
                        MouseMoveChain.Invoke(sender, e);
                }));
                if (MouseMoveEvent == false)
                    return;
                if (Frm_Main.ObjectControlMode)
                    MousePositionCrosshair(e.X, e.Y);
                else
                {
                    MouseCursorChange(e.X, e.Y);
                }

                RectangleF _Retangle = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();
                if (PrintCoord)
                {
                    double x = Util.fnCalCoordinate(_Retangle.X, e.X, _Retangle.Width, Pic_LayerView.Width, 0);
                    double y = Util.fnCalCoordinate(_Retangle.Y, e.Y, _Retangle.Height, Pic_LayerView.Height, 0);
                    if (x >= Program.frm_Main.m_UcRcpTeachPic2.GetScrMat().Width)
                        x = Program.frm_Main.m_UcRcpTeachPic2.GetScrMat().Width;
                    if (x < 0)
                        x = 0;
                    if (y >= Program.frm_Main.m_UcRcpTeachPic2.GetScrMat().Height)
                        y = Program.frm_Main.m_UcRcpTeachPic2.GetScrMat().Height;
                    if (y < 0)
                        y = 0;
                    Program.frm_Pos_GL.SetMouseCoord(x, y);
                }

                // 화면 이동
                if ((e.Button == MouseButtons.Left && (Frm_Main.ExploringUserMode) || e.Button == MouseButtons.Right))
                {
                    Program.Painter.ClearDrawingPolygon();
                    var Temp = Program.frm_Layers.GetRegistedLayerList();
                    foreach (Layer layer in Temp)
                    {
                        LayerTransparentTemp.Add(new LayerTransparent(layer.Name, layer.Transparency));
                        layer.Transparency = 0;
                    }

                }

                if (!(Frm_Main.ExploringUserMode) && Frm_Main.CreatingObjectMode && !bSelectedRegion && nSelectedGuideRect < 0)
                {
                    if (e.Button == MouseButtons.Left && (Frm_ObjectDrawExtend.RectMode || Frm_ObjectDrawExtend.CircleMode))
                    {
                        bool PressCtrl = false;
                        if (System.Windows.Input.Keyboard.Modifiers == System.Windows.Input.ModifierKeys.Control)
                            PressCtrl = true;

                        MouseRectTemp = new RectangleF(RectPosTemp.X, RectPosTemp.Y, (e.X - RectPosTemp.X), (e.Y - RectPosTemp.Y));
                        if (PressCtrl)
                        {
                            MouseRectTemp.X -= (e.X - RectPosTemp.X);
                            MouseRectTemp.Y -= (e.Y - RectPosTemp.Y);
                            MouseRectTemp.Width += (e.X - RectPosTemp.X);
                            MouseRectTemp.Height += (e.Y - RectPosTemp.Y);

                            ControlRectTemp = MouseRectTemp;
                        }
                    }
                    else if (Frm_ObjectDrawExtend.PolygonMode)
                    {
                        MouseRectTemp.X = e.X;
                        MouseRectTemp.Y = e.Y;
                    }
                }

                if (e.Button == MouseButtons.Left && bSelectedRegion)
                {
                    if (Program.frm_Layers.GetRegistedLayerList() != null)
                    {
                        if (Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count > 0 && ObjectTemp.Count <= 0)
                        {
                            Program.TaskMgr.Do_Task(new Task_Snapshot(Program.frm_Layers.GetBaseBoard().GetMat(), Program.frm_Layers.GetRegistedLayerList()));

                            ObjectTemp = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs_DeepCopy();
                            Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Clear();
                            foreach (Layer layer in Program.frm_Layers.GetRegistedLayerList())
                            {
                                LayerVisibleTemp.Add(new LayerVisible(layer.Name, (bool)layer.Visible));
                                if (layer.Selected != true)
                                    layer.Visible = false;
                            }
                        }

                        PointF pntCurrentPos = new PointF((Util.fnCalCoordinate(_Retangle.X, e.X, _Retangle.Width, Pic_LayerView.Width, 0)) - ScrollTemp.X,
                                                          (Util.fnCalCoordinate(_Retangle.Y, e.Y, _Retangle.Height, Pic_LayerView.Height, 0)) - ScrollTemp.Y);
                        if (ObjectTemp.Count() >= 0)
                        {
                            List<ShapeGuideLine> ShapeTemp = new List<ShapeGuideLine>();
                            Parallel.For(0, ObjectTemp.Count(), i =>
                           {
                               if (ObjectTemp[i].Selected)
                               {
                                   List<PointF> Temp = new List<PointF>();
                                   var polygonList = ObjectTemp[i].GetPolyonList();
                                   for (int j = 0; j < polygonList.Count; j++)
                                   {
                                       Temp.Add(new PointF(polygonList[j].X + pntCurrentPos.X, polygonList[j].Y + pntCurrentPos.Y));
                                   }
                                   List<PointF> PointTemp = new List<PointF>();
                                   for (int idx = 0; idx < Temp.Count; ++idx)
                                   {
                                       if (Program.Painter.ValidateRect(Temp.ToArray(), Program.frm_Main.m_UcRcpTeachPic2.GetImageRect()))
                                       {
                                           PointTemp.Add(new PointF(Util.fnCalCoordinate(0, Temp[idx].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()),
                                               Util.fnCalCoordinate(0, Temp[idx].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio())));
                                       }
                                   }
                                   if (PointTemp.Count > 0)
                                   {
                                       lock (Locker)
                                           ShapeTemp.Add(new ShapeGuideLine(Color.AliceBlue, 2, PointTemp));
                                   }
                               }
                           });
                            Program.Painter.DrawingPolygon(ShapeTemp);
                            Pic_LayerView.Invalidate();
                        }
                    }
                }
                else if (e.Button == MouseButtons.Left && nSelectedGuideRect >= 0)
                {
                    PointF pntStartPos = new PointF(Util.fnCalCoordinate(0, ScrollTemp.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()),
                                                     Util.fnCalCoordinate(0, ScrollTemp.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()));
                    PointF pntCurrentPos = new PointF(e.X - pntStartPos.X, e.Y - pntStartPos.Y);
                    if (Program.frm_Layers.GetRegistedLayerList() != null)
                    {
                        if (Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count > 0 && ObjectTemp.Count <= 0)
                        {
                            Program.TaskMgr.Do_Task(new Task_Snapshot(Program.frm_Layers.GetBaseBoard().GetMat(), Program.frm_Layers.GetRegistedLayerList()));

                            ObjectTemp = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs_DeepCopy();
                            Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Clear();
                            foreach (Layer layer in Program.frm_Layers.GetRegistedLayerList())
                            {
                                LayerVisibleTemp.Add(new LayerVisible(layer.Name, (bool)layer.Visible));
                                if (layer.Selected != true)
                                    layer.Visible = false;
                            }
                        }

                        if (ObjectTemp.Count >= 0)
                        {
                            List<ShapeGuideLine> ShapeTemp = new List<ShapeGuideLine>();
                            bool PressCtrl = false;
                            if (System.Windows.Input.Keyboard.Modifiers == System.Windows.Input.ModifierKeys.Control)
                                PressCtrl = true;


                            Parallel.For(0, ObjectTemp.Count, i =>
                            {

                                if (ObjectTemp[i].Selected)
                                {
                                    var polygonList = ObjectTemp[i].GetPolyonList();

                                    List<PointF> GuideLineList = new List<PointF>();
                                    List<PointF> GuideLineCenterList = new List<PointF>();

                                    GuideLineList = DrawPolygonGuideLine(polygonList);
                                    GuideLineCenterList = DrawPolygonCenterGuideLine(polygonList);

                                    for (int a = 0; a < GuideLineCenterList.Count(); a++)
                                        GuideLineList.Add(GuideLineCenterList[a]);


                                    float dIncreaseRate_X = 0, dIncreaseRate_Y = 0;
                                    PointF GuideLine_oppo = new PointF();

                                    if (PressCtrl)
                                        GuideLine_oppo = new PointF((GuideLineList.Max(Point => Point.X) + GuideLineList.Min(Point => Point.X)) / 2, (GuideLineList.Max(Point => Point.Y) + GuideLineList.Min(Point => Point.Y)) / 2);
                                    else
                                    {
                                        if (nSelectedGuideRect < 4)
                                        {
                                            GuideLine_oppo = new PointF(GuideLineList[(nSelectedGuideRect + 2) % 4].X, GuideLineList[(nSelectedGuideRect + 2) % 4].Y);

                                        }
                                        else
                                        {

                                            switch (nSelectedGuideRect)
                                            {
                                                case 4:
                                                    GuideLine_oppo = new PointF(GuideLineList[2].X, GuideLineList[2].Y);
                                                    break;
                                                case 5:
                                                    GuideLine_oppo = new PointF(GuideLineList[3].X, GuideLineList[3].Y);
                                                    break;
                                                case 6:
                                                    GuideLine_oppo = new PointF(GuideLineList[0].X, GuideLineList[0].Y);
                                                    break;
                                                case 7:
                                                    GuideLine_oppo = new PointF(GuideLineList[1].X, GuideLineList[1].Y);
                                                    break;
                                            }


                                        }
                                    }

                                    PointF pntGuideLineLength = new PointF(Math.Abs(GuideLineList[nSelectedGuideRect].X - GuideLine_oppo.X), Math.Abs(GuideLineList[nSelectedGuideRect].Y - GuideLine_oppo.Y));

                                    if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_TOP)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X - pntCurrentPos.X) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y - pntCurrentPos.Y) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_BOTTOM)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X - pntCurrentPos.X) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y + pntCurrentPos.Y) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.RIGHT_BOTTOM)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X + pntCurrentPos.X) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y + pntCurrentPos.Y) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.RIGHT_TOP)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X + pntCurrentPos.X) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y - pntCurrentPos.Y) / pntGuideLineLength.Y;
                                    }

                                    ///////////상, 하, 좌, 우/////////
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.TOP_CENTER)
                                    {
                                        dIncreaseRate_X = 1;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y - pntCurrentPos.Y) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_CENTER)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X - pntCurrentPos.X) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = 1;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.BOTTOM_CENTER)
                                    {
                                        dIncreaseRate_X = 1;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y + pntCurrentPos.Y) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.RIGHT_CENTER)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X + pntCurrentPos.X) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = 1;
                                    }

                                    List<PointF> Temp = new List<PointF>();



                                    for (int j = 0; j < polygonList.Count; j++)
                                    {
                                        PointF point = new PointF(Util.fnCalCoordinate(0, polygonList[j].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()),
                                                         Util.fnCalCoordinate(0, polygonList[j].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()));
                                        PointF fAfterLength = fAfterLength = new PointF(Math.Abs(point.X - GuideLine_oppo.X) * dIncreaseRate_X, Math.Abs(point.Y - GuideLine_oppo.Y) * dIncreaseRate_Y);
                                        PointF pntAfterPos = new PointF(GuideLine_oppo.X, GuideLine_oppo.Y);

                                        if (PressCtrl)
                                        {
                                            if (point.X < GuideLine_oppo.X && point.Y <= GuideLine_oppo.Y)
                                            {
                                                pntAfterPos.X -= fAfterLength.X;
                                                pntAfterPos.Y -= fAfterLength.Y;
                                            }
                                            else if (point.X <= GuideLine_oppo.X && point.Y > GuideLine_oppo.Y)
                                            {
                                                pntAfterPos.X -= fAfterLength.X;
                                                pntAfterPos.Y += fAfterLength.Y;
                                            }
                                            else if (point.X > GuideLine_oppo.X && point.Y >= GuideLine_oppo.Y)
                                            {
                                                pntAfterPos.X += fAfterLength.X;
                                                pntAfterPos.Y += fAfterLength.Y;
                                            }
                                            else
                                            {
                                                pntAfterPos.X += fAfterLength.X;
                                                pntAfterPos.Y -= fAfterLength.Y;
                                            }
                                        }
                                        else
                                        {
                                            if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_TOP || nSelectedGuideRect == (int)Enums.OriginPosition.TOP_CENTER)
                                            {
                                                pntAfterPos.X -= fAfterLength.X;
                                                pntAfterPos.Y -= fAfterLength.Y;
                                            }
                                            else if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_BOTTOM || nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_CENTER)
                                            {
                                                pntAfterPos.X -= fAfterLength.X;
                                                pntAfterPos.Y += fAfterLength.Y;
                                            }
                                            else if (nSelectedGuideRect == (int)Enums.OriginPosition.RIGHT_BOTTOM || nSelectedGuideRect == (int)Enums.OriginPosition.BOTTOM_CENTER)
                                            {
                                                pntAfterPos.X += fAfterLength.X;
                                                pntAfterPos.Y += fAfterLength.Y;
                                            }
                                            else
                                            {
                                                pntAfterPos.X += fAfterLength.X;
                                                pntAfterPos.Y -= fAfterLength.Y;
                                            }
                                        }

                                        pntAfterPos.X = Util.fnCalCoordinate(_Retangle.X, pntAfterPos.X, _Retangle.Width, Pic_LayerView.Width, 0);
                                        pntAfterPos.Y = Util.fnCalCoordinate(_Retangle.Y, pntAfterPos.Y, _Retangle.Height, Pic_LayerView.Height, 0);
                                        Temp.Add(pntAfterPos);
                                    }
                                    List<PointF> PointTemp = new List<PointF>();
                                    for (int idx = 0; idx < Temp.Count; ++idx)
                                    {
                                        if (Program.Painter.ValidateRect(Temp.ToArray(), Program.frm_Main.m_UcRcpTeachPic2.GetImageRect()))
                                        {
                                            PointTemp.Add(new PointF(Util.fnCalCoordinate(0, Temp[idx].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()),
                                                Util.fnCalCoordinate(0, Temp[idx].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio())));
                                        }
                                    }
                                    lock (Locker)
                                        ShapeTemp.Add(new ShapeGuideLine(Color.AliceBlue, 2, PointTemp));
                                }
                  
                    
                    });





                            Program.Painter.DrawingPolygon(ShapeTemp);
                            Pic_LayerView.Invalidate();
                            //Program.Painter.LayerPaint(Program.frm_Layers.GetRegistedLayerList(), Program.frm_Main.m_UcRcpTeachPic2.GetRatio(), Program.frm_Main.m_UcRcpTeachPic2.GetImageRect());
                        }
                    }
                }
                else if (e.Button == MouseButtons.Left && Frm_Main.NormalCursorMode)
                {
                    ObjectSelectArea.X = RectPosTemp.X;
                    ObjectSelectArea.Y = RectPosTemp.Y;
                    ObjectSelectArea.Width = e.X - RectPosTemp.X;
                    ObjectSelectArea.Height = e.Y - RectPosTemp.Y;
                    Pic_LayerView.Invalidate();
                }
                else if (e.Button == MouseButtons.Left && Frm_Main.AreaEnlargementMode)
                {
                    AreaEnlargeArea.X = RectPosTemp.X;
                    AreaEnlargeArea.Y = RectPosTemp.Y;
                    AreaEnlargeArea.Width = e.X - RectPosTemp.X;
                    AreaEnlargeArea.Height = e.Y - RectPosTemp.Y;
                    Pic_LayerView.Invalidate();
                }
                else if (e.Button == MouseButtons.Left && Frm_Main.ObjectControlMode)
                {

                    PointF pntCurrentPos = new PointF((Util.fnCalCoordinate(_Retangle.X, e.X, _Retangle.Width, Pic_LayerView.Width, 0)) - ScrollTemp.X,
                                                          (Util.fnCalCoordinate(_Retangle.Y, e.Y, _Retangle.Height, Pic_LayerView.Height, 0)) - ScrollTemp.Y);


                    int nObjectControlMode_Select = 0;

                    RectangleF RectDownPosition = new RectangleF(Util.fnCalCoordinate(0, ScrollTemp.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()), Util.fnCalCoordinate(0, ScrollTemp.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()), (e.X - pRectPosTemp.X), (e.Y - pRectPosTemp.Y));

                    bool bMoving = false;
                    List<ShapeGuideLine> ShapeTemp = new List<ShapeGuideLine>();
                    for (int i = 0; i < Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count; i++)
                    {

                        if (i != nSelectedObject)
                        {
                            continue;
                        }

                        List<PointF> Temp = new List<PointF>();

                        var polygonList = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[i].GetPolyonList();


                        int nMoveControlMode = 0;
                        Point[] pntPolygonList = new Point[polygonList.Count];
                        for (int j = 0; j < polygonList.Count; j++)
                        {
                            int point_x = (int)((float)Util.fnCalCoordinate(0, polygonList[j].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()));

                            int point_y = (int)((float)Util.fnCalCoordinate(0, polygonList[j].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()));
                            pntPolygonList[j] = new Point(point_x, point_y);

                            IntPtr hRgn = CreateEllipticRgn(pntPolygonList[j].X - 10, pntPolygonList[j].Y - 10, pntPolygonList[j].X + 10, pntPolygonList[j].Y + 10);
                            nObjectControlMode_Select = PtInRegion(hRgn, (int)RectDownPosition.X, (int)RectDownPosition.Y);

                            DeleteObject(hRgn);

                            if (nObjectControlMode_Select != 0)
                            {
                                bMoving = true;
                                nMoveControlMode = j;
                                break;
                            }
                        }

                        for (int j = 0; j < polygonList.Count; j++)
                        {
                            if (nMoveControlMode == j && bMoving)
                                Temp.Add(new PointF(polygonList[j].X + pntCurrentPos.X, polygonList[j].Y + pntCurrentPos.Y));
                            else
                                Temp.Add(new PointF(polygonList[j].X, polygonList[j].Y));
                        }

                        List<PointF> PointTemp = new List<PointF>();
                        for (int idx = 0; idx < Temp.Count; ++idx)
                        {
                            PointTemp.Add(new Point((int)((float)Util.fnCalCoordinate(0, Temp[idx].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio())),
                           (int)((float)Util.fnCalCoordinate(0, Temp[idx].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()))));
                        }
                        ShapeTemp.Add(new ShapeGuideLine(Color.AliceBlue, 2, PointTemp.ConvertAll(s => s)));
                    }
                    if (bMoving)
                        Program.Painter.DrawingPolygon(ShapeTemp);
                }
                // 지점간 측정 기능 :: 첫 지점 선택 후 마우스 이동 점선 그리기
                if (Frm_Main.MeasureLengthMode && bRectBtnDown)
                {
                    pntLastPosTemp = new Point(e.X, e.Y);

                    pntImageLastPosTemp.X = Util.fnCalCoordinate(_Retangle.X, pntLastPosTemp.X, _Retangle.Width, Pic_LayerView.Width, 1);
                    pntImageLastPosTemp.Y = Util.fnCalCoordinate(_Retangle.Y, pntLastPosTemp.Y, _Retangle.Height, Pic_LayerView.Height, 1);

                }
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public void MargnetZoomIn(int MouseMoveX, int MouseMoveY)
        {
            Program.Painter.DrawImg(MouseMoveX, MouseMoveY);
        }
        public void MouseCursorChange(int MouseMoveX, int MouseMoveY)
        {
            try
            {
                MouseCursor_.X = MouseMoveX;
                MouseCursor_.Y = MouseMoveY;
                MouseCursor_.Width = (MouseMoveX - RectPosTemp.X);
                MouseCursor_.Height = (MouseMoveY - RectPosTemp.Y);

                int SelObject = 0;

                RectangleF RectDownPositionTest = MouseCursor_;
                if (Program.frm_Layers.GetRegistedLayerList().Count <= 0)
                    return;
                for (int i = 0; i < Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count(); i++)
                {
                    if (!Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[i].Selected)
                        continue;

                    List<Point> Temp = new List<Point>();


                    List<PointF> GuideLineList = new List<PointF>();
                    List<PointF> GuideLineCenterList = new List<PointF>();


                    var polygonList = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[i].GetPolyonList();

                    GuideLineList = DrawPolygonGuideLine(polygonList);
                    GuideLineCenterList = DrawPolygonCenterGuideLine(polygonList);

                    Parallel.For(0, GuideLineCenterList.Count, e =>
                        {
                            GuideLineList.Add(GuideLineCenterList[e]);
                        });



                    Point[] pntPolygonList = new Point[GuideLineList.Count];

                    for (int j = 0; j < GuideLineList.Count; j++)
                    {
                        pntPolygonList[j] = new Point((int)GuideLineList[j].X, (int)GuideLineList[j].Y);

                        IntPtr hRgn = CreateEllipticRgn(pntPolygonList[j].X - 10, pntPolygonList[j].Y - 10, pntPolygonList[j].X + 10, pntPolygonList[j].Y + 10);
                        SelObject += PtInRegion(hRgn, (int)RectDownPositionTest.X, (int)RectDownPositionTest.Y);

                        DeleteObject(hRgn);

                        if (SelObject != 0)
                        {
                            bFlagResize = true;
                            nCursor = j;
                            break;
                        }
                        else
                            bFlagResize = false;
                    }
                }

                if (bFlagResize)
                {
                    switch (nCursor)
                    {
                        case (int)Enums.MouseCursors.LEFT_TOP:
                        case (int)Enums.MouseCursors.RIGHT_BOTTOM:
                            this.Cursor = Cursors.SizeNWSE;
                            bOnly = false;

                            break;

                        case (int)Enums.MouseCursors.LEFT_BOTTOM:
                        case (int)Enums.MouseCursors.RIGHT_TOP:
                            this.Cursor = Cursors.SizeNESW;
                            bOnly = false;

                            break;

                        case (int)Enums.MouseCursors.TOP_CENTER:
                        case (int)Enums.MouseCursors.LEFT_CENTER:

                            this.Cursor = Cursors.SizeNS;
                            bOnly = true;
                            break;

                        case (int)Enums.MouseCursors.BOTTOM_CENTER:
                        case (int)Enums.MouseCursors.RIGHT_CENTER:
                            this.Cursor = Cursors.SizeWE;
                            bOnly = true;
                            break;

                        default:
                            break;
                    }
                }
                else
                    this.Cursor = Cursors.Default;
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public void MousePositionCrosshair(int MouseMoveX, int MouseMoveY)
        {
            try
            {

                int test = 0;

                RectangleF RectDownPositionTest = new RectangleF(MouseMoveX, MouseMoveY, 0, 0);

                for (int i = 0; i < Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count(); i++)
                {
                    //if (!Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[i].Selected)
                    //    //continue;

                    List<Point> Temp = new List<Point>();
                    var polygonList = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[i].GetPolyonList();

                    Point[] pntPolygonList = new Point[polygonList.Count];
                    for (int j = 0; j < polygonList.Count; j++)
                    {
                        int point_x = (int)((float)Util.fnCalCoordinate(0, polygonList[j].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()));

                        int point_y = (int)((float)Util.fnCalCoordinate(0, polygonList[j].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()));
                        pntPolygonList[j] = new Point(point_x, point_y);

                        IntPtr hRgn = CreateEllipticRgn(pntPolygonList[j].X - 10, pntPolygonList[j].Y - 10, pntPolygonList[j].X + 10, pntPolygonList[j].Y + 10);
                        test += PtInRegion(hRgn, (int)RectDownPositionTest.X, (int)RectDownPositionTest.Y);

                        DeleteObject(hRgn);


                        if (test != 0)
                        {
                            bFlagCross = true;
                            break;
                        }
                        else
                        {
                            bFlagCross = false;
                        }

                    }

                }
                if (bFlagCross || ObjectControlMode_MouseCursor)
                {
                    this.Cursor = Cursors.Cross;
                }
                else
                {
                    this.Cursor = Cursors.Default;
                }
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void Pic_LayerView_MouseLeave(object sender, EventArgs e)
        {
            try
            {
                this.BeginInvoke(new Action(() =>
                {
                    if (MouseLeaveChain != null)
                        MouseLeaveChain.Invoke(sender, e);
                }));
                if (MouseLeaveEvent == false)
                    return;
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void Pic_LayerView_MouseDown(object sender, MouseEventArgs e)
        {
            try
            {
                Program.Painter.SetDrag(true);

                if (MouseDownEvent == false)
                    return;
                Program.frm_ObjectAlignment.SetLeftRotate(false);
                Program.frm_ObjectAlignment.SetReset();
                Program.Painter.SetDegree(0);

                MouseCursorSize = true;
                ObjectControlMode_MouseCursor = true;
                if (Program.frm_Layers.GetRegistedLayerList().Count <= 0)
                    return;
                this.BeginInvoke(new Action(() =>
                {
                    if (MouseDownChain != null)
                        MouseDownChain.Invoke(sender, e);
                }));

                RectangleF _Retangle = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();

                ScrollTemp.X = Util.fnCalCoordinate(_Retangle.X, e.X, _Retangle.Width, Pic_LayerView.Width, 0);
                ScrollTemp.Y = Util.fnCalCoordinate(_Retangle.Y, e.Y, _Retangle.Height, Pic_LayerView.Height, 0);
                // 맵 탐색
                if (e.Button == MouseButtons.Left && !(Frm_Main.ExploringUserMode))
                {
                    RectPosTemp.X = e.X;
                    RectPosTemp.Y = e.Y;
                    if (Program.frm_Layers.GetRegistedLayerList() != null)
                    {
                        if (Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count() >= 0)
                        {
                            int nSelected = 0;
                            for (int i = 0; i < Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count(); i++)
                            {
                                if (!Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[i].Selected)
                                    continue;

                                var polygonList = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[i].GetPolyonList();
                                List<Point> pntPolygonList = new List<Point>();
                                for (int j = 0; j < polygonList.Count; j++)
                                {
                                    Point point = new Point((int)Util.fnCalCoordinate(0, polygonList[j].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()),
                                                            (int)Util.fnCalCoordinate(0, polygonList[j].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()));
                                    pntPolygonList.Add(new Point(point.X, point.Y));
                                }

                                List<PointF> GuideLineList = new List<PointF>();
                                List<PointF> GuideLineCenterList = new List<PointF>();

                                float Min_X = 0, Min_Y = 0, Max_X = 0, Max_Y = 0;

                                GuideLineList = DrawPolygonGuideLine(polygonList, out Min_X, out Min_Y, out Max_X, out Max_Y);
                                GuideLineCenterList = DrawPolygonCenterGuideLine(polygonList);

                                for (int w = 0; w < GuideLineCenterList.Count(); w++)
                                    GuideLineList.Add(GuideLineCenterList[w]);

                                for (int j = 0; j < 8; ++j)
                                {
                                    if (Math.Abs(GuideLineList[j].X - RectPosTemp.X) < 10 && Math.Abs(GuideLineList[j].Y - RectPosTemp.Y) < 10)
                                    {
                                        nSelectedGuideRect = j;
                                        break;
                                    }
                                }
                                if (nSelectedGuideRect == -1)
                                    nSelected += Util.GetSelectedObject(new Point((int)RectPosTemp.X, (int)RectPosTemp.Y), pntPolygonList);
                            }
                            if (nSelected > 0)
                                bSelectedRegion = true;
                            else
                                bSelectedRegion = false;
                        }
                    }
                }
                // 그리기 모드
                if (e.Button == MouseButtons.Left && !(Frm_Main.ExploringUserMode) && (Frm_ObjectDrawExtend.RectMode || Frm_ObjectDrawExtend.CircleMode) && !bSelectedRegion)
                {
                    RectPosTemp.X = e.X;
                    RectPosTemp.Y = e.Y;
                }

                if (e.Button == MouseButtons.Left && Frm_Main.NormalCursorMode)
                {
                    RectPosTemp.X = e.X;
                    RectPosTemp.Y = e.Y;
                }
                // 객체 사이즈 조절
                if (e.Button == MouseButtons.Left && Frm_Main.ObjectControlMode)
                {
                    //MouseDown
                    ResizeObject = null;
                    Program.frm_Layers.UnSelectedObject();
                    RectPosTemp.X = e.X;
                    RectPosTemp.Y = e.Y;
                    pRectPosTemp.X = e.X;
                    pRectPosTemp.Y = e.Y;

                    if (Program.frm_Layers.GetRegistedLayerList() != null)
                    {
                        if (Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count() >= 0)
                        {
                            for (int i = 0; i < Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count(); i++)
                            {
                                var polygonList = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[i].GetPolyonList();
                                for (int j = 0; j < polygonList.Count; ++j)
                                {
                                    PointF pntPolygon = new PointF(Util.fnCalCoordinate(0, polygonList[j].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()),
                                                           Util.fnCalCoordinate(0, polygonList[j].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()));
                                    RectangleF rectClickVertexArea = new RectangleF(pntPolygon.X - 8, pntPolygon.Y - 8, 16, 16);
                                    if (rectClickVertexArea.Contains(new PointF(RectPosTemp.X, RectPosTemp.Y)))
                                    {
                                        nSelectedObject = i;
                                        nSelectedPolyPos = j;
                                        ResizeObject = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[i];
                                    }
                                }
                            }
                        }
                    }
                }

                if (Frm_Main.AreaEnlargementMode && e.Button == MouseButtons.Left)
                {
                    RectangleF AreaEnlarg = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();
                    pntDownAreaZoomIn.X = Util.fnCalCoordinate(AreaEnlarg.X, e.X, AreaEnlarg.Width, Pic_LayerView.Width, 0);
                    pntDownAreaZoomIn.Y = Util.fnCalCoordinate(AreaEnlarg.Y, e.Y, AreaEnlarg.Height, Pic_LayerView.Height, 0);
                }
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void Pic_LayerView_MouseEnter(object sender, EventArgs e)
        {
            try
            {
                if (MouseEnterEvent == false)
                    return;
                this.BeginInvoke(new Action(() =>
                {
                    if (MouseEnterChain != null)
                        MouseEnterChain.Invoke(sender, e);
                }));
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void Pic_LayerView_MouseClick(object sender, MouseEventArgs e)
        {
            try
            {
                this.BeginInvoke(new Action(() =>
                {
                    if (MouseClickChain != null)
                        MouseClickChain.Invoke(sender, e);
                }));
                if (MouseClickEvent == false)
                    return;
                // 지점간 측정 기능 :: 마우스 클릭 시 작업
                if (e.Button == MouseButtons.Right && Frm_Main.MeasureLengthMode)
                {
                    RectangleF _Retangle = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();
                    // 측정 시작 지점 클릭 좌표 저장
                    if (!bRectBtnDown)
                    {
                        pntPosTemp = new PointF(e.X, e.Y);
                        pntImagePosTemp.X = Util.fnCalCoordinate(_Retangle.X, pntPosTemp.X, _Retangle.Width, Pic_LayerView.Width, 1);
                        pntImagePosTemp.Y = Util.fnCalCoordinate(_Retangle.Y, pntPosTemp.Y, _Retangle.Height, Pic_LayerView.Height, 1);

                        bRectBtnDown = true;
                    }
                    // 측정 End 지점 클릭 좌표 저장
                    else
                    {
                        pntLastPosTemp = new PointF(e.X, e.Y);
                        pntImageLastPosTemp.X = Util.fnCalCoordinate(_Retangle.X, pntLastPosTemp.X, _Retangle.Width, Pic_LayerView.Width, 1);
                        pntImageLastPosTemp.Y = Util.fnCalCoordinate(_Retangle.Y, pntLastPosTemp.Y, _Retangle.Height, Pic_LayerView.Height, 1);

                        bRectBtnDown = false;
                    }
                }
                if (e.Button == MouseButtons.Left && !Frm_Main.ExploringUserMode && Frm_Main.CreatingObjectMode && Frm_ObjectDrawExtend.PolygonMode)
                {
                    if ((pntsPolygon.Count > 0 || !bSelectedRegion) && nSelectedGuideRect < 0)
                    {
                        RectangleF _Retangle = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();
                        PointF pntTemp = new PointF(e.X, e.Y);
                        pntsPolygon.Add(pntTemp);

                        pntTemp.X = Util.fnCalCoordinate(_Retangle.X, pntTemp.X, _Retangle.Width, Pic_LayerView.Width, 0);
                        pntTemp.Y = Util.fnCalCoordinate(_Retangle.Y, pntTemp.Y, _Retangle.Height, Pic_LayerView.Height, 0);
                        pntsImagePolygon.Add(pntTemp);
                    }
                }
                else if (e.Button == MouseButtons.Right && !Frm_Main.ExploringUserMode && Frm_Main.CreatingObjectMode && Frm_ObjectDrawExtend.PolygonMode)
                {
                    pntsPolygon.Clear();
                    if (Program.frm_Layers.GetRegistedLayerList().Count <= 0)
                    {
                        int Info = 0;
                        keybd_event((byte)18, 0, 0, ref Info); // ALT key Down
                        keybd_event((byte)18, 0, 2, ref Info); // ALT key Up
                        new Frm_MessageBox(Constants.AlarmType_Light, "추가할 레이어가 없습니다.").ShowDialog();
                        return;
                    }
                    if (Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].Type == Constants.ScanArea)
                    {
                        int Info = 0;
                        keybd_event((byte)18, 0, 0, ref Info); // ALT key Down
                        keybd_event((byte)18, 0, 2, ref Info); // ALT key Up
                        new Frm_MessageBox(Constants.AlarmType_Light, "Scan Area에는 객체를 추가할 수 없습니다.").ShowDialog();
                        pntsImagePolygon.Clear();
                        return;
                    }
                    List<PointF> Temp = new List<PointF>();
                    for (int i = 0; i < pntsImagePolygon.Count; i++)
                    {
                        Temp.Add(pntsImagePolygon[i]);
                    }
                    if (Util.GetPolygonArea(Temp) <= Constants.MinimumShapeSize)
                    {
                        pntsImagePolygon.Clear();
                        return;
                    }

                    List<ShapeGuideLine> ShapeTemp = new List<ShapeGuideLine>();
                    if (pntsImagePolygon.Count >= 3)
                    {
                        if (!(bool)Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].Lock)
                        {
                            Program.frm_Layers.UnSelectedObject();
                            Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].DrawShapes(new Polygon_Obj(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count, Temp) { Selected = true });
                            Program.frm_CoordExtend.SetSelectedObjectCount(1);
                            var IdTemp = new List<Shape_Obj>();
                            IdTemp.Add(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count - 1]);
                            Program.frm_CoordExtend.SetCurrentObjId(IdTemp);

                            var SizeTemp = new List<Shape_Obj>();
                            SizeTemp.Add(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count - 1]);
                            Program.frm_CoordExtend.SetCurrentObjSize(SizeTemp);

                            float Min_X = 0, Min_Y = 0, Max_X = 0, Max_Y = 0;

                            ShapeTemp.Add(new ShapeGuideLine(Color.FloralWhite, 1, DrawPolygonGuideLine(Temp, out Min_X, out Min_Y, out Max_X, out Max_Y)));
                        }
                        Program.Painter.DrawingPolygon(ShapeTemp);
                    }
                    pntsImagePolygon.Clear();
                }
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void Pic_LayerView_MouseUp(object sender, MouseEventArgs e)
        {
            Program.Painter.SetDrag(false);

            try
            {
                if (MouseUpEvent == false)
                    return;
                if (Program.frm_Layers.GetRegistedLayerList().Count <= 0)
                    return;
                MouseCursorSize = false;
                ObjectControlMode_MouseCursor = false;

                RectangleF _Retangle = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();

                RectPosTemp.Width = (e.X);
                RectPosTemp.Height = (e.Y);
                RectangleF AdjustArea = new RectangleF(ScrollTemp.X, ScrollTemp.Y, Util.fnCalCoordinate(_Retangle.X, RectPosTemp.Width, _Retangle.Width, Pic_LayerView.Width, 0), Util.fnCalCoordinate(_Retangle.Y, RectPosTemp.Height, _Retangle.Height, Pic_LayerView.Height, 0));

                if (LayerTransparentTemp != null)
                {
                    if (LayerTransparentTemp.Count > 0)
                        foreach (Layer layer in Program.frm_Layers.GetRegistedLayerList())
                        {
                            var Temp = LayerTransparentTemp.Find(x => x.Name == layer.Name);
                            if (Temp != null)
                                layer.Transparency = Temp.Value;
                        }
                    LayerTransparentTemp.Clear();
                }
                if (e.Button == MouseButtons.Left && !(Frm_Main.ExploringUserMode) && Frm_Main.CreatingObjectMode && Frm_ObjectDrawExtend.RectMode && !bSelectedRegion && nSelectedGuideRect < 0)
                {
                    MouseRectTemp = new Rectangle();
                    if (Program.frm_Layers.GetRegistedLayerList().Count <= 0)
                    {
                        int Info = 0;
                        keybd_event((byte)18, 0, 0, ref Info); // ALT key Down
                        keybd_event((byte)18, 0, 2, ref Info); // ALT key Up
                        new Frm_MessageBox(Constants.AlarmType_Light, "추가할 레이어가 없습니다.").ShowDialog();
                        return;
                    }
                    if (Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].Type == Constants.ScanArea)
                    {
                        int Info = 0;
                        keybd_event((byte)18, 0, 0, ref Info); // ALT key Down
                        keybd_event((byte)18, 0, 2, ref Info); // ALT key Up
                        new Frm_MessageBox(Constants.AlarmType_Light, "Scan Area에는 객체를 추가할 수 없습니다.").ShowDialog();
                        return;
                    }
                    Program.TaskMgr.Do_Task(new Task_Snapshot(Program.frm_Layers.GetBaseBoard().GetMat(), Program.frm_Layers.GetRegistedLayerList()));
                    List<PointF> Temp = new List<PointF>();
                    Util.AdjustCoordinate(ref AdjustArea);

                    float W = (AdjustArea.Width - AdjustArea.X) / 2;
                    float H = (AdjustArea.Height - AdjustArea.Y) / 2;

                    if (!(System.Windows.Input.Keyboard.Modifiers == System.Windows.Input.ModifierKeys.Control))
                    {
                        Temp.Add(new PointF(AdjustArea.X, AdjustArea.Y));
                        Temp.Add(new PointF(AdjustArea.Width, AdjustArea.Y));
                        Temp.Add(new PointF(AdjustArea.Width, AdjustArea.Height));
                        Temp.Add(new PointF(AdjustArea.X, AdjustArea.Height));
                    }
                    else
                    {
                        float fScrollWidth = Util.fnCalCoordinate(_Retangle.X, RectPosTemp.Width, _Retangle.Width, Pic_LayerView.Width, 0);
                        float fScrollHeight = Util.fnCalCoordinate(_Retangle.Y, RectPosTemp.Height, _Retangle.Height, Pic_LayerView.Height, 0);
                        Temp.Add(new PointF(ScrollTemp.X - (fScrollWidth - ScrollTemp.X), ScrollTemp.Y - (fScrollHeight - ScrollTemp.Y)));
                        Temp.Add(new PointF(ScrollTemp.X - (fScrollWidth - ScrollTemp.X), ScrollTemp.Y + (fScrollHeight - ScrollTemp.Y)));
                        Temp.Add(new PointF(ScrollTemp.X + (fScrollWidth - ScrollTemp.X), ScrollTemp.Y + (fScrollHeight - ScrollTemp.Y)));
                        Temp.Add(new PointF(ScrollTemp.X + (fScrollWidth - ScrollTemp.X), ScrollTemp.Y - (fScrollHeight - ScrollTemp.Y)));

                    }
                    if (Util.GetPolygonArea(Temp) <= Constants.MinimumShapeSize)
                        return;

                    List<ShapeGuideLine> ShapeTemp = new List<ShapeGuideLine>();
                    if (!(bool)Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].Lock)
                    {
                        Program.frm_Layers.UnSelectedObject();
                        Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].DrawShapes(new Rectangle_Obj(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count, Temp) { Selected = true });
                        Program.frm_CoordExtend.SetSelectedObjectCount(1);
                        var IdTemp = new List<Shape_Obj>();
                        IdTemp.Add(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count - 1]);
                        Program.frm_CoordExtend.SetCurrentObjId(IdTemp);

                        var SizeTemp = new List<Shape_Obj>();
                        SizeTemp.Add(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count - 1]);
                        Program.frm_CoordExtend.SetCurrentObjSize(SizeTemp);

                        float Min_X = 0, Min_Y = 0, Max_X = 0, Max_Y = 0;

                        ShapeTemp.Add(new ShapeGuideLine(Color.FloralWhite, 1, DrawPolygonGuideLine(Temp, out Min_X, out Min_Y, out Max_X, out Max_Y)));
                        //Program.TaskMgr.Do_Task(new Task_Snapshot(Program.frm_Layers.GetBaseBoard().GetMat(), Program.frm_Layers.GetRegistedLayerList()));
                    }

                    Program.Painter.DrawingPolygon(ShapeTemp);
                }
                else if (e.Button == MouseButtons.Left && !(Frm_Main.ExploringUserMode) && Frm_Main.CreatingObjectMode && Frm_ObjectDrawExtend.CircleMode && !bSelectedRegion && nSelectedGuideRect < 0)
                {
                    if (Program.frm_Layers.GetRegistedLayerList().Count <= 0)
                    {
                        int Info = 0;
                        keybd_event((byte)18, 0, 0, ref Info); // ALT key Down
                        keybd_event((byte)18, 0, 2, ref Info); // ALT key Up
                        new Frm_MessageBox(Constants.AlarmType_Light, "추가할 레이어가 없습니다.").ShowDialog();
                        return;
                    }
                    MouseRectTemp = new Rectangle();
                    if (Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].Type == Constants.ScanArea)
                    {
                        int Info = 0;
                        keybd_event((byte)18, 0, 0, ref Info); // ALT key Down
                        keybd_event((byte)18, 0, 2, ref Info); // ALT key Up
                        new Frm_MessageBox(Constants.AlarmType_Light, "Scan Area에는 객체를 추가할 수 없습니다.").ShowDialog();
                        return;
                    }
                    Program.TaskMgr.Do_Task(new Task_Snapshot(Program.frm_Layers.GetBaseBoard().GetMat(), Program.frm_Layers.GetRegistedLayerList()));
                    List<PointF> Temp = new List<PointF>();
                    Util.AdjustCoordinate(ref AdjustArea);
                    float W = (AdjustArea.Width - AdjustArea.X) / 2;
                    float H = (AdjustArea.Height - AdjustArea.Y) / 2;

                    PointF pntCenter = new PointF();
                    if (!(System.Windows.Input.Keyboard.Modifiers == System.Windows.Input.ModifierKeys.Control))
                    {
                        pntCenter = new PointF((AdjustArea.X + AdjustArea.Width) / 2, (AdjustArea.Y + AdjustArea.Height) / 2);

                        for (double dRadian = 0; dRadian < 3.14159265 * 2; dRadian += 3.14159265 * 2 / 16)
                        {
                            PointF pntTemp = new PointF();
                            pntTemp.X = (float)(pntCenter.X + W * Math.Cos(dRadian));
                            pntTemp.Y = (float)(pntCenter.Y + H * Math.Sin(dRadian));
                            Temp.Add(pntTemp);
                        }
                    }
                    else
                    {
                        pntCenter = new PointF(ScrollTemp.X, ScrollTemp.Y);

                        for (double dRadian = 0; dRadian < 3.14159265 * 2; dRadian += 3.14159265 * 2 / 16)
                        {
                            PointF pntTemp = new PointF();
                            pntTemp.X = (float)(pntCenter.X + W * 2 * Math.Cos(dRadian));
                            pntTemp.Y = (float)(pntCenter.Y + H * 2 * Math.Sin(dRadian));
                            Temp.Add(pntTemp);
                        }
                    }
                    if (Util.GetPolygonArea(Temp) <= Constants.MinimumShapeSize)
                        return;

                    if (!(bool)Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].Lock)
                    {
                        Program.frm_Layers.UnSelectedObject();
                        Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].DrawShapes(new Circle_Obj(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count, Temp) { Selected = true });
                        Program.frm_CoordExtend.SetSelectedObjectCount(1);

                        var IdTemp = new List<Shape_Obj>();

                        IdTemp.Add(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count - 1]);
                        Program.frm_CoordExtend.SetCurrentObjId(IdTemp);

                        var SizeTemp = new List<Shape_Obj>();
                        SizeTemp.Add(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count - 1]);
                        Program.frm_CoordExtend.SetCurrentObjSize(SizeTemp);

                        float Min_X, Min_Y, Max_X, Max_Y = 0;


                        List<ShapeGuideLine> ShapeTemp = new List<ShapeGuideLine>();
                  

                        ShapeTemp.Add(new ShapeGuideLine(Color.FloralWhite, 1, DrawPolygonGuideLine(Temp, out Min_X, out Min_Y, out Max_X, out Max_Y)));

                        Program.Painter.DrawingPolygon(ShapeTemp);


                    }
                }
                // 객체 모양 변경
                if (e.Button == MouseButtons.Left && !(Frm_Main.ExploringUserMode) && Frm_Main.ObjectControlMode && nSelectedObject >= 0)
                {
                    if (Program.frm_Layers.GetRegistedLayerList() != null)
                    {
                        if (ResizeObject != null)
                        {
                            Program.TaskMgr.Do_Task(new Task_Snapshot(Program.frm_Layers.GetBaseBoard().GetMat(), Program.frm_Layers.GetRegistedLayerList()));
                            var polygonList = ResizeObject.GetPolyonList();
                            List<PointF> Temp = new List<PointF>();

                            for (int i = 0; i < polygonList.Count; i++)
                            {
                                if (i == nSelectedPolyPos)
                                {
                                    Temp.Add(new PointF(polygonList[i].X + AdjustArea.Width - AdjustArea.X, polygonList[i].Y + AdjustArea.Height - AdjustArea.Y));
                                }
                                else
                                    Temp.Add(polygonList[i]);
                            }
                            Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].SetShapes(nSelectedObject, Temp);

                            SmartList<Shape_Obj> ShapeTemp = new SmartList<Shape_Obj>();
                            ShapeTemp.Add(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeFromID(nSelectedObject));
                            Program.frm_CoordExtend.SetSelectedObjectCount(1);
                            Program.frm_CoordExtend.SetCurrentObjId(ShapeTemp);
                            Program.frm_CoordExtend.SetCurrentObjSize(ShapeTemp);

                            nSelectedObject = -1;
                            nSelectedPolyPos = -1;
                            //Program.TaskMgr.Do_Task(new Task_Snapshot(Program.frm_Layers.GetBaseBoard().GetMat(), Program.frm_Layers.GetRegistedLayerList()));
                        }
                    }
                }
                // 위치변경
                else if (e.Button == MouseButtons.Left && bSelectedRegion)
                {
                    Program.Painter.ClearDrawingPolygon();

                    if (Program.frm_Layers.GetRegistedLayerList() != null)
                    {
                        if (ObjectTemp.Count() >= 0)
                        {
                            List<ShapeGuideLine> ShapeTemp = new List<ShapeGuideLine>();
                            Parallel.For(0, ObjectTemp.Count(), i =>
                            {
                                if (ObjectTemp[i].Selected)
                                {
                                    List<PointF> Temp = new List<PointF>();
                                    var polygonList = ObjectTemp[i].GetPolyonList();

                                    for (int j = 0; j < polygonList.Count; j++)
                                    {
                                        Temp.Add(new PointF(polygonList[j].X + AdjustArea.Width - AdjustArea.X, polygonList[j].Y + AdjustArea.Height - AdjustArea.Y));
                                    }
                                    ObjectTemp[(int)i].SetPolygonList(Temp);
                                    ObjectTemp[i].Selected = true;

                                    List<PointF> PointTemp;
                                    float Min_X = 0, Min_Y = 0, Max_X = 0, Max_Y = 0;
                                    PointTemp = DrawPolygonGuideLine(Temp, out Min_X, out Min_Y, out Max_X, out Max_Y);
                                    lock (Locker)
                                        ShapeTemp.Add(new ShapeGuideLine(Color.FloralWhite, 1, PointTemp));
                                }
                            });
                            Program.Painter.DrawingPolygon(ShapeTemp);

                            Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].DrawShapes(ObjectTemp);
                            foreach (Layer layer in Program.frm_Layers.GetRegistedLayerList())
                            {
                                if (LayerVisibleTemp.Count > 0)
                                    layer.Visible = LayerVisibleTemp.Find(x1 => x1.Name == layer.Name).Flag;
                            }
                            var Counting = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().FindAll(x => x.Selected == true);
                            Program.frm_CoordExtend.SetSelectedObjectCount(Counting.Count);
                            Program.frm_CoordExtend.SetCurrentObjId(Counting);
                            Program.frm_CoordExtend.SetCurrentObjSize(Counting);
                            ObjectTemp.Clear();
                            LayerVisibleTemp.Clear();
                            //Program.TaskMgr.Do_Task(new Task_Snapshot(Program.frm_Layers.GetBaseBoard().GetMat(), Program.frm_Layers.GetRegistedLayerList()));
                            Program.frm_Layers.DrawObject();
                        }
                    }
                    bSelectedRegion = false;
                    nSelectedGuideRect = -1;
                }
                else if (e.Button == MouseButtons.Left && nSelectedGuideRect >= 0)
                {
                    RectangleF rect = new RectangleF(AdjustArea.X, AdjustArea.Y, AdjustArea.Width, AdjustArea.Height);
                    rect.X = Util.fnCalCoordinate(0, AdjustArea.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                    rect.Y = Util.fnCalCoordinate(0, AdjustArea.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                    rect.Width = Util.fnCalCoordinate(0, AdjustArea.Width - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                    rect.Height = Util.fnCalCoordinate(0, AdjustArea.Height - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                    if (Program.frm_Layers.GetRegistedLayerList() != null)
                    {
                        if (ObjectTemp.Count() >= 0)
                        {
                            List<ShapeGuideLine> ShapeTemp = new List<ShapeGuideLine>();
                            bool PressCtrl = false;
                            if (System.Windows.Input.Keyboard.Modifiers == System.Windows.Input.ModifierKeys.Control)
                                PressCtrl = true;

                            Parallel.For(0, ObjectTemp.Count(), i =>
                            {
                                if (ObjectTemp[i].Selected)
                                {

                                    var polygonList = ObjectTemp[i].GetPolyonList();

                                    List<PointF> GuideLineList = new List<PointF>();
                                    List<PointF> GuideLineCenterList = new List<PointF>();
                                    GuideLineList = DrawPolygonGuideLine(polygonList);
                                    GuideLineCenterList = DrawPolygonCenterGuideLine(polygonList);

                                    for (int a = 0; a < GuideLineCenterList.Count(); a++)
                                        GuideLineList.Add(GuideLineCenterList[a]);

                                    PointF GuideLine_oppo = new PointF();
                                    PointF pntGuideLineLength;

                                    if (PressCtrl)
                                        GuideLine_oppo = new PointF((GuideLineList.Max(Point => Point.X) + GuideLineList.Min(Point => Point.X)) / 2, (GuideLineList.Max(Point => Point.Y) + GuideLineList.Min(Point => Point.Y)) / 2);
                                    else
                                    {
                                        if (nSelectedGuideRect < 4)
                                        {
                                            GuideLine_oppo = new PointF(GuideLineList[(nSelectedGuideRect + 2) % 4].X, GuideLineList[(nSelectedGuideRect + 2) % 4].Y);

                                        }
                                        else
                                        {

                                            switch (nSelectedGuideRect)
                                            {
                                                case 4:
                                                    GuideLine_oppo = new PointF(GuideLineList[2].X, GuideLineList[2].Y);
                                                    break;
                                                case 5:
                                                    GuideLine_oppo = new PointF(GuideLineList[3].X, GuideLineList[3].Y);
                                                    break;
                                                case 6:
                                                    GuideLine_oppo = new PointF(GuideLineList[0].X, GuideLineList[0].Y);
                                                    break;
                                                case 7:
                                                    GuideLine_oppo = new PointF(GuideLineList[1].X, GuideLineList[1].Y);
                                                    break;
                                            }


                                        }
                                    }


                                    pntGuideLineLength = new PointF(Math.Abs(GuideLineList[nSelectedGuideRect].X - GuideLine_oppo.X), Math.Abs(GuideLineList[nSelectedGuideRect].Y - GuideLine_oppo.Y));

                                    float dIncreaseRate_X = 0, dIncreaseRate_Y = 0;
                                    if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_TOP)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X - (rect.Width - rect.X)) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y - (rect.Height - rect.Y)) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_BOTTOM)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X - (rect.Width - rect.X)) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y + (rect.Height - rect.Y)) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.RIGHT_BOTTOM)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X + (rect.Width - rect.X)) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y + (rect.Height - rect.Y)) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.RIGHT_TOP)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X + (rect.Width - rect.X)) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y - (rect.Height - rect.Y)) / pntGuideLineLength.Y;
                                    }



                                    ///////////상, 하, 좌, 우/////////
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.TOP_CENTER)
                                    {
                                        dIncreaseRate_X = 1;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y - (rect.Height - rect.Y)) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_CENTER)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X - (rect.Width - rect.X)) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = 1;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.BOTTOM_CENTER)
                                    {
                                        dIncreaseRate_X = 1;
                                        dIncreaseRate_Y = (pntGuideLineLength.Y + (rect.Height - rect.Y)) / pntGuideLineLength.Y;
                                    }
                                    else if (nSelectedGuideRect == (int)Enums.OriginPosition.RIGHT_CENTER)
                                    {
                                        dIncreaseRate_X = (pntGuideLineLength.X + (rect.Width - rect.X)) / pntGuideLineLength.X;
                                        dIncreaseRate_Y = 1;
                                    }
                                    /////////////////////////////////

                                    List<PointF> Temp = new List<PointF>();
                                    for (int j = 0; j < polygonList.Count; j++)
                                    {
                                        PointF point = new PointF(Util.fnCalCoordinate(0, polygonList[j].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()),
                                                     Util.fnCalCoordinate(0, polygonList[j].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio()));
                                        PointF fAfterLength = new PointF(Math.Abs(point.X - GuideLine_oppo.X) * dIncreaseRate_X, Math.Abs(point.Y - GuideLine_oppo.Y) * dIncreaseRate_Y);

                                        PointF pntAfterPos = new PointF();

                                        if (PressCtrl)
                                        {
                                            if (point.X < GuideLine_oppo.X && point.Y <= GuideLine_oppo.Y)
                                                pntAfterPos = new PointF(GuideLine_oppo.X - fAfterLength.X, GuideLine_oppo.Y - fAfterLength.Y);
                                            else if (point.X <= GuideLine_oppo.X && point.Y > GuideLine_oppo.Y)
                                                pntAfterPos = new PointF(GuideLine_oppo.X - fAfterLength.X, GuideLine_oppo.Y + fAfterLength.Y);
                                            else if (point.X > GuideLine_oppo.X && point.Y >= GuideLine_oppo.Y)
                                                pntAfterPos = new PointF(GuideLine_oppo.X + fAfterLength.X, GuideLine_oppo.Y + fAfterLength.Y);
                                            else
                                                pntAfterPos = new PointF(GuideLine_oppo.X + fAfterLength.X, GuideLine_oppo.Y - fAfterLength.Y);
                                        }
                                        else
                                        {
                                            if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_TOP || nSelectedGuideRect == (int)Enums.OriginPosition.TOP_CENTER)
                                                pntAfterPos = new PointF(GuideLine_oppo.X - fAfterLength.X, GuideLine_oppo.Y - fAfterLength.Y);
                                            else if (nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_BOTTOM || nSelectedGuideRect == (int)Enums.OriginPosition.LEFT_CENTER)
                                                pntAfterPos = new PointF(GuideLine_oppo.X - fAfterLength.X, GuideLine_oppo.Y + fAfterLength.Y);
                                            else if (nSelectedGuideRect == (int)Enums.OriginPosition.RIGHT_BOTTOM || nSelectedGuideRect == (int)Enums.OriginPosition.BOTTOM_CENTER)
                                                pntAfterPos = new PointF(GuideLine_oppo.X + fAfterLength.X, GuideLine_oppo.Y + fAfterLength.Y);
                                            else
                                                pntAfterPos = new PointF(GuideLine_oppo.X + fAfterLength.X, GuideLine_oppo.Y - fAfterLength.Y);

                                        }
                                        pntAfterPos.X = Util.fnCalCoordinate(_Retangle.X, pntAfterPos.X, _Retangle.Width, Pic_LayerView.Width, 0);
                                        pntAfterPos.Y = Util.fnCalCoordinate(_Retangle.Y, pntAfterPos.Y, _Retangle.Height, Pic_LayerView.Height, 0);
                                        Temp.Add(pntAfterPos);
                                    }
                                    ObjectTemp[(int)i].SetPolygonList(Temp);
                                    ObjectTemp[i].Selected = true;

                                    float Min_X = 0, Min_Y = 0, Max_X = 0, Max_Y = 0;
                                    lock (Locker)
                                        ShapeTemp.Add(new ShapeGuideLine(Color.FloralWhite, 1, DrawPolygonGuideLine(Temp, out Min_X, out Min_Y, out Max_X, out Max_Y)));
                                }

                            });


                            Program.Painter.DrawingPolygon(ShapeTemp);


                            Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].DrawShapes(ObjectTemp.ConvertAll(s => s));
                            foreach (Layer layer in Program.frm_Layers.GetRegistedLayerList())
                            {
                                if (LayerVisibleTemp.Count > 0)
                                    layer.Visible = LayerVisibleTemp.Find(x1 => x1.Name == layer.Name).Flag;
                            }
                            var Counting = Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().FindAll(x => x.Selected == true);
                            Program.frm_CoordExtend.SetSelectedObjectCount(Counting.Count);
                            Program.frm_CoordExtend.SetCurrentObjId(Counting);
                            Program.frm_CoordExtend.SetCurrentObjSize(Counting);
                            ObjectTemp.Clear();
                            LayerVisibleTemp.Clear();
                            Pic_LayerView.Invalidate();
                          
                        }
                    }
                    nSelectedGuideRect = -1;
                }
                // Select Object
                else if (e.Button == MouseButtons.Left && Frm_Main.NormalCursorMode)
                {
                    Program.Painter.ClearDrawingPolygon();

                    ObjectSelectArea = new RectangleF();
                    Program.frm_Layers.UnSelectedObject();
                    //RectPosTemp.Width = e.X;
                    //RectPosTemp.Height = e.Y;
                    RectangleF rect = new RectangleF(AdjustArea.X, AdjustArea.Y, AdjustArea.Width, AdjustArea.Height);
                    rect.X = Util.fnCalCoordinate(0, AdjustArea.X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                    rect.Y = Util.fnCalCoordinate(0, AdjustArea.Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                    rect.Width = Util.fnCalCoordinate(0, AdjustArea.Width - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                    rect.Height = Util.fnCalCoordinate(0, AdjustArea.Height - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());

                    Util.AdjustCoordinate(ref rect);
                    Program.frm_Layers.SelectObjs(rect);
                }
                else if (e.Button == MouseButtons.Left && Frm_Main.AreaEnlargementMode)
                {
                    Program.Painter.ClearDrawingPolygon();

                    AreaEnlargeArea = new RectangleF();
                    RectangleF AreaEnlarg = Program.frm_Main.m_UcRcpTeachPic2.GetImageRect();
                    PointF pntPixelSize = Program.frm_Main.m_UcRcpTeachPic2.GetPixelSize();

                    pntUpAreaZoomIn.X = Util.fnCalCoordinate(AreaEnlarg.X, e.X, AreaEnlarg.Width, Pic_LayerView.Width, 0);
                    pntUpAreaZoomIn.Y = Util.fnCalCoordinate(AreaEnlarg.Y, e.Y, AreaEnlarg.Height, Pic_LayerView.Height, 0);

                    if (pntDownAreaZoomIn.X > pntUpAreaZoomIn.X)
                    {
                        Swap(ref pntDownAreaZoomIn, ref pntUpAreaZoomIn, "x");
                    }
                    else if (pntDownAreaZoomIn.Y > pntUpAreaZoomIn.Y)
                    {
                        Swap(ref pntDownAreaZoomIn, ref pntUpAreaZoomIn, "y");
                    }

                    pntUpAreaZoomIn.Width = Math.Abs(pntUpAreaZoomIn.X - pntDownAreaZoomIn.X);
                    pntUpAreaZoomIn.Height = Math.Abs(pntUpAreaZoomIn.Y - pntDownAreaZoomIn.Y);

                    float dRatios = ConvertRatio(pntPixelSize, pntUpAreaZoomIn);

                    pntUpAreaZoomIn.Width = (float)Math.Round(pntPixelSize.X / (dRatios));
                    pntUpAreaZoomIn.Height = (float)Math.Round(pntPixelSize.Y / (dRatios));

                    pntUpAreaZoomIn.X = pntDownAreaZoomIn.X;
                    pntUpAreaZoomIn.Y = pntDownAreaZoomIn.Y;

                    //if (pntUpAreaZoomIn.Width < pntUpAreaZoomIn.Height)
                    //    pntUpAreaZoomIn.X -= (pntUpAreaZoomIn.Width / 2);

                    Program.frm_Main.m_UcRcpTeachPic2.SetImageRect(pntUpAreaZoomIn);
                    Program.frm_Main.m_UcRcpTeachPic2.SetRatio(dRatios);

                }
                Program.frm_Layers.DrawObject();
                ShapeMoveGuideLine.Clear();
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private float ConvertRatio(PointF a, RectangleF b)
        {
            float dRatioX = a.X / b.Width;
            float dRatioY = a.Y / b.Height;
            float dRatio;

            if (dRatioX > dRatioY)
            {
                dRatio = dRatioY;
            }
            else
            {
                dRatio = dRatioX;
            }

            return dRatio;
        }
        private void Swap(ref RectangleF a, ref RectangleF b, string strc)
        {
            switch (strc)
            {
                case "x":
                    RectangleF tempX = a;
                    a.X = b.X;
                    b.X = tempX.X;
                    break;

                case "y":
                    RectangleF tempY = a;
                    a.Y = b.Y;
                    b.Y = tempY.Y;
                    break;
            }




        }

        private void Pic_LayerView_MouseWheel(object sender, MouseEventArgs e)
        {
            try
            {
                this.BeginInvoke(new Action(() =>
                {
                    if (MouseWheelChainCb != null)
                        MouseWheelChainCb.Invoke(sender, e);
                }));
                if (MouseWheelEvent == false)
                    return;
                Program.Painter.ClearDrawingPolygon();
                if (e.Delta / 120 < 0)
                {
                    if (RectPosTemp.Y < 0)
                        RectPosTemp.Y = 0;
                    else
                        RectPosTemp.Y -= Constants.ScrollSize;

                    if (Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y + Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height + 1 > Program.frm_Main.m_UcRcpTeachPic2.GetScrMat().Height)
                    {
                    }
                    else if (bRectBtnDown)
                        pntPosTemp.Y -= Constants.ScrollSize;
                    else
                    {
                        pntPosTemp.Y -= Constants.ScrollSize;
                        pntLastPosTemp.Y -= Constants.ScrollSize;
                    }
                }
                else if (e.Delta / 120 > 0)
                {
                    if (Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y <= 0)
                    {

                    }
                    else
                    {
                        if (Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y + Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height > Program.frm_Main.m_UcRcpTeachPic2.GetScrMat().Height)
                        {

                        }
                        else
                            RectPosTemp.Y += Constants.ScrollSize;

                        if (bRectBtnDown)
                        {
                            pntPosTemp.Y += Constants.ScrollSize;
                        }
                        else
                        {
                            pntPosTemp.Y += Constants.ScrollSize;
                            pntLastPosTemp.Y += Constants.ScrollSize;
                        }
                    }
                }

                MouseRectTemp.Y = RectPosTemp.Y;
                Pic_LayerView.Invalidate();
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void Pic_LayerView_MouseWheel_Event(object sender, MouseEventArgs e)
        {
            //if (e.Delta / 120 > 0)
            //    ZoomIn(1);
            //else if (e.Delta / 120 < 0)
            //    ZoomOut(1);
        }

        private List<PointF> DrawPolygonGuideLine(List<PointF> polygonList)
        {
            try
            {
                float pntMin_X = Util.fnCalCoordinate(0, polygonList.Min(Point => Point.X) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                float pntMin_Y = Util.fnCalCoordinate(0, polygonList.Min(Point => Point.Y) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                float pntMax_X = Util.fnCalCoordinate(0, polygonList.Max(Point => Point.X) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                float pntMax_Y = Util.fnCalCoordinate(0, polygonList.Max(Point => Point.Y) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());

                List<PointF> PointTemp = new List<PointF>();
                PointTemp.Add(new PointF(pntMin_X, pntMin_Y));
                PointTemp.Add(new PointF(pntMin_X, pntMax_Y));
                PointTemp.Add(new PointF(pntMax_X, pntMax_Y));
                PointTemp.Add(new PointF(pntMax_X, pntMin_Y));

                return PointTemp;
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        private List<PointF> DrawPolygonCenterGuideLine(List<PointF> polygonList)
        {
            try
            {
                float pntMin_X = Util.fnCalCoordinate(0, polygonList.Min(Point => Point.X) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                float pntMin_Y = Util.fnCalCoordinate(0, polygonList.Min(Point => Point.Y) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                float pntMax_X = Util.fnCalCoordinate(0, polygonList.Max(Point => Point.X) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                float pntMax_Y = Util.fnCalCoordinate(0, polygonList.Max(Point => Point.Y) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());

                List<PointF> PointTemp = new List<PointF>();
                PointTemp.Add(new PointF((pntMin_X + pntMax_X) / 2, pntMin_Y)); //위
                PointTemp.Add(new PointF(pntMin_X, (pntMin_Y + pntMax_Y) / 2)); //왼쪽
                PointTemp.Add(new PointF((pntMin_X + pntMax_X) / 2, pntMax_Y)); //아래
                PointTemp.Add(new PointF(pntMax_X, (pntMin_Y + pntMax_Y) / 2)); //오른쪽

                return PointTemp;
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        private List<PointF> DrawPolygonGuideLine(List<PointF> polygonList, out float min_x, out float min_y, out float max_x, out float max_y)
        {
            try
            {
                float pntMin_X = Util.fnCalCoordinate(0, polygonList.Min(Point => Point.X) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                float pntMin_Y = Util.fnCalCoordinate(0, polygonList.Min(Point => Point.Y) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                float pntMax_X = Util.fnCalCoordinate(0, polygonList.Max(Point => Point.X) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                float pntMax_Y = Util.fnCalCoordinate(0, polygonList.Max(Point => Point.Y) - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());

                min_x = pntMin_X;
                min_y = pntMin_Y;
                max_x = pntMax_X;
                max_y = pntMax_Y;

                List<PointF> PointTemp = new List<PointF>();
                PointTemp.Add(new PointF(pntMin_X, pntMin_Y));
                PointTemp.Add(new PointF(pntMin_X, pntMax_Y));
                PointTemp.Add(new PointF(pntMax_X, pntMax_Y));
                PointTemp.Add(new PointF(pntMax_X, pntMin_Y));

                return PointTemp;
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        private List<PointF> DrawRectangleGuideLine(List<PointF> polygonList, out PointF Top_Center, out PointF Left_Center, out PointF Bottom_Center, out PointF Right_Center)
        {
            try
            {
                float Top_CenterX = (polygonList.Max(Point => Point.X) + polygonList.Min(Point => Point.X)) / 2;
                float Top_CenterY = polygonList.Min(Point => Point.Y);

                float Left_CenterX = polygonList.Min(Point => Point.X);
                float Left_CenterY = (polygonList.Max(Point => Point.Y) + polygonList.Min(Point => Point.Y)) / 2;

                float Bottom_CenterX = (polygonList.Max(Point => Point.X) + polygonList.Min(Point => Point.X)) / 2;
                float Bottom_CenterY = polygonList.Max(Point => Point.Y);

                float Right_CenterX = polygonList.Max(Point => Point.X);
                float Right_CenterY = (polygonList.Max(Point => Point.Y) + polygonList.Min(Point => Point.Y)) / 2;

                Top_Center = new PointF(Top_CenterX, Top_CenterY);
                Left_Center = new PointF(Left_CenterX, Left_CenterY);
                Bottom_Center = new PointF(Bottom_CenterX, Bottom_CenterY);
                Right_Center = new PointF(Right_CenterX, Right_CenterY);

                List<PointF> PointTemp = new List<PointF>();
                PointTemp.Add(new PointF(Top_CenterX, Top_CenterY));
                PointTemp.Add(new PointF(Left_CenterX, Left_CenterY));
                PointTemp.Add(new PointF(Bottom_CenterX, Bottom_CenterY));
                PointTemp.Add(new PointF(Right_CenterX, Right_CenterY));

                return PointTemp;
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }
    }
}
