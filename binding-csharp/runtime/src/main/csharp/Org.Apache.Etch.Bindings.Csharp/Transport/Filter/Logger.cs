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
using System.Globalization;
using System.IO;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Filter
{
    public class Logger : AbstractMessageFilter
    {

        private const String FILE_PATH = "Logger.filePath";

        private String fileName = null;

        private TextWriter writer = null;

        public static bool testFlag = false;

        private string testRenamedFileName = null;

        enum Direction
        {
            UP, DOWN
        }

        public Logger(TransportMessage transport, URL uri, Resources resources)
            : base(transport, uri, resources)
        {
            InitializeLogger(uri);
           
        }

        private void InitializeLogger(URL uri)
        {
            string newFullFileName;
            fileName = uri.GetTerm(FILE_PATH);
            if (fileName == null)
            {
                fileName = "Log.txt";
            }
            
            if (!testFlag)
            {
                if (File.Exists(fileName))
                {
                    //      string newFileName = Path.GetFileNameWithoutExtension(fileName) + DateTime.Now.ToString("yyyy.MM.dd.HH.mm.ss", CultureInfo.CreateSpecificCulture("en-US")) + Path.GetExtension(fileName);
                    //    newFullFileName = Path.Combine(new FileInfo(fileName).DirectoryName, newFileName);
                    newFullFileName = GenerateNewFileName(fileName, new FileInfo(fileName).DirectoryName);
                    File.Move(fileName, newFullFileName);
                    File.Delete(fileName);
                }
                writer = TextWriter.Synchronized(new StreamWriter(fileName));
            }
            else
            {
                testRenamedFileName = GenerateNewFileName(fileName,null);
            }
        }

        private string GenerateNewFileName(string fileName, string pathName )
        {
            if (pathName == null)
                pathName = "";
            string newFileName = Path.GetFileNameWithoutExtension(fileName) + DateTime.Now.ToString("yyyy.MM.dd.HH.mm.ss", CultureInfo.CreateSpecificCulture("en-US")) + Path.GetExtension(fileName);
            string newFullFileName = Path.Combine(pathName, newFileName);
            return newFullFileName;
        }

        public override string ToString()
        {
            return "Logger/" + transport;
        }

        public string GetFileName()
        {
            return fileName;
        }

        public string GetTestRenamedFileName()
        {
            return testRenamedFileName;
        }

        public override bool SessionMessage(Who sender, Message msg)
        {
            WriteToFile(Direction.UP, sender, msg.ToString());
            return base.SessionMessage(sender, msg);
        }

        public override void TransportMessage(Who recipient, Message msg)
        {
            WriteToFile(Direction.DOWN, recipient, msg.ToString());
            base.TransportMessage(recipient, msg);
        }
        protected override bool SessionUp()
        {
            WriteToFile(Direction.UP, null, transport.ToString());
            return true;
        }

        protected override bool SessionDown()
        {
            WriteToFile(Direction.DOWN, null, transport.ToString());
            return true;
        }

        private void WriteToFile(Direction direction, Who msgOrigin, String msg)
        {
            if (writer == null)
                return;
            try
            {
                lock (writer)
                {
                    writer.Write(DateTime.Now.ToString("HH:mm:ss.fff", CultureInfo.CreateSpecificCulture("en-US")));
                    writer.Write(": ");
                    writer.Write(ConvertDirToString(direction));
                    writer.Write(": ");
                    if (msgOrigin != null)
                    {
                        writer.Write(msgOrigin.ToString());
                        writer.Write(": ");
                    }
                    writer.Write(msg);
                    writer.WriteLine();
                    writer.Flush();
                }
            }
            catch (Exception e)
            {
                writer.Close();
                throw e;
            }

        }

        private String ConvertDirToString(Direction dir)
        {
            if (Direction.DOWN == dir)
                return "Outgoing";
            return "Incoming";
        }


    }
}
