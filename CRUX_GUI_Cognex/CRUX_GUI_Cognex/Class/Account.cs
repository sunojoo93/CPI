using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CRUX_GUI_Cognex.Class
{
    class AccountManager
    {
        private static AccountManager Account_Collection;
        private static string UserDataPath = $@"D:\CRUX\DATA\INI\UserData.xml";
        private static List<User> UserList;

        private static User CurrentUser;

        public static AccountManager Instance()
        {
            if(Account_Collection == null)            
                Account_Collection = new AccountManager(UserDataPath);
            
            return Account_Collection;
        }

        private AccountManager(string data_path)
        {
            if (!fileProc.FileExists(data_path))
            {
                UserList = new List<User>();
                User Man = new User();
                UserList.Add(Man);
                CurrentUser = Man;
            }                
            else
            {

            }
        }
        public User GetCurrentAccount()
        {
            return CurrentUser;
        }
    }
    public class User
    {
        public string CreateDate { get; set; } = DateTime.Now.ToString("yyyy-MM-dd").ToString();
        public string Id { get; set; } = "Guest";
        public string Pw { get; set; } = "Guest";
        public Enums.AccountLevel Authority { get; set; } = Enums.AccountLevel.Guest;
    }
}
