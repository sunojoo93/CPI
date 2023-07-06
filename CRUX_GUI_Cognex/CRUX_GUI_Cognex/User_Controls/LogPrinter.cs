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

namespace CRUX_GUI_Cognex.User_Controls
{
    public partial class LogPrinter : UserControl
    {
        string CtrlName = string.Empty;
        ConcurrentQueue<LogData> MessageQueue = new ConcurrentQueue<LogData>();
        CancellationTokenSource TokenSource = new CancellationTokenSource();
        const int MAX_LogLine = 1000;
        object obj = new object();
        bool AutoScroll = true;
        
        public LogPrinter()
        {
            InitializeComponent();
        }
        public void StartTaskDequeue(CancellationToken token)
        {
            while (true && !token.IsCancellationRequested)
            {
                LogData Msg = null;
                if (MessageQueue.TryDequeue(out Msg))
                {
                    if (Msg.Lv == LogLevel.INFO)
                    {
                        LstB_InspLog.Invoke(new MethodInvoker(delegate ()
                        {
                            LstB_InspLog.Items.Add(Msg.Data);
                            if (AutoScroll)
                                LstB_InspLog.SelectedIndex = LstB_InspLog.Items.Count - 1;
                            if (LstB_InspLog.Items.Count > MAX_LogLine)
                                LstB_InspLog.Items.Clear();
                        }));
                    }
                    else if(Msg.Lv == LogLevel.OPERATION)
                    {
                        LstB_OpLog.Invoke(new MethodInvoker(delegate ()
                        {
                            LstB_OpLog.Items.Add(Msg.Data);
                            if (AutoScroll)
                                LstB_OpLog.SelectedIndex = LstB_OpLog.Items.Count - 1;
                            if (LstB_OpLog.Items.Count > MAX_LogLine)
                                LstB_OpLog.Items.Clear();
                        }));
                    }
                }
                Thread.Sleep(5);
            }
        }
        public void ReStartTaskDequeue()
        {
            TokenSource = new CancellationTokenSource();
            Thread DequeueWork = new Thread(() => { StartTaskDequeue(TokenSource.Token); });
            DequeueWork.Start();
        }
        public void Enqueue(LogLevel lv, string data)
        {
            if (!TokenSource.Token.IsCancellationRequested)
            {
                LogData Temp = new LogData();
                Temp.Lv = lv;
                Temp.Data = data;
                lock (obj)
                {
                    MessageQueue.Enqueue(Temp);
                }
            }
        }

        private void Chk_LogPrint_CheckedChanged(object sender, EventArgs e)
        {
            bool Manual = false;
            if ((string)Tag == "Auto")
                Manual = false;
            else
                Manual = true;

            if (Chk_LogPrint.CheckState == CheckState.Checked)
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + "Checkd", true, Manual);
                ReStartTaskDequeue();
            }
            else if (Chk_LogPrint.CheckState == CheckState.Unchecked)
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + "UnCheckd", true, Manual);
                TokenSource.Cancel();
            }
        }

        private void Chk_LogAutoScroll_CheckedChanged(object sender, EventArgs e)
        {
            bool Manual = false;
            if ((string)Tag == "Auto")
                Manual = false;
            else
                Manual = true;

            if (Chk_LogAutoScroll.CheckState == CheckState.Checked)
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + "Checkd", true, Manual);
                AutoScroll = true;
            }
            else if (Chk_LogAutoScroll.CheckState == CheckState.Unchecked)
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + "UnCheckd", true, Manual);
                AutoScroll = false;
            }
        }


    }
}
