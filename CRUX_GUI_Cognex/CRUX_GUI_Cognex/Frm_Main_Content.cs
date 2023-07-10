using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Ex_Form;
using CRUX_GUI_Cognex.Main_Form;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex
{
    public partial class Frm_MainContent : Form
    {
        
        public string CurrentFormName = string.Empty;
        public int CurFormIndex = 0;
        bool LoadingComplete = false;
        public Recipes Recipe;
        // 메뉴 바 폼
        public Ex_Frm_Menubar Frm_MenuBar;

        // 오토 폼
        public Main_Frm_Auto_For_CHIPPING Frm_Auto_Chipping;
        public Main_Frm_Auto_For_ALM Frm_Auto_ALM;
        public Main_Frm_Auto Frm_Auto_Default;
        // 메뉴얼 폼
        public Main_Frm_Manual Frm_Manual;
        // 레시피 폼
        public Main_Frm_Recipe Frm_Recipe;
        // 광학 폼
        public Main_Frm_Optical Frm_Optical;
        public Main_Frm_Algorithm Frm_Algorithm;
        public Frm_MainContent ()
        {
            InitializeComponent();
            Initialize();
        }
        public Recipes GetRecipe()
        {
            return Recipe;
        }
        public void SetRecipe(Recipes recipe)
        {
            try
            {
                Recipe = recipe;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void LinkRecipe(ref Recipes recipe)
        {
            try
            {
                if (Globals.Insp_Type[0] == 5)
                {
                    Frm_Auto_Default.SetRecipe(ref recipe);
                }
                if (Globals.Insp_Type[0] == 6)
                {
                    Frm_Auto_Default.SetRecipe(ref recipe);
                }

                Frm_Manual.SetRecipe(ref recipe);
                Frm_Recipe.SetRecipe(ref recipe);
                Frm_Optical.SetRecipe(ref recipe);
                Frm_Algorithm.SetRecipe(ref Recipe);
                Frm_Recipe.RefeshRecipe();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void Initialize()
        {
            try
            {
                this.Dock = DockStyle.Fill;
                this.TopLevel = false;
                Frm_MenuBar = new Ex_Frm_Menubar() { Owner = this.Owner };
                if (Globals.Insp_Type[0] == 5)
                {
                    Frm_Auto_Default = new Main_Frm_Auto_For_CHIPPING() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
                    Frm_Auto_Default.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
                }
                else if (Globals.Insp_Type[0] == 6)
                {
                    Frm_Auto_Default = new Main_Frm_Auto_For_ALM() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
                    Frm_Auto_Default.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
                }
                else
                {
                    Frm_Auto_Default = new Main_Frm_Auto() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
                    Frm_Auto_Default.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
                }

                Frm_Manual = new Main_Frm_Manual() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
                Frm_Manual.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
                Frm_Manual.SetRecipe(ref Recipe);

                Frm_Recipe = new Main_Frm_Recipe() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
                Frm_Recipe.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);


                Frm_Optical = new Main_Frm_Optical() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
                Frm_Optical.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);


                Frm_Algorithm = new Main_Frm_Algorithm() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
                Frm_Algorithm.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);

                Tlp_MainForm.Controls.Add(Frm_MenuBar, 0, 1);

                Tlp_MainForm.Controls.Add(Frm_Auto_Default, 0, 0);
                CurrentFormName = Frm_Auto_Default.Name;

                LoadingComplete = true;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public void ChangeMainForm(Form name)
        {
            try
            {
                var Result = Tlp_MainForm.Controls.Find(name.Name, false);
                if (Result.Count() == 0)
                {
                    name.Hide();
                    Tlp_MainForm.Controls.Remove(Tlp_MainForm.Controls.Find(CurrentFormName, false)[0]);
                    Tlp_MainForm.Controls.Add(name, 0, 0);
                    CurrentFormName = name.Name;
                    name.Show();
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        private delegate void ChangeOpacityDelegate (Form form,double value);
        private void ChangeMainFormEffect(Form form, double value)
        {
            try
            {
                if (this.InvokeRequired)
                {
                    ChangeOpacityDelegate del = new ChangeOpacityDelegate(ChangeMainFormEffect);
                    object[] parameters = { value };

                    this.Invoke(del, value);
                }
                else
                {
                    form.Opacity = value;
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Frm_MainContent_Shown (object sender, EventArgs e)
        {

        }      
    }
}
