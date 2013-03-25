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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "util/EtchURL.h"
#include "capu/os/StringUtils.h"

EtchURL::EtchURL()
 : terms(ETCH_DEFAULT_URL_TERMS_HASH_BIT_SIZE) {

}

EtchURL::EtchURL(const char* urlStr) {
  if (urlStr == NULL) {
    //already initialized
  } else if (strlen(urlStr) > 0) {
    EtchString str(urlStr);
    if (parse(&str) != ETCH_OK) {
      fragment.set("");
      host.set("");
      password.set("");
      port = 0;
      scheme.set("");
      uri.set("");
      user.set("");
    }
  }
}

EtchURL::EtchURL(const EtchString& urlStr) {
  if (urlStr.c_str() == NULL) {
    //already initialized
  } else if (urlStr.length() > 0) {
    if (parse(&urlStr) != ETCH_OK) {
      fragment.set("");
      host.set("");
      password.set("");
      port = 0;
      scheme.set("");
      uri.set("");
      user.set("");
    }
  }
}

EtchURL::~EtchURL() {
  params.clear();
  terms.clear();
}

status_t EtchURL::parse(const EtchString* s) {
  if (s == NULL)
    return ETCH_EINVAL;
  // s is scheme:[//[user[:password]@]host[:port]/]uri[;params][?terms][#fragment]
  capu::int32_t result = s->leftFind(':');
  status_t error_result;
  if (result == -1)
    return ETCH_ERROR;
  //parsing of scheme
  EtchString x[2];
  //x[0] contains scheme
  error_result = s->substring(0, result, &x[0]);
  if (error_result != ETCH_OK)
    return error_result;
  //x[1] will contain rest of url [//[user[:password]@]host[:port]/]uri[;params][?terms][#fragment]
  error_result = s->substring(result + 1, s->length()-(result + 1), &x[1]);
  if (error_result != ETCH_OK)
    return error_result;

  setScheme(x[0]);
  EtchString _s(x[1]);

  // _s is [//[user[:password]@]host[:port]/]uri[;params][?terms][#fragment]
  result = _s.leftFind('#');

  if (result != -1) {
    //parse the fragments
    //x[0] will contain the rest of url [//[user[:password]@]host[:port]/]uri[;params][?terms]
    error_result = _s.substring(0, result, &x[0]);
    if (error_result != ETCH_OK)
      return error_result;
    //x[1] will contain the fragments
    error_result = _s.substring(result + 1, _s.length()-(result + 1), &x[1]);
    if (error_result != ETCH_OK)
      return error_result;
    //set the fragments
    setFragment(x[1]);
    _s = x[0];
  }

  // s is [//[user[:password]@]host[:port]/]uri[;params][?terms]
  result = _s.leftFind('?');

  if (result != -1) {
    //parse terms
    //x[0] will contain rest of url [//[user[:password]@]host[:port]/]uri[;params]
    error_result = _s.substring(0, result, &x[0]);
    if (error_result != ETCH_OK)
      return error_result;
    //x[1] will contain the terms
    error_result = _s.substring(result + 1, _s.length()-(result + 1), &x[1]);
    if (error_result != ETCH_OK)
      return error_result;
    //parse and sets the terms
    error_result = parseTerms(&x[1]);
    if (error_result != ETCH_OK)
      return error_result;
    _s = x[0];
  }

  // s is [//[user[:password]@]host[:port]/]uri[;params]
  result = _s.leftFind(';');
  if (result != -1) {
    //parse terms
    //x[0] will contain rest of url [//[user[:password]@]host[:port]/]uri
    error_result = _s.substring(0, result, &x[0]);
    if (error_result != ETCH_OK)
      return error_result;
    error_result = _s.substring(result + 1, _s.length()-(result + 1), &x[1]);
    if (error_result != ETCH_OK)
      return error_result;
    //x[1] will contain the params
    error_result = parseParams(&x[1]);
    if (error_result != ETCH_OK)
      return error_result;
    _s = x[0];
  }

  // s is [//[user[:password]@]host[:port]/]uri
  char *str = new char [_s.length() + 1];
  etch_strcpy_s(str, _s.length() + 1, _s.c_str());

  if ((str[0] == '/') && (str[1] == '/')) {
    //eleminate "//"
    _s.set(&str[2]);
    // s is [user[:password]@]host[:port]/uri
    //parse uri
    result = _s.leftFind('/');
    if (result != -1) {
      //x[0] contains rest of uri [user[:password]@]host[:port]
      error_result = _s.substring(0, result, &x[0]);
      if (error_result != ETCH_OK)
        return error_result;
      //x[1] will contain the uri
      error_result = _s.substring(result + 1, _s.length()-(result + 1), &x[1]);
      if (error_result != ETCH_OK)
        return error_result;
      // parse [user[:password]@]host[:port]
      error_result = parseHost(&x[0]);
      if (error_result != ETCH_OK)
        return error_result;
      //now s contains the uri
      _s = x[1];
    } else {
      // s is [user[:password]@]host[:port]
      error_result = parseHost(&_s);
      if (error_result != ETCH_OK)
        return error_result;
      //uri is not given
      _s.set("");
    }
  }
  setUri(_s);
  delete [] str;
  return ETCH_OK;
}

status_t EtchURL::parseHost(const EtchString* s) {
  if (s == NULL) {
    return ETCH_EINVAL;
  }
  // s is [user[:password]@]host[:port]
  capu::int32_t result = s->leftFind('@');
  EtchString x[2];
  if (result != -1) {
    if (s->substring(0, result, &x[0]) != ETCH_OK)
      return ETCH_ERROR;
    if (s->substring(result + 1, s->length()-(result + 1), &x[1]) != ETCH_OK)
      return ETCH_ERROR;
    parseUserPassword(&x[0]);
    parseHostPort(&x[1]);
  } else {
    parseHostPort(s);
  }
  return ETCH_OK;
}

status_t EtchURL::parseUserPassword(const EtchString* s) {
  if (s == NULL) {
    return ETCH_EINVAL;
  }
  // s is user[:password]
  capu::int32_t result = s->leftFind(':');

  EtchString x[2];

  if (result != -1) {
    if (s->substring(0, result, &x[0]) != ETCH_OK)
      return ETCH_ERROR;
    if (s->substring(result + 1, s->length()-(result + 1), &x[1]) != ETCH_OK)
      return ETCH_ERROR;
    setUser(x[0]);
    setPassword(x[1]);
  } else {
    setUser(*s);
  }
  return ETCH_OK;
}

status_t EtchURL::parseHostPort(const EtchString* s) {
  if (s == NULL) {
    return ETCH_EINVAL;
  }
  capu::int32_t result = s->leftFind(':');

  EtchString x[2];

  if (x != NULL) {
    if (s->substring(0, result, &x[0]) != ETCH_OK)
      return ETCH_ERROR;
    if (s->substring(result + 1, s->length()-(result + 1), &x[1]) != ETCH_OK)
      return ETCH_ERROR;
    setHost(x[0]);
    EtchString *p = &x[1];
    capu::int32_t _port = atoi(p->c_str());
    setPort(_port);
  } else {
    setHost(*s);
  }
  return ETCH_OK;
}

status_t EtchURL::parseParams(const EtchString* s) {
  // s is param[;param]*
  if (s == NULL) {
    return ETCH_EINVAL;
  }
  if (s->length() == 0)
    return ETCH_OK; //THERE IS NOTHING TO PARSE
  EtchString _s = *s;
  EtchString x[2];
  capu::int32_t result = _s.leftFind(';');
  while (result != -1) {
    if (_s.substring(0, result, &x[0]) != ETCH_OK)
      return ETCH_ERROR;
    if (_s.substring(result + 1, _s.length()-(result + 1), &x[1]) != ETCH_OK)
      return ETCH_ERROR;
    addParam(x[0]);
    _s.set(x[1].c_str());
    result = _s.leftFind(';');
  }
  addParam(_s);
  return ETCH_OK;
}

status_t EtchURL::parseTerms(const EtchString* s) {
  if (s == NULL) {
    return ETCH_EINVAL;
  }
  if (s->length() == 0)
    return ETCH_OK; //THERE IS NOTHING TO PARSE
  EtchString _s = *s;
  EtchString x[2];
  capu::int32_t result = _s.leftFind('&');
  while (result != -1) {
    if (_s.substring(0, result, &x[0]) != ETCH_OK)
      return ETCH_ERROR;
    if (_s.substring(result + 1, _s.length()-(result + 1), &x[1]) != ETCH_OK)
      return ETCH_ERROR;
    parseTerm(&x[0]);
    _s.set(x[1].c_str());
    result = _s.leftFind('&');
  }
  return parseTerm(&_s);
}

status_t EtchURL::parseTerm(const EtchString * s) {
  if (s == NULL) {
    return ETCH_EINVAL;
  }
  // s is name[=value]
  if (s->length() == 0)
    return ETCH_OK; //THERE IS NOTHING TO PARSE
  capu::int32_t result = s->leftFind('=');
  EtchString x[2];
  if (s->substring(0, result, &x[0]) != ETCH_OK)
    return ETCH_ERROR;
  if (s->substring(result + 1, s->length()-(result + 1), &x[1]) != ETCH_OK)
    return ETCH_ERROR;
  if (result != -1)
    addTerm(x[0], x[1]);
  return ETCH_OK;
}

EtchString& EtchURL::getScheme() {
  return scheme;
}

void EtchURL::setScheme(const EtchString& scheme) {
  this->scheme.set(scheme.c_str());
}

status_t EtchURL::setScheme(EtchString* scheme) {
  if (scheme != NULL) {
    this->scheme.set(scheme->c_str());
    return ETCH_OK;
  }
  return ETCH_EINVAL;
}

EtchString& EtchURL::getUser() {
  return user;
}

void EtchURL::setUser(const EtchString& user) {
  this->user.set(user.c_str());
}

EtchString& EtchURL::getPassword() {
  return password;
}

void EtchURL::setPassword(const EtchString& password) {
  this->password.set(password.c_str());
}

EtchString& EtchURL::getHost() {
  return host;
}

void EtchURL::setHost(const EtchString& host) {
  this->host.set(host.c_str());
}

capu::uint16_t EtchURL::getPort() {
  return port;
}

void EtchURL::setPort(const capu::uint16_t port) {
  this->port = port;
}

EtchString& EtchURL::getUri() {
  return uri;
}

void EtchURL::setUri(const EtchString& uri) {
  this->uri.set(uri.c_str());
}

capu::bool_t EtchURL::hasParams() {
  return !(params.isEmpty());
}

void EtchURL::addParam(EtchString& param) {
  this->params.add(param);
}

void EtchURL::clearParams() {
  params.clear();
}

status_t EtchURL::addTerm(EtchString& name, EtchString& value) {
  return terms.put(name, value);
}

status_t EtchURL::addTerm(EtchString& name, capu::int32_t value) {
  char result [10];
  EtchString str;
  capu::StringUtils::Sprintf(result, 10, "%d", value);
  str.set(result);
  return addTerm(name, str);
}

status_t EtchURL::addTerm(EtchString& name, capu::bool_t value) {
  char result [2];
  EtchString str;
  capu::StringUtils::Sprintf(result, 2, "%d", value);
  str.set(result);
  return addTerm(name, str);
}

status_t EtchURL::addTerm(EtchString& name, capu::double_t value) {
  char result [10];
  EtchString str;
  capu::StringUtils::Sprintf(result, 10, "%f", value);
  str.set(result);
  return addTerm(name, str);
}

void EtchURL::clearTerms() {
  terms.clear();
}

EtchString& EtchURL::getFragment() {
  return fragment;
}

void EtchURL::setFragment(EtchString& fragment) {
  this->fragment.set(fragment.c_str());
}

EtchList<EtchString>& EtchURL::getParams() {
  return params;
}

EtchHashTable<EtchString, EtchString>& EtchURL::getTerms() {
  return terms;
}
