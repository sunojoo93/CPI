using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.User_Controls;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using CRUX_GUI_Cognex.Interface;
using System.Threading;

namespace CRUX_GUI_Cognex.Ex_Form
{
    public partial class Ex_Frm_Optic_Area_Content : Form, ICamBase
    {
        List<Control> ItControl = new List<Control>();
        Uctrl_MainPic MainView;
        Uctrl_MiniMap MiniView;
        public string CurrentFormName = string.Empty;
        public int CurFormIndex = 0;
        int CamIndex = 0;
        public Recipes Shared_Recipe;
        // 티칭용 가이드라인
        private Rectangle m_rectGuideLine;
        // 티칭용 가이드라인에 대한 각도
        private double m_dGuideLineOffsetT = 0.0F;
        public Ex_Frm_Optic_Area_Content()
        {
            try
            {
                InitializeComponent();
                TopLevel = false;
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
                //WindowState = FormWindowState.Maximized;
                //Show();
                // ListView 가로 스크롤 제거
                WinApis.ShowScrollBar(LstV_GuideLine.Handle, 1, true);
                

                MiniView = new Uctrl_MiniMap(CurFormIndex) { Location = new Point(0,0), Margin = new Padding(0, 0, 0, 0) };
                MainView = new Uctrl_MainPic() { Location = new Point(0,0), Margin = new Padding(0, 0, 0, 0) };
                
                Label lblb1 = new Label();
                Label lblb2 = new Label();
                Label lblb3 = new Label();
                Label lblb4 = new Label();
                Label lblb5 = new Label();
                Label lblb6 = new Label();
                Label lblb7 = new Label();
                Label lblb8 = new Label();
                Label lblb9 = new Label();
                Label lblb10= new Label();
                Label lblb11 = new Label();
                Label lblb12 = new Label();
                Label lblb13= new Label();
                Label lblb14 = new Label();
                Label lblb15 = new Label();
                Label lblb16 = new Label();
                Label lblb17 = new Label();
                MainView.Initialize(this, ref MiniView, ref lblb1, (int)Enums.ImageLoadFlag.CAMERA_LOAD_IMG, CurFormIndex, true, Pnl_ImageArea.Width,Pnl_ImageArea.Height, Pnl_ImageArea.Location.X, Pnl_ImageArea.Location.Y);

                MiniView.ucRefMainImg(ref MainView);
                MainView.m_fnLinkCurCoord(ref lblb2, ref lblb3);

                MainView.m_fnLinkLineProfileCoord(ref lblb4, ref lblb5, ref lblb6, ref lblb7);
                MainView.m_fnLinkUniformityValue(ref lblb8);

                MainView.m_fnLinkNowRatio(ref lblb9);
                MainView.m_fnLinkPointCoord(ref lblb10, ref lblb11);
                MainView.m_fnLinkCurRGBVal(ref lblb12, ref lblb13, ref lblb14);
                MainView.m_fnLinkCurFocusVal(ref lblb15);
                // ROI 좌표 등록 사용 안함
                MainView.m_fnUseEventRectOff();
                // ROI 좌표 중복 등록 사용 안함
                MainView.m_fnUseRoiMultiLine(false);

                MiniView.Dock = DockStyle.Fill;
                MainView.Dock = DockStyle.Fill;

                Pnl_ImageArea.Controls.Add(MainView);
                Pnl_MiniMap.Controls.Add(MiniView);

         
                //MainView.Show();
                InitGuideLine();

                InitDataTables();
            }
            catch (Exception ex)
            {
                throw ex;
            }            
        }
        private void SetGuideLine(bool use, Enums.GuideLine pos, bool bUseDivision = false)
        {
            try
            {
                //int SelIdx = LstV_GuideLine.SelectedIndices.toInt();
                //double dPxlSize = m_dResolution;
                m_rectGuideLine = new Rectangle();


                //  if (dPxlSize <= 0) dPxlSize = 1;

                Point pntStartCoord = new Point();

                //             if (bUseDivision)
                //             {
                //                 pntStartCoord.X = m_UcRcpTeachPic.m_fnGetImgSize().X / 2;
                //                 pntStartCoord.Y = m_UcRcpTeachPic.m_fnGetImgSize().Y / 2;
                //             }
                //             else
                pntStartCoord = MainView.m_fnGetImgSize();


                // 얼라인 좌표 등록 사용 안함
                MainView.InitGuideLineData(Color.Red);
                MainView.InitLineProfileData(Color.Yellow);
                MainView.InitContrastData(Color.LightBlue);
                MainView.m_fnUseEventGuideLineView(use);
                //Thread.Sleep(100);
                //MainView.DataToSetGuideLineArea(m_rectGuideLine, m_dGuideLineOffsetT);


                //2021.10.28 KYH

                double Width = 0;
                double Height = 0;
                IniSection Section;
                IniValue x, y, width, height;
                switch (pos)
                {
                    case Enums.GuideLine.LEFT_TOP:
                        Section = null;
                        Systems.RecipeData_Collection[CurFormIndex]["GuideLine.ini"].TryGetSection(Enums.GuideLine.LEFT_TOP.ToString(), out Section);
                        if (Section != null)
                        {
                            Section.TryGetValue("X", out x);
                            Section.TryGetValue("Y", out y);
                            Width = Math.Abs(pntStartCoord.X * Convert.ToDouble(x.Value?.ToString()));
                            Height = Math.Abs(pntStartCoord.Y * Convert.ToDouble(y.Value?.ToString()));

                            m_rectGuideLine.X = -10;
                            m_rectGuideLine.Y = -10;
                            m_rectGuideLine.Width = (int)Width;
                            m_rectGuideLine.Height = (int)Height;
                        }
                        break;

                    case Enums.GuideLine.LEFT_BOT:
                        Section = null;
                        Systems.RecipeData_Collection[CurFormIndex]["GuideLine.ini"].TryGetSection(Enums.GuideLine.LEFT_BOT.ToString(), out Section);
                        if (Section != null)
                        {
                            Section.TryGetValue("X", out x);
                            Section.TryGetValue("Y", out y);
                            Width = Math.Abs(pntStartCoord.X * Convert.ToDouble(x.Value?.ToString()));
                            Height = Math.Abs(pntStartCoord.Y * Convert.ToDouble(y.Value?.ToString()));

                            m_rectGuideLine.X = -10;
                            m_rectGuideLine.Y = (int)Height;
                            m_rectGuideLine.Width = (int)Width;
                            m_rectGuideLine.Height = pntStartCoord.Y + 10;
                        }
                        break;

                    case Enums.GuideLine.RIGHT_TOP:
                        Section = null;
                        Systems.RecipeData_Collection[CurFormIndex]["GuideLine.ini"].TryGetSection(Enums.GuideLine.RIGHT_TOP.ToString(), out Section);
                        if (Section != null)
                        {
                            Section.TryGetValue("X", out x);
                            Section.TryGetValue("Y", out y);
                            Width = Math.Abs(pntStartCoord.X * Convert.ToDouble(x.Value?.ToString()));
                            Height = Math.Abs(pntStartCoord.Y * Convert.ToDouble(y.Value?.ToString()));

                            m_rectGuideLine.X = (int)Width;
                            m_rectGuideLine.Y = (int)Height;
                            m_rectGuideLine.Width = pntStartCoord.X + 10;
                            m_rectGuideLine.Height = pntStartCoord.Y + 10;
                        }
                        break;

                    case Enums.GuideLine.RIGHT_BOT:
                        Section = null;
                        Systems.RecipeData_Collection[CurFormIndex]["GuideLine.ini"].TryGetSection(Enums.GuideLine.RIGHT_BOT.ToString(), out Section);
                        if (Section != null)
                        {
                            Section.TryGetValue("X", out x);
                            Section.TryGetValue("Y", out y);
                            Width = Math.Abs(pntStartCoord.X * Convert.ToDouble(x.Value?.ToString()));
                            Height = Math.Abs(pntStartCoord.Y * Convert.ToDouble(y.Value?.ToString()));

                            m_rectGuideLine.X = (int)Width;
                            m_rectGuideLine.Y = -10;
                            m_rectGuideLine.Width = pntStartCoord.X + 10;
                            m_rectGuideLine.Height = (int)Height;
                        }
                        break;

                    case Enums.GuideLine.CENTER:
                        Section = null;
                        Systems.RecipeData_Collection[CurFormIndex]["GuideLine.ini"].TryGetSection(Enums.GuideLine.CENTER.ToString(), out Section);
                        if (Section != null)
                        {
                            Section.TryGetValue("X", out x);
                            Section.TryGetValue("Y", out y);
                            Section.TryGetValue("Width", out width);
                            Section.TryGetValue("Height", out height);
 
                            int CoordX = (int)Math.Abs(pntStartCoord.X * Convert.ToDouble(x.Value?.ToString()));
                            int CoordY = (int)Math.Abs(pntStartCoord.Y * Convert.ToDouble(y.Value?.ToString()));
                            Width = Math.Abs(pntStartCoord.X * Convert.ToDouble(width.Value?.ToString()));
                            Height = Math.Abs(pntStartCoord.Y * Convert.ToDouble(height.Value?.ToString()));

                            m_rectGuideLine.X = CoordX;
                            m_rectGuideLine.Y = CoordY;
                            m_rectGuideLine.Width = (int)(Width - CoordX);
                            m_rectGuideLine.Height = (int)(Height - CoordY);
                        }
                        break;
                }

                MainView.DataToSetGuideLineArea(m_rectGuideLine, m_dGuideLineOffsetT);

            }
            catch (Exception ex)
            {
                throw ex;
            }
            //checkGuideLineView.Checked = true;
        }
        public void InitDataTables()
        {
            try
            {
                //////////// Area Dgv 초기화
                DataTable Dt_Area = new DataTable();
                Dt_Area.Columns.Add("Use", typeof(bool));
                Dt_Area.Columns.Add("Name");

                Dgv_Area.DataSource = Dt_Area;

                Dgv_Area.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;

                for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
                {
                    Dgv_Area.Columns[i].SortMode = DataGridViewColumnSortMode.Automatic;
                }

                //////////// 패턴 Dgv 초기화
                DataTable Dt_Pattern = new DataTable();
                Dt_Pattern.Columns.Add("Use", typeof(bool));
                Dt_Pattern.Columns.Add("Name");

                Dgv_Pattern.DataSource = Dt_Pattern;

                for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
                {
                    Dgv_Pattern.Columns[i].SortMode = DataGridViewColumnSortMode.Automatic;
                }
                Dgv_Pattern.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
                //////////// 카메라 컨디션 Dgv 초기화
                DataTable Dt_GrabCond = new DataTable();
                Dt_GrabCond.Columns.Add("Use", typeof(bool));
                Dt_GrabCond.Columns.Add("Name");
                Dt_GrabCond.Columns.Add("Exp");
                Dt_GrabCond.Columns.Add("Gain");

                Dgv_GrabCond.DataSource = Dt_GrabCond;

                for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
                {
                    Dgv_GrabCond.Columns[i].SortMode = DataGridViewColumnSortMode.Automatic;
                }
                Dgv_GrabCond.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;

                //////////// 조명 Dgv 초기화
                DataTable Dt_LightCond = new DataTable();
                Dt_LightCond.Columns.Add("Use", typeof(bool));
                Dt_LightCond.Columns.Add("Name");
                for (int i = 0; i < Consts.MAX_LIGHT_CHANNEL_COUNT; ++i)
                {
                    Dt_LightCond.Columns.Add($"Ch_{i}");
                }
                Dgv_LightCond.DataSource = Dt_LightCond;

                for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
                {
                    if (i == 0)
                        Dgv_LightCond.Columns[0].Width = 50;
                    else if (i == 1)
                        Dgv_LightCond.Columns[1].Width = 50;
                    else
                        Dgv_LightCond.Columns[i].Width = 50;
                }
                for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
                {
                    Dgv_LightCond.Columns[i].SortMode = DataGridViewColumnSortMode.Automatic;
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        private void InitGuideLine()
        {
            try
            {
                WinApis.ShowScrollBar(LstV_GuideLine.Handle, 0, false);
                foreach (Enums.GuideLine item in Enum.GetValues(typeof(Enums.GuideLine)))
                {
                    LstV_GuideLine.Items.Add(item.ToString());
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void BtnTemp_Click(object sender, EventArgs e)
        {
      
        }

        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        public void SetRecipe(ref Recipes recipe)
        {
            try
            {
                Shared_Recipe = recipe;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void LstV_GuideLine_MouseUp(object sender, MouseEventArgs e)
        {

        }

        private void Btn_AllOn_Click(object sender, EventArgs e)
        {
    
        }

        private void Ex_Frm_Optic_Area_Content_Shown(object sender, EventArgs e)
        {
            try
            {
                DataTable AreasData = Dgv_Area.DataSource as DataTable;
                Dgv_Area.DataSource = RecipeManager.CvtDtAreaInfo(AreasData, Shared_Recipe.ViewRecipe.Area_Data);
            }
            catch (Exception ex)
            {
                throw ex;
            }

            //DataTable PatternData = Dgv_Pattern.DataSource as DataTable;
            //Dgv_Pattern.DataSource = RecipeManager.CvtDtPatternInfo(PatternData, Shared_Recipe.ViewRecipe.Area_Data.Area);

            //DataTable GrabCondData = Dgv_GrabCond.DataSource as DataTable;
            //Dgv_GrabCond.DataSource = RecipeManager.CvtDtGrabCond(GrabCondData, Shared_Recipe.ViewRecipe.Area_Data);

            //DataTable LightCodData = Dgv_LightCond.DataSource as DataTable;
            //Dgv_LightCond.DataSource = RecipeManager.CvtDtLightInfo(LightCodData, Shared_Recipe.ViewRecipe.Area_Data);
        }

        private void Btn_GrabStart_Click(object sender, EventArgs e)
        {
            LiveStart();
        }

        //public void LiveStart()
        //{

        //}
        delegate void DelLiveStopCallBack();
        public void LiveStop()
        {
            Utility.LoadingStart();
            int nRet = MainView.LiveEnd();
            if (nRet == Consts.APP_OK)
            {
                //18.12.11 Auto Run 시 Camera Live 꺼지게 소스 변경 - JSO
                if (this.InvokeRequired)
                {
                    DelLiveStopCallBack delStopCam = new DelLiveStopCallBack(LiveStop);
                    this.Invoke(delStopCam);
                    //return nRet;
                }
                //checkUseGV.Checked = false;
                //chkDivision.Checked = false;
                //if (SendLightOff() == Consts.APP_OK)
                    SetGuideLine(false, Enums.GuideLine.None, false);
                MainView.initImage();
                //btnLiveStart.Text = "Live Start";
               // btnFocusFigure.Enabled = false;
                //pnlTeachMenu.Enabled = false;
                //grbGuideLineCtrl.Enabled = false;

                MainView.SetLiveFlg(false);

                //btnCameraVal.Enabled = true;
            }
            Utility.LoadingStop();
            //return nRet;
        }

        void ICamBase.LightOn()
        {
            throw new NotImplementedException();
        }

        void ICamBase.LightOff()
        {
            throw new NotImplementedException();
        }

        public void LiveStart()
        {
            try
            {
                bool bRet = MainView.LiveStart(0, Pnl_ImageArea.Width, Pnl_ImageArea.Height);
                {
                    //checkUseGV.Checked = true;
                    //m_UcRcpTeachPic.initImage();

                    //  if (SendDataLightTask() == Consts.APP_OK)
                    //      SetGuideLine(true);
                    // btnLiveStart.Text = "Live Stop";
                    //  btnFocusFigure.Enabled = true;
                    // pnlTeachMenu.Enabled = true;
                    //  grbGuideLineCtrl.Enabled = true;
                    // m_UcRcpTeachPic.m_fnUseGvView(checkUseGV.Checked);
                    // btnCameraVal.Enabled = false;
                }
                //return bRet;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Btn_GrabStop_Click(object sender, EventArgs e)
        {
            LiveStop();
        }

        private void LstV_GuideLine_SelectedIndexChanged(object sender, EventArgs e)
        {


        }

        private void LstV_GuideLine_MouseClick(object sender, MouseEventArgs e)
        {
            int SelIdx = LstV_GuideLine.SelectedItems[0].Index;
            switch (SelIdx)
            {
                case 0:
                    SetGuideLine(false, Enums.GuideLine.None, false);
                    break;
                case 1:
                    SetGuideLine(true, Enums.GuideLine.LEFT_TOP, false);
                    break;
                case 2:
                    SetGuideLine(true, Enums.GuideLine.RIGHT_TOP, false);
                    break;
                case 3:
                    SetGuideLine(true, Enums.GuideLine.LEFT_BOT, false);
                    break;
                case 4:
                    SetGuideLine(true, Enums.GuideLine.RIGHT_BOT, false);
                    break;
                case 5:
                    SetGuideLine(true, Enums.GuideLine.CENTER, false);
                    break;
                default:
                    break;
            }
        }
    }
}
