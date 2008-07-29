using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

using cisco.uc;

namespace vmailnotifier
{
    public delegate void OnNewVoiceMailMessageDelegate(string cid, string time, long mid);

    public class VmailNotifierClientImpl : MessagingClient
    {
        public event OnNewVoiceMailMessageDelegate onNewVoiceMailMessage = null;

        public void notifyMailStored(long? mailstoreId, cisco.uc.ConstsMessaging.Mail mail)
        {    
            string cid = mail.from;
            DateTime d = new DateTime(mail.receivedOn.Value, DateTimeKind.Utc);
            string time = d.ToShortDateString() + " " + d.ToShortTimeString();
            if (this.onNewVoiceMailMessage != null)
                this.onNewVoiceMailMessage(cid, time, mail.id.Value);
        }

        public void bogus()
        {
        }
    }
}
