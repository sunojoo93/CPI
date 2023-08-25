using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Ex_Form;
using CRUX_GUI_Cognex.Main_Form;
using CRUX_GUI_Cognex.User_Controls;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.ExceptionServices;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex
{
    static class Program
    {
        /// <summary>
        /// 가급적이면 폼 클래스만 Program의 전역으로 둔다.
        /// System 관련 기능 클래스는 Systems 클래스에 전역으로 둔다.
        /// </summary> 
        private static List<ProcessSet> ProcessList = new List<ProcessSet>();
        // 메인 폼       
        public static List<Frm_MainContent> Frm_MainContent_;
        public static Frm_Main Frm_Main;
        public static List<LogPrinter> Ui_LogPrint_Auto;
        public static List<LogPrinter> UI_LogPrint_Manual;
        // For multi ui thread
        public static Ex_Frm_Others_Loading LoadingForm;
        public static System_Information SysInfo;
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        static void Main ()
        {

            AppDomain.CurrentDomain.ProcessExit += new EventHandler(CurrentDomain_ProcessExit);

            //Application.ThreadException += new System.Threading.ThreadExceptionEventHandler(exceptionDump);
            //Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);
            //AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(CurrentDomain_UnhandledException);

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            #if !DEBUG
            {
            var Temp = IsExistProcess();
            if ( Temp != null )
            {
                DialogResult dr = MessageBox.Show($"현재 프로그램이 실행 중 입니다. \nPID : {Temp.Id}, Start Time : {Temp.StartTime} \n (Yes : 기존 프로그램 활성화) (No : 모두 종료)", "알림", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);

                IntPtr procHandler = WinApis.FindWindow(null, Temp.MainWindowTitle);
                if (dr == DialogResult.Yes)
                {
                    //활성화
                    WinApis.ShowWindow(procHandler, (int)Enums.WINDOWS_STATE.SW_SHOW);
                    WinApis.SetForegroundWindow(procHandler);
                }
                else if (dr == DialogResult.No)
                {
                    Temp.Kill();
                    Application.Exit();
                }               

                return;
            }
            #endif
            Frm_Main = new Frm_Main();
            Application.Run(Frm_Main);
        }
        public static List<ProcessSet> GetProcessList()
        {
            return ProcessList;
        }
      
        static Process IsExistProcess ()
        {
            List<Process> Process_ = Process.GetProcesses().ToList();

            string CurProcessName = Process.GetCurrentProcess().ProcessName;

            var Temp = Process_?.FindAll(x => x.ProcessName == CurProcessName) ?? null;
            if ( Temp == null )
                return null;
            if ( Temp.Count == 1 )
                return null;

            else
                return FindOldestProcess(Temp);

        }
        static Process FindOldestProcess (List<Process> procs)
        {
            var Temp = procs.OrderBy(x => x.StartTime);
            return Temp.ElementAt(0);
        }
        public static void StartVSServer ()
        {
            var procs = Process.GetProcessesByName("VisualStationServer");
            if ( procs.Length == 0 )
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\\VisualStationServer.exe");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartLight(string task_name, int task_num, string ini_name)
        {
            var procs = Process.GetProcessesByName(task_name);
            if (procs.Length == 0)
            {
                //bool Temp = fileProc.FileExists($@"D:\CRUX\DATA\INI\TestCam1.ini");
                ProcessStartInfo Psi = new ProcessStartInfo($@"D:\CRUX\EXE\{task_name}.exe", $@" {task_num} {ini_name}.ini");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartMainInterface ()
        {
            var procs = Process.GetProcessesByName("Crux_MainPcInterface");
            if ( procs.Length == 0 )
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\\Crux_MainPcInterface.exe");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartAutoFocus()
        {
            var procs = Process.GetProcessesByName("AFTS_SimplifiedExample_MFC");
            if (procs.Length == 0)
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\\AFTS_SimplifiedExample_MFC.exe", "1 AFCamera1.ini" );
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartDalsaCameraTask ()
        {
            var procs = Process.GetProcessesByName("DalsaLineCameraTask");
            if ( procs.Length == 0 )
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\\DalsaLineCameraTask.exe", "1 DalsaLineCamera1.ini");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartSimulCameraTask ()
        {
            var procs = Process.GetProcessesByName("SimulationCameraTask");
            if ( procs.Length == 0 )
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\\SimulationCameraTask.exe", " 1 SimulationCamera1.ini");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartCameraTask(string task_name, int task_num, string ini_name)
        {

                var procs = Process.GetProcessesByName(task_name);
                if (procs.Length == 0)
                {
                //bool Temp = fileProc.FileExists($@"D:\CRUX\DATA\INI\TestCam1.ini");
                    ProcessStartInfo Psi = new ProcessStartInfo($@"D:\CRUX\EXE\{task_name}.exe", $@" {task_num} {ini_name}.ini");
                    Psi.WindowStyle = ProcessWindowStyle.Minimized;
                    var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                    ProcessList.Add(temp);
                }
            
        }
        public static void StartTrivisionLight ()
        {
            var procs = Process.GetProcessesByName("TrivisionLightTask");
            if ( procs.Length == 0 )
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\TrivisionLightTask.exe", "1 TrivisionLight.ini");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartLLightLight ()
        {
            var procs = Process.GetProcessesByName("LLightLightTask");
            if ( procs.Length == 0 )
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\LLightLightTask.exe", "2 LLightLight.ini");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartSequence ()
        {
            var procs = Process.GetProcessesByName("VSSequenceTask");
            if ( procs.Length == 0 )
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\\VSSequenceTask.exe");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartAlgorithm ()
        {
            var procs = Process.GetProcessesByName("VSAlgorithmTask");
            if ( procs.Length == 0 )
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\\VSAlgorithmTask.exe");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void StartSpaceManager ()
        {
            var procs = Process.GetProcessesByName("SpaceManagerCS");
            if ( procs.Length == 0 )
            {
                ProcessStartInfo Psi = new ProcessStartInfo(@"D:\CRUX\EXE\\SpaceManagerCS.exe");
                Psi.WindowStyle = ProcessWindowStyle.Minimized;
                var temp = new ProcessSet(Process.Start(Psi), Enums.WINDOWS_STATE.SW_MINIMIZE);
                ProcessList.Add(temp);
            }
        }
        public static void KillTasks ()
        {
            try
            {
                Process[] procList;
                procList = Process.GetProcessesByName("VisualStationServer");
                foreach (var proc in procList)
                    proc.Kill();
                procList = Process.GetProcessesByName("Crux_MainPcInterface");
                foreach (var proc in procList)
                    proc.Kill();
                procList = Process.GetProcessesByName("TestCamTask");
                foreach (var proc in procList)
                    proc.Kill();
                procList = Process.GetProcessesByName("TrivisionLightTask");
                foreach ( var proc in procList )
                    proc.Kill();
                procList = Process.GetProcessesByName("LLightLightTask");
                foreach ( var proc in procList )
                    proc.Kill();
                procList = Process.GetProcessesByName("VSSequenceTask");
                foreach ( var proc in procList )
                    proc.Kill();
                procList = Process.GetProcessesByName("VSAlgorithmTask");
                foreach ( var proc in procList )
                    proc.Kill();
                procList = Process.GetProcessesByName("SpaceManagerCS");
                foreach ( var proc in procList )
                    proc.Kill();
                procList = Process.GetProcessesByName("SimulationCameraTask");
                foreach ( var proc in procList )
                    proc.Kill();
                procList = Process.GetProcessesByName("NeepsLightTask");
                foreach (var proc in procList)
                    proc.Kill();
                procList = Process.GetProcessesByName("DalsaLineCameraTask");
                foreach (var proc in procList)
                    proc.Kill();
                procList = Process.GetProcessesByName("IPulsLightTask");
                foreach (var proc in procList)
                    proc.Kill();
                procList = Process.GetProcessesByName("AFTS_SimplifiedExample_MFC");
                foreach (var proc in procList)
                    proc.Kill();
                procList = Process.GetProcessesByName("VistekCameraTask");
                foreach (var proc in procList)
                    proc.Kill();
            }
            catch(Exception ex)
            {
                throw ex;
            }
        }

        public static void KillAllTask ()
        {
            try
            {
                foreach (var proc in ProcessList)
                {
                    proc.Proc.Kill();                    
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", false, false);
            }
        }
        public static ProcessSet GetProcess(string name)
        {
            return ProcessList.Find(x => x.Proc.ProcessName == name);
        }
        static void CurrentDomain_ProcessExit(object sender, EventArgs e)
        {
            KillTasks();
            //Systems.LogWriter.Info("Process Exit");
            Systems.WriteLog(0, Enums.LogLevel.ERROR, "[ GUI ] Process Exit", false, false);
        }

        ////이벤트 클래스(처리되지 않은 예외)
        //static void CurrentDomain_UnhandledException (object sender, UnhandledExceptionEventArgs e)
        //{
        //    //Exception e = args.Exception;
        //    //Console.WriteLine("errMsg: " + e.Message);
        //    //Console.WriteLine("errPos: " + e.TargetSite);

        //    //덤프 파일 경로 설정(MinidumpHelp.cs 에서도 수정)
        //    //MinidumpHelp.Minidump.install_self_mini_dump(Application.StartupPath);

        //    MinidumpHelp.Minidump.install_self_mini_dump();
        //}
        ////이벤트 클래스(UI 예외)
        //static void exceptionDump (object sender, System.Threading.ThreadExceptionEventArgs args)
        //{
        //    //Exception e = args.Exception;
        //    //Console.WriteLine("errMsg: " + e.Message);
        //    //Console.WriteLine("errPos: " + e.TargetSite);

        //    //덤프 파일 경로 설정(MinidumpHelp.cs 에서도 수정)
        //    //MinidumpHelp.Minidump.install_self_mini_dump(Application.StartupPath);

        //    MinidumpHelp.Minidump.install_self_mini_dump();
        //}
        //[HandleProcessCorruptedStateExceptions]
        //static public void HandleCorruptedStateException ()
        //{
        //    try
        //    {
        //        var ptr = new IntPtr(42);
        //        Marshal.StructureToPtr(42, ptr, true);
        //    }
        //    catch ( Exception ex )
        //    {
        //        Debug.WriteLine(ex.Message);
        //    }
        //}

    }
}
