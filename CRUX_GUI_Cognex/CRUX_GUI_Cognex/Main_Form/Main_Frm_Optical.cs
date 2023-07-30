using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Ex_Form;
using System.Collections.Generic;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Main_Form
{
    public partial class Main_Frm_Optical : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex = 0;
        public Recipes Shared_Recipe;
        Ex_Frm_Optic_Line LineCamForm;
        List<Ex_Frm_Optic_Area_Content> AreaCamForm = new List<Ex_Frm_Optic_Area_Content>();
        public Main_Frm_Optical ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Initialize();
            Show();
        }
        public void SetRecipe(ref Recipes recipe)
        {
            Shared_Recipe = recipe;
            foreach(Ex_Frm_Optic_Area_Content item in AreaCamForm)
                item.SetRecipe(ref Shared_Recipe);
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        private void Initialize()
        {
            foreach(string item in Systems.AvaliableCamNameList)
            {
                Ex_Frm_Optic_Area_Content Temp = new Ex_Frm_Optic_Area_Content() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex, CamName = item };
                Temp.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex, item);
                AreaCamForm.Add(Temp);
                TabPage Tp = new TabPage(item);
                tab_caminfo.TabPages.Add(Tp);
                Temp.Show();
                Tp.Controls.Add(Temp); 
            }

            //LineCamForm = new Ex_Frm_Optic_Line() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
         

            //AreaCamForm.Dock = DockStyle.None;
            //LineCamForm.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
       
     
            //tab_caminfo.TabPages[0].Dock = DockStyle.Fill;
            //tab_CAM_1.Controls.Add(AreaCamForm);            
            //LineCamForm.Show();
            //tab_CAM_2.Controls.Add(AreaCamForm);
            //AreaCamForm.Show();
            //LineCamForm.Show();
       
        }
    }
}
