using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using System.Windows.Forms;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Optical : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex = 0;
        public Recipes Shared_Recipe;
        Ex_Frm_Optic_Line LineCamForm;
        Ex_Frm_Optic_Area_Content AreaCamForm;
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
            AreaCamForm.SetRecipe(ref Shared_Recipe);
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        private void Initialize()
        {
            //LineCamForm = new Ex_Frm_Optic_Line() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            AreaCamForm = new Ex_Frm_Optic_Area_Content() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };


            //LineCamForm.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
            AreaCamForm.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
            tab_CAM_1.Controls.Add(AreaCamForm);            
            //LineCamForm.Show();
            //tab_CAM_2.Controls.Add(AreaCamForm);
            //AreaCamForm.Show();
            //LineCamForm.Show();
            AreaCamForm.Show();
        }
    }
}
