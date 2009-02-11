// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.Diagnostics;
using Extensibility;
using EnvDTE;
using EnvDTE80;
using Microsoft.VisualStudio.CommandBars;

namespace Etch
{
	/// <summary>The object for implementing an Add-in.</summary>
	/// <seealso class='IDTExtensibility2' />
    public class Connect : IDTExtensibility2, IDTCommandTarget
	{
		/// <summary>Implements the constructor for the Add-in object. Place your initialization code within this method.</summary>
		public Connect()
		{
		}

        /// <summary>
        /// FindNamedCommand
        /// </summary>
        /// <param name="commandName">command name of the addin</param>
        /// <param name="command">the command to find</param>
        /// <returns></returns>
        public bool FindNamedCommand(string commandName, out Command command)
        {
            try
            {
                Commands commands = _applicationObject.Commands;
                command = commands.Item(commandName, -1);
                return true;
            }
            catch (System.Exception)
            {
            }
            command = null;
            return false;
        }

		/// <summary>Implements the OnConnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being loaded.</summary>
		/// <param term='application'>Root object of the host application.</param>
		/// <param term='connectMode'>Describes how the Add-in is being loaded.</param>
		/// <param term='addInInst'>Object representing this Add-in.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnConnection(object application, ext_ConnectMode connectMode, object addInInst, ref Array custom)
		{
			_applicationObject = (DTE2)application;
			_addInInstance = (AddIn)addInInst;

            if (connectMode == Extensibility.ext_ConnectMode.ext_cm_Startup)
            {
                Command command, commandS, commandC, commandX;
                CommandBar itemCmdBar;
                object[] contextGUIDS = new object[] { };

                try
                {
                    itemCmdBar = ((CommandBars)_applicationObject.CommandBars)["Item"];
                    if (itemCmdBar == null)
                    {
                        System.Windows.Forms.MessageBox.Show("Cannot get Solution Explorer Item menubar", "Error",
                            System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
                    }
                    else
                    {
                        // Create a Command with name Etch and then add it to the "Item" menubar for the SolutionExplorer
                        if (!FindNamedCommand("Etch.Connect.Etch", out command))
                        {
                            command = _applicationObject.Commands.AddNamedCommand(_addInInstance, "Etch", "Etch (Both)", "Generate Etch bindings", true, 6, ref contextGUIDS, (int)vsCommandStatus.vsCommandStatusSupported + (int)vsCommandStatus.vsCommandStatusEnabled);
                            CommandBarControl cb = command.AddControl(itemCmdBar, 4) as CommandBarControl;
                            cb.BeginGroup = true;
                        }
                        if (!FindNamedCommand("Etch.Connect.EtchS", out commandS))
                        {
                            commandS = _applicationObject.Commands.AddNamedCommand(_addInInstance, "EtchS", "Etch (Server)", "Executes Etch server binding", true, 39, ref contextGUIDS, (int)vsCommandStatus.vsCommandStatusSupported + (int)vsCommandStatus.vsCommandStatusEnabled);
                            commandS.AddControl(itemCmdBar, 5);
                        }
                        if (!FindNamedCommand("Etch.Connect.EtchC", out commandC))
                        {
                            commandC = _applicationObject.Commands.AddNamedCommand(_addInInstance, "EtchC", "Etch (Client)", "Executes Etch client binding", true, 41, ref contextGUIDS, (int)vsCommandStatus.vsCommandStatusSupported + (int)vsCommandStatus.vsCommandStatusEnabled);
                            commandC.AddControl(itemCmdBar, 6);
                        }
                        /*
                        if(!FindNamedCommand("Etch.Connect.EtchM", out commandM))
                        {
                            commandM = _applicationObject.Commands.AddNamedCommand(_addInInstance, "EtchM", "Etch Mixin (Both)", "Executes Etch Mixin bindings", true, 6, ref contextGUIDS, (int) vsCommandStatus.vsCommandStatusSupported + (int) vsCommandStatus.vsCommandStatusEnabled);
                            commandM.AddControl(itemCmdBar, 7);
                        }
                        if(!FindNamedCommand("Etch.Connect.EtchMS", out commandMS))
                        {
                            commandMS = _applicationObject.Commands.AddNamedCommand(_addInInstance, "EtchMS", "Etch Mixin (Server)", "Executes Etch Mixin server binding", true, 39, ref contextGUIDS, (int) vsCommandStatus.vsCommandStatusSupported + (int) vsCommandStatus.vsCommandStatusEnabled);
                            commandMS.AddControl(itemCmdBar, 8);
                        }
                        if(!FindNamedCommand("Etch.Connect.EtchMC", out commandMS))
                        {
                            commandMC = _applicationObject.Commands.AddNamedCommand(_addInInstance, "EtchMC", "Etch Mixin (Client)", "Executes Etch Mixin client binding", true, 41, ref contextGUIDS, (int) vsCommandStatus.vsCommandStatusSupported + (int) vsCommandStatus.vsCommandStatusEnabled);
                            commandMC.AddControl(itemCmdBar, 9);
                        }
                        */ 
                        if(!FindNamedCommand("Etch.Connect.EtchX", out commandX))
                        {
                            commandX = _applicationObject.Commands.AddNamedCommand(_addInInstance, "EtchX", "Etch (XML)", "Executes Etch XML binding", true, 6, ref contextGUIDS, (int) vsCommandStatus.vsCommandStatusSupported + (int) vsCommandStatus.vsCommandStatusEnabled);
                            commandX.AddControl(itemCmdBar, 10);
                        }
                    }
                }
                catch (Exception ex)
                {
                    System.Windows.Forms.MessageBox.Show(ex.ToString(), "Error",
                        System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
                }
            }			
		}

		/// <summary>Implements the OnDisconnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being unloaded.</summary>
		/// <param term='disconnectMode'>Describes how the Add-in is being unloaded.</param>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnDisconnection(ext_DisconnectMode disconnectMode, ref Array custom)
		{
		}

		/// <summary>Implements the OnAddInsUpdate method of the IDTExtensibility2 interface. Receives notification when the collection of Add-ins has changed.</summary>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />		
		public void OnAddInsUpdate(ref Array custom)
		{
		}

		/// <summary>Implements the OnStartupComplete method of the IDTExtensibility2 interface. Receives notification that the host application has completed loading.</summary>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnStartupComplete(ref Array custom)
		{
		}

		/// <summary>Implements the OnBeginShutdown method of the IDTExtensibility2 interface. Receives notification that the host application is being unloaded.</summary>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnBeginShutdown(ref Array custom)
		{
		}
		
		private DTE2 _applicationObject;
		private AddIn _addInInstance;

        #region IDTCommandTarget Members

        public void Exec(string CmdName, vsCommandExecOption ExecuteOption, ref object VariantIn, ref object VariantOut, ref bool Handled)
        {
            Handled = false;
            if (ExecuteOption == vsCommandExecOption.vsCommandExecOptionDoDefault)
            {
                if (CmdName == "Etch.Connect.Etch" || CmdName == "Etch.Connect.EtchS" || CmdName == "Etch.Connect.EtchC" ||
                    CmdName == "Etch.Connect.EtchM" || CmdName == "Etch.Connect.EtchMS" || CmdName == "Etch.Connect.EtchMC" ||
                    CmdName == "Etch.Connect.EtchX")
                {
                    try
                    {
                        UIHierarchy uih = _applicationObject.ToolWindows.SolutionExplorer;
                        UIHierarchyItem item = (uih.SelectedItems as System.Array).GetValue(0) as UIHierarchyItem;
                        ProjectItem prjItem = item.Object as ProjectItem;
                        string path = prjItem.Properties.Item("FullPath").Value.ToString();
                        string dir = System.IO.Path.GetDirectoryName(path);

                        ProcessStartInfo psi = new ProcessStartInfo();
                        psi.UseShellExecute = true;
                        psi.WorkingDirectory = dir;
                        psi.FileName = "etch.bat";

                        if(CmdName == "Etch.Connect.EtchX")
                        {
                            psi.Arguments = "-b xml -w all ";
                        }
                        else
                        {
                            psi.Arguments = "-b csharp ";

                            if(CmdName == "Etch.Connect.EtchC")
                                psi.Arguments += "-w client,main,impl,helper ";
                            else if(CmdName == "Etch.Connect.EtchS")
                                psi.Arguments += "-w server,main,impl,helper ";
                            else if(CmdName == "Etch.Connect.Etch")
                                psi.Arguments += "-w all ";
                        }

                        psi.Arguments += "\"" + path + "\"";

                        System.Diagnostics.Process p = System.Diagnostics.Process.Start(psi);
                        p.WaitForExit(60000);                        
                    }
                    catch (Exception ex)
                    {
                        System.Windows.Forms.MessageBox.Show(ex.ToString(), "Error",
                            System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
                    }

                    Handled = true;
                    return;
                }
            }
        }

        public void QueryStatus(string CmdName, vsCommandStatusTextWanted NeededText, ref vsCommandStatus StatusOption, ref object CommandText)
        {
            if (NeededText == vsCommandStatusTextWanted.vsCommandStatusTextWantedNone)
            {
                if (CmdName == "Etch.Connect.Etch" || CmdName == "Etch.Connect.EtchS" || CmdName == "Etch.Connect.EtchC" ||
                    CmdName == "Etch.Connect.EtchM" || CmdName == "Etch.Connect.EtchMS" || CmdName == "Etch.Connect.EtchMC" || CmdName == "Etch.Connect.EtchX")
                {
                    //Dynamically enable & disable the command. If the selected file name is *.etch, then make the command visible.
                    UIHierarchy uih = _applicationObject.ToolWindows.SolutionExplorer;
                    UIHierarchyItem item = (uih.SelectedItems as System.Array).GetValue(0) as UIHierarchyItem;
                    StatusOption = vsCommandStatus.vsCommandStatusSupported;
                    string name = item.Name.ToLower();
                    if (!name.EndsWith(".etch"))
                    {
                        StatusOption |= vsCommandStatus.vsCommandStatusInvisible;
                    }
                    else
                    {
                        StatusOption |= vsCommandStatus.vsCommandStatusEnabled;
                    }
                }
                else
                {
                    StatusOption = vsCommandStatus.vsCommandStatusUnsupported;
                }
            }
        }

        #endregion
    }
}