/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ROBOT_BASE_HH
#define ROBOT_BASE_HH
#include "TerrainBase.hh"
#include "EnergySources.hh"
#include "Goals.hh"
#include "RobotPositionServer.hh"

class RobotBase
{
public:
  RobotBase(const TerrainBase* _terrain, EnergySources* _resources, Goals* _goals,
            RobotPositionServer* _posServer)
    : m_Terrain(_terrain), m_EnergySources(_resources), m_Goals(_goals),
      m_RobotPositionServer(_posServer)
  {
  }

  virtual ~RobotBase()
  {
  }
  
  virtual void displayRobot(void) = 0;

  virtual const std::vector<double> processCommand(const std::string& cmd, double parameter) = 0;

protected:
  const TerrainBase* m_Terrain;
  EnergySources* m_EnergySources;
  Goals* m_Goals;
  RobotPositionServer* m_RobotPositionServer;
};

#endif // ROBOT_BASE_HH
