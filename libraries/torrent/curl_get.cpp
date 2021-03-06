/* This file was originally from libtorrent's example program,
 * simple_client - I didn't write it, all credit goes to Rakshasa */


// rTorrent - BitTorrent client
// Copyright (C) 2005, Jari Sundell
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// In addition, as a special exception, the copyright holders give
// permission to link the code of portions of this program with the
// OpenSSL library under certain conditions as described in each
// individual source file, and distribute linked combinations
// including the two.
//
// You must obey the GNU General Public License in all respects for
// all of the code used other than OpenSSL.  If you modify file(s)
// with this exception, you may extend this exception to your version
// of the file(s), but you are not obligated to do so.  If you do not
// wish to do so, delete this exception statement from your version.
// If you delete this exception statement from all source files in the
// program, then also delete it here.
//
// Contact:  Jari Sundell <jaris@ifi.uio.no>
//
//           Skomakerveien 33
//           3185 Skoppum, NORWAY


#include <iostream>
#include <stdexcept>
#include <curl/curl.h>
#include <curl/easy.h>

#include "curl_get.h"
#include "curl_stack.h"

namespace core {

size_t
curl_get_receive_write(void* data, size_t size, size_t nmemb, void* handle) {
  if (!((CurlGet*)handle)->stream()->write((char*)data, size * nmemb).fail())
    return size * nmemb;
  else
    return 0;
}

CurlGet::CurlGet(CurlStack* s) :
  m_handle(NULL),
  m_stack(s) {

  if (m_stack == NULL)
    throw std::logic_error("Tried to create CurlGet without a valid CurlStack");
}

CurlGet::~CurlGet() {
  close();
}

CurlGet*
CurlGet::new_object(CurlStack* s) {
  return new CurlGet(s);
}

void
CurlGet::start() {
  if (is_busy())
    throw std::logic_error("Tried to call CurlGet::start on a busy object");

  if (m_stream == NULL)
    throw std::logic_error("Tried to call CurlGet::start without a valid output stream");

  m_handle = curl_easy_init();

  curl_easy_setopt(m_handle, CURLOPT_URL,            m_url.c_str());
  curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION,  &curl_get_receive_write);
  curl_easy_setopt(m_handle, CURLOPT_WRITEDATA,      this);

  if (m_timeout != 0) {
    curl_easy_setopt(m_handle, CURLOPT_CONNECTTIMEOUT, 60);
    curl_easy_setopt(m_handle, CURLOPT_TIMEOUT,        m_timeout);
  }

  curl_easy_setopt(m_handle, CURLOPT_FORBID_REUSE,   1);
  curl_easy_setopt(m_handle, CURLOPT_NOSIGNAL,       1);
  curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(m_handle, CURLOPT_MAXREDIRS,      5);

  m_stack->add_get(this);
}

void
CurlGet::close() {
  if (!is_busy())
    return;

  m_stack->remove_get(this);

  curl_easy_cleanup(m_handle);

  m_handle = NULL;
}


double
CurlGet::size_done() {
  double d = 0.0;
  curl_easy_getinfo(m_handle, CURLINFO_SIZE_DOWNLOAD, &d);

  return d;
}

double
CurlGet::size_total() {
  double d = 0.0;
  curl_easy_getinfo(m_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &d);

  return d;
}

void
CurlGet::set_user_agent(const char* s) {
  curl_easy_setopt(m_handle, CURLOPT_USERAGENT, s);
}

void
CurlGet::perform(CURLMsg* msg) {
  if (msg->msg != CURLMSG_DONE)
    throw std::logic_error("CurlGet::process got CURLMSG that isn't done");

  if (msg->data.result == CURLE_OK)
    m_signalDone.emit();
  else
    m_signalFailed.emit(curl_easy_strerror(msg->data.result));

  if (m_handle != NULL)
    throw std::logic_error("CurlGet::perform finished but the object wasn't closed.");
}

}
