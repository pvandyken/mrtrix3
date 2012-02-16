/*
   Copyright 2008 Brain Research Institute, Melbourne, Australia

   Written by J-Donald Tournier, 27/06/08.

   This file is part of MRtrix.

   MRtrix is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   MRtrix is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

 */

#include "app.h"
#include "thread/exec.h"
#include "thread/mutex.h"
#include "file/config.h"

namespace MR
{
  namespace Thread
  {

    size_t available_cores ()
    {
      static const size_t number_of_threads = File::Config::get_int ("NumberOfThreads", 1);
      return (number_of_threads);
    }




    Exec::Common::Common () :
      refcount (0), attributes (new pthread_attr_t) {
        debug ("initialising threads...");
        pthread_attr_init (attributes);
        pthread_attr_setdetachstate (attributes, PTHREAD_CREATE_JOINABLE);

        previous_print = print;
        previous_error = error;
        previous_inform = inform;
        previous_debug = debug;

        print = thread_print;
        error = thread_error;
        inform = thread_inform;
        debug = thread_debug;
      }

    Exec::Common::~Common () 
    {
      print = previous_print;
      error = previous_error;
      inform = previous_inform;
      debug = previous_debug;

      debug ("uninitialising threads...");

      pthread_attr_destroy (attributes);
      delete attributes;
    }

    void Exec::Common::thread_print (const std::string& msg)
    {
      Thread::Mutex::Lock lock (common->mutex);
      std::cout << msg;
    }

    void Exec::Common::thread_error (const std::string& msg)
    {
      if (App::log_level) {
        Thread::Mutex::Lock lock (common->mutex);
        std::cerr << App::NAME << ": " << msg << "\n";
      }
    }

    void Exec::Common::thread_inform (const std::string& msg)
    {
      if (App::log_level > 1) {
        Thread::Mutex::Lock lock (common->mutex);
        std::cerr << App::NAME << " [INFO]: " <<  msg << "\n";
      }
    }

    void Exec::Common::thread_debug (const std::string& msg)
    {
      if (App::log_level > 2) {
        Thread::Mutex::Lock lock (common->mutex);
        std::cerr << App::NAME << " [DEBUG]: " <<  msg << "\n";
      }
    }


    Exec::Common* Exec::common = NULL;




  }
}

