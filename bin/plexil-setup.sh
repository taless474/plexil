#!/bin/sh

# Copyright (c) 2006-2008, Universities Space Research Association (USRA).
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Universities Space Research Association nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Check environment variables and UE executable
if [ ! $PLEXIL_HOME ]
  then
    echo Error: Please set environment variable PLEXIL_HOME
    echo to the full pathname of your 'plexil' or 'trunk' directory.
    echo Exiting.

else

    export PATH=${PATH}:${PLEXIL_HOME}/bin

    _plexil_libpath=$PLEXIL_HOME/lib

    # Defining variables (redundantly) for both Mac and Linux.

    # Linux
    export LD_LIBRARY_PATH=$_plexil_libpath

    # Mac
    export DYLD_LIBRARY_PATH=$_plexil_libpath
	# Seems to clobber SG's svn binary
    #export DYLD_BIND_AT_LAUNCH=YES

    unset _plexil_libpath

fi
