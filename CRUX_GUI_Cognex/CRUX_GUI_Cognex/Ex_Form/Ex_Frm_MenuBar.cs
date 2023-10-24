using Cognex.VisionPro;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Main_Form;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Ex_Form
{
    public partial class Ex_Frm_Menubar : Form
    {
        public Ex_Frm_Menubar ()
        {
            try
            {
                InitializeComponent();
                TopLevel = false;
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
                Show();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Auto_Click (object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Auto_Default);

                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Manual_Click (object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Manual);
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Job_Click (object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Recipe);

                Temp.Frm_Recipe.RefeshRecipe();
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Optical_Click (object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Optical);

                Temp.Frm_Optical.FrmUpdate();
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Advance_Click (object sender, EventArgs e)
        {
            try
            {

                //CmdMsgParam SendParam = new CmdMsgParam();

                //SendParam.ClearOffset();                

                //int Ret = Systems.g_Ipc.SendCommand((ushort)((0 + 1) * 100 + IpcConst.SEQ_TASK), IpcConst.SEQ_FUNC, IpcConst.SEQ_SEND_MANUAL_GRABAndINSP,
                //                                          IpcInterface.CMD_TYPE_RES, 100000, SendParam.GetByteSize(), SendParam.GetParam());


                //var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                ////if (Temp != null)
                ////Program.Frm_Recipe.DisplayJob();
                //bool Te1p = (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING_Ver2).GetUseGrabMode();
                //Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);

                OpenFileDialog FileDialog = new OpenFileDialog();
                FileDialog.Filter = "BMP files (*.bmp)|*.BMP|All files (*.*)|*.*";
                FileDialog.FilterIndex = 0;
                FolderBrowserDialog FolderDialog = new FolderBrowserDialog();
                CogImage8Grey TestImage = null;
     
                //dialog.IsFolderPicker = false; // true : 폴더 선택 / false : 파일 선택
                //CommonFileDialogFilterCollection DialogFilter = dialog.Filters;
                //DialogFilter.Add(new CommonFileDialogFilter("Bitmap", ".bmp"));
                //Cog_Display.AutoFit = true;
                string FilePath = string.Empty;
                string FolderPath = string.Empty;

     
                // ManualPathList.Clear();
                //
                if (FileDialog.ShowDialog() == DialogResult.OK)
                {
                    TestImage = Cognex_Helper.Load_Image(FileDialog.FileName);
                }
                //
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $@"[ GUI ] {DateTime.Now.ToString("HH/mm/ss.fff")}", true, false);
                using (var TTT = TestImage.ToBitmap())
                {
                    int bpp = Image.GetPixelFormatSize(TTT.PixelFormat) / 8;

                    BitmapData src_data = TTT.LockBits(new Rectangle(0, 0, TTT.Width, TTT.Height), ImageLockMode.WriteOnly, TTT.PixelFormat);

                    byte[] src_bytes = new byte[src_data.Stride * src_data.Height];
                    Marshal.Copy(src_data.Scan0, src_bytes, 0, src_bytes.Length);
                    // Copy the bytes from the image into a byte array
                    byte[] dst_bytes = new byte[TTT.Height * TTT.Width];

                    for (int i = 0; i < dst_bytes.Length; ++i)
                        dst_bytes[i] = (byte)((src_bytes[i * bpp + 0] + src_bytes[i * bpp + 1] + src_bytes[i * bpp + 2]) / 3);

                    //Parallel.For(0, dst_bytes.Length, i =>
                    //{
                    //    dst_bytes[i] = (byte)((src_bytes[i * bpp + 0] + src_bytes[i * bpp + 1] + src_bytes[i * bpp + 2]) / 3);
                    //});
                    TTT.UnlockBits(src_data);

                    TTT.Save($@"D:\Test1.jpg", ImageFormat.Jpeg);
                }

        
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $@"[ GUI ] {DateTime.Now.ToString("HH/mm/ss.fff")}", true, false);
     
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, "[ GUI ] Start Inspection_GUI Image Save", true, false);


                //Systems.LogWriter.Info("Start Inspection_GUI_Image Save");
                //Cognex_Helper.SaveToFile(TestImage, $@"D:\Test.jpg");
                
                //Bitmap TTT = TestImage.ToBitmap()..Save($@"D:\Test1.jpg", ImageFormat.Jpeg);
                //origin.ToBitmap().Save(path, System.Drawing.Imaging.ImageFormat.Bmp);
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, "[ GUI ] Done Inspection_GUI Image Save", true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Ex_Frm_Menubar_Shown(object sender, EventArgs e)
        {
            try
            {
                WinApis.SetWindowRgn(Btn_Auto.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Auto.Width, Btn_Auto.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Simulation.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Simulation.Width, Btn_Simulation.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Job.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Job.Width, Btn_Job.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Optical.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Optical.Width, Btn_Optical.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Advance.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Advance.Width, Btn_Advance.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Alg.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Alg.Width, Btn_Alg.Height, 15, 15), true);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Alg_Click(object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Algorithm);
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
    }
}
