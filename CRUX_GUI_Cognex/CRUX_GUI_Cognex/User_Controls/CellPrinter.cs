using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.Collections.Concurrent;
using static CRUX_GUI_Cognex.Enums;
using CRUX_GUI_Cognex.Class;
using System.Reflection;
using Cognex.VisionPro.Implementation;
using CRUX_GUI_Cognex.Utils;
using System.Drawing.Imaging;
using Cognex.VisionPro;

namespace CRUX_GUI_Cognex.User_Controls
{
    public partial class CellPrinter : UserControl
    {
        string CtrlName = string.Empty;
        ConcurrentQueue<LogData> MessageQueue = new ConcurrentQueue<LogData>();
        CancellationTokenSource TokenSource = new CancellationTokenSource();
        const int MAX_LogLine = 1000;
        object obj = new object();
        bool AutoScroll = true;

        int PcNum = 0;
        public CellPrinter()
        {
            try
            {
                InitializeComponent();

                CogDisplayStatusBar_Pad.Display = CogRecordPad;
                CogDisplayToolBar_Pad.Display = CogRecordPad;

                CogDisplayStatusBar_Right.Display = CogRecordRight;
                CogDisplayToolBar_Right.Display = CogRecordRight;

                CogDisplayStatusBar_Top.Display = CogRecordTop;
                CogDisplayToolBar_Top.Display = CogRecordTop;

                CogDisplayStatusBar_Bottom.Display = CogRecordBottom;
                CogDisplayToolBar_Bottom.Display = CogRecordBottom;
                CogDisplayStatusBar_Pad.ShowZoomPane = false;

                CogRecordPad.AutoFit = true;
                CogRecordRight.AutoFit = true;
                CogRecordBottom.AutoFit = true;
                CogRecordTop.AutoFit = true;

                CogRecordPad.AutoFitWithGraphics = true;
                CogRecordRight.AutoFitWithGraphics = true;
                CogRecordTop.AutoFitWithGraphics = true;
                CogRecordBottom.AutoFitWithGraphics = true;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
        public void RecordClear()
        {
            CogRecordPad.Record = null;
            CogRecordRight.Record = null;
            CogRecordBottom.Record = null;
            CogRecordTop.Record = null;
        }
        public void Initialize(int pc_num)
        {
            PcNum = pc_num;
        }
        public void SetRecordPad(CogRecord record, string path, string id, bool manual)
        {
            CogRecordPad.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordPad.DrawingEnabled = false;
                    CogRecordPad.Record = null; ;
                    CogRecordPad.Record = record;
                    CogRecordPad.DrawingEnabled = true;
                    CogRecordPad.Refresh();
                    if(!manual)
                     CogRecordPad.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Pad.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(PcNum, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Pad Done", true, false);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }
        public void SetRecordRight(CogRecord record, string path, string id, bool manual)
        {
            CogRecordRight.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordRight.DrawingEnabled = false;
                    CogRecordRight.Record = null;
                    CogRecordRight.Record = record;
                    CogRecordRight.DrawingEnabled = true;
                    CogRecordRight.Refresh();
                    if (!manual)
                        CogRecordRight.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Right.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(PcNum, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Right Done", true, false);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }
        public void UpdateDisplayRecord(List<DisplayData> data, string id)
        {
            CogRecordPad.Record = null;
            CogRecordRight.Record = null;
            CogRecordTop.Record = null;
            CogRecordBottom.Record = null;
            List<WriteRecordData> Records = new List<WriteRecordData>();
            foreach (DisplayData item in data)
            {
                if (!fileProc.DirExists(item.Path))
                {
                    fileProc.CreateDirectory($@"{item.Path}");
                }

                if (item.AreaIndex == 0 || item.AreaName.ToUpper().Contains("PAD"))
                {
                    try
                    {
                        CogRecordPad.Invoke(new MethodInvoker(delegate ()
                        {
                            CogRecordPad.DrawingEnabled = false;
                            CogRecordPad.Record = null; ;
                            CogRecordPad.Record = item.Result;
                            CogRecordPad.DrawingEnabled = true;
                            CogRecordPad.Refresh();
                        }));
                        //if (!item.Manual)
                        //{
    
                        //CogRecordPad.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Pad.bmp", ImageFormat.Bmp);
                        //}

                        Systems.WriteLog(PcNum, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Pad Done", true, false);
                    }
                    catch (Exception ex)
                    {
                        Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                    }

                }
                else if (item.AreaIndex == 1 || item.AreaName.ToUpper().Contains("RIGHT"))
                {

                    try
                    {
                        CogRecordRight.Invoke(new MethodInvoker(delegate ()
                        {
                            CogRecordRight.DrawingEnabled = false;
                            CogRecordRight.Record = null;
                            CogRecordRight.Record = item.Result;
                            CogRecordRight.DrawingEnabled = true;
                            CogRecordRight.Refresh();
                        }));
                        //if (!item.Manual)
                        //{
                        //WriteRecordData Temp = new WriteRecordData();
                        //Temp.Name = "RIGHT";
                        //Temp.Path = item.Path;
                        //Temp.RecordImage = CogRecordRight.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
                        //Records.Add(Temp);
                        //CogRecordRight.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Right.bmp", ImageFormat.Bmp);
                        //}
                        Systems.WriteLog(PcNum, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Right Done", true, false);
                    }
                    catch (Exception ex)
                    {
                        Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                    }

                }
                else if (item.AreaIndex == 2 || item.AreaName.ToUpper().Contains("BOTTOM"))
                {

                    try
                    {
                        CogRecordBottom.Invoke(new MethodInvoker(delegate ()
                        {
                            CogRecordBottom.DrawingEnabled = false;
                            CogRecordBottom.Record = null;
                            CogRecordBottom.Record = item.Result;
                            CogRecordBottom.DrawingEnabled = true;
                            CogRecordBottom.Refresh();

                        }));
                        //if (!item.Manual)
                        //{
                        //WriteRecordData Temp = new WriteRecordData();
                        //Temp.Name = "BOTTOM";
                        //Temp.Path = item.Path;
                        //Temp.RecordImage = CogRecordBottom.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
                        //Records.Add(Temp);
                        // CogRecordBottom.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Bottom.bmp", ImageFormat.Bmp);
                        //}
                        Systems.WriteLog(PcNum, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Bottom Done", true, false);
                    }
                    catch (Exception ex)
                    {
                        Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                    }

                }
                else if (item.AreaIndex == 3 || item.AreaName.ToUpper().Contains("TOP"))
                {

                    try
                    {
                        CogRecordTop.Invoke(new MethodInvoker(delegate ()
                        {
                            CogRecordTop.DrawingEnabled = false;
                            CogRecordTop.Record = null;
                            CogRecordTop.Record = item.Result;
                            CogRecordTop.DrawingEnabled = true;
                            CogRecordTop.Refresh();
                        }));
                        //if (!item.Manual)
                        //{
                        //WriteRecordData Temp = new WriteRecordData();
                        //Temp.Name = "TOP";
                        //Temp.Path = item.Path;

                        //Temp.RecordImage = CogRecordTop.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
                        //Records.Add(Temp);
                        // CogRecordTop.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Top.bmp", ImageFormat.Bmp);
                        // }
                        Systems.WriteLog(PcNum, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord TOP Done", true, false);
                    }
                    catch (Exception ex)
                    {
                        Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                    }
                }
            }
            //WriteRecordData Temp1 = new WriteRecordData();
            //Temp1.Name = "PAD";
            //Temp1.Path = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{id}\{Paths.RECORD_IMAGE_PATH[Globals.CurrentPCno]}";
            //Temp1.RecordImage = CogRecordPad.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
            //Records.Add(Temp1);

            //WriteRecordData Temp2 = new WriteRecordData();
            //Temp2.Name = "RIGHT";
            //Temp2.Path = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{id}\{Paths.RECORD_IMAGE_PATH[Globals.CurrentPCno]}";
            //Temp2.RecordImage = CogRecordRight.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
            //Records.Add(Temp2);

            //WriteRecordData Temp3 = new WriteRecordData();
            //Temp3.Name = "BOTTOM";
            //Temp3.Path = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{id}\{Paths.RECORD_IMAGE_PATH[Globals.CurrentPCno]}"; ;
            //Temp3.RecordImage = CogRecordBottom.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
            //Records.Add(Temp3);

            //WriteRecordData Temp4 = new WriteRecordData();
            //Temp4.Name = "TOP";
            //Temp4.Path = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{id}\{Paths.RECORD_IMAGE_PATH[Globals.CurrentPCno]}";
            //Temp4.RecordImage = CogRecordTop.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
            //Records.Add(Temp4);
            //WriteRecordImage(Records);
        }

        public Image GetImagePad()
        {
            return CogRecordPad.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
        }

        public Image GetImageRight()
        {
            return CogRecordRight.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
        }

        public Image GetImageTop()
        {
            return CogRecordTop.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
        }

        public Image GetImageBottom()
        {
            return CogRecordBottom.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image);
        }

        private void WriteRecordImage(List<WriteRecordData> data)
        {
            if (data.Count > 0)
            {
                Task t = new Task(new Action(() =>
                {

                    foreach(WriteRecordData item in data)
                    {
                        item.RecordImage.Save($@"{item.Path}\{item.Name}.bmp", ImageFormat.Bmp);
                    }                    
                }));
                t.Start();
            }
        }
        public void SaveRecords(Dictionary<string, Image> records, string path, string id)
        {

        }
        public void SetRecordBottom(CogRecord record, string path, string id, bool manual)
        {
            CogRecordBottom.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordBottom.DrawingEnabled = false;
                    CogRecordBottom.Record = null;
                    CogRecordBottom.Record = record;
                    CogRecordBottom.DrawingEnabled = true;
                    CogRecordBottom.Refresh();
                    if (!manual)
                        CogRecordBottom.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Bottom.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(PcNum, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Bottom Done", true, false);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }

        public void SetRecordTop(CogRecord record, string path, string id, bool manual)
        {
            CogRecordTop.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordTop.DrawingEnabled = false;
                    CogRecordTop.Record = null;
                    CogRecordTop.Record = record;
                    CogRecordTop.DrawingEnabled = true;
                    CogRecordTop.Refresh();
                    if (!manual)
                        CogRecordTop.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Top.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(PcNum, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord TOP Done", true, false);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }
        public void GetRecordTopImage()
        {
            CogRecordTop.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    Cognex_Helper.SaveToFile(CogRecordTop.Image as CogImage8Grey, $@"D:\TestTop.bmp");
                    Systems.WriteLog(PcNum, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord TOP Done", true, false);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }

    }
}
