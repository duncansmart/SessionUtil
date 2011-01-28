using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;

namespace LogoffUser
{
    class Program
    {
        static int Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("Usage: logoffuser username1 [username2 ...]");
                return 1;
            }

            foreach (string username in args)
            {
                try
                {
                    var session = RemoteDesktopServerUtil.GetSessions()
                        .FirstOrDefault(s => s.UserName != null && s.UserName.Equals(username, StringComparison.CurrentCultureIgnoreCase));

                    if (session == null)
                    {
                        Console.WriteLine("Session for '{0}' not found.", username);
                        continue;
                    }

                    session.LogOff(true);

                    Console.WriteLine("{0}'s session logged off.", session.UserName);
                }
                catch (Exception error)
                {
                    Console.WriteLine(error.Message);
                    return 2;
                }
            }
            return 0;
        }
    }
}
