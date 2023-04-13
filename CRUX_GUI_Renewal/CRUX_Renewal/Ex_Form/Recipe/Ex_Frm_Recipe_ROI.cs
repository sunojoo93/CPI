using Cognex.VisionPro;
using Cognex.VisionPro.Display;
using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.User_Controls;
using CRUX_Renewal.Utils;
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

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Recipe_ROI : Form
    {
        Label m_lbImageState = new Label();
        bool _AltIsDown = false;
        bool LeftMouseDown = false;

        ListViewItem.ListViewSubItem curSB;
        ListViewItem curItem;
        bool cancelEdit;
        bool Display_Draging = false;
        ToolTip Tip = new ToolTip();

        bool AltIsDown
        {
            get
            {
                return _AltIsDown;
            }
            set
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
        }

        public Ex_Frm_Recipe_ROI()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
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
            Show();
            InitPGE();        

            InputBox.KeyDown += InputBox_KeyDown;
            InputBox.Leave += InputBox_Leave;
            InputBox.Hide();

            

        }
        private void InitPGE()
        {
            IniFile Ini = new IniFile();
            Ini.Load($@"{Paths.ROI_PROPERTY}ROI_Property.dat");
            LstV_ROI.Columns.Add("Name",90);
            LstV_ROI.Columns.Add("Category",120);
            LstV_ROI.Columns.Add("X",150);
            LstV_ROI.Columns.Add("Y", 150);
            LstV_ROI.Columns.Add("Width", 150);
            LstV_ROI.Columns.Add("Height", 150);
            LstV_ROI.Columns.Add("Object",120);

            foreach (var inner_item in Ini.Values)
            {
                LstB_ROI.Items.Add(inner_item["Name"].ToString());
                LstB_Category.Items.Add(inner_item["Name"].ToString());
                ROI_Property Prop = new ROI_Property();
                Prop.Name = inner_item["Name"].ToString();
                Prop.LineColor = Utility.EnumUtil<CogColorConstants>.Parse(inner_item["LineColor"].ToString()) ;
                Prop.LineStyle = Utility.EnumUtil<CogGraphicLineStyleConstants>.Parse(inner_item["LineStyle"].ToString());
                Prop.SelectedLineStyle = Utility.EnumUtil<CogGraphicLineStyleConstants>.Parse(inner_item["SelectedLineStyle"].ToString());
                Prop.SelectedLineColor = Utility.EnumUtil<CogColorConstants>.Parse(inner_item["SelectedColor"].ToString());
                Prop.DragLineStyle = Utility.EnumUtil<CogGraphicLineStyleConstants>.Parse(inner_item["DragLineStyle"].ToString());
                Prop.DragLineColor = Utility.EnumUtil<CogColorConstants>.Parse(inner_item["DragLineColor"].ToString());
                Prop.DefaultScale = inner_item["DefaultScale"].ToDouble();
                CustomProperty Cp = new CustomProperty(inner_item["Name"].ToString(), Prop, true, "Appearance", inner_item["Description"].ToString(), true) { IsBrowsable = true };

                PGE_ROIProp.Item.Add(Cp);
                ListViewGroup Group = new ListViewGroup(inner_item["Name"].ToString(), HorizontalAlignment.Left) { Header = inner_item["Name"].ToString(), Name = inner_item["Name"].ToString() };
                LstV_ROI.Groups.Add(Group);                
            }
            LstB_Category.Hide();
            LstV_ROI.Refresh();
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
            //PGE_ROIList.ContextMenu = cm;

            if(LstB_ROI.Items.Count > 0)
                LstB_ROI.SelectedIndex = 0;           
        }
        public void SaveROIList()
        {
            List<string> SectionList = new List<string>();
            Dictionary<string, IniSection> Sections = new Dictionary<string, IniSection>();

            foreach (ListViewGroup item in LstV_ROI.Groups)
            {
                SectionList.Add(item.Name);
                Sections.Add(item.Name, new IniSection());
            }

            foreach(ListViewItem item in LstV_ROI.Items)
            {
                string GroupName = item.Group.Name;
                for(int i =0; i < Sections.Count; ++i)
                {
                    if(SectionList[i] == GroupName)
                    {
                        Sections[GroupName].Add("Name", item.SubItems["Name"].Text);
                        Sections[GroupName].Add("Category", item.SubItems["Category"].Text);
                        Sections[GroupName].Add("X", item.SubItems["X"].Text);
                        Sections[GroupName].Add("Y", item.SubItems["Y"].Text);
                        Sections[GroupName].Add("Width", item.SubItems["Width"].Text);
                        Sections[GroupName].Add("Height", item.SubItems["Height"].Text);
                       
                    }
                }
                
            }         
            
            //Ini.Save(Systems.Ini_Collection[Systems.CurDisplayIndex]["ROI_Property.dat"].GetIniPath(), System.IO.FileMode.Create);
        }
        public void SaveROIProperty()
        {
            CustomPropertyCollection Props = PGE_ROIProp.Item;
            IniFile Ini = Systems.Ini_Collection[Systems.CurDisplayIndex]["ROI_Property.dat"];
            Ini.Clear();
            foreach(CustomProperty item in Props)
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
                if (AltIsDown)
                {
                    string SelectedROICategory = LstB_ROI.SelectedItem as string;
                    if (SelectedROICategory == null)
                    {
                        Ex_Frm_Notification_Announce Ano = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "선택된 ROI가 없습니다.");
                        Ano.ShowDialog();
                        return;
                    }
  
                    Ex_Frm_Others_New_Input Input = new Ex_Frm_Others_New_Input("입력", LstV_ROI.Items, SelectedROICategory);
                    Input.ShowDialog();
                    if (Input.DialogResult == DialogResult.Cancel || Input.ResultName == null)
                        return;

                    string ROI_Name = Input.ResultName;

                    CogRectangle Rect = new CogRectangle();
                    var ROIProp = PGE_ROIProp.Item.FindName(SelectedROICategory);
                    ROI_Property Rp = (ROIProp.Value as ROI_Property);

                    Rect.X = (Cog_ROI_Display.Image.Width / 2) - Cog_ROI_Display.PanX;
                    Rect.Y = (Cog_ROI_Display.Image.Height / 2) - Cog_ROI_Display.PanY;

                    Rect.Width = (Cog_ROI_Display.DisplayRectangle.Width / Cog_ROI_Display.Zoom * Rp.DefaultScale);
                    Rect.Height = (Cog_ROI_Display.DisplayRectangle.Height / Cog_ROI_Display.Zoom * Rp.DefaultScale);
                    Rect.X -= (Rect.Width / 2);
                    Rect.Y -= (Rect.Height / 2);

                    Rect.SelectedLineStyle = Rp.SelectedLineStyle;
                    Rect.SelectedColor = Rp.SelectedLineColor;
                    Rect.LineStyle = Rp.LineStyle;
                    Rect.DragLineStyle = Rp.DragLineStyle;
                    Rect.DragColor = Rp.DragLineColor;
                    Rect.Color = Rp.LineColor;

                    Cog_ROI_Display.DrawingEnabled = false;
                    Rect.Interactive = true;
                    Rect.Dragging += new CogDraggingEventHandler(MRect_Dragging);
                    Rect.DraggingStopped += new CogDraggingStoppedEventHandler(MRect_DraggingStopped);
                    Rect.GraphicDOFEnable = CogRectangleDOFConstants.All;

                    string ROIName = $"{SelectedROICategory}^{Input.ResultName}";

                    Cog_ROI_Display.InteractiveGraphics.Add(Rect, ROIName, false);
                    int ROICount = Cog_ROI_Display.InteractiveGraphics.FindItem(ROIName, CogDisplayZOrderConstants.Front);
                    Cog_ROI_Display.DrawingEnabled = true;
                    
                    ROI_Data Data = new ROI_Data();
                    Data.Name = Input.ResultName;
                    Data.Category = SelectedROICategory;
                    Data.X = Rect.X;
                    Data.Y = Rect.Y;
                    Data.Width = Rect.Width;
                    Data.Height = Rect.Height;
                    Data.Object = Rect;
                    LstV_ROI.BeginUpdate();

                    ListViewItem Lvi = new ListViewItem(Input.ResultName, SelectedROICategory) { Name = "Name" };
                    Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Data.Category, Name = "Category"});
                    Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Data.X.ToString(), Name = "X" });
                    Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Data.Y.ToString(), Name = "Y" });
                    Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Data.Width.ToString(), Name = "Width" });
                    Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Data.Height.ToString(), Name = "Height" });
                    Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Name = "Object" ,Text = Data.Object.ToString(), Tag = Data.Object });
                    LstV_ROI.Items.Add(Lvi);

                    LstV_ROI.Groups[SelectedROICategory].Items.Add(Lvi);
                    LstV_ROI.EndUpdate();
                }
                else if(Cog_ROI_Display.Selection.Count > 0)
                {
                    CogRectangle Rect = Cog_ROI_Display.Selection[0] as CogRectangle;
                    foreach (ListViewGroup item in LstV_ROI.Groups)
                    {
                        for(int i = 0; i < item.Items.Count; ++i)
                        {
                            if(item.Items[i].SubItems["Object"].Tag as CogRectangle == Rect)
                            {
                                item.Items[i].Selected = true;
                            }
                        }
                    }
                    LstV_ROI.Invalidate();
                    LstV_ROI.Focus();
                }
            }
            catch (Exception ex)
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
            LeftMouseDown = e.Button == MouseButtons.Left ? true : false;

        }
        private void MRect_DraggingStopped(object sender, CogDraggingEventArgs e)
        {
            MRect_Dragging(sender, e);
            //LstV_ROI.EndUpdate();
            LstV_ROI.Refresh();
            Display_Draging = false;
        }

        private void MRect_Dragging(object sender, CogDraggingEventArgs e)
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
                if(Cog_ROI_Display.InteractiveGraphics[i].Selected == true)
                    FindSelRect = (Cog_ROI_Display.InteractiveGraphics[i]) as CogRectangle;                    
                
            }

            foreach(ListViewItem item in LstV_ROI.Items)
            {               
                if ((item.SubItems["Object"].Tag as CogRectangle) == FindSelRect)
                {
                    item.SubItems["X"].Text = dragRect.X.ToString();
                    item.SubItems["Y"].Text = dragRect.Y.ToString();
                    item.SubItems["Width"].Text = dragRect.Width.ToString();
                    item.SubItems["Height"].Text = dragRect.Height.ToString();
                }
            }
            LstV_ROI.Invalidate();
        }
        public void FormInitialize(int no)
        {
            
        }
        private void Ex_Frm_Recipe_ROI_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
            LstV_ROI.DoubleBuffered(true);
        }
        private void Ex_Frm_Recipe_ROI_Shown(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 5, 5));
            ////WinApis.SetWindowRgn(Btn_Manage.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Manage.Width, Btn_Manage.Height, 15, 15), true);
            ////WinApis.SetWindowRgn(Btn_Login.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Login.Width, Btn_Login.Height, 15, 15), true);
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
                LstB_ROI.SelectedItem = name;
                ROI_Property Prop = new ROI_Property();
                Prop.Name = name;
                CustomProperty Cp = new CustomProperty(name, Prop, true, "Appearance", name, true) { IsBrowsable = true };

                PGE_ROIProp.Item.Add(Cp);

                if (LstV_ROI.Groups[name] == null)
                    LstV_ROI.Groups.Add(new ListViewGroup(name, HorizontalAlignment.Left) { Header = name, Name = name });

                PGE_ROIProp.Refresh();
            }
        }

        private void Btn_ROI_Del_Click(object sender, EventArgs e)
        {
          
            string name = LstB_ROI.SelectedItem as string;
            if (name == "")
                return;
            if (LstV_ROI.Groups[name] != null)
            {
                Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "ROI 데이터가 존재합니다.\n ROI 데이터와 함께 삭제됩니다.");
                LstV_ROI.BeginUpdate();
                if (LstV_ROI.Groups[name].Items.Count > 0)
                {                   
                    Noti.ShowDialog();

                    if (Noti.DialogResult == DialogResult.OK)
                    {
                        foreach (ListViewItem item in LstV_ROI.Items)
                        {
                            if (item.Group.Name == name)
                            {
                                string RectName = $"{name}^{item.Text}";
                                Cog_ROI_Display.InteractiveGraphics.Remove(RectName);
                                LstV_ROI.Items.Remove(item);
                            }
                        }
                    }
                }
                LstV_ROI.Groups.Remove(LstV_ROI.Groups[name]);
                LstV_ROI.EndUpdate();
                LstV_ROI.Invalidate();
            }
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
            if (Ofd.FileName.Length > 0)
            {
                var Temp = Cognex_Helper.Load_Image(Ofd.FileName);
                Cog_ROI_Display.DrawingEnabled = false;
                Cog_ROI_Display.Image = Temp;
                Cog_ROI_Display.Fit(true);
                Cog_ROI_Display.DrawingEnabled = true;

            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //SaveROIProperty();
            SaveROIList();
        }

        private void PGE_ROIProp_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            Cog_ROI_Display.DrawingEnabled = false;
            ROI_Property Rp = (e.ChangedItem.Parent.Value as ROI_Property);

            ListViewGroup Temp = LstV_ROI.Groups[Rp.Name];
            foreach (ListViewItem item in Temp.Items)
            {
                CogRectangle Rect = (item.SubItems["Object"].Tag as CogRectangle);
                Rect.Color = Rp.LineColor;
                Rect.LineStyle = Rp.LineStyle;
                Rect.SelectedColor = Rp.SelectedLineColor;
                Rect.SelectedLineStyle = Rp.SelectedLineStyle;
                Rect.DragColor = Rp.DragLineColor;
            }
            Cog_ROI_Display.DrawingEnabled = true;
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
            Tip.InitialDelay =200;
            switch (idxSub)
            {
                case 0: // 0번째 subitem 만 수정가능하게

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
                case 1: // 1번째 subitem 만 수정가능하게
                    if (Tip.GetToolTip(InputBox) != "")
                        Tip.RemoveAll();
                    LstB_Category.BringToFront();
                    //LstB_Category.SetBounds(lLeft, curSB.Bounds.Top + LstV_ROI.Top, lWidth, curSB.Bounds.Height);
                    LstB_Category.Location = new Point(lLeft, curSB.Bounds.Top + LstV_ROI.Top);
                    LstB_Category.Size = new Size(lWidth, 30);
                    LstB_Category.SelectedItem = curSB.Text;
                    LstB_Category.Show();
                    LstB_Category.Focus();
                    Tip.SetToolTip(LstB_Category, "카테고리를 변경합니다.");
                    break;
                case 2: // 2번째 subitem 만 수정가능하게
                case 3: // 3번째 subitem 만 수정가능하게
                case 4: // 4번째 subitem 만 수정가능하게
                case 5: // 5번째 subitem 만 수정가능하게         
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

        private void InputBox_KeyDown(object sender, KeyEventArgs e)
        {
            // 엔터키 수정 ESC키 취소

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

        private void InputBox_Leave(object sender, EventArgs e)
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
                            ROI_Data Rd = new ROI_Data();
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

                                    Rd.Name = InputBox.Text;
                                    Rd.Category = curItem.Group.Name;
                                    if (curSB.Text == InputBox.Text)
                                    {
                                        return;
                                    }
                                    foreach (ListViewItem item in LstV_ROI.Items)
                                    {
                                        if (item.Group.Name == Rd.Category)
                                        {
                                            if (item.Text == Rd.Name)
                                            {
                                                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "동일한 이름이 존재합니다.");
                                                Noti.ShowDialog();
                                                InputBox.Hide();
                                                LstV_ROI.Focus();
                                                return;
                                            }
                                        }
                                    }
                                    ObjectName = $"{curItem.Group.Name}^{curItem.SubItems["Name"].Text}";
                                    curSB.Text = InputBox.Text;
                                    NewObjectName = $"{curItem.Group.Name}^{InputBox.Text}";

                                    if (ObjectName == NewObjectName)
                                    {
                                        InputBox.Hide();
                                        LstV_ROI.Focus();
                                        return;
                                    }
                                }
                            }
                            else if (Col == "Category")
                            {
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
                            }
                            else if (Col == "X")
                            {
                                CoordChange = double.TryParse(InputBox.Text, out Num);
                                if (CoordChange)
                                    (Item.SubItems["Object"].Tag as CogRectangle).X = Num;
                            }
                            else if (Col == "Y")
                            {
                                CoordChange = double.TryParse(InputBox.Text, out Num);
                                if (CoordChange)
                                    (Item.SubItems["Object"].Tag as CogRectangle).Y = Num;
                            }
                            else if (Col == "Width")
                            {
                                CoordChange = double.TryParse(InputBox.Text, out Num);
                                if (CoordChange)
                                    (Item.SubItems["Object"].Tag as CogRectangle).Width = Num;
                            }
                            else if (Col == "Height")
                            {
                                CoordChange = double.TryParse(InputBox.Text, out Num);
                                if (CoordChange)                                
                                    (Item.SubItems["Object"].Tag as CogRectangle).Height = Num;
                                
                            }
                            if (CoordChange)
                            {
                                curSB.Text = Num.ToString();
                            }
                            else if (NameChange)
                            {
                                CogRectangle Rect = new CogRectangle();

                                Rd.X = Item.SubItems["X"].Text.toDbl();
                                Rd.Y = Item.SubItems["Y"].Text.toDbl();
                                Rd.Width = Item.SubItems["Width"].Text.toDbl();
                                Rd.Height = Item.SubItems["Height"].Text.toDbl(); ;

                                Rect.X = Rd.X;
                                Rect.Y = Rd.Y;
                                Rect.Width = Rd.Width;
                                Rect.Height = Rd.Height;

                                var ROIProp = PGE_ROIProp.Item.FindName(Rd.Category);
                                ROI_Property Rp = (ROIProp.Value as ROI_Property);

                                Rect.SelectedLineStyle = Rp.SelectedLineStyle;
                                Rect.SelectedColor = Rp.SelectedLineColor;
                                Rect.LineStyle = Rp.LineStyle;
                                Rect.DragLineStyle = Rp.DragLineStyle;
                                Rect.DragColor = Rp.DragLineColor;
                                Rect.Color = Rp.LineColor;

                                Cog_ROI_Display.DrawingEnabled = false;
                                Rect.Interactive = true;
                                Rect.Dragging += new CogDraggingEventHandler(MRect_Dragging);
                                Rect.DraggingStopped += new CogDraggingStoppedEventHandler(MRect_DraggingStopped);
                                Rect.GraphicDOFEnable = CogRectangleDOFConstants.All;
                                Cog_ROI_Display.InteractiveGraphics.Remove(ObjectName);
                                Cog_ROI_Display.InteractiveGraphics.Add(Rect, NewObjectName, false);
                                Cog_ROI_Display.DrawingEnabled = true;

                                Rd.Object = Rect;

                                LstV_ROI.BeginUpdate();
                                LstV_ROI.Items.Remove(curItem);
                                ListViewItem Lvi = new ListViewItem(Rd.Name, Rd.Category) { Name = "Name" };
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.Category, Name = "Category" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.X.ToString(), Name = "X" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.Y.ToString(), Name = "Y" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.Width.ToString(), Name = "Width" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.Height.ToString(), Name = "Height" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Name = "Object", Text = Rd.Object.ToString(), Tag = Rd.Object });
                                LstV_ROI.Items.Add(Lvi);

                                LstV_ROI.Groups[Rd.Category].Items.Add(Lvi);
                                LstV_ROI.EndUpdate();
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

        private void LstV_ROI_DrawSubItem(object sender, DrawListViewSubItemEventArgs e)
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

        private void LstV_ROI_DrawItem(object sender, DrawListViewItemEventArgs e)
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

        private void LstV_ROI_SelectedIndexChanged(object sender, EventArgs e)
        {
            ListView Obj = sender as ListView;
            if(Obj.SelectedItems.Count > 0)
            {
                ListViewItem Item = Obj.SelectedItems[0];
                (Item.SubItems["Object"].Tag as CogRectangle).Selected = true;
            }            
        }

        private void LstV_ROI_MouseClick(object sender, MouseEventArgs e)
        {
            //오른쪽 클릭일 경우
            if (e.Button.Equals(MouseButtons.Right))
            {
                ContextMenu m = new ContextMenu();

                MenuItem m1 = new MenuItem();
                MenuItem m2 = new MenuItem();

                m1.Text = "복사";
                m2.Text = "삭제";

                m1.Click += (senders, es) =>
                {

                };
                m2.Click += (senders, es) =>
                {
                    ListViewItem Item = LstV_ROI.SelectedItems[0];
                    string RectName = $"{Item.SubItems["Category"].Text}^{Item.Text}";
                    Cog_ROI_Display.InteractiveGraphics.Remove(RectName);
                    LstV_ROI.Items.Remove(Item);
                };

                m.MenuItems.Add(m1);
                m.MenuItems.Add(m2);

                m.Show(LstV_ROI, new Point(e.X, e.Y));
            }
        }

        private void InputBox_KeyPress(object sender, KeyPressEventArgs e)
        {

        }

        private void LstB_Category_Leave(object sender, EventArgs e)
        {
            if (LstB_Category.Visible)
            {
                if (cancelEdit == false)
                {
                    if (LstB_Category.Text.Trim() != "")
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
                            ROI_Data Rd = new ROI_Data();
                            if (Col == "Name")
                            {

                            }
                            else if (Col == "Category")
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

                                    string Category = LstB_Category.SelectedItem as string;
                                    if (Category != null)
                                    {
                                        Rd.Name = curItem.SubItems["Name"].Text;
                                        Rd.Category = Category;
                                        if (curSB.Text == Category)
                                        {
                                            LstB_Category.Hide();
                                            LstV_ROI.Focus();
                                            return;
                                        }
                                        foreach (ListViewItem item in LstV_ROI.Items)
                                        {
                                            if (item.Group.Name == Rd.Category)
                                            {
                                                if (item.Text == Rd.Name)
                                                {
                                                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "동일한 카테고리가 존재합니다.");
                                                    Noti.ShowDialog();
                                                    LstB_Category.Hide();
                                                    LstV_ROI.Focus();
                                                    return;
                                                }
                                            }
                                        }
                                        ObjectName = $"{curItem.Group.Name}^{curItem.SubItems["Name"].Text}";
                                        curSB.Text = Category;
                                        NewObjectName = $"{Category}^{curItem.SubItems["Name"].Text}";

                                        if (ObjectName == NewObjectName)
                                        {
                                            LstB_Category.Hide();
                                            LstV_ROI.Focus();
                                            return;
                                        }
                                    }
                                }
                            }
                            else if (Col == "X")
                            {

                            }
                            else if (Col == "Y")
                            {

                            }
                            else if (Col == "Width")
                            {

                            }
                            else if (Col == "Height")
                            {

                            }
                            if (CoordChange)
                            {

                            }
                            else if (NameChange)
                            {
                                CogRectangle Rect = new CogRectangle();

                                Rd.X = Item.SubItems["X"].Text.toDbl();
                                Rd.Y = Item.SubItems["Y"].Text.toDbl();
                                Rd.Width = Item.SubItems["Width"].Text.toDbl();
                                Rd.Height = Item.SubItems["Height"].Text.toDbl(); ;

                                Rect.X = Rd.X;
                                Rect.Y = Rd.Y;
                                Rect.Width = Rd.Width;
                                Rect.Height = Rd.Height;

                                var ROIProp = PGE_ROIProp.Item.FindName(Rd.Category);
                                ROI_Property Rp = (ROIProp.Value as ROI_Property);

                                Rect.SelectedLineStyle = Rp.SelectedLineStyle;
                                Rect.SelectedColor = Rp.SelectedLineColor;
                                Rect.LineStyle = Rp.LineStyle;
                                Rect.DragLineStyle = Rp.DragLineStyle;
                                Rect.DragColor = Rp.DragLineColor;
                                Rect.Color = Rp.LineColor;

                                Cog_ROI_Display.DrawingEnabled = false;
                                Rect.Interactive = true;
                                Rect.Dragging += new CogDraggingEventHandler(MRect_Dragging);
                                Rect.DraggingStopped += new CogDraggingStoppedEventHandler(MRect_DraggingStopped);
                                Rect.GraphicDOFEnable = CogRectangleDOFConstants.All;
                                Cog_ROI_Display.InteractiveGraphics.Remove(ObjectName);
                                Cog_ROI_Display.InteractiveGraphics.Add(Rect, NewObjectName, false);
                                Cog_ROI_Display.DrawingEnabled = true;

                                Rd.Object = Rect;

                                LstV_ROI.BeginUpdate();
                                LstV_ROI.Items.Remove(curItem);
                                ListViewItem Lvi = new ListViewItem(Rd.Name, Rd.Category) { Name = "Name" };
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.Category, Name = "Category" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.X.ToString(), Name = "X" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.Y.ToString(), Name = "Y" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.Width.ToString(), Name = "Width" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = Rd.Height.ToString(), Name = "Height" });
                                Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Name = "Object", Text = Rd.Object.ToString(), Tag = Rd.Object });
                                LstV_ROI.Items.Add(Lvi);

                                LstV_ROI.Groups[Rd.Category].Items.Add(Lvi);
                                LstV_ROI.EndUpdate();
                            }
                        }
                    }
                    else
                    {
                        cancelEdit = false;
                    }
                }
            }
            LstB_Category.Hide();
            LstV_ROI.Focus();
        }

        private void LstB_Category_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
