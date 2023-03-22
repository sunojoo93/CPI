using Cognex.VisionPro.QuickBuild;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CRUX_Renewal.Utils
{
    public static class Cognex_Helper
    {
        public static T GetJobList<T>(CogJobManager manager) where T : List<string> ,new ()
        {
            T Temp =null;
            for(int i = 0; i < manager.JobCount; ++i)
            {
                Temp = Temp ?? new T();
                Temp.Add(manager.Job(i).Name);
            }
            return Temp;
        }
        public static List<CogJob> GetJobs(CogJobManager manager)
        {
            List<CogJob> Temp = null;
            for (int i = 0; i < manager.JobCount; ++i)
            {
                Temp = Temp ?? new List<CogJob>();
                Temp.Add(manager.Job(i));
            }
            return Temp;
        }
        public static void ChangeJobName(CogJobManager manager, string source_name, string result_name)
        {
            for (int i = 0; i < manager.JobCount; ++i)
                if (manager.Job(i).Name == source_name)
                    manager.Job(i).Name = result_name;
        }
        public static void DeleteJob(CogJobManager manager, string name)
        {
            manager?.JobRemove(name);
        }
        public static void DeleteJob(CogJobManager manager, CogJob job)
        {
            manager?.JobRemove(job);
        }
        public static CogJob GetJob(CogJobManager manager, string name)
        {
            return manager?.Job(name);
        }
        public static CogJob GetJob(CogJobManager manager, int idx)
        {
            return manager?.Job(idx);
        }
    }
}
