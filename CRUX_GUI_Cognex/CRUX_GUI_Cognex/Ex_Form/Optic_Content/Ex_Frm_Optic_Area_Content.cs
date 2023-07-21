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
using System.Runtime.InteropServices;
using System.Text;
using System.Reflection;
using System.Linq;

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


                MiniView = new Uctrl_MiniMap(CurFormIndex) { Location = new Point(0, 0), Margin = new Padding(0, 0, 0, 0) };
                MainView = new Uctrl_MainPic() { Location = new Point(0, 0), Margin = new Padding(0, 0, 0, 0) };

                Label lblb1 = new Label();
                Label lblb2 = new Label();
                Label lblb3 = new Label();
                Label lblb4 = new Label();
                Label lblb5 = new Label();
                Label lblb6 = new Label();
                Label lblb7 = new Label();
                Label lblb8 = new Label();
                Label lblb9 = new Label();
                Label lblb10 = new Label();
                Label lblb11 = new Label();
                Label lblb12 = new Label();
                Label lblb13 = new Label();
                Label lblb14 = new Label();
                Label lblb15 = new Label();
                Label lblb16 = new Label();
                Label lblb17 = new Label();
                MainView.Initialize(this, ref MiniView, ref lblb1, (int)Enums.ImageLoadFlag.CAMERA_LOAD_IMG, CurFormIndex, true, Pnl_ImageArea.Width, Pnl_ImageArea.Height, Pnl_ImageArea.Location.X, Pnl_ImageArea.Location.Y);

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
                m_rectGuideLine = new Rectangle();

                Point pntStartCoord = new Point();


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

                            m_rectGuideLine.X = -30;
                            m_rectGuideLine.Y = -30;
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

                            m_rectGuideLine.X = -30;
                            m_rectGuideLine.Y = (int)Height;
                            m_rectGuideLine.Width = (int)Width;
                            m_rectGuideLine.Height = pntStartCoord.Y + 30;
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
                            m_rectGuideLine.Width = pntStartCoord.X + 30;
                            m_rectGuideLine.Height = pntStartCoord.Y + 30;
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
                            m_rectGuideLine.Y = -30;
                            m_rectGuideLine.Width = pntStartCoord.X + 30;
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
                Dt_Area.Columns.Add(new DataColumn("Name") { ReadOnly = true });

                Dgv_Area.DataSource = Dt_Area;

                for (int i = 0; i < Dgv_Area.Columns.Count; ++i)
                {
                    Dgv_Area.Columns[i].SortMode = DataGridViewColumnSortMode.NotSortable;
                    if (i == Dgv_Area.ColumnCount - 1)
                        Dgv_Area.Columns[i].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                }
                Dgv_Area.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.ColumnHeader;
                //////////// 패턴 Dgv 초기화
                DataTable Dt_Pattern = new DataTable();
                Dt_Pattern.Columns.Add("Use", typeof(bool));
                Dt_Pattern.Columns.Add("Vac", typeof(bool));
                Dt_Pattern.Columns.Add(new DataColumn("Name") { ReadOnly = true });

                Dgv_Pattern.DataSource = Dt_Pattern;

                for (int i = 0; i < Dgv_Pattern.Columns.Count; ++i)
                {
                    Dgv_Pattern.Columns[i].SortMode = DataGridViewColumnSortMode.NotSortable;
                    if (i == Dgv_Pattern.ColumnCount - 1)
                        Dgv_Pattern.Columns[i].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                }
                Dgv_Pattern.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.ColumnHeader;
                //////////// 카메라 컨디션 Dgv 초기화
                DataTable Dt_GrabCond = new DataTable();
                Dt_GrabCond.Columns.Add("Use", typeof(bool));
                Dt_GrabCond.Columns.Add(new DataColumn("Name") { ReadOnly = true });
                Dt_GrabCond.Columns.Add(new DataColumn("Type") { ReadOnly = true });
                Dt_GrabCond.Columns.Add("Exp");
                Dt_GrabCond.Columns.Add("Gain");
                Dt_GrabCond.Columns.Add("PS");
                Dt_GrabCond.Columns.Add("Delay");
                Dt_GrabCond.Columns.Add("CountF");
                Dt_GrabCond.Columns.Add("CountB");
                Dt_GrabCond.Columns.Add("StartF");
                Dt_GrabCond.Columns.Add("StartB");
                Dt_GrabCond.Columns.Add("StopF");
                Dt_GrabCond.Columns.Add("StopB");
                Dt_GrabCond.Columns.Add("PeriodF");
                Dt_GrabCond.Columns.Add("PeriodB");

                Dgv_GrabCond.DataSource = Dt_GrabCond;

                for (int i = 0; i < Dgv_GrabCond.Columns.Count; ++i)
                {
                    Dgv_GrabCond.Columns[i].SortMode = DataGridViewColumnSortMode.NotSortable;
                    Dgv_GrabCond.Columns[i].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                    if (i == Dgv_GrabCond.ColumnCount)
                        Dgv_GrabCond.Columns[i].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                }
                Dgv_GrabCond.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.ColumnHeader;

                //////////// 조명 Dgv 초기화
                DataTable Dt_LightCond = new DataTable();
                Dt_LightCond.Columns.Add("Use", typeof(bool));
                Dt_LightCond.Columns.Add("Port_No");
                Dt_LightCond.Columns.Add("Crtl_No");
                for (int i = 0; i < Consts.MAX_LIGHT_CHANNEL_COUNT; ++i)
                {
                    Dt_LightCond.Columns.Add($"Ch_{i}");
                }
                Dgv_LightCond.DataSource = Dt_LightCond;

                for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
                {
                    Dgv_LightCond.Columns[i].SortMode = DataGridViewColumnSortMode.NotSortable;
                    Dgv_LightCond.Columns[i].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                    if (i == Dgv_LightCond.ColumnCount)
                        Dgv_LightCond.Columns[i].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                }
                Dgv_LightCond.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.ColumnHeader;
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
            int nRet = Consts.APP_OK;
            try
            {

                DataTable AreasData = Dgv_Area.DataSource as DataTable;
                Dgv_Area.DataSource = RecipeManager.CvtDtAreaInfo(AreasData, Shared_Recipe.ViewRecipe.Area_Data);
                if (Dgv_Area.Rows.Count > 0)
                {
                    Dgv_Area.Rows[0].Selected = true;

                    DataTable PatternsData = Dgv_Pattern.DataSource as DataTable;
                    Dgv_Pattern.DataSource = RecipeManager.CvtDtPatternInfo(PatternsData, Shared_Recipe.ViewRecipe.Area_Data.Area[Dgv_Area.SelectedRows[0].Index]);
                    if (Dgv_Pattern.Rows.Count > 0)
                    {
                        Dgv_Pattern.Rows[0].Selected = true;
                        DataTable CamCond = Dgv_GrabCond.DataSource as DataTable;
                        Dgv_GrabCond.DataSource = RecipeManager.CvtDtGrabCond(CamCond, Shared_Recipe.ViewRecipe.Area_Data.Area[Dgv_Area.SelectedRows[0].Index].Patterns[Dgv_Pattern.SelectedRows[0].Index].Grab_Data);

                        DataTable LightCond = Dgv_LightCond.DataSource as DataTable;
                        Dgv_LightCond.DataSource = RecipeManager.CvtDtLightCond(LightCond, Shared_Recipe.ViewRecipe.Area_Data.Area[Dgv_Area.SelectedRows[0].Index].Patterns[Dgv_Pattern.SelectedRows[0].Index].Grab_Data);
                    }
                }


                if (!Modes.NET_SIMULATION_MODE)
                {
                    ST_CAM_INFOMATION CamInfo = new ST_CAM_INFOMATION();

                    CmdMsgParam Param = new CmdMsgParam();
                    Param.SetStruct(CamInfo);


                    nRet = Systems.g_Ipc.SendCommand((ushort)((Globals.CurrentPCno + 1) * 100 + IpcConst.CAMERA_TASK + CamIndex), IpcConst.CAMERA_FUNC, IpcConst.GET_CAMINFO,
                                                    IpcInterface.CMD_TYPE_RES, 100000, Param.GetByteSize(), Param.GetParam());
                    Param.SetOffset(0);
                    CamInfo = (ST_CAM_INFOMATION)Param.GetStruct(typeof(ST_CAM_INFOMATION), (int)Param.GetByteSize());

                    SetCameraInfo(CamInfo);

                    Tmr_CamTemperature.Start();
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void SetCameraInfo(ST_CAM_INFOMATION data)
        {
            Lb_CameraNameContent.Text = $"{data.Name.toUniString()}";
            Lb_WidthContent.Text = $"{data.Width.ToString()} Pixel";
            Lb_HeightContent.Text = $"{data.Height.ToString()} Pixel";
            Lb_DepthContent.Text = $"{data.Depth.ToString()} Bit";
            Lb_TempContent.Text = $"{data.Temp.ToString()} ℃";
        }
        private void Btn_GrabStart_Click(object sender, EventArgs e)
        {
            LiveStart();
        }

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
        private void UpdatePattern()
        {
            try
            {
                if (Dgv_Area.SelectedRows.Count > 0)
                {
                    //ClearRecipeControl();
                    Area Temp = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Dgv_Area.SelectedRows[0].Cells["Name"].Value.ToString());
                    if (Temp == null)
                        return;

                    DataTable Dt = Dgv_Pattern.DataSource as DataTable;
                    Dt.Rows.Clear();
                    Dgv_Pattern.DataSource = Dt;
                    Dgv_Pattern.Refresh();

                    foreach (Pattern item in Temp.Patterns)
                    {
                        Dt.Rows.Add(item.Grab, item.Vacuum, item.Name);
                    }
                    Dgv_Pattern.DataSource = Dt;
                    if (Dgv_Pattern.Rows.Count > 0)
                    {
                        Dgv_Pattern.Rows[0].Selected = true;
                    }
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        private void UpdateGrabCondition()
        {
            try
            {
                if (Dgv_Pattern.SelectedRows.Count > 0 && Dgv_Area.SelectedRows.Count > 0)
                {
                    string selAreaName = Dgv_Area.SelectedRows[0].Cells["Name"].Value.ToString();
                    string SelPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                    Optics FindOpticInfo = Shared_Recipe?.ViewRecipe?.Area_Data?.Area?.Find(x => x.Name == selAreaName)?.Patterns?.Find(x => x.Name == SelPtnName)?.Grab_Data;

                    List<CameraInfo> CamInfoList = FindOpticInfo.Camera_Data;

                    DataTable Dt = Dgv_GrabCond.DataSource as DataTable;
                    Dt.Clear();

                    int MemberCount = typeof(CameraInfo).GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance).Length;
                    for (int i = 0; i < CamInfoList.Count; ++i)
                    {
                        DataRow Dr = Dt.NewRow();
                        List<object> ArrayTemp = new List<object>();

                        for (int j = 0; j < MemberCount; ++j)
                        {
                            ArrayTemp.Add(CamInfoList[i][j]);
                        }
                        Dr.ItemArray = ArrayTemp.ToArray();/*new object[] {CamInfoList[i].Use, CamInfoList[i].Name, CamInfoList[i].CamType, CamInfoList[i].Expose, CamInfoList[i].Gain, CamInfoList[i].PS, CamInfoList[i].Delay, CamInfoList[i].nCountF, CamInfoList[i].nCountB, CamInfoList[i].nStartF, CamInfoList[i].nStartB, CamInfoList[i].nStopF, CamInfoList[i].nStopB, CamInfoList[i].nPeriodF, CamInfoList[i].nPeriodB};*/
                        Dt.Rows.Add(Dr);
                    }

                    Dgv_GrabCond.DataSource = Dt;
                    Dgv_GrabCond.Refresh();
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }


        private void UpdateLightCondition()
        {
            try
            {
                if (Dgv_Pattern.SelectedRows.Count > 0 && Dgv_Area.SelectedRows.Count > 0)
                {
                    string selAreaName = Dgv_Area.SelectedRows[0].Cells["Name"].Value.ToString();
                    string SelPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                    Optics FindOpticInfo = Shared_Recipe?.ViewRecipe?.Area_Data?.Area?.Find(x => x.Name == selAreaName)?.Patterns?.Find(x => x.Name == SelPtnName)?.Grab_Data;

                    List<LightInfo> LightInfoList = FindOpticInfo.Light_Data;

                    DataTable Dt = Dgv_LightCond.DataSource as DataTable;
                    Dt.Clear();

                    foreach (LightInfo item in LightInfoList)
                    {
                        List<object> ArrayTemp = new List<object>();

                        uint[] LightValueTemp = Enumerable.Repeat<uint>(0, Consts.MAX_LIGHT_CHANNEL_COUNT + 3).ToArray();

                        for (int j = 0; j < LightValueTemp.Length; ++j)
                            ArrayTemp.Add(LightValueTemp[j]);
                      
                        DataRow Dr = Dt.NewRow();

                        ArrayTemp[0] = item.Use;
                        ArrayTemp[1] = item.Port_No;
                        ArrayTemp[2] = item.Controller_No;

                        for (int j = 0; j < item.LightValue.Count; ++j)
                            ArrayTemp[j+3] = item.LightValue[j];     

                        Dr.ItemArray = ArrayTemp.ToArray();
                        Dt.Rows.Add(Dr);
                    }

                    Dgv_LightCond.DataSource = Dt;
                    Dgv_LightCond.Refresh();
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        private void LstV_GuideLine_MouseClick(object sender, MouseEventArgs e)
        {
            try
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
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Tmr_CamTemperature_Tick(object sender, EventArgs e)
        {
            try
            {
                int nRet = Consts.APP_OK;
                uint Temp = 0;
                CmdMsgParam Param = new CmdMsgParam();
                Param.ClearOffset();
                Param.SetUInteger((uint)0);

                nRet = Systems.g_Ipc.SendCommand((ushort)((Globals.CurrentPCno + 1) * 100 + IpcConst.CAMERA_TASK + CamIndex), IpcConst.CAMERA_FUNC, IpcConst.GET_CAMTEMPATURE,
                                                IpcInterface.CMD_TYPE_RES, 1000, Param.GetByteSize(), Param.GetParam());
                if (nRet == Consts.APP_OK)
                {
                    Param.SetOffset(0);
                    Temp = Param.GetUInteger();
                }

                Lb_TempContent.Text = $"{Temp.ToString()} ℃";
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Btn_ProperyApply_Click(object sender, EventArgs e)
        {

        }

        private void Dgv_Area_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (Dgv_Area.SelectedRows.Count > 0)
            {
                //string AreaName = Dgv_Area.SelectedRows[0].Cells["Name"].Value.ToString();
                UpdatePattern();
                UpdateGrabCondition();
                UpdateLightCondition();
            }
        }

        private void Dgv_Pattern_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            UpdateGrabCondition();
            UpdateLightCondition();
        }

        private void Dgv_GrabCond_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {

        }
        private void m_GrabCondItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            switch (e.ClickedItem.Text)
            {
                case "추가":
                    foreach (DataGridViewCell cell in Dgv_GrabCond.SelectedCells)
                    {
                        cell.OwningRow.Cells["Check"].Value = true;
                    }
                    break;
                case "삭제":
                    foreach (DataGridViewCell cell in Dgv_GrabCond.SelectedCells)
                    {
                        cell.OwningRow.Cells["Check"].Value = false;
                    }
                    break;

                default:
                    break;
            }
        }
        private void m_LightCondItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            switch (e.ClickedItem.Text)
            {
                case "추가":
                    foreach (DataGridViewCell cell in Dgv_LightCond.SelectedCells)
                    {
                        cell.OwningRow.Cells["Check"].Value = true;
                    }
                    break;
                case "삭제":
                    foreach (DataGridViewCell cell in Dgv_LightCond.SelectedCells)
                    {
                        cell.OwningRow.Cells["Check"].Value = false;
                    }
                    break;

                default:
                    break;
            }
        }
        private void Dgv_LightCond_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {

        }

        private void Dgv_GrabCond_MouseUp(object sender, MouseEventArgs e)
        {
            DataGridView.HitTestInfo hitTestInfo; //Hit 위치 

            if (e.Button == MouseButtons.Right)
            {
                hitTestInfo = Dgv_GrabCond.HitTest(e.X, e.Y);

                int col = hitTestInfo.ColumnIndex;
                int row = hitTestInfo.RowIndex;

                ContextMenuStrip m = new ContextMenuStrip();

                m.Items.Add("추가");
                m.Items.Add("삭제");

                if (row == -1)
                    // 선택한 Row가 0일 때 삭제 버튼 Enable = false
                    m.Items[1].Enabled = false;

                if (col < 0) col = 0;
                if (row < 0) row = 0;

                if(Dgv_GrabCond.Rows.Count > 0)
                    this.Dgv_GrabCond.CurrentCell = this.Dgv_GrabCond[col, row]; //선택되게 설정  

                m.ItemClicked += new ToolStripItemClickedEventHandler(m_GrabCondItemClicked);

                m.Show(Dgv_GrabCond, new Point(e.X, e.Y));
            }
        }

        private void Dgv_LightCond_MouseUp(object sender, MouseEventArgs e)
        {
            DataGridView.HitTestInfo hitTestInfo; //Hit 위치 

            if (e.Button == MouseButtons.Right)
            {
                hitTestInfo = Dgv_LightCond.HitTest(e.X, e.Y);

                int col = hitTestInfo.ColumnIndex;
                int row = hitTestInfo.RowIndex;

                ContextMenuStrip m = new ContextMenuStrip();

                m.Items.Add("추가");
                m.Items.Add("삭제");

                if (row == -1)
                    // 선택한 Row가 0일 때 삭제 버튼 Enable = false
                    m.Items[1].Enabled = false;

                if (col < 0) col = 0;
                if (row < 0) row = 0;
                if (Dgv_GrabCond.Rows.Count > 0)
                    this.Dgv_LightCond.CurrentCell = this.Dgv_LightCond[col, row]; //선택되게 설정  

                m.ItemClicked += new ToolStripItemClickedEventHandler(m_LightCondItemClicked);

                m.Show(Dgv_LightCond, new Point(e.X, e.Y));
            }
        }
    }
}
