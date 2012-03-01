/* $Id$
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements. See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version
 * 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "util/EtchURL.h"
#include "common/EtchString.h"

TEST(EtchURLTest, constructorTest) {
  EtchURL * url = new EtchURL("tcp://admin:metreos@localhost:10000/uri;param2;param1?term1=value1&term2=value2#frag");
  EXPECT_TRUE(url != NULL);
  delete url;
}

TEST(EtchURLTest, getTests) {
  EtchURL url("tcp://admin:metreos@localhost:10000/uri;param2;param1?term1=value1&term2=value2#frag");

  EXPECT_TRUE(strcmp(url.getScheme().c_str(), "tcp") == 0);
  EXPECT_TRUE(strcmp(url.getFragment().c_str(), "frag") == 0);
  EXPECT_TRUE(strcmp(url.getUser().c_str(), "admin") == 0);
  EXPECT_TRUE(strcmp(url.getPassword().c_str(), "metreos") == 0);
  EXPECT_TRUE(strcmp(url.getHost().c_str(), "localhost") == 0);
  EXPECT_TRUE(strcmp(url.getUri().c_str(), "uri") == 0);
  EXPECT_TRUE(url.getPort() == 10000);
}

TEST(EtchURLTest, setAndGetTests) {
  EtchString defScheme("tcp");
  EtchString defUser("admin");
  EtchString defPassword("metreos");
  EtchString defHost("localhost");
  EtchString defFragment("frag");
  capu::int32_t defPort = 10000;
  EtchString defUri("uri");
  EtchURL url;

  url.setFragment(defFragment);
  url.setHost(defHost);
  url.setPassword(defPassword);
  url.setPort(defPort);
  url.setUri(defUri);
  url.setScheme(defScheme);
  url.setUser(defUser);

  EXPECT_TRUE(strcmp(defFragment.c_str(), url.getFragment().c_str()) == 0);
  EXPECT_TRUE(strcmp(defScheme.c_str(), url.getScheme().c_str()) == 0);
  EXPECT_TRUE(strcmp(defUser.c_str(), url.getUser().c_str()) == 0);
  EXPECT_TRUE(strcmp(defPassword.c_str(), url.getPassword().c_str()) == 0);
  EXPECT_TRUE(strcmp(defHost.c_str(), url.getHost().c_str()) == 0);
  EXPECT_TRUE(strcmp(defUri.c_str(), url.getUri().c_str()) == 0);
  EXPECT_TRUE(defPort == url.getPort());
}

TEST(EtchURLTest, AddParamTest) {
  EtchString tmp("");
  EtchString defUri("uri");
  EtchURL url;

  //url.addParam(NULL);
  //should not add invalid parameters
  EXPECT_TRUE(url.getParams().size() == 0);

  url.addParam(defUri);
  EXPECT_TRUE(url.getParams().size() == 1);

  url.getParams().get(0, &tmp);
  EXPECT_TRUE(tmp.equals(&defUri));

}

TEST(EtchURLTest, AddTermTest) {
  EtchString tmp("");
  EtchString defKey("key");
  EtchString defUri("uri");
  EtchURL url;

  EXPECT_TRUE(url.getTerms().count() == 0);

  url.addTerm(defKey, defUri);
  EXPECT_TRUE(url.getTerms().count() == 1);

  url.getTerms().get(defKey, &tmp);
  EXPECT_TRUE(tmp.equals(&defUri));
}

TEST(EtchURLTest, HasParamTest) {
  EtchString tmp("");
  EtchString defUri("uri");
  EtchURL url;

  EXPECT_FALSE(url.hasParams());

  url.addParam(defUri);
  EXPECT_TRUE(url.getParams().size() == 1);
  EXPECT_TRUE(url.hasParams());

  url.getParams().get(0, &tmp);
  EXPECT_TRUE(tmp.equals(&defUri));
}

TEST(EtchURLTest, clearParamTest) {
  EtchString tmp("");
  EtchString defUri("uri");
  EtchURL url;

  url.addParam(defUri);
  EXPECT_TRUE(url.getParams().size() == 1);
  //clear the params
  url.clearParams();
  EXPECT_TRUE(url.getParams().size() == 0);
}

TEST(EtchURLTest, clearTermTest) {
  EtchString tmp("");
  EtchString defUri("uri");
  EtchString defKey("key");
  EtchURL url;

  url.addTerm(defKey, defUri);
  EXPECT_TRUE(url.getTerms().count() == 1);
  //clear the terms
  url.clearTerms();
  EXPECT_TRUE(url.getTerms().count() == 0);
}