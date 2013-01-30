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

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    public class StringTokenizer
    {
        public StringTokenizer(string s, string seps)
        {
            this.s = s;
            this.seps = seps;
        }

        private readonly string s;

        private readonly string seps;

        private int index = 0;

        public bool HasMoreTokens()
        {
            if (token == null)
                token = GetNextToken();

            return token != null;
        }

        public string NextToken()
        {
            if (token == null)
            {
                token = GetNextToken();
                if (token == null)
                    throw new IndexOutOfRangeException("no more tokens");
            }

            string t = token;
            token = null;
            return t;
        }

        private string GetNextToken()
        {
            SkipSeps();

            int i = index;
            while (index < s.Length  &&  !IsSep(s[index]))
                index++;

            if (index > i)
                return s.Substring(i, index - i);

            return null;
        }

        private void SkipSeps()
        {
            while (index < s.Length && IsSep(s[index]))
                index++;
        }

        private bool IsSep(char c)
        {
            return seps.IndexOf(c) >= 0;
        }

        private string token;
    }
}
