using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Data;

using Metreos.AxlSoap504;



namespace etch.examples.ccmuser
{
    public class CcmUserServerImpl : CcmUserServer
    {
        public CcmUserClient client;

        private AXLAPIService axlClient;
        
        public CcmUserServerImpl(CcmUserClient client)
        {
            this.client = client;
        }

        public void login(string serverIp, string user, string password)
        {
            axlClient = new AXLAPIService(serverIp, user, password);
           
        }

        
        public ConstsCcmUser.UserConfiguration getUserConfig(string user)
        {
            executeSQLQuery request = new executeSQLQuery();
            request.sql = "select t1.name, t1.pkid, t3.devicetype, t1.description, t1.label, t1.speeddialnumber from table(multiset(select  name, label, speeddialnumber, description, tkmodel, speeddial.pkid from device left outer join speeddial on speeddial.fkdevice = device.pkid)) t1, table(multiset(select name, userid from device left outer join enduserdevicemap_cmvw on enduserdevicemap_cmvw.devicepkid = device.pkid)) t2, table(multiset(select name devicetype, enum from typemodel)) t3 where t1.name = t2.name and t1.name like 'SEP%' and userid = '" + user + "' and t1.tkmodel = t3.enum order by t1.name";

            executeSQLQueryResponse response = null;
            try
            {
                response = axlClient.executeSQLQuery(request);
            }
            catch(System.Web.Services.Protocols.SoapException e)
            {
                Console.WriteLine("SoapException: " + e);
                return null;
            }
            catch(Exception e)
            {
                Console.WriteLine("Exception: " + e);
                return null;
            }

            DataTable table = ParseToTable(response);

            ConstsCcmUser.UserConfiguration userConfig = new ConstsCcmUser.UserConfiguration();

            string currentDevice = null;
            ConstsCcmUser.Device thisDevice = null;
            List<ConstsCcmUser.Device> devices = new List<ConstsCcmUser.Device>();
            List<ConstsCcmUser.Speeddial> speeddials = new List<ConstsCcmUser.Speeddial>();

            if(table != null)
            {
                foreach(DataRow row in table.Rows)
                {
                    string deviceName = ParseDb(row["name"]);
                    string deviceType = ParseDb(row["devicetype"]);
                    string deviceDescription = ParseDb(row["description"]);
                    string speeddialLabel = ParseDb(row["label"]);
                    string speeddialNumber = ParseDb(row["speeddialnumber"]);
                    string speeddialPkid = ParseDb(row["pkid"]);

                    // is device new?
                    if(deviceName != currentDevice)
                    {
                        // before moving on, apply speeddials to last device
                        if(thisDevice != null && speeddials.Count > 0)
                        {
                            thisDevice.speeddials = speeddials.ToArray();
                            speeddials.Clear();
                        }

                        // also before moving on, add the last device to list
                        if(thisDevice != null)
                        {
                            devices.Add(thisDevice);
                        }

                        thisDevice = new ConstsCcmUser.Device();
                        thisDevice.name = deviceName;
                        thisDevice.description = deviceDescription;
                        thisDevice.type = deviceType;

                        currentDevice = deviceName;
                    }

                    if(speeddialNumber != null)
                    {
                        ConstsCcmUser.Speeddial speeddial = new ConstsCcmUser.Speeddial();
                        speeddial.number = speeddialNumber;
                        speeddial.label = speeddialLabel;
                        speeddial.pkid = speeddialPkid;
                        speeddials.Add(speeddial);
                    }
                }


                // one last time as loop leaves
                // before moving on, apply speeddials to last device
                if(thisDevice != null && speeddials.Count > 0)
                {
                    thisDevice.speeddials = speeddials.ToArray();
                    speeddials.Clear();
                }

                // also before moving on, add the last device to list
                if(thisDevice != null)
                {
                    devices.Add(thisDevice);
                }
            }

            if(devices.Count > 0)
            {
                userConfig.devices = devices.ToArray();

            }

            // THE FOLLOWING LINES WILL CAUSE ISSUE TOO
            //ConstsCcmUser.Device device = new ConstsCcmUser.Device();
            //ConstsCcmUser.UserConfiguration myUser = new ConstsCcmUser.UserConfiguration();
            //myUser.devices = new ConstsCcmUser.Device[1];
            //myUser.devices[0] = device;

            return userConfig;
        }

        public void setSpeeddialLabel(string pkid, string newLabel)
        {
            // label is key field (what to do about labelascii?)
            executeSQLUpdate update = new executeSQLUpdate();

            // test value: 76ed1232-3025-e269-dd52-55ba23f4a999
            update.sql = "update speeddial set label = '" + newLabel + "' where speeddial.pkid = '" + pkid + "'";

            executeSQLUpdateResponse response = null;
            try
            {
                response = axlClient.executeSQLUpdate(update);
            }
            catch(System.Web.Services.Protocols.SoapException e)
            {
                Console.WriteLine("SoapException: " + e);
            }
            catch(Exception e)
            {
                Console.WriteLine("Exception: " + e);
            }
        }

        public void setSpeeddialNumber(string pkid, string newNumber)
        {
            // speeddialnumber is key field
            executeSQLUpdate update = new executeSQLUpdate();

            // test value: 76ed1232-3025-e269-dd52-55ba23f4a999
            update.sql = "update speeddial set speeddialnumber = '" + newNumber + "' where pkid = '" + pkid + "'";

            executeSQLUpdateResponse response = null;
            try
            {
                response = axlClient.executeSQLUpdate(update);
            }
            catch(System.Web.Services.Protocols.SoapException e)
            {
                Console.WriteLine("SoapException: " + e);
            }
            catch(Exception e)
            {
                Console.WriteLine("Exception: " + e);
            }
        }

        public void logout()
        {
            if(axlClient != null)
            {
                try
                {
                    axlClient.Channel.Close();
                }
                catch(Exception e)
                {
                    Console.WriteLine("Unable to close channel to AXL server: " + e);
                }
            }
        }
        private string ParseDb(object value)
        {
            if(value != null && value != DBNull.Value)
            {
                return value.ToString();
            }
            else
            {
                return null;
            }
        }

        private DataTable ParseToTable(executeSQLQueryResponse response)
        {
            DataTable table = new DataTable();
            if(response == null || response.@return == null || response.@return.Length == 0)
            {
                // No items in response, do nothing
                return null;
            }
            else
            {
                object[] rows = response.@return;

                bool firstPass = true;

                foreach(object row in rows)
                {
                    if(row != null)
                    {
                        // Build rows

                        if(row is System.Array)
                        {
                            Array elements = row as Array;

                            if(firstPass)
                            {
                                foreach(object element in elements)
                                {
                                    if(element is XmlElement)
                                    {
                                        XmlElement xmlNode = element as XmlElement;
                                        string columnName = xmlNode.Name;
                                        table.Columns.Add(columnName, typeof(string));
                                    }
                                }
                                firstPass = false;
                            }

                            DataRow newRow = table.NewRow();

                            foreach(object element in elements)
                            {
                                if(element is XmlElement)
                                {
                                    XmlElement xmlNode = element as XmlElement;

                                    string columnName = xmlNode.Name;
                                    string columnValue = null;
                                    if(!xmlNode.IsEmpty) // empty tag means null, (<tag/>), full tag means value or empty string (<tag></tag>)
                                    {
                                        columnValue = xmlNode.InnerText;
                                    }

                                    newRow[columnName] = columnValue;
                                }
                            }

                            table.Rows.Add(newRow);
                        }
                        else
                        {
                            // assume 1 element return

                            DataRow newRow = table.NewRow();

                            if(row is XmlElement)
                            {
                                if(firstPass)
                                {
                                    XmlElement xmlNode = row as XmlElement;
                                    string columnName = xmlNode.Name;
                                    table.Columns.Add(columnName);
                                    firstPass = false;
                                }

                                XmlElement xmlNode2 = row as XmlElement;

                                string columnName2 = xmlNode2.Name;
                                string columnValue2 = null;
                                if(!xmlNode2.IsEmpty) // empty tag means null, (<tag/>), full tag means value or empty string (<tag></tag>)
                                {
                                    columnValue2 = xmlNode2.InnerText;
                                }

                                newRow[columnName2] = columnValue2;
                            }

                            table.Rows.Add(newRow);
                            firstPass = false;
                        }
                    }
                }
            }

            return table;
        }
 
    }
}
