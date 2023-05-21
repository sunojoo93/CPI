using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.Main_Form;
using CRUX_Renewal.Utils;
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

namespace CRUX_Renewal
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
        public Main_Frm_Auto Frm_Auto;
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
            Recipe = recipe;
        }
        public void LinkRecipe(ref Recipes recipe)
        {
            Frm_Auto.SetRecipe(ref recipe);
            Frm_Manual.SetRecipe(ref recipe);
            Frm_Recipe.SetRecipe(ref recipe);
            Frm_Optical.SetRecipe(ref recipe);
            Frm_Algorithm.SetRecipe(ref Recipe);
            Frm_Recipe.RefeshRecipe();
        }
        public void Initialize()
        {
            this.Dock = DockStyle.Fill;
            this.TopLevel = false;
            Frm_MenuBar = new Ex_Frm_Menubar() { Owner = this.Owner };
            //if (Systems.RecipeContent.MainRecipe[CurFormIndex].Manager.JobCount > 0)
           //     Systems.CurrentSelectedPtnName[CurFormIndex] = Systems.RecipeContent.MainRecipe[CurFormIndex].Manager.Job(0).Name;
            Frm_Auto = new Main_Frm_Auto() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            Frm_Auto.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
            //Frm_Auto.SetRecipe(ref Recipe);

            Frm_Manual = new Main_Frm_Manual() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            Frm_Manual.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
            Frm_Manual.SetRecipe(ref Recipe);

            Frm_Recipe = new Main_Frm_Recipe() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            Frm_Recipe.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
            //Frm_Recipe.SetRecipe(ref Recipe);

            Frm_Optical = new Main_Frm_Optical() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            Frm_Optical.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
            //Frm_Optical.SetRecipe(ref Recipe);

            Frm_Algorithm = new Main_Frm_Algorithm() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            Frm_Algorithm.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
            //Frm_Algorithm.SetRecipe(ref Recipe);

            Tlp_MainForm.Controls.Add(Frm_MenuBar, 0, 1);
            Tlp_MainForm.Controls.Add(Frm_Auto, 0, 0);

            CurrentFormName = Frm_Auto.Name;
            LoadingComplete = true;
           
        }

        private void Frm_MainContent_Load (object sender, EventArgs e)
        {            
           
        }

        public void ChangeMainForm(Form name)
        {            
            var Result = Tlp_MainForm.Controls.Find(name.Name, false);
            if ( Result.Count() == 0)
            {
                name.Hide();
                Tlp_MainForm.Controls.Remove(Tlp_MainForm.Controls.Find(CurrentFormName, false)[0]);
                Tlp_MainForm.Controls.Add(name, 0, 0);
                //Tlp_Main_.SetColumnSpan(name, 7);                
                CurrentFormName = name.Name;
                name.Show();
            }
        }
        private delegate void ChangeOpacityDelegate (Form form,double value);
        private void ChangeMainFormEffect(Form form, double value)
        {
            if ( this.InvokeRequired )
            {
                ChangeOpacityDelegate del = new ChangeOpacityDelegate(ChangeMainFormEffect);
                object[] parameters = { value };

                this.Invoke(del, value);
            }
            else
            {
                //Your code goes here, in this case:                          
                form.Opacity = value;
                //Tlp_Main.Refresh();
            }
        }

        public bool GetLoadingComplete()
        {
            return LoadingComplete;
        }

        private void Frm_MainContent_Shown (object sender, EventArgs e)
        {

        }      
    }
}
