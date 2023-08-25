using Cognex.VisionPro;
using Cognex.VisionPro.Display;
using Cognex.VisionPro.QuickBuild;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Ex_Form;
using CRUX_GUI_Cognex.User_Controls;
using CRUX_GUI_Cognex.Utils;
using PropertyGridExt;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Design;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.ListView;

namespace CRUX_GUI_Cognex.Ex_Form
{
    public partial class Ex_Frm_Recipe_ROI : Form
    {
        Point CurWindowPosition = new Point();
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        Label m_lbImageState = new Label();
        bool _AltIsDown = false;
        bool LeftMouseDown = false;

        ListViewItem.ListViewSubItem curSB;
        ListViewItem curItem;
        bool cancelEdit;
        bool Display_Draging = false;
        ToolTip Tip = new ToolTip();
        Recipes Recipe;
        public Pattern CurPattern;
        public Pattern OriginPattern;

        bool AltIsDown
        {
            get
            {
                return _AltIsDown;
            }
            set
            {
                try
                {
                    _AltIsDown = value;
                    if (Lb_Tooltip != null)
                    {
                        if (_AltIsDown)
                        {
                            this.Invoke(new MethodInvoker(delegate ()
                            {
                                Lb_Tooltip.BackColor = Color.LightGreen;
                            }));
                        }
                        else
                        {
                            this.Invoke(new MethodInvoker(delegate ()
                            {
                                Lb_Tooltip.BackColor = Color.Orange;
                            }));
                        }
                    }
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                    throw ex;
                }
            }
        }
        public void SetRecipe(ref Recipes recipe)
        {
            try
            {
                Recipe = recipe;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            try
            {
                CurrentFormName = name;
                CurFormIndex = index;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }
        public Ex_Frm_Recipe_ROI()
        {
            try
            {
                InitializeComponent();
                ///TopLevel = false;
                //Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;

                Cog_ROI_Display.MouseDown += Cog_ROI_Display_MouseDown;
                Cog_ROI_Display.MouseMove += Cog_ROI_Display_MouseMove;
                Cog_ROI_Display.MouseUp += Cog_ROI_Display_MouseUp;
                Cog_ROI_Display.ScalingMethod = Cognex.VisionPro.Display.CogDisplayScalingMethodConstants.Integer;
                Cog_ROI_Display.Click += Cog_ROI_Display_Click;
                Cog_ROI_Display.MouseWheelMode = CogDisplayMouseWheelModeConstants.Zoom1;


                Cog_Display_Toolbar.Display = Cog_ROI_Display;
                Cog_Display_Status.Display = Cog_ROI_Display;
                Cog_ROI_Display.MultiSelectionEnabled = false;
                //Show();
                //InitPGE();

                InputBox.KeyDown += InputBox_KeyDown;
                InputBox.Leave += InputBox_Leave;
                InputBox.Hide();
                //RefeshRoiDataView();      

            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }
        public void Init(ICogImage image, ref Pattern ptn)
        {
            try
            {
                Cog_ROI_Display.Image = image;
                Cog_ROI_Display.AutoFit = true;
                OriginPattern = ptn;
                CurPattern = Utility.DeepCopy(ptn);
                InitControl();
                RefeshRoiDataView();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }
        public void RefeshRoiDataView()
        {
            try
            {
                if (CurPattern?.ROI_Data.Count >= 0)
                {
                    Cog_ROI_Display.DrawingEnabled = false;
                    //Dictionary<string, List<ROI_Data>> ROI_List = Systems.RecipeContent.ViewRecipe[CurFormIndex].ROI_List;
                    Cog_ROI_Display.InteractiveGraphics.Clear();
                    LstV_ROI.Groups.Clear();
                    LstV_ROI.Items.Clear();
                    PGE_ROIProp.Item.Clear();
                    //if (!CurPattern.ROI_Coord.ContainsKey(Systems.CurrentSelectedAreaName[CurFormIndex]))
                    //    return;
                    List<ROI> Temp = CurPattern.ROI_Data;

                    //foreach (string item in LstB_Category.Items)
                    //    LstV_ROI.Groups.Add(new ListViewGroup(item, item));

                    if (Temp != null && Temp.Count > 0)
                    {
                        foreach (ROI item in Temp)
                        {
                            //string Category = item.Coord.Category;
                            CogRectangle Rect = new CogRectangle();
                            
                            //var ROIProp = PGE_ROIProp.Item.FindName(item.Name);
                            ROI_PropertyData Rp = item.ROI_Property;

                            Rect.X = item.Coord.X.toDbl();
                            Rect.Y = item.Coord.Y.toDbl();
                            Rect.Width = item.Coord.Width.toDbl(); ;
                            Rect.Height = item.Coord.Height.toDbl();

                            Rect.SelectedLineStyle = Cognex_Helper.GetLineStyleFromString(Rp.SelectedLineStyle);
                            Rect.SelectedColor = Cognex_Helper.GetColorFromString(Rp.SelectedLineColor);
                            Rect.LineStyle = Cognex_Helper.GetLineStyleFromString(Rp.LineStyle);
                            Rect.DragLineStyle = Cognex_Helper.GetLineStyleFromString(Rp.DragLineStyle);
                            Rect.DragColor = Cognex_Helper.GetColorFromString(Rp.DragLineColor);
                            Rect.Color = Cognex_Helper.GetColorFromString(Rp.LineColor);

                            Cog_ROI_Display.DrawingEnabled = false;
                            Rect.Interactive = true;
                            Rect.Dragging += new CogDraggingEventHandler(MRect_Dragging);
                            Rect.DraggingStopped += new CogDraggingStoppedEventHandler(MRect_DraggingStopped);
                            Rect.GraphicDOFEnable = CogRectangleDOFConstants.All;
                            string ROIName = item.Name;
                            string RectROIName = $"{ROIName}";

                            Cog_ROI_Display.InteractiveGraphics.Add(Rect, RectROIName, false);

                            //ROI_Data Data = new ROI_Data();
                            //Data.Name = ROIName;
                            //Data.JobName = item.JobName;
                            //Data.Category = item.Category;                           
                            //Data.Object = Rect;
                            ROI_Property Prop = new ROI_Property();
                            Prop.Name = item.Name.ToString();
                            Prop.LineColor = Cognex_Helper.GetColorFromString(item.ROI_Property.LineColor);
                            Prop.LineStyle = Cognex_Helper.GetLineStyleFromString(item.ROI_Property.LineStyle);
                            Prop.SelectedLineStyle = Cognex_Helper.GetLineStyleFromString(item.ROI_Property.SelectedLineStyle);
                            Prop.SelectedLineColor = Cognex_Helper.GetColorFromString(item.ROI_Property.SelectedLineColor);
                            Prop.DragLineStyle = Cognex_Helper.GetLineStyleFromString(item.ROI_Property.DragLineStyle);
                            Prop.DragLineColor = Cognex_Helper.GetColorFromString(item.ROI_Property.DragLineColor);
                            Prop.DefaultScale = item.ROI_Property.DefaultScale;
                            CustomProperty Cp = new CustomProperty(item.Name.ToString(), Prop, true, "Appearance", "", true) { IsBrowsable = true };

                            PGE_ROIProp.Item.Add(Cp);
                            LstV_ROI.BeginUpdate();

                            ListViewItem Lvi = new ListViewItem(ROIName) { Name = "Name" };
                            //Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Data.JobName, Name = "JobName" });
                            //Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Data.Category, Name = "Category" });
                            Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rect.X.ToString(), Name = "X" });
                            Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rect.Y.ToString(), Name = "Y" });
                            Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rect.Width.ToString(), Name = "Width" });
                            Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rect.Height.ToString(), Name = "Height" });
                            Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Name = "Object", Text = Rect.ToString(), Tag = Rect });
                            LstV_ROI.Items.Add(Lvi);
                            //if (LstV_ROI.Groups[item.Category] == null)
                            //{
                            //    LstV_ROI.Groups.Add(new ListViewGroup(item.Category, item.Category));
                            //}
                            //LstV_ROI.Groups[item.Category].Items.Add(Lvi);
                            LstV_ROI.EndUpdate();
                            //RefeshROI(Rp, Rect, Systems.CurrentSelectedAreaName[CurFormIndex], Category, item.Name, true);
                        }
                    }
                    Cog_ROI_Display.DrawingEnabled = true;
                    PGE_ROIProp.Refresh();
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $"[ GUI ] {Name}_RefreshROIDataView Done", false, false);
                }
            }
            catch(Exception ex)
            {
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.CAUTION, "에러가 발생했습니다. 에러 로그를 확인해주세요.");
                Noti.ShowDialog();

                //Console.WriteLine(ex.Message);
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        public void ClearRecipeROI()
        {
            try
            {
                LstV_ROI.Items.Clear();
                LstV_ROI.Groups.Clear();
                Cog_ROI_Display.Image = null;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void InitControl()
        {
            try
            {
                //IniFile Ini = Systems.RecipeData_Collection[CurFormIndex]["ROI_Property.dat"];

                LstV_ROI.Columns.Add("Name", 90);
                //LstV_ROI.Columns.Add("JobName", 90);
                //LstV_ROI.Columns.Add("Category", 120);
                LstV_ROI.Columns.Add("X", 150);
                LstV_ROI.Columns.Add("Y", 150);
                LstV_ROI.Columns.Add("Width", 150);
                LstV_ROI.Columns.Add("Height", 150);
                LstV_ROI.Columns.Add("Object", 100);

                //foreach (ROI item in CurPattern.ROI_Coord)
                //{
                //    //LstB_ROI.Items.Add(item.Name.ToString());
                //    //LstB_Category.Items.Add(item.Name.ToString());
                //    ROI_Property Prop = new ROI_Property();
                //    Prop.Name = item.Name.ToString();
                //    Prop.LineColor = Cognex_Helper.GetColorFromString(item.ROI_Property.LineColor);
                //    Prop.LineStyle = Cognex_Helper.GetLineStyleFromString(item.ROI_Property.LineStyle);
                //    Prop.SelectedLineStyle = Cognex_Helper.GetLineStyleFromString(item.ROI_Property.SelectedLineStyle);
                //    Prop.SelectedLineColor = Cognex_Helper.GetColorFromString(item.ROI_Property.SelectedLineColor);
                //    Prop.DragLineStyle = Cognex_Helper.GetLineStyleFromString(item.ROI_Property.DragLineStyle);
                //    Prop.DragLineColor = Cognex_Helper.GetColorFromString(item.ROI_Property.DragLineColor);
                //    Prop.DefaultScale = item.ROI_Property.DefaultScale;
                //    CustomProperty Cp = new CustomProperty(item.Name.ToString(), Prop, true, "Appearance", "", true) { IsBrowsable = true };

                //    PGE_ROIProp.Item.Add(Cp);
                //    //ListViewGroup Group = new ListViewGroup(item.Name.ToString(), HorizontalAlignment.Left) { Header = item.Name.ToString(), Name = item.Name.ToString() };
                //    //LstV_ROI.Groups.Add(Group);
                //}
                //LstB_Category.Hide();
                LstV_ROI.Refresh();
                //PGE_ROIProp.Refresh();
                MenuItem[] mis = new MenuItem[4];
                MenuItem mi1 = new MenuItem("추가");
                MenuItem mi2 = new MenuItem("삭제");
                MenuItem mi3 = new MenuItem("복사");
                MenuItem mi4 = new MenuItem("이동");
                mis[0] = mi1;
                mis[1] = mi2;
                mis[2] = mi3;
                mis[3] = mi4;
                ContextMenu cm = new ContextMenu(mis);

                mi1.Click += Mi1_Click;
                mi2.Click += Mi2_Click;
                mi3.Click += Mi3_Click;
                mi4.Click += Mi4_Click;
                //PGE_ROIList.ContextMenu = cm;

                //if (LstB_ROI.Items.Count > 0)
                //    LstB_ROI.SelectedIndex = 0;
            }
            catch (Exception ex)
            {

                throw ex;
            }
        }
        public void SaveRecipe()
        {
            try
            {
                Recipe.ViewRecipe.SavePatterns();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void SaveROIList()
        {
            try
            {
                int TotalIndex = 0;
                Systems.RecipeData_Collection[Systems.CurDisplayIndex]["ROI.list"].Clear();
                //foreach(KeyValuePair<string, List<ROI_Data>> job_item in Systems.RecipeContent.ViewRecipe[CurFormIndex].ROI_List)
                //{
                //    for(int i = 0; i < job_item.Value.Count; ++i)
                //    {
                //        IniSection IniSec = new IniSection();
                //        IniSec.Add("Name", job_item.Value[i].Name);
                //        IniSec.Add("JobName", job_item.Value[i].JobName);
                //        IniSec.Add("Category", job_item.Value[i].Category);
                //        IniSec.Add("X", job_item.Value[i].X);
                //        IniSec.Add("Y", job_item.Value[i].Y);
                //        IniSec.Add("Width", job_item.Value[i].Width);
                //        IniSec.Add("Height", job_item.Value[i].Height);
                //        Systems.RecipeData_Collection[Systems.CurDisplayIndex]["ROI.list"].Add(TotalIndex.ToString(), IniSec);
                //        ++TotalIndex;
                //    }                    
                //}

                Systems.RecipeData_Collection[Systems.CurDisplayIndex]["ROI.list"].Save(Systems.RecipeData_Collection[Systems.CurDisplayIndex]["ROI.list"].GetIniPath());
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $"[ GUI ] {Name}_SaveROIList Done", false, false);
            }
            catch (Exception ex)
            {
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.CAUTION, "에러가 발생했습니다. 에러 로그를 확인해주세요.");
                Noti.ShowDialog();

                Console.WriteLine(ex.Message);
                //Systems.LogWriter.Error($@"{this.Name}, Exception : {ex}");
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] ROI Save Fail, Exception Message : {ex.Message}", true, false);
                throw ex;
            }
            //Ini.Save(Systems.Ini_Collection[Systems.CurDisplayIndex]["ROI_Property.dat"].GetIniPath(), System.IO.FileMode.Create);
        }

        public void SaveROIProperty()
        {
            try
            {
                CustomPropertyCollection Props = PGE_ROIProp.Item;
                IniFile Ini = Systems.RecipeData_Collection[Systems.CurDisplayIndex]["ROI_Property.dat"];
                Ini.Clear();
                foreach (CustomProperty item in Props)
                {
                    IniSection Section = new IniSection();
                    Section.Add("Name", (item.Value as ROI_Property).Name);
                    Section.Add("LineColor", ((int)(item.Value as ROI_Property).LineColor).ToString());
                    Section.Add("LineStyle", ((int)(item.Value as ROI_Property).LineStyle).ToString());
                    Section.Add("SelectedLineStyle", ((int)(item.Value as ROI_Property).SelectedLineStyle).ToString());
                    Section.Add("SelectedColor", ((int)(item.Value as ROI_Property).SelectedLineColor).ToString());
                    Section.Add("DragLineStyle", ((int)(item.Value as ROI_Property).DragLineStyle).ToString());
                    Section.Add("DragLineColor", ((int)(item.Value as ROI_Property).DragLineColor).ToString());
                    Section.Add("DefaultScale", (item.Value as ROI_Property).DefaultScale);
                    Section.Add("Description", (item.Value as ROI_Property).Description);
                    Ini.Add((item.Value as ROI_Property).Name, Section);
                }
                Ini.Save(Systems.RecipeData_Collection[Systems.CurDisplayIndex]["ROI_Property.dat"].GetIniPath(), System.IO.FileMode.Create);
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $"[ GUI ] {Name}_SaveROIProperty Done", false, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }
        private void Mi4_Click(object sender, EventArgs e)
        {

        }

        private void Mi3_Click(object sender, EventArgs e)
        {

        }

        private void Mi2_Click(object sender, EventArgs e)
        {
            //var T = PGE_ROIList.SelectedGridItem;
            //CustomPropertyCollection T2 = (PGE_ROIList.SelectedObject as CustomPropertyCollection);
        }

        private void Mi1_Click(object sender, EventArgs e)
        {

        }

        private void Cog_ROI_Display_Click(object sender, EventArgs e)
        {
            try
            {
                //#region Create New ROI
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + "Display Click", true, false);
                if (AltIsDown)
                {
                    Ex_Frm_Others_New_Input Input = new Ex_Frm_Others_New_Input("입력", LstV_ROI.Items);
                    Input.ShowDialog();
                    if (Input.DialogResult == DialogResult.Cancel || Input.ResultName == null)
                    {
                        AltIsDown = false;
                        return;
                    }

                    string ROI_Name = Input.ResultName;

                    CogRectangle Rect = new CogRectangle();

                    ROI_PropertyData Rp = new ROI_PropertyData();

                    Rect.X = (Cog_ROI_Display.Image.Width / 2) - Cog_ROI_Display.PanX;
                    Rect.Y = (Cog_ROI_Display.Image.Height / 2) - Cog_ROI_Display.PanY;

                    Rect.Width = (Cog_ROI_Display.DisplayRectangle.Width / Cog_ROI_Display.Zoom * Rp.DefaultScale);
                    Rect.Height = (Cog_ROI_Display.DisplayRectangle.Height / Cog_ROI_Display.Zoom * Rp.DefaultScale);
                    Rect.X -= (Rect.Width / 2);
                    Rect.Y -= (Rect.Height / 2);

                    Rect.SelectedLineStyle = Cognex_Helper.GetLineStyleFromString(Rp.SelectedLineStyle);
                    Rect.SelectedColor = Cognex_Helper.GetColorFromString(Rp.SelectedLineColor);
                    Rect.LineStyle = Cognex_Helper.GetLineStyleFromString(Rp.LineStyle);
                    Rect.DragLineStyle = Cognex_Helper.GetLineStyleFromString(Rp.DragLineStyle);
                    Rect.DragColor = Cognex_Helper.GetColorFromString(Rp.DragLineColor);
                    Rect.Color = Cognex_Helper.GetColorFromString(Rp.LineColor);

                    Rect.Interactive = true;
                    Rect.Dragging += new CogDraggingEventHandler(MRect_Dragging);
                    Rect.DraggingStopped += new CogDraggingStoppedEventHandler(MRect_DraggingStopped);
                    Rect.GraphicDOFEnable = CogRectangleDOFConstants.All;

                    string ROIName = $"{ROI_Name}";

                    ROI NewROI = new ROI();
                    NewROI.Coord = new Coordinate() { X = Rect.X, Y = Rect.Y, Width = Rect.Width, Height = Rect.Height };
                    NewROI.Algo_List = new List<Algorithm>();
                    NewROI.Name = ROI_Name;
                    NewROI.ROI_Property = Rp;

                    CurPattern.ROI_Data.Add(NewROI);

                    ROI_Property Prop = new ROI_Property();
                    Prop.Name = ROIName;
                    Prop.LineColor = Cognex_Helper.GetColorFromString(Rp.LineColor);
                    Prop.LineStyle = Cognex_Helper.GetLineStyleFromString(Rp.LineStyle);
                    Prop.SelectedLineStyle = Cognex_Helper.GetLineStyleFromString(Rp.SelectedLineStyle);
                    Prop.SelectedLineColor = Cognex_Helper.GetColorFromString(Rp.SelectedLineColor);
                    Prop.DragLineStyle = Cognex_Helper.GetLineStyleFromString(Rp.DragLineStyle);
                    Prop.DragLineColor = Cognex_Helper.GetColorFromString(Rp.DragLineColor);
                    Prop.DefaultScale = Rp.DefaultScale;
                    CustomProperty Cp = new CustomProperty(ROIName, Prop, true, "Appearance", "", true) { IsBrowsable = true };

                    PGE_ROIProp.Item.Add(Cp);
                    PGE_ROIProp.Refresh();

                    RefeshRoiDataView();
                    AltIsDown = false;

                }
                else if (Cog_ROI_Display.Selection.Count > 0)
                {
                    CogRectangle Rect = Cog_ROI_Display.Selection[0] as CogRectangle;
                    foreach (ListViewItem item in LstV_ROI.Items)
                    {
                        if (item.SubItems["Object"].Tag as CogRectangle == Rect)
                        {
                            item.Selected = true;
                        }
                    }
                    LstV_ROI.Invalidate();
                    LstV_ROI.Focus();
                }
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $"[ GUI ] {Name}_ROI_Display Click Done", false, false);
            }
            catch (Exception ex)
            {
                Cog_ROI_Display.DrawingEnabled = true;
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.CAUTION, "에러가 발생했습니다. 에러 로그를 확인해주세요.");
                Noti.ShowDialog();

                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        public void ConvertListFromListView()
        {
            //Dictionary<string, List<ROI_Data>> Data = Systems.MainRecipe.ROI_List;
        }
       
        private void Cog_ROI_Display_MouseUp(object sender, MouseEventArgs e)
        {

        }

        private void Cog_ROI_Display_MouseMove(object sender, MouseEventArgs e)
        {

        }

        private void Cog_ROI_Display_MouseDown(object sender, MouseEventArgs e)
        {
            LeftMouseDown = e.Button == MouseButtons.Left ? true : false;

        }
        private void MRect_DraggingStopped(object sender, CogDraggingEventArgs e)
        {
            try
            {
                MRect_Dragging(sender, e);
                //LstV_ROI.EndUpdate();
                LstV_ROI.Refresh();
                Display_Draging = false;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void MRect_Dragging(object sender, CogDraggingEventArgs e)
        {
            try
            {
                CogRectangle dragRect = (CogRectangle)e.DragGraphic;
                CogRectangle Sel = Cog_ROI_Display.Selection[0] as CogRectangle;
                CogRectangle FindSelRect = null;
                if (Display_Draging == false)
                {
                    //LstV_ROI.BeginUpdate();
                    Display_Draging = true;
                }

                for (int i = 0; i < Cog_ROI_Display.InteractiveGraphics.Count; ++i)
                {
                    if (Cog_ROI_Display.InteractiveGraphics[i].Selected == true)
                        FindSelRect = (Cog_ROI_Display.InteractiveGraphics[i]) as CogRectangle;

                }

                foreach (ListViewItem item in LstV_ROI.Items)
                {
                    if ((item.SubItems["Object"].Tag as CogRectangle) == FindSelRect)
                    {
                        ROI FindItem = CurPattern.ROI_Data?.Find(x => x.Name == item.Text);
                        FindItem.Coord.X = dragRect.X;
                        FindItem.Coord.Y = dragRect.Y;
                        FindItem.Coord.Width = dragRect.Width;
                        FindItem.Coord.Height = dragRect.Height;
                        item.SubItems["X"].Text = dragRect.X.ToString();
                        item.SubItems["Y"].Text = dragRect.Y.ToString();
                        item.SubItems["Width"].Text = dragRect.Width.ToString();
                        item.SubItems["Height"].Text = dragRect.Height.ToString();

                    }
                }
                //RefeshRoiDataView();
                LstV_ROI.Invalidate();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }
        public void FormInitialize(int no)
        {

        }
        private void Ex_Frm_Recipe_ROI_Load(object sender, EventArgs e)
        {
            try
            {
                //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
                LstV_ROI.DoubleBuffered(true);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }
        private void Ex_Frm_Recipe_ROI_Shown(object sender, EventArgs e)
        {
            try
            {
                Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 5, 5));
                ////WinApis.SetWindowRgn(Btn_Manage.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Manage.Width, Btn_Manage.Height, 15, 15), true);
                ////WinApis.SetWindowRgn(Btn_Login.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Login.Width, Btn_Login.Height, 15, 15), true);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Btn_ROI_Add_Click(object sender, EventArgs e)
        {
            //string name = Tb_ROI_Name.Text.ToString();
            //if (name == "")
            //{
            //    Ex_Frm_Notification_Announce Frm_Announcement = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "ROI 이름이 입력되지 않았습니다.");
            //    Frm_Announcement.ShowDialog();
            //    return;
            //}

            //if (FindOverlapROIName(name))
            //{
            //    Ex_Frm_Notification_Announce Frm_Announcement = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "ROI 이름이 중복입니다.");
            //    Frm_Announcement.ShowDialog();
            //}
            //else
            //{
            //    LstB_ROI.Items.Add(name);
            //    LstB_ROI.SelectedItem = name;
            //    ROI_Property Prop = new ROI_Property();
            //    Prop.Name = name;
            //    CustomProperty Cp = new CustomProperty(name, Prop, true, "Appearance", name, true) { IsBrowsable = true };

            //    PGE_ROIProp.Item.Add(Cp);

            //    if (LstV_ROI.Groups[name] == null)
            //        LstV_ROI.Groups.Add(new ListViewGroup(name, HorizontalAlignment.Left) { Header = name, Name = name });

            //    PGE_ROIProp.Refresh();
            //}
        }

        private void Btn_ROI_Del_Click(object sender, EventArgs e)
        {

            //string name = LstB_ROI.SelectedItem as string;
            //if (name == "")
            //    return;
            //if (LstV_ROI.Groups[name] != null)
            //{
            //    Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "ROI 데이터가 존재합니다.\n ROI 데이터와 함께 삭제됩니다.");
            //    LstV_ROI.BeginUpdate();
            //    if (LstV_ROI.Groups[name].Items.Count > 0)
            //    {
            //        Noti.ShowDialog();

            //        if (Noti.DialogResult == DialogResult.OK)
            //        {
            //            foreach (ListViewItem item in LstV_ROI.Items)
            //            {
            //                if (item.Group.Name == name)
            //                {
            //                    string RectName = $"{name}^{item.Text}";
            //                    Cog_ROI_Display.InteractiveGraphics.Remove(RectName);
            //                    LstV_ROI.Items.Remove(item);
            //                }
            //            }
            //        }
            //    }
            //    LstV_ROI.Groups.Remove(LstV_ROI.Groups[name]);
            //    LstV_ROI.EndUpdate();
            //    LstV_ROI.Invalidate();
            //}
            //LstB_ROI.Items.Remove(name);
        }
        //private bool FindOverlapROIName(string name)
        //{
        //    //var Data = LstB_ROI.Items;
        //    //foreach (var item in Data)
        //    //{
        //    //    if ((item as string) == name)
        //    //        return true;
        //    //}
        //    //return false;
        //}

        private void LstB_ROI_SelectedIndexChanged(object sender, EventArgs e)
        {
            //Tb_ROI_Name.Text = (sender as ListBox).SelectedItem as string;
        }

        private void Btn_ImageLoad_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
                OpenFileDialog Ofd = new OpenFileDialog();
                Ofd.DefaultExt = "bmp";
                Ofd.Filter = "BMP File(*.bmp)|*bmp|PNG File(*.png)|*png";
                Ofd.ShowDialog();
                if (Ofd.FileName.Length > 0)
                {
                    var Temp = Cognex_Helper.Load_Image(Ofd.FileName);
                    Cog_ROI_Display.DrawingEnabled = false;
                    Cog_ROI_Display.Image = Temp;
                    Cog_ROI_Display.Fit(true);
                    Cog_ROI_Display.DrawingEnabled = true;
                    //Systems.ClearRecipe();
                    RefeshRoiDataView();
                    //SetRecipeROI();          
                    //Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.SelectRecipe(Systems.CurrentApplyRecipeName[Systems.CurDisplayIndex].GetString());
                    //Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.SelectJob(Systems.CurrentApplyRecipeName[Systems.CurDisplayIndex].GetString());
                    //Systems.CurrentApplyRecipeName[CurFormIndex].SetString(

                }
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $"[ GUI ] {Name}_Load Image Done", false, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }
        private string CurrentRecipe_;
        public string CurrentRecipe
        {
            get
            { return CurrentRecipe_; }
            set
            {
                try
                {
                    CurrentRecipe_ = value;
                    Program.Frm_Main?.SetRunModelName(CurrentRecipe_);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                    throw ex;
                }
            }
        }
        public void Rund()
        {
      
        }
        public void SaveROIData()
        {
            try
            {
                SaveROIProperty();
                SaveROIList();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void PGE_ROIProp_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            try
            {
                PropertyGridEx PGEx = s as PropertyGridEx;
                GridItem SelectedItem = PGEx.SelectedGridItem;
                string ROIName = SelectedItem.Parent.Label;

                ROI_PropertyData CurROIProp = CurPattern?.ROI_Data?.Find(x => x.Name == ROIName)?.ROI_Property;
                CurROIProp.LineColor = (SelectedItem.Parent.Value as ROI_Property).LineColor.ToString();
                CurROIProp.LineStyle = (SelectedItem.Parent.Value as ROI_Property).LineStyle.ToString();
                CurROIProp.SelectedLineColor = (SelectedItem.Parent.Value as ROI_Property).SelectedLineColor.ToString();
                CurROIProp.SelectedLineStyle = (SelectedItem.Parent.Value as ROI_Property).SelectedLineStyle.ToString();
                CurROIProp.DragLineColor = (SelectedItem.Parent.Value as ROI_Property).DragLineColor.ToString();
                CurROIProp.DragLineStyle = (SelectedItem.Parent.Value as ROI_Property).DragLineStyle.ToString();

                RefeshRoiDataView();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Ex_Frm_Recipe_ROI_KeyDown(object sender, KeyEventArgs e)
        {
            AltIsDown = e.Alt;
        }

        private void Ex_Frm_Recipe_ROI_KeyUp(object sender, KeyEventArgs e)
        {
            AltIsDown = e.Alt;
        }
        private void LstV_ROI_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            try
            {
                //InputBox.Show();
                // Name 컬럼의 데이터 더블클릭 시 텍스트 박스 범위 수정 필요
                curItem = LstV_ROI.GetItemAt(e.X, e.Y);

                if (curItem == null)
                    return;
                curSB = curItem.GetSubItemAt(e.X, e.Y);
                Tip.IsBalloon = true;
                Tip.ToolTipTitle = "Tip";
                int idxSub = curItem.SubItems.IndexOf(curSB);

                int lLeft = curSB.Bounds.Left + 2;
                int lWidth = curSB.Bounds.Width;
                Tip.InitialDelay = 200;
                switch (idxSub)
                {
                    case 0: // Name

                        if (Tip.GetToolTip(InputBox) != "")
                        {
                            Tip.RemoveAll();
                        }

                        Tip.SetToolTip(InputBox, "공백과 '^' 문자는 포함할 수 없습니다.");
                        InputBox.SetBounds(lLeft, curSB.Bounds.Top + LstV_ROI.Top, lWidth, curSB.Bounds.Height);
                        InputBox.Text = curSB.Text;
                        InputBox.Show();
                        InputBox.Focus();
                        break;
                    //case 2: // Category
                    //    if (Tip.GetToolTip(InputBox) != "")
                    //        Tip.RemoveAll();
                    //    LstB_Category.BringToFront();
                    //    //LstB_Category.SetBounds(lLeft, curSB.Bounds.Top + LstV_ROI.Top, lWidth, curSB.Bounds.Height);
                    //    LstB_Category.Location = new Point(lLeft, curSB.Bounds.Top + LstV_ROI.Top);
                    //    LstB_Category.Size = new Size(lWidth, 30);
                    //    LstB_Category.SelectedItem = curSB.Text;
                    //    LstB_Category.Show();
                    //    LstB_Category.Focus();
                    //    Tip.SetToolTip(LstB_Category, "카테고리를 변경합니다.");
                    //    break;
                    case 1: // X
                    case 2: // Y
                    case 3: // W
                    case 4: // H
                        if (Tip.GetToolTip(InputBox) != "")
                            Tip.RemoveAll();
                        Tip.SetToolTip(InputBox, "숫자만 입력해주세요.");
                        InputBox.SetBounds(lLeft, curSB.Bounds.Top + LstV_ROI.Top, lWidth, curSB.Bounds.Height);
                        InputBox.Text = curSB.Text;
                        InputBox.Show();
                        InputBox.Focus();
                        break;
                    default:
                        return;
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void InputBox_KeyDown(object sender, KeyEventArgs e)
        {
            // 엔터키 수정 ESC키 취소

            try
            {
                switch (e.KeyCode)
                {
                    case System.Windows.Forms.Keys.Enter:
                        cancelEdit = false;
                        e.Handled = true;
                        Tip.RemoveAll();

                        InputBox.Hide();
                        break;

                    case System.Windows.Forms.Keys.Escape:
                        cancelEdit = true;
                        e.Handled = true;
                        InputBox.Hide();
                        break;
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void InputBox_Leave(object sender, EventArgs e)
        {
            try
            {
                if (InputBox.Visible)
                {
                    if (cancelEdit == false)
                    {
                        if (InputBox.Text.Trim() != "")
                        {
                            if (LstV_ROI.SelectedItems.Count > 0)
                            {
                                ListViewItem Item = LstV_ROI.SelectedItems[0];

                                string Col = curSB.Name;
                                double Num = 0;
                                bool CoordChange = false;
                                bool NameChange = false;
                                string ObjectName = string.Empty;
                                string NewObjectName = string.Empty;

                                ROI OriginItem = new ROI();
                                ROI FindItem = new ROI();

                                OriginItem = CurPattern.ROI_Data.Find(x => x.Name == curItem.SubItems["Name"].Text);

                                if (Col == "Name")
                                {
                                    if (InputBox.Text.Contains("^") || InputBox.Text.Contains(" "))
                                    {
                                        NameChange = false;
                                        Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "이름을 확인해주세요.");
                                        Noti.ShowDialog();
                                    }
                                    else
                                    {
                                        NameChange = true;

                                        FindItem = CurPattern.ROI_Data.Find(x => x.Name == InputBox.Text);
                                        if (curSB.Text == InputBox.Text)
                                        {
                                            InputBox.Hide();
                                            LstV_ROI.Focus();
                                            return;
                                        }
                                        if (FindItem != null)
                                        {
                                            Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "동일한 이름이 존재합니다.");
                                            Noti.ShowDialog();
                                            InputBox.Hide();
                                            LstV_ROI.Focus();
                                            return;
                                        }

                                        ObjectName = $"{curItem.SubItems["Name"].Text}";
                                        curSB.Text = InputBox.Text;
                                        NewObjectName = $"{InputBox.Text}";

                                        if (ObjectName == NewObjectName)
                                        {
                                            InputBox.Hide();
                                            LstV_ROI.Focus();
                                            return;
                                        }
                                    }
                                }
                                else if (Col == "JobName")
                                {

                                }
                                else if (Col == "Category")
                                {
                                    #region Hide
                                    //if (InputBox.Text.Contains("^") || InputBox.Text.Contains(" "))
                                    //{
                                    //    NameChange = false;
                                    //    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "이름을 확인해주세요.");
                                    //    Noti.ShowDialog();
                                    //}
                                    //else
                                    //{
                                    //    NameChange = true;

                                    //    Rd.Name = curItem.SubItems["Name"].Text;
                                    //    Rd.Category = InputBox.Text;
                                    //    if (curSB.Text == InputBox.Text)
                                    //    {
                                    //        InputBox.Hide();
                                    //        LstV_ROI.Focus();
                                    //        return;
                                    //    }
                                    //    foreach (ListViewItem item in LstV_ROI.Items)
                                    //    {
                                    //        if (item.Group.Name == Rd.Category)
                                    //        {
                                    //            if (item.Text == Rd.Name)
                                    //            {
                                    //                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "동일한 카테고리가 존재합니다.");
                                    //                Noti.ShowDialog();
                                    //                InputBox.Hide();
                                    //                LstV_ROI.Focus();
                                    //                return;
                                    //            }
                                    //        }
                                    //    }
                                    //    ObjectName = $"{curItem.Group.Name}^{curItem.SubItems["Name"].Text}";
                                    //    curSB.Text = InputBox.Text;
                                    //    NewObjectName = $"{InputBox.Text}^{curItem.SubItems["Name"].Text}";

                                    //    if (ObjectName == NewObjectName)
                                    //    {
                                    //        InputBox.Hide();
                                    //        LstV_ROI.Focus();
                                    //        return;
                                    //    }
                                    //}
                                    #endregion
                                }
                                else if (Col == "X")
                                {
                                    CoordChange = double.TryParse(InputBox.Text, out Num);
                                    if (CoordChange)
                                    {
                                        OriginItem = CurPattern.ROI_Data.Find(x => x.Name == curItem.SubItems["Name"].Text);
                                        OriginItem.Coord.X = Num;
                                    }
                                }
                                else if (Col == "Y")
                                {
                                    CoordChange = double.TryParse(InputBox.Text, out Num);
                                    if (CoordChange)
                                    {
                                        OriginItem = CurPattern.ROI_Data.Find(x => x.Name == curItem.SubItems["Name"].Text);
                                        OriginItem.Coord.Y = Num;
                                    }
                                }
                                else if (Col == "Width")
                                {
                                    CoordChange = double.TryParse(InputBox.Text, out Num);
                                    if (CoordChange)
                                    {
                                        OriginItem = CurPattern.ROI_Data.Find(x => x.Name == curItem.SubItems["Name"].Text);
                                        OriginItem.Coord.Width = Num;
                                    }
                                }
                                else if (Col == "Height")
                                {
                                    CoordChange = double.TryParse(InputBox.Text, out Num);
                                    if (CoordChange)
                                    {
                                        OriginItem = CurPattern.ROI_Data.Find(x => x.Name == curItem.SubItems["Name"].Text);
                                        OriginItem.Coord.Height = Num;
                                    }
                                }
                                if (CoordChange)
                                {
                                    RefeshRoiDataView();
                                }
                                else if (NameChange)
                                {
                                    OriginItem.Name = InputBox.Text;
                                    RefeshRoiDataView();
                                }
                            }
                        }
                        else
                        {
                            cancelEdit = false;
                        }
                    }
                }
                InputBox.Hide();
                LstV_ROI.Focus();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void LstV_ROI_DrawSubItem(object sender, DrawListViewSubItemEventArgs e)
        {
            try
            {
                if (this.LstV_ROI.SelectedIndices.Contains(e.ItemIndex))
                {
                    e.SubItem.BackColor = Color.Black;
                    e.SubItem.ForeColor = Color.White;
                }
                else
                {
                    e.SubItem.BackColor = Color.White;
                    e.SubItem.ForeColor = Color.Black;
                }

                e.DrawBackground();

                e.DrawText();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void LstV_ROI_DrawItem(object sender, DrawListViewItemEventArgs e)
        {
            try
            {
                if (this.LstV_ROI.SelectedIndices.Contains(e.ItemIndex))
                {
                    e.Item.BackColor = Color.Black;
                    e.Item.ForeColor = Color.White;
                }
                else
                {
                    e.Item.BackColor = Color.White;
                    e.Item.ForeColor = Color.Black;
                }
                e.DrawBackground();

                e.DrawText();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void LstV_ROI_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                ListView Obj = sender as ListView;
                if (Obj.SelectedItems.Count > 0)
                {
                    ListViewItem Item = Obj.SelectedItems[0];
                    (Item.SubItems["Object"].Tag as CogRectangle).Selected = true;
                }

            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void LstV_ROI_MouseClick(object sender, MouseEventArgs e)
        {
            //오른쪽 클릭일 경우
            try
            {
                if (e.Button.Equals(MouseButtons.Right))
                {
                    ContextMenu m = new ContextMenu();

                    MenuItem m1 = new MenuItem();
                    MenuItem m2 = new MenuItem();

                    m1.Text = "복사";
                    m2.Text = "삭제";

                    m1.Click += (senders, es) =>
                    {
                        Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $"[ GUI ] {Name}_ROI 복사 Done", false, false);
                    };
                    m2.Click += (senders, es) =>
                    {
                        ListViewItem Item = LstV_ROI.SelectedItems[0];
                        string Name = Item.SubItems["Name"].Text;


                        var FindItem = CurPattern.ROI_Data.Find(x => x.Name == Name);
                        if (FindItem != null)
                        {
                            CurPattern.ROI_Data.Remove(FindItem);
                            RefeshRoiDataView();
                        }
                        Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $"[ GUI ] {Name}_ROI 삭제 Done", false, false);
                    };

                    m.MenuItems.Add(m1);
                    m.MenuItems.Add(m2);

                    m.Show(LstV_ROI, new Point(e.X, e.Y));
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void InputBox_KeyPress(object sender, KeyPressEventArgs e)
        {

        }

        private void LstB_Category_Leave(object sender, EventArgs e)
        {
            //if (LstB_Category.Visible)
            //{
            //    if (cancelEdit == false)
            //    {
            //        if (LstB_Category.Text.Trim() != "")
            //        {
            //            if (LstV_ROI.SelectedItems.Count > 0)
            //            {
            //                ListViewItem Item = LstV_ROI.SelectedItems[0];

            //                string Col = curSB.Name;
            //                bool CoordChange = false;
            //                bool NameChange = false;
            //                string ObjectName = string.Empty;
            //                string NewObjectName = string.Empty;

            //                string SelectedJobName = Program.Frm_MainContent_[CurFormIndex].Frm_Recipe.GetSelectedJob();
            //                ROI_Data OriginItem = new ROI_Data();
            //                ROI_Data FindItem = new ROI_Data();
            //                string Category = string.Empty;
            //                OriginItem = Systems.RecipeContent.ViewRecipe[CurFormIndex].ROI_List[SelectedJobName].Find(x => x.Name == curItem.SubItems["Name"].Text && x.Category == curItem.SubItems["Category"].Text);

            //                if (Col == "Name")
            //                {

            //                }
            //                else if (Col == "Category")
            //                {

            //                    if (InputBox.Text.Contains("^") || InputBox.Text.Contains(" "))
            //                    {
            //                        NameChange = false;
            //                        Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "이름을 확인해주세요.");
            //                        Noti.ShowDialog();
            //                    }
            //                    else
            //                    {
            //                        NameChange = true;
            //                        Category = LstB_Category.SelectedItem as string;
            //                        if (Category != null)
            //                        {
            //                            FindItem = Systems.RecipeContent.ViewRecipe[CurFormIndex].ROI_List[SelectedJobName].Find(x => x.Name == InputBox.Text && x.Category == curItem.SubItems["Category"].Text);
            //                            if (curSB.Text == Category)
            //                            {
            //                                LstB_Category.Hide();
            //                                LstV_ROI.Focus();
            //                                return;
            //                            }
            //                            if (FindItem != null)
            //                            {
            //                                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "동일한 카테고리가 존재합니다.");
            //                                Noti.ShowDialog();
            //                                LstB_Category.Hide();
            //                                LstV_ROI.Focus();
            //                                return;
            //                            }

            //                            ObjectName = $"{curItem.Group.Name}^{curItem.SubItems["Name"].Text}";
            //                            curSB.Text = Category;
            //                            NewObjectName = $"{Category}^{curItem.SubItems["Name"].Text}";

            //                            if (ObjectName == NewObjectName)
            //                            {
            //                                LstB_Category.Hide();
            //                                LstV_ROI.Focus();
            //                                return;
            //                            }
            //                        }
            //                    }                              
            //                }
            //                else if (Col == "X")
            //                {

            //                }
            //                else if (Col == "Y")
            //                {

            //                }
            //                else if (Col == "Width")
            //                {

            //                }
            //                else if (Col == "Height")
            //                {

            //                }
            //                if (CoordChange)
            //                {

            //                }
            //                else if (NameChange)
            //                {
            //                    OriginItem.Category = Category;
            //                    RefeshRoiDataView();
            //                }
            //            }
            //        }
            //        else
            //        {
            //            cancelEdit = false;
            //        }
            //    }
            //}
            //LstB_Category.Hide();
            //LstV_ROI.Focus();
        }

        private void LstB_Category_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void Btn_Close_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
                //DialogResult = DialogResult.No;
                OriginPattern.ROI_Data = CurPattern.ROI_Data;
                Close();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Btn_Apply_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);

                OriginPattern.ROI_Data = CurPattern.ROI_Data;
                DialogResult = DialogResult.OK;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Lb_ROIManage_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Lb_ROIManage_MouseMove(object sender, MouseEventArgs e)
        {
            try
            {
                if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
                {
                    Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Btn_AutoFit_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
                CogImage8Grey CogImage = Cog_ROI_Display.Image as CogImage8Grey;
                SelectedListViewItemCollection SelItem = LstV_ROI.SelectedItems;
                if (SelItem.Count > 0)
                {
                    string ROIName = SelItem[0].Text;
                    CogRectangle Rect = SelItem[0].SubItems["Object"].Tag as CogRectangle;
                    Cog_ROI_Display.DrawingEnabled = false;
                    Rect.X = 0;
                    Rect.Y = 0;
                    Rect.Width = CogImage.Width;
                    Rect.Height = CogImage.Height;
                    Cog_ROI_Display.DrawingEnabled = true;

                    //int ROIIdx = Recipe.ViewRecipe.Patterns_Data.Pattern.FindIndex(x => x.Name == ROIName);

                    ROI Temp = CurPattern.ROI_Data.Find(x => x.Name == ROIName);
                    Temp.Coord.X = Rect.X;
                    Temp.Coord.Y = Rect.Y;
                    Temp.Coord.Width = Rect.Width;
                    Temp.Coord.Height = Rect.Height;

                    RefeshRoiDataView();
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $"[ GUI ] {Name}_AutoFit Done", false, false);
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Ex_Frm_Recipe_ROI_Paint(object sender, PaintEventArgs e)
        {
            try
            {
                Rectangle borderRectangle = this.ClientRectangle;
                borderRectangle.Inflate(-3, -3);
                ControlPaint.DrawBorder3D(e.Graphics, borderRectangle,
                    Border3DStyle.Etched);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }

        private void PGE_ROIProp_Click(object sender, EventArgs e)
        {
      
        }

        private void PGE_ROIProp_MouseClick(object sender, MouseEventArgs e)
        {
     
        }
    }
}
