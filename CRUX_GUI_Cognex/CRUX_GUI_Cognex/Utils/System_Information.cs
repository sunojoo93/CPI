using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Management;
using System.Collections.ObjectModel;
using System.Threading;
using System.IO;

namespace CRUX_GUI_Cognex.Utils
{
    class System_Information
    {
        public PCClass PC = new PCClass();
        private Thread thread;
        PerformanceCounter cpu = new PerformanceCounter("Processor Information", "% Processor Utility", "0,_Total");
        public System_Information()
        {
            thread = new Thread(ScanStatus);
            thread.IsBackground = true;

            thread.Start();
        }
        private async void ScanStatus()
        {
            try
            {
                do
                {
                    await Task.Delay(500);
                    ScanHddDisk();
                    ScanMemory();
                    ScanCPU();
                } while (true);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
        private void ScanHddDisk()
        {
            DriveInfo[] hddDrives = DriveInfo.GetDrives();
            ObservableCollection<HardDiskClass> tmpHdd = new ObservableCollection<HardDiskClass>();
            HardDiskClass hdd;
            foreach (DriveInfo drive in hddDrives)
            {
                if (drive.DriveType == DriveType.Fixed)
                {

                    hdd = new HardDiskClass();
                    hdd.Name = drive.Name.ToString();
                    hdd.TotalSize = drive.TotalSize / 1024 / 1024 / 1024;
                    hdd.FreeSize = drive.AvailableFreeSpace / 1024 / 1024 / 1024;
                    hdd.UseSize = (drive.TotalSize - drive.AvailableFreeSpace) / 1024 / 1024 / 1024;

                    tmpHdd.Add(hdd);
                }
            }
            PC.HardDisk = tmpHdd;
        }

        private void ScanMemory()
        {
            var wmi = new ManagementObjectSearcher("select * from Win32_OperatingSystem");
            var info = wmi.Get().Cast<ManagementObject>().Select(mo => new MemoryClass()
            {
                TotalSize = Math.Round(double.Parse(mo["TotalVisibleMemorySize"].ToString()) / 1024 / 1024, 2),
                FreeSize = Math.Round(double.Parse(mo["FreePhysicalMemory"].ToString()) / 1024 / 1024, 2),
                UseSize = Math.Round((double.Parse(mo["TotalVisibleMemorySize"].ToString()) - double.Parse(mo["FreePhysicalMemory"].ToString())) / 1024 / 1024, 2)
            }).FirstOrDefault();

            PC.Memory = info;
        }

        private void ScanCPU()
        {
            var sValue = Math.Round(cpu.NextValue(), 2);
            PC.CpuUsage = sValue;
        }

        public class PCClass
        {
            // CPU 사용량
            private double _cpuUsage;

            // Memory 사용량
            private MemoryClass _memory = new MemoryClass();

            // HDD (2개 이상 될수 있기 때문에 ObservableCollection에 정보를 저장)
            private ObservableCollection<HardDiskClass> _hardDisk = new ObservableCollection<HardDiskClass>();
            public double CpuUsage
            {
                get { return _cpuUsage; }
                set { _cpuUsage = value; }
            }
            public MemoryClass Memory
            {
                get { return _memory; }
                set { _memory = value; }
            }
            public ObservableCollection<HardDiskClass> HardDisk
            {
                get { return _hardDisk; }
                set { _hardDisk = value; }
            }

        }

        public class MemoryClass
        {
            // 메모리 총 용량
            private double _totalSize = 0;
            // 여유 공간
            private double _freeSize = 0;
            // 사용 공간
            private double _useSize = 0;
            public double TotalSize
            {
                get { return _totalSize; }
                set { _totalSize = value; }
            }
            public double FreeSize
            {
                get { return _freeSize; }
                set
                {
                    _freeSize = value;
                }
            }
            public double UseSize
            {
                get { return _useSize; }
                set { _useSize = value; }
            }
        }
        public class HardDiskClass
        {
            //하드디스크 이름
            private string _name = string.Empty;
            // Total 용량
            private long _totalSize = 0;
            // 여유 공간
            private long _freeSize = 0;
            // 사용 공간
            private long _useSize = 0;

            public string Name
            {
                get { return _name; }
                set { _name = value; }
            }
            public long TotalSize
            {
                get { return _totalSize; }
                set { _totalSize = value; }
            }

            public long FreeSize
            {
                get { return _freeSize; }
                set
                {
                    _freeSize = value;
                }
            }
            public long UseSize
            {
                get { return _useSize; }
                set { _useSize = value; }
            }
        }
    }
}
