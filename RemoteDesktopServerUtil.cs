using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace LogoffUser
{
    class RemoteDesktopServerUtil
    {
        #region PInvoke
        [DllImport("wtsapi32.dll")]
        static extern bool WTSLogoffSession(IntPtr hServer, int sessionId, bool bWait);

        [DllImport("wtsapi32.dll", SetLastError = true)]
        static extern int WTSEnumerateSessionsEx(
            IntPtr hServer,
            ref int pLevel,
            int filter,
            out IntPtr ppSessionInfo,
            out int pCount
            );

        [DllImport("wtsapi32.dll")]
        static extern void WTSFreeMemoryEx(IntPtr pMemory);
        #endregion

        /// <summary>
        /// WTS_SESSION_INFO_1
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public class SessionInfo
        {
            public readonly int ExecEnvId;
            public readonly ConnectionState State;
            public readonly int SessionId;
            public readonly string SessionName;
            public readonly string HostName;
            public readonly string UserName;
            public readonly string DomainName;
            public readonly string FarmName;

            public void LogOff(bool wait)
            {
                WTSLogoffSession(IntPtr.Zero, this.SessionId, wait);
            }
        }

        /// <summary>
        /// WTS_CONNECTSTATE_CLASS
        /// </summary>
        public enum ConnectionState
        {
            Active, Connected, ConnectQuery, Shadow, Disconnected, Idle, Listen, Reset, Down, Init
        }

        public static IEnumerable<SessionInfo> GetSessions()
        {
            IntPtr pSessionInfo;
            int count;
            int level = 1;
            int result = WTSEnumerateSessionsEx(IntPtr.Zero, ref level, 0, out pSessionInfo, out count);
            if (result == 0)
                throw new ApplicationException("WTSEnumerateSessionsEx error: " + new Win32Exception().Message);

            try
            {
                IntPtr pCurrent = pSessionInfo;
                for (int i = 0; i < count; i++)
                {
                    SessionInfo info = (SessionInfo)Marshal.PtrToStructure(pCurrent, typeof(SessionInfo));
                    yield return info;
                    pCurrent += Marshal.SizeOf(info);
                }
            }
            finally
            {
                WTSFreeMemoryEx(pSessionInfo);
                pSessionInfo = IntPtr.Zero;
            }
        }

    }
}
