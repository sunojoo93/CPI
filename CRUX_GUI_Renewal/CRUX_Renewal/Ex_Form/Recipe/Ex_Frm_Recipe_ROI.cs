using Cognex.VisionPro;
using Cognex.VisionPro.Display;
using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.User_Controls;
using CRUX_Renewal.Utils;
using PropertyGridEx;
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

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Recipe_ROI : Form
    {
        Label m_lbImageState = new Label();
        bool _AltIsDown = false;
        bool LeftMouseDown = false;
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
            //Cog_ROI_Display.GridColor = Color.Red;
            Cog_ROI_Display.MouseUp += Cog_ROI_Display_MouseUp;
            Cog_ROI_Display.ScalingMethod = Cognex.VisionPro.Display.CogDisplayScalingMethodConstants.Integer;
            Cog_ROI_Display.Click += Cog_ROI_Display_Click;
            Cog_ROI_Display.MouseWheelMode = CogDisplayMouseWheelModeConstants.Zoom1;

    
            Cog_Display_Toolbar.Display = Cog_ROI_Display;
            Cog_Display_Status.Display = Cog_ROI_Display;
            Cog_ROI_Display.MultiSelectionEnabled = false;
            //Cog_ROI_Display.Refresh();
            Show();
            InitPGE();

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
                Prop.LineColor = Utility.EnumUtil<CogColorConstants>.Parse(inner_item["LineColor"].ToString()) ;
                Prop.LineStyle = Utility.EnumUtil<CogGraphicLineStyleConstants>.Parse(inner_item["LineStyle"].ToString());
                Prop.SelectedLineStyle = Utility.EnumUtil<CogGraphicLineStyleConstants>.Parse(inner_item["SelectedLineStyle"].ToString());
                Prop.SelectedLineColor = Utility.EnumUtil<CogColorConstants>.Parse(inner_item["SelectedColor"].ToString());
                Prop.DragLineStyle = Utility.EnumUtil<CogGraphicLineStyleConstants>.Parse(inner_item["DragLineStyle"].ToString());
                Prop.DragLineColor = Utility.EnumUtil<CogColorConstants>.Parse(inner_item["DragLineColor"].ToString());
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

            if(LstB_ROI.Items.Count > 0)
                LstB_ROI.SelectedIndex = 0;           
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

        }

        private void Cog_ROI_Display_MouseUp(object sender, MouseEventArgs e)
        {
            LeftMouseDown = e.Button == MouseButtons.Left ? true : false;
            try
            {
                #region Create New ROI
                if (AltIsDown)
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
                    if (Input.DialogResult == DialogResult.Cancel || Input.ResultName == null)
                        return;

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
             
                    Cog_ROI_Display.InteractiveGraphics.Add(Rect, SelectedROICategory, false);
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

                    CustomProperty Cp = new CustomProperty(Input.ResultName, Data, false, SelectedROICategory, SelectedROICategory, true) { IsBrowsable = true, };

                    PGE_ROIList.Item.Add(Cp);

                    PGE_ROIList.Refresh();
                    // 선택한 객체가 CustomProperty 일 경우
                    if ((PGE_ROIList.SelectedGridItem?.Parent?.Parent != null))
                    {
                        GridItem Cpcollection = PGE_ROIList.SelectedGridItem.Parent.Parent ?? null;
                        for (int i = 0; i < Cpcollection.GridItems.Count; ++i)
                        {
                            if (Cpcollection.GridItems[i].Label == SelectedROICategory)
                            {
                                CustomPropertyCollection Temp = Cpcollection.Value as CustomPropertyCollection;
                                for (int j = 0; j < Temp.Count; ++j)
                                {
                                    if (Temp[j].Name == Input.ResultName && Temp[j].Category == SelectedROICategory)
                                    {
                                        ((Temp[j].Value as ROI_Data).Object as CogRectangle).Selected = true;
                                        PGE_ROIList.SelectedGridItem = Cpcollection.GridItems[i].GridItems[Input.ResultName];
                                    }
                                }
                            }
                        }
                    }
                    // 선택한 객체가 Category 일 경우
                    else if ((PGE_ROIList.SelectedGridItem?.Parent != null))
                    {
                        GridItem Cpcollection = PGE_ROIList.SelectedGridItem.Parent ?? null;
                        for (int i = 0; i < Cpcollection.GridItems.Count; ++i)
                        {
                            if (Cpcollection.GridItems[i].Label == SelectedROICategory)
                            {
                                CustomPropertyCollection Temp = Cpcollection.Value as CustomPropertyCollection;
                                for (int j = 0; j < Temp.Count; ++j)
                                {
                                    if (Temp[j].Name == Input.ResultName && Temp[j].Category == SelectedROICategory)
                                    {
                                        ((Temp[j].Value as ROI_Data).Object as CogRectangle).Selected = true;
                                        PGE_ROIList.SelectedGridItem = Cpcollection.GridItems[i].GridItems[Input.ResultName];
                                    }
                                }
                            }
                        }
                    }
                    PGE_ROIList.Focus();
                    AltIsDown = false;
                }
                #endregion
                #region ROI 클릭 시 PGE 하이라이트 기능
                else
                {
                    var Sel = Cog_ROI_Display.Selection[0];
                    // 선택한 객체가 CustomProperty 일 경우
                    if ((PGE_ROIList.SelectedGridItem?.Parent?.Parent != null))
                    {
                        GridItem Cpcollection = PGE_ROIList.SelectedGridItem.Parent.Parent ?? null;
                        for (int i = 0; i < Cpcollection.GridItems.Count; ++i)
                        {
                            for (int j = 0; j < Cpcollection.GridItems[i].GridItems.Count; ++j)
                            {
                                if ((Cpcollection.GridItems[i].GridItems[j].Value as ROI_Data).Object.Equals(Sel))
                                {
                                    PGE_ROIList.SelectedGridItem = (Cpcollection.GridItems[i].GridItems[j]);
                                }
                            }
                        }
                    }
                    // 선택한 객체가 Category 일 경우
                    else if ((PGE_ROIList.SelectedGridItem?.Parent != null))
                    {
                        GridItem Cpcollection = PGE_ROIList.SelectedGridItem.Parent ?? null;
                        for (int i = 0; i < Cpcollection.GridItems.Count; ++i)
                        {
                            for (int j = 0; j < Cpcollection.GridItems[i].GridItems.Count; ++j)
                            {
                                if ((Cpcollection.GridItems[i].GridItems[j].Value as ROI_Data).Object.Equals(Sel))
                                {
                                    PGE_ROIList.SelectedGridItem = (Cpcollection.GridItems[i].GridItems[j]);
                                }
                            }
                        }
                    }
                }
                #endregion
                if (Cog_ROI_Display.Selection.Count > 0)
                {
                    Cog_ROI_Display.DrawingEnabled = false;
                    CogRectangle Sel = Cog_ROI_Display.Selection[Cog_ROI_Display.Selection.Count -1] as CogRectangle;
                    for (int i = 0; i < PGE_ROIList.Item.Count; ++i)
                    {
                        if ((PGE_ROIList.Item[i].Value as ROI_Data).Object.Equals(Sel))
                        {
                            (PGE_ROIList.Item[i].Value as ROI_Data).X = Sel.X;
                            (PGE_ROIList.Item[i].Value as ROI_Data).Y = Sel.Y;
                            (PGE_ROIList.Item[i].Value as ROI_Data).Width = Sel.Width;
                            (PGE_ROIList.Item[i].Value as ROI_Data).Height = Sel.Height;
                        }
                    }
                    PGE_ROIList.Refresh();
                    Cog_ROI_Display.DrawingEnabled = true;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

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
        }

        private void MRect_Dragging(object sender, CogDraggingEventArgs e)
        {
            //try
            //{

            ////    CogRectangle dragRect = (CogRectangle)e.DragGraphic;
            ////    dragRect.Selected = true;
            ////    if (Cog_ROI_Display.Selection.Count > 0 && Cog_ROI_Display.Focused)
            ////    {
            ////        CogRectangle Sel = null;
            ////        Cog_ROI_Display.DrawingEnabled = false;

            ////        Sel = Cog_ROI_Display.Selection[0] as CogRectangle;

            ////        int aa = Cog_ROI_Display.InteractiveGraphics.FindItem(dragRect, CogDisplayZOrderConstants.Back);
            ////        for (int i = 0; i < PGE_ROIList.Item.Count; ++i)
            ////        {
            ////            if (((PGE_ROIList.Item[i].Value as ROI_Data).Object as CogRectangle) == (Sel))
            ////            {
            ////                (PGE_ROIList.Item[i].Value as ROI_Data).X = dragRect.X;
            ////                (PGE_ROIList.Item[i].Value as ROI_Data).Y = dragRect.Y;
            ////                (PGE_ROIList.Item[i].Value as ROI_Data).Width = dragRect.Width;
            ////                (PGE_ROIList.Item[i].Value as ROI_Data).Height = dragRect.Height;
            ////            }
            ////        }
            ////        Cog_ROI_Display.DrawingEnabled = true;
            ////        PGE_ROIList.Refresh();
            ////    }
            ////    Cog_ROI_Display.Selection.Clear();
            ////}
            ////catch (Exception ex)
            ////{
            ////    Console.WriteLine(ex.Message);
            ////}

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
            if (Ofd.FileName.Length > 0)
            {
                var Temp = Cognex_Helper.Load_Image(Ofd.FileName);
                //var Temp = Cognex_Helper.Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp");
                //var Temp2 = Temp.ToBitmap();
                Cog_ROI_Display.Image = Temp;

                Cog_ROI_Display.Fit(true);
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            SaveROIProperty();
        }

        private void PGE_ROIProp_Validating(object sender, CancelEventArgs e)
        {
            int a = 0;
        }

        private void PGE_ROIProp_Validated(object sender, EventArgs e)
        {
            int a = 0;
        }

        private void PGE_ROIProp_SelectedGridItemChanged(object sender, SelectedGridItemChangedEventArgs e)
        {
            int a = 0;
        }

        private void PGE_ROIProp_SelectedObjectsChanged(object sender, EventArgs e)
        {
            int b = 0;
        }

        private void PGE_ROIProp_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            Cog_ROI_Display.DrawingEnabled = false;
            ROI_Property Rp = (e.ChangedItem.Parent.Value as ROI_Property);

            for (int j = 0; j < PGE_ROIList.Item.Count; ++j)
            {
                if ((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Category == Rp.Name)
                {
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).Color = Rp.LineColor;
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).LineStyle = Rp.LineStyle;
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).SelectedColor = Rp.SelectedLineColor;
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).SelectedLineStyle = Rp.SelectedLineStyle;
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).DragColor = Rp.DragLineColor;
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).DragLineStyle = Rp.DragLineStyle;
                }
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

        private void PGE_ROIList_SelectedGridItemChanged(object sender, SelectedGridItemChangedEventArgs e)
        {
            var Grid = e.NewSelection.Value as ROI_Data;
            if (Grid != null)
            {
                int Idx = Cog_ROI_Display.InteractiveGraphics.FindItem(Grid?.Object as CogRectangle, CogDisplayZOrderConstants.Back);
                Cog_ROI_Display.InteractiveGraphics[Idx].Selected = true;             
            }
        }

        private void PGE_ROIList_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            Cog_ROI_Display.DrawingEnabled = false;
            ROI_Data Rd = (e.ChangedItem.Parent.Value as ROI_Data);

            for (int j = 0; j < PGE_ROIList.Item.Count; ++j)
            {
                if ((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Name == Rd.Name)
                {
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).X = Rd.X;
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).Y = Rd.Y;
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).Width = Rd.Width;
                    (((PGE_ROIList.Item[j]?.Value as ROI_Data)?.Object) as CogRectangle).Height = Rd.Height;
                }
            }
            Cog_ROI_Display.DrawingEnabled = true;
        }
    }
}
