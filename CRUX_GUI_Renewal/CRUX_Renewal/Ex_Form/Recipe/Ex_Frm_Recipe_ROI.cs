using Cognex.VisionPro;
using Cognex.VisionPro.Display;
using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Class;
using CRUX_Renewal.User_Controls;
using CRUX_Renewal.Utils;
using PropertyGridEx;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Recipe_ROI : Form
    {
        Label m_lbImageState = new Label();
        CogRectangle mRect2;
        bool _shiftIsDown = false;
        public Ex_Frm_Recipe_ROI()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();

            Cog_Display_Toolbar.Display = Cog_ROI_Display;
            Cog_Display_Status.Display = Cog_ROI_Display;

            //MouseWheel += Main_Frm_Manual_MouseWheel;
            Cog_ROI_Display.MouseDown += Cog_ROI_Display_MouseDown;
            Cog_ROI_Display.MouseMove += Cog_ROI_Display_MouseMove;
            Cog_ROI_Display.GridColor = Color.Red;
            Cog_ROI_Display.MouseUp += Cog_ROI_Display_MouseUp;
            Cog_ROI_Display.ScalingMethod = Cognex.VisionPro.Display.CogDisplayScalingMethodConstants.Integer;
            Cog_ROI_Display.Click += Cog_ROI_Display_Click;
            Cog_ROI_Display.MouseWheelMode = CogDisplayMouseWheelModeConstants.Zoom1;

            Cog_ROI_Display.Fit(true);

            mRect2 = new CogRectangle();
            mRect2.Dragging += new CogDraggingEventHandler(MRect2_Dragging);
            mRect2.DraggingStopped += new CogDraggingStoppedEventHandler(MRect2_DraggingStopped);

            mRect2.Interactive = true;
            mRect2.GraphicDOFEnable = CogRectangleDOFConstants.All;
            Cog_ROI_Display.InteractiveGraphics.Add(mRect2, "Rect", true);
            Prop_Grid_Ex.ShowCustomProperties = true;
            MenuItem[] mis = new MenuItem[3];
            MenuItem mi1 = new MenuItem("추가");
            MenuItem mi2 = new MenuItem("삭제");
            MenuItem mi3 = new MenuItem("이동");
            mis[0] = mi1;
            mis[1] = mi2;
            mis[2] = mi3;
            ContextMenu cm = new ContextMenu(mis);

            Prop_Grid_Ex.ContextMenu = cm;


        }

        private void Cog_ROI_Display_Click(object sender, EventArgs e)
        {
            if (_shiftIsDown)
            {
                CogRectangle Rect = new CogRectangle();
                Rect.X = (Cog_ROI_Display.Image.Width / 2) - Cog_ROI_Display.PanX;
                Rect.Y = (Cog_ROI_Display.Image.Height / 2) - Cog_ROI_Display.PanY;

                Rect.Width = (Cog_ROI_Display.DisplayRectangle.Width / Cog_ROI_Display.Zoom * Globals.ROI_RATIO);
                Rect.Height = (Cog_ROI_Display.DisplayRectangle.Height / Cog_ROI_Display.Zoom * Globals.ROI_RATIO);
                Rect.X -= (Rect.Width / 2);
                Rect.Y -= (Rect.Height / 2);

                Cog_ROI_Display.DrawingEnabled = false;
                Rect.Interactive = true;
                Rect.GraphicDOFEnable = CogRectangleDOFConstants.All;
                Cog_ROI_Display.InteractiveGraphics.Add(Rect, "Rect", true);
                var tt = Cog_ROI_Display.InteractiveGraphics[0] as CogRectangle;
                Cog_ROI_Display.DrawingEnabled = true;
            }
        }

        private void Cog_ROI_Display_MouseUp(object sender, MouseEventArgs e)
        {
  
        }

        private void Cog_ROI_Display_MouseMove(object sender, MouseEventArgs e)
        {
   
        }

        private void Cog_ROI_Display_MouseDown(object sender, MouseEventArgs e)
        {

        }
        private void MRect2_DraggingStopped(object sender, CogDraggingEventArgs e)
        {
            MRect2_Dragging(sender, e);

            int a = 0;
        }

        private void MRect2_Dragging(object sender, CogDraggingEventArgs e)
        {
            CogRectangle dragRect = (CogRectangle)e.DragGraphic;

            if (_shiftIsDown)
            {
                mRect2.Width = dragRect.Width;
                mRect2.Height = dragRect.Height;
            }

            int a = 0;
        }
        public void FormInitialize(int no)
        {
            //Frm_CursorInfo = new Ex_Frm_Recipe_CursorInfo();
            //Frm_ViewInfo = new Ex_Frm_Recipe_ViewInfo();

            //Tlp_ROI.Controls.Add(Frm_CursorInfo, 0, 2);
            //Tlp_ROI.Controls.Add(Frm_ViewInfo, 0, 3);

           // MiniPic = new Uct_Mini_View();// m_UcRcpTeachPic.m_ucPicMiniMap;
           // MiniPic.Dock = DockStyle.Fill;
           // Tlp_ROI.Controls.Add(MiniPic,1,0);
        
           // MiniPic.miniMapSize(MiniPic.Width, MiniPic.Height, MiniPic.Location.X, MiniPic.Location.Y);
           // MiniPic.Location = new Point(MiniPic.Location.X, MiniPic.Location.Y);
           // MiniPic.BackColor = Color.Yellow;
       
           
           // MainPic = new Uct_main_View(this, ref MiniPic, true/*, pnlMapGuideLine.Width, pnlMapGuideLine.Height, pnlMapGuideLine.Location.X, pnlMapGuideLine.Location.Y*/);
           // Tlp_ROI.Controls.Add(MainPic, 0, 0);
           // MainPic.Dock = DockStyle.Fill;
           // Tlp_ROI.SetRowSpan(MainPic, 2);
           // MiniPic.ucRefMainImg(ref MainPic); 
           //// MainPic.m_fnSetCameraFovRatio(MainPic.m_fnGetCameraFovSize());
           // //m_nCamPsModeRatio = MainPic.m_fnGetCameraFovRatio();
           // //MainPic.BackColor = Color.Red;
           // //MainPic.m_fnLinkCurCoord(ref lbCoordX, ref lbCoordY);
           // /////////MainPic.m_fnLinkNowRatio(ref lbRatio);
           // //MainPic.m_fnLinkPointCoord(ref lbPntX1, ref lbPntY1);
           // //MainPic.m_fnLinkGdOriginCoord(ref lbGdOriginX1, ref lbGdOriginY1);
           // //MainPic.m_fnLinkCurRGBVal(ref lbClrR, ref lbClrG, ref lbClrB);

           // //MainPic.m_fnLinkDrawLineCoord(ref lblDrawLineX1, ref lblDrawLineY1, ref lblDrawLineX2, ref lblDrawLineY2);

           // // ROI 좌표 등록 사용 안함
           // MainPic.m_fnUseEventRectOff();
           // MainPic.InitRectColor((int)Enums.DrawRct.INSP, Color.Red);
           // MainPic.InitRectColor((int)Enums.DrawRct.RND, Color.LightSkyBlue);
           // MainPic.InitRectColor((int)Enums.DrawRct.NON, Color.Yellow);
           // MainPic.InitRectColor((int)Enums.DrawRct.HOLE, Color.Purple);   //2019.02.20 for Hole ROI
           // MainPic.InitRectColor((int)Enums.DrawRct.PAD, Color.Pink);
           // MainPic.InitRectColor((int)Enums.DrawRct.FDC_MARK, Color.Orange);
           // MainPic.InitRectColor((int)Enums.DrawRct.ALIGN_MARK, Color.LightSteelBlue);
           // MainPic.InitRectColor((int)Enums.DrawRct.PAD_INSP, Color.Crimson);
           // MainPic.InitRectColor((int)Enums.DrawRct.PAD_NONE, Color.Gold);

            // point 좌표 등록 사용 안함
            //MainPic.m_fnUseEventPointOff();
            //MainPic.InitPointColor((int)Enums.DrawPnt.WORK_ORIGN, Color.LightGreen);
            //MainPic.InitPointColor((int)Enums.DrawPnt.GD_ORIGIN, Color.LightGoldenrodYellow);
            //MainPic.InitPointColor((int)Enums.DrawPnt.FDC_POINT, Color.Red, (int)Enums.E_PNT_VIEW_KIND.POINT);
            //MainPic.InitPointColor((int)Enums.DrawPnt.POLYGON_PAD_INSP, Color.LightGreen, (int)Enums.E_PNT_VIEW_KIND.POLYGON);
            //MainPic.InitPointColor((int)Enums.DrawPnt.POLYGON_PAD_NONE, Color.LightGreen, (int)Enums.E_PNT_VIEW_KIND.POLYGON);

            //// ROI 좌표 중복 등록 사용 안함
            //MainPic.m_fnUseRoiMultiLine(false);

            //// 가이드 라인 사용 안함
            //MainPic.m_fnUseEventGuideLineView(false);
        }
        private void Ex_Frm_AccountManage_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
        }

        private void Ex_Frm_AccountManage_Shown(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 5, 5));
            ////WinApis.SetWindowRgn(Btn_Manage.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Manage.Width, Btn_Manage.Height, 15, 15), true);
            ////WinApis.SetWindowRgn(Btn_Login.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Login.Width, Btn_Login.Height, 15, 15), true);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //Mat tt = OpenCvSharp.Cv2.ImRead(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\All-1.bmp", OpenCvSharp.ImreadModes.Unchanged);
            //uctrl_MainPic1.m_fnSetBitmap(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\All-1.bmp");
        }

        private void button1_Click_1(object sender, EventArgs e)
        {

        }

        private void button1_Click_2(object sender, EventArgs e)
        {
            //Mat tt = OpenCvSharp.Cv2.ImRead(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\All-1.bmp", OpenCvSharp.ImreadModes.Unchanged);
            //MainPic.OpenImage(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp");
            //MainPic.OpenImage(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\All-1.bmp");
            var Temp = Cognex_Helper.Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp");
            //var Temp2 = Temp.ToBitmap();
            Cog_ROI_Display.Image = Temp;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            CustomProperty AA = new CustomProperty("ROI1", "111", true, "Non Insp", "ROI", true) { IsBrowsable = true };
            CustomProperty BB = new CustomProperty("ROI2", "222", true, "Non Insp", "ROI", true) { IsBrowsable = true };
            CustomProperty CC = new CustomProperty("ROI3", "333", true, "Non Insp", "ROI", true) { IsBrowsable = true };
            CC.Value = new ROI_Property() {  };
            Prop_Grid_Ex.Item.Add(AA);
            Prop_Grid_Ex.Item.Add(BB);
            Prop_Grid_Ex.Item.Add(CC);



            Prop_Grid_Ex.Item.Add(new CustomProperty("ROI1", "111", true, "Insp", "ROI", true));
            //Prop_Grid_Ex.Item.Add("Test22", "111", true, "TEMP", "ROI", true);
            Prop_Grid_Ex.Refresh();
        }

        private void Cog_ROI_Display_KeyDown(object sender, KeyEventArgs e)
        {
            _shiftIsDown = e.Shift;
        }

        private void Cog_ROI_Display_KeyUp(object sender, KeyEventArgs e)
        {
            _shiftIsDown = e.Shift;
        }

        private void Btn_ROI_Add_Click(object sender, EventArgs e)
        {
            string name = Tb_ROI_Name.Text.ToString();
            if(name == "")
            {
                Ex_Frm_Notification_Announce Frm_Announcement = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "ROI 이름이 입력되지 않았습니다.");
                Frm_Announcement.ShowDialog();
                return;
            }

            if(FindOverlapROIName(name))
            {
                Ex_Frm_Notification_Announce Frm_Announcement = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "ROI 이름이 중복입니다.");
                Frm_Announcement.ShowDialog();
            }
            else
            {
                LstB_ROI.Items.Add(name);
            }
        }

        private void Btn_ROI_Del_Click(object sender, EventArgs e)
        {
            string name = LstB_ROI.SelectedItem as string;
            if (name == "")
                return;
            LstB_ROI.Items.Remove(name);
        }
        private bool FindOverlapROIName(string name)
        {
            var Data = LstB_ROI.Items;
            foreach(var item in Data)
            {
                if ((item as string) == name)
                    return true;
            }
            return false;
        }

        private void LstB_ROI_SelectedIndexChanged(object sender, EventArgs e)
        {
            Tb_ROI_Name.Text = (sender as ListBox).SelectedItem as string;
        }
    }
}
