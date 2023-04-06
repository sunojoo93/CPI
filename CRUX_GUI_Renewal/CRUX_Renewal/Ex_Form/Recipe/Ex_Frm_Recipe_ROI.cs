using Cognex.VisionPro;
using Cognex.VisionPro.Display;
using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.User_Controls;
using CRUX_Renewal.Utils;
using PropertyGridEx;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Recipe_ROI : Form
    {
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]

        Label m_lbImageState = new Label();
        bool _shiftIsDown = false;
        public Ex_Frm_Recipe_ROI()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();
            InitPGE();
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

            //mRect2 = new CogRectangle();
            //mRect2.Dragging += new CogDraggingEventHandler(MRect2_Dragging);
            //mRect2.DraggingStopped += new CogDraggingStoppedEventHandler(MRect2_DraggingStopped);

            //mRect2.Interactive = true;
            //mRect2.GraphicDOFEnable = CogRectangleDOFConstants.All;
            //Cog_ROI_Display.InteractiveGraphics.Add(mRect2, "Rect", true);

        }
        private void InitPGE()
        {
            IniFile Ini = new IniFile();
            Ini.Load($@"{Paths.ROI_PROPERTY}ROI_Property.dat");

            foreach (var inner_item in Ini.Values)
            {
                LstB_ROI.Items.Add(inner_item["Name"].ToString());
                ROI_Property Prop = new ROI_Property();
                Prop.Name = inner_item["Name"].ToString();
                Prop.LineColor = inner_item["LineColor"].ToInt();
                Prop.LineStyle = inner_item["LineStyle"].ToInt();
                Prop.SelectedLineStyle = inner_item["SelectedLineStyle"].ToInt();
                Prop.SelectedLineColor = inner_item["SelectedColor"].ToInt();
                Prop.DragLineStyle = inner_item["DragLineStyle"].ToInt();
                Prop.DragLineColor = inner_item["DragLineColor"].ToInt();
                Prop.DefaultScale = inner_item["DefaultScale"].ToDouble();

                CustomProperty Cp = new CustomProperty("Appearance", Prop, true, inner_item["Name"].ToString(), inner_item["Description"].ToString(), true) { IsBrowsable = true };

                PGE_ROIProp.Item.Add(Cp);
            }

            PGE_ROIProp.Refresh();
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
            PGE_ROIList.ContextMenu = cm;
           
        }
        public void SaveROIProperty()
        {
            CustomPropertyCollection Props = PGE_ROIProp.Item;
            IniFile Ini = Systems.Ini_Collection[Systems.CurDisplayIndex]["ROI_Property.dat"];
            Ini.Clear();
            foreach(CustomProperty item in Props)
            {
                FieldInfo[] Field = typeof(ROI_Property).GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
                int i = 0;
                IniSection Section = new IniSection();
                Section.Add("Name", (item.Value as ROI_Property).Name);
                Section.Add("LineColor", (item.Value as ROI_Property).LineColor);
                Section.Add("LineStyle", (item.Value as ROI_Property).LineStyle);
                Section.Add("SelectedLineStyle", (item.Value as ROI_Property).SelectedLineStyle);
                Section.Add("SelectedColor", (item.Value as ROI_Property).SelectedLineColor.ToString());
                Section.Add("DragLineStyle", (item.Value as ROI_Property).DragLineStyle);
                Section.Add("DragLineColor", (item.Value as ROI_Property).DragLineColor);
                Section.Add("DefaultScale", (item.Value as ROI_Property).DefaultScale);
                Section.Add("Description", (item.Value as ROI_Property).Description);
                Ini.Add(item.Category, Section);
            }
            Ini.Save(Systems.Ini_Collection[Systems.CurDisplayIndex]["ROI_Property.dat"].GetIniPath(), System.IO.FileMode.Create);
        }
        private void Mi4_Click(object sender, EventArgs e)
        {
      
        }

        private void Mi3_Click(object sender, EventArgs e)
        {
        
        }

        private void Mi2_Click(object sender, EventArgs e)
        {
            
            var T = PGE_ROIList.SelectedGridItem;
            CustomPropertyCollection T2 = (PGE_ROIList.SelectedObject as CustomPropertyCollection);

       
 
        }

        private void Mi1_Click(object sender, EventArgs e)
        {
           
        }

        private void Cog_ROI_Display_Click(object sender, EventArgs e)
        {
            try
            {
                if (_shiftIsDown)
                {
                    string SelectedROICategory = LstB_ROI.SelectedItem as string;
                    if (SelectedROICategory == null)
                    {
                        Ex_Frm_Notification_Announce Ano = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "선택된 ROI가 없습니다.");
                        Ano.ShowDialog();
                        return;
                    }     
                    Ex_Frm_Others_New_Input Input = new Ex_Frm_Others_New_Input("입력", PGE_ROIList.SelectedObjects[0], SelectedROICategory);
                    Input.ShowDialog();
                    if(Input.DialogResult == DialogResult.Cancel || Input.ResultName == null)
                    {
                        return;
                    }            

                    CustomPropertyCollection TT = PGE_ROIList.SelectedObjects[0] as CustomPropertyCollection;

                    string ROI_Name = Input.ResultName;
  
                    
                    CogRectangle Rect = new CogRectangle();
                    var ROIProp = PGE_ROIProp.Item.FindCategory(SelectedROICategory);
                    ROI_Property Rp = (ROIProp.Value as ROI_Property);


                    Rect.X = (Cog_ROI_Display.Image.Width / 2) - Cog_ROI_Display.PanX;
                    Rect.Y = (Cog_ROI_Display.Image.Height / 2) - Cog_ROI_Display.PanY;

                    Rect.Width = (Cog_ROI_Display.DisplayRectangle.Width / Cog_ROI_Display.Zoom * Rp.DefaultScale);
                    Rect.Height = (Cog_ROI_Display.DisplayRectangle.Height / Cog_ROI_Display.Zoom * Rp.DefaultScale);
                    Rect.X -= (Rect.Width / 2);
                    Rect.Y -= (Rect.Height / 2);

                    //Rect.SelectedLineStyle = CogGraphicLineStyleConstants.Solid;
                    //Rect.SelectedColor = CogColorConstants.Black;
                    //Rect.LineStyle = CogGraphicLineStyleConstants.Solid;
                    //Rect.DragLineStyle = CogGraphicLineStyleConstants.Solid;
                    //Rect.DragColor = CogColorConstants.Black;

                    double co = CogColorConstants.Cyan.toDbl();
                    Cog_ROI_Display.DrawingEnabled = false;
                    Rect.Interactive = true;
                    Rect.Dragging += new CogDraggingEventHandler(MRect_Dragging);
                    Rect.DraggingStopped += new CogDraggingStoppedEventHandler(MRect_DraggingStopped);
                    Rect.GraphicDOFEnable = CogRectangleDOFConstants.All;
                    Cog_ROI_Display.InteractiveGraphics.Add(Rect, ROI_Name, true);
                    int ROICount = Cog_ROI_Display.InteractiveGraphics.FindItem(ROI_Name, CogDisplayZOrderConstants.Front);
                    Cog_ROI_Display.DrawingEnabled = true;

       
                    ROI_Data Data = new ROI_Data();
                    Data.Name = Input.ResultName;
                    Data.Category = SelectedROICategory;
                    Data.X = Rect.X;
                    Data.Y = Rect.Y;
                    Data.Width = Rect.Width;
                    Data.Height = Rect.Height;
                    Data.Object = Rect;

                    CustomProperty Cp = new CustomProperty(Input.ResultName, Data, false, SelectedROICategory, SelectedROICategory, true) { IsBrowsable = true };
                    PGE_ROIList.Item.Add(Cp);
                    PGE_ROIList.Refresh();
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
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
        private void MRect_DraggingStopped(object sender, CogDraggingEventArgs e)
        {
            MRect_Dragging(sender, e);
        }

        private void MRect_Dragging(object sender, CogDraggingEventArgs e)
        {
            CogRectangle dragRect = (CogRectangle)e.DragGraphic;
        }
        public void FormInitialize(int no)
        {
            
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

                ROI_Property Prop = new ROI_Property();
                Prop.Name = name;
                Prop.LineColor = 0;
                Prop.LineStyle = 0;
                Prop.DefaultScale = Globals.ROI_RATIO;
                CustomProperty Cp = new CustomProperty("Appearance", Prop, true, name.ToString(), name, true) { IsBrowsable = true };

                PGE_ROIProp.Item.Add(Cp);
                PGE_ROIProp.Refresh();
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

        private void Btn_ImageLoad_Click(object sender, EventArgs e)
        {
            OpenFileDialog Ofd = new OpenFileDialog();
            Ofd.DefaultExt = "bmp";
            Ofd.Filter = "BMP File(*.bmp)|*bmp|PNG File(*.png)|*png";         
            Ofd.ShowDialog();
            if (Ofd.FileName.Length >= 0)
            {
                var Temp = Cognex_Helper.Load_Image(Ofd.FileName);
                //var Temp = Cognex_Helper.Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp");
                //var Temp2 = Temp.ToBitmap();
                Cog_ROI_Display.Image = Temp;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            SaveROIProperty();
        }
    }
}
