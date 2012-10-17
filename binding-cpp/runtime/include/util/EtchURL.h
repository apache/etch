/* $Id$
 *
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

#ifndef __ETCHURL_H__
#define __ETCHURL_H__

#include "common/EtchString.h"
#include "common/EtchHashTable.h"
#include "common/EtchList.h"
#include "util/EtchUtil.h"
#include "common/EtchConfig.h"
#include "capu/os/StringUtils.h"

class EtchURL {
public:

  /**
   * Constructs an empty url.
   */
  EtchURL();

  /**
   * Constructs a url from a string
   * @param urlStr
   */
  EtchURL(const char * urlStr);

  /**
   * Constructs a url from a string.
   * @param urlStr
   */
  EtchURL(const EtchString& urlStr);

  /**
   * Deconstructor
   */
  virtual ~EtchURL();

  /**
   * @return the scheme.
   */
  EtchString& getScheme();

  /**
   * Sets the scheme.
   *
   * @param scheme the scheme to set which may be null but not blank.
   */
  void setScheme(const EtchString& scheme);

  /**
   * Sets the scheme.
   *
   * @param scheme the scheme to set which may be null but not blank.
   */
  status_t setScheme(EtchString* scheme);

  /**
   * Tests the scheme for a match. The schemes are case-insensitive.
   *
   * @param testScheme a scheme to test against.
   *
   * @return true if the schemes match, false otherwise.
   */
  capu::bool_t isScheme(EtchString& testScheme);

  //////////
  // USER //
  //////////

  /**
   * @return the user.
   */
  EtchString& getUser();

  /**
   * Sets the user.
   *
   * @param user the user to set which may be null but not blank.
   */
  void setUser(const EtchString& user);

  //////////////
  // PASSWORD //
  //////////////

  /**
   * @return the password.
   */
  EtchString& getPassword();

  /**
   * Sets the password.
   *
   * @param password the password to set which may be null but not blank.
   */
  void setPassword(const EtchString& password);

  //////////
  // HOST //
  //////////

  /**
   * @return the host.
   */
  EtchString& getHost();

  /**
   * Sets the host.
   *
   * @param host the host to set which may be null but not blank.
   */
  void setHost(const EtchString& host);

  //////////
  // PORT //
  //////////

  /**
   * @return true if a port has been specified, false otherwise.
   */
  capu::bool_t hasPort();

  /**
   * @return the port. Null if no port has been specified.
   */
  capu::uint16_t getPort();

  /**
   * Sets the port.
   *
   * @param port the port to set which may be between 0 and 65535.
   */
  void setPort(capu::uint16_t port);

  /////////
  // URI //
  /////////

  /**
   * @return the uri.
   */
  EtchString& getUri();

  /**
   * Sets the uri.
   *
   * @param uri the uri to set which may be null or blank.
   */
  void setUri(const EtchString& uri);

  ////////////
  // PARAMS //
  ////////////

  /**
   * @return true if there is at least one param.
   */
  capu::bool_t hasParams();

  /**
   * Adds a param to the list of params for this url. Only the unique
   * params are maintained. Duplicate param values are suppressed.
   *
   * @param param a param (e.g., "transport=tcp" or "01831864574898475").
   */
  void addParam(EtchString& param);

  /**
   * Removes all the params.
   */
  void clearParams();

  /////////////////
  // QUERY TERMS //
  /////////////////

  /**
   * Adds the specified query term to the list of query terms. Duplicate
   * name/value pairs are suppressed.
   * @param name
   * @param value
   * @return ETCH_EINVAL if the parameters are NULL
   *         ETCH_OK if term is successfully added
   *         ETCH_ENO_MEMORY if allocation of term is failed
   */
  status_t addTerm(EtchString& name, EtchString& value);

  /**
   * Adds the specified query term to the list of query terms. Duplicate
   * name/value pairs are suppressed.
   * @param name
   * @param value
   * @return ETCH_EINVAL if the parameters are NULL
   *         ETCH_OK if term is successfully added
   *         ETCH_ENO_MEMORY if allocation of term is failed
   */
  status_t addTerm(EtchString& name, capu::int32_t value);

  /**
   * Adds the specified query term to the list of query terms. Duplicate
   * name/value pairs are suppressed.
   * @param name
   * @param value
   * @return ETCH_EINVAL if the parameters are NULL
   *         ETCH_OK if term is successfully added
   *         ETCH_ENO_MEMORY if allocation of term is failed
   */
  status_t addTerm(EtchString& name, capu::double_t value);

  /**
   * Adds the specified query term to the list of query terms. Duplicate
   * name/value pairs are suppressed.
   * @param name
   * @param value
   * @return ETCH_EINVAL if the parameters are NULL
   *         ETCH_OK if term is successfully added
   *         ETCH_ENO_MEMORY if allocation of term is failed
   */
  status_t addTerm(EtchString& name, capu::bool_t value);

  /**
   * Removes all the query terms.
   */
  void clearTerms();


  //////////////
  // FRAGMENT //
  //////////////

  /**
   * @return the fragment.
   */
  EtchString& getFragment();

  /**
   * Sets the fragment.
   *
   * @param fragment the fragment to set which may be null but not blank.
   */
  void setFragment(EtchString& fragment);

  /**
   *
   * @return list of parameters
   */
  EtchList<EtchString>& getParams();

  /**
   *
   * @return hash table which composed of terms
   */
  EtchHashTable<EtchString, EtchString>& getTerms();

private:

  EtchString scheme;
  EtchString user;
  EtchString password;
  EtchString host;
  capu::uint16_t port;
  EtchString uri;
  EtchString fragment;
  EtchList<EtchString> params;
  EtchHashTable<EtchString, EtchString> terms;

  status_t parse(const EtchString* s);

  status_t parseHost(const EtchString* s);

  status_t parseUserPassword(const EtchString* s);

  status_t parseHostPort(const EtchString* s);

  status_t parseParams(const EtchString* s);

  status_t parseTerms(const EtchString* s);

  status_t parseTerm(const EtchString *s);

};

#endif

