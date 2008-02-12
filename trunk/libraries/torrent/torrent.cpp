/* This file was originally from libtorrent's example program,
 * simple_client - I didn't write it, all credit goes to Rakshasa */

/* Simple modifications to make it work as a part of tuxcast rather
 * than a standalone program are by David 'Cillian' Turner */


// Under the GPL with OpenSSL exception, see the libtorrent license.

#include <cerrno>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <torrent/download.h>
#include <torrent/object_stream.h>
#include <torrent/object.h>
#include <torrent/torrent.h>
#include <torrent/http.h>
#include <torrent/poll_select.h>
#include <torrent/connection_manager.h>
#include <sigc++/bind.h>
#include <sigc++/hide.h>

#include "curl_get.h"
#include "curl_stack.h"

#include "torrent.h"
#include "torrent_exceptions.h"

bool doShutdown = false;

void
chunk_passed(torrent::Download d) {
  std::cout << d.name() << ' ' << d.chunks_hashed() << '/' << d.chunks_seen() << std::endl;
}

void
finished_download(torrent::Download d) {
  std::cout << "Finished: " << d.name() << std::endl;

  d.stop();

  // torrent::Download::close() closes the tracker manager, so it
  // won't send the STOPPED message. Do this later.
  //d.close();

  //torrent::ConnectionManager::listen_close();
  torrent::connection_manager()->listen_close();

  // Do a quick shutdown without cleaning up or sending messages to
  // the trackers.
  doShutdown = true;
}

void
hash_check_done(torrent::Download d) {
  std::cout << "Hash check completed." << std::endl;
  d.start();
  chunk_passed(d);
}

void
http_done(torrent::Http* curlGet) {
  curlGet->close();
  std::cout << "Finished http download." << std::endl << std::flush;

  torrent::Object *obj=new torrent::Object();
  
  *(curlGet->stream()) >> * obj;

  //torrent::Download d = torrent::download_add(curlGet->stream());
  torrent::Download d = torrent::download_add(obj);

  d.signal_hash_done(sigc::bind(sigc::ptr_fun(&hash_check_done), d));
  d.signal_download_done(sigc::bind(sigc::ptr_fun(&finished_download), d));
  d.signal_chunk_passed(sigc::hide(sigc::bind(sigc::ptr_fun(&chunk_passed), d)));
  d.open();
  d.hash_check(true);
}

void
http_failed(const std::string& msg, torrent::Http* curlGet) {
  std::cout << "Failed http download: " << msg << "." << std::endl << std::flush;
}

void bittorrent(string filename)
{
   string URL;
   URL="file://";
   URL += filename;
    // Network connections can throw SIGPIPE, so it is best to ignore it.
    signal(SIGPIPE, SIG_IGN);

    // 512 max open sockets, just a random number. A real client
    // checks sysconf(OC_OPEN_MAX). libTorrent uses this to allocate a
    // resonable amount of file descriptors for open files, network
    // connections and some for the client to use.
    //
    // See torrent::initialize for the ratios.
    torrent::PollSelect* pollSelect = torrent::PollSelect::create(512);

    core::CurlStack::global_init();
    core::CurlStack curlStack;

    // 'torrent::Http::set_factory(...)' must be set to a slot that
    //
    // will create an object that handle http downloads. This is used
    // for tracker requests.
    torrent::Http::set_factory(curlStack.get_http_factory());
    torrent::Http* curlGet = torrent::Http::call_factory();

    torrent::initialize(pollSelect);

    // Fix the bug caused by not calling this?
    if (!torrent::connection_manager()->listen_open(10000, 14000))
      throw std::runtime_error("Could not open a listening port.");

    std::stringstream httpDownload;

    curlGet->signal_done().connect(sigc::bind(sigc::ptr_fun(&http_done), curlGet));
    curlGet->signal_failed().connect(sigc::bind(sigc::ptr_fun(&http_failed), curlGet));
    curlGet->set_url(URL.c_str());
    curlGet->set_stream(&httpDownload);
    curlGet->start();

    std::cout << "Starting download." << std::endl;

    fd_set readSet;
    fd_set writeSet;
    fd_set errorSet;

    // Shutdown should wait for torrent::is_inactive to return true,
    // with a resonable timeout. This will make sure trackers receive
    // the STOPPED message.

    while (!doShutdown) {
      FD_ZERO(&readSet);
      FD_ZERO(&writeSet);
      FD_ZERO(&errorSet);

      unsigned int maxFd = pollSelect->fdset(&readSet, &writeSet, &errorSet);

      if (curlStack.is_busy())
	maxFd = std::max(maxFd, curlStack.fdset(&readSet, &writeSet, &errorSet));

      int64_t t = std::min<int64_t>(1000000, torrent::next_timeout());
      timeval timeout = { t / 1000000, t % 1000000 };

      if (select(maxFd + 1, &readSet, &writeSet, &errorSet, &timeout) == -1 &&
	  errno != EINTR)
	throw std::runtime_error("Error polling.");

      if (curlStack.is_busy())
	curlStack.perform();

      // 'torrent::perform()' updates the cached time and runs any
      // scheduled tasks. We call it again to remove any task that
      // might have immediate timeout or that have timed out during
      // the call to 'pollSelect::perform(...)'.
      torrent::perform();
      pollSelect->perform(&readSet, &writeSet, &errorSet);
      torrent::perform();
    }

    // Cleanup is for the weak.
    torrent::cleanup();

    core::CurlStack::global_cleanup();
    delete pollSelect;

/*  } catch (std::exception& e) {
    std::cout << "Caught: " << e.what() << std::endl;
  }*/
  
}
