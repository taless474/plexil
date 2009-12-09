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

#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>

#include "MyOpenGL.hh"
#include "Macros.hh"
#include "Robot.hh"
#include "MutexGuard.hh"

Robot::Robot(const TerrainBase* _terrain, EnergySources* _resources,
             Goals* _goals, RobotPositionServer* _posServer, const std::string _name, 
             int initRow, int initCol, double red, double green,
             double blue)
    : RobotBase(_terrain, _resources, _goals, _posServer), m_Name(_name), m_Row(initRow), 
      m_Col(initCol), 
      m_Red(red), m_Green(green), m_Blue(blue), m_EnergyLevel(1.0), 
      m_DirOffset(5, std::vector<int>(2)), m_BeamWidth(0.01), m_ScanScale(0.0)
  {
    pthread_mutex_init(&m_RobotPositionMutex, NULL);
    pthread_mutex_init(&m_RobotEnergyLevelMutex, NULL);
    m_SSWGClient.connectToServer(m_Name, "127.0.0.1", 6164, this);
    m_RobotPositionServer->setRobotPosition(m_Name, m_Row, m_Col);
    m_DirOffset[0][0] = -1; // N
    m_DirOffset[0][1] = 0;
    m_DirOffset[1][0] = 0;  // E
    m_DirOffset[1][1] = 1;
    m_DirOffset[2][0] = 1;  // S
    m_DirOffset[3][1] = 0;
    m_DirOffset[3][0] = 0;  // W
    m_DirOffset[3][1] = -1;
    m_DirOffset[4][0] = 0;  // Current Position
    m_DirOffset[4][1] = 0;
  }

Robot::~Robot()
{
  std::cout << "Deleting robot: " << m_Name << std::endl;
  pthread_mutex_destroy(&m_RobotPositionMutex);
  pthread_mutex_destroy(&m_RobotEnergyLevelMutex);
}

void Robot::displayRobot(void)
{
  double rWidth = 2.0 / static_cast<double>(m_Terrain->getWidth());
  double rWidthBy2 = rWidth / 2.0;
  double wallThickness = 0.005;
  
  // Use the locally cached value here since we do not want to overload 
  // the position server.
  int row, col;
  getRobotPositionLocal(row, col);
  
  double xCenter = -1.0+col*rWidth+rWidthBy2;
  double yCenter = 1.0-row*rWidth-rWidthBy2;
  double xUB = xCenter + rWidthBy2 - wallThickness;
  double xLB = xCenter - rWidthBy2 + wallThickness;
  double yUB = yCenter - rWidthBy2 + wallThickness;
  double yLB = yCenter + rWidthBy2 - wallThickness;
  
  /* draw unit square polygon */
  glColor3f(m_EnergyLevel*m_Red, m_EnergyLevel*m_Green, m_EnergyLevel*m_Blue);
  
  // Square
  glBegin(GL_POLYGON);
  glVertex2f(xLB, yLB);
  glVertex2f(xLB, yUB);
  glVertex2f(xUB, yUB);
  glVertex2f(xUB, yLB);
  glEnd();
  
  // The East sensors
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(xUB, yCenter);
  glVertex2f(xUB+m_ScanScale*rWidthBy2, yCenter-m_ScanScale*m_BeamWidth);
  glVertex2f(xUB+m_ScanScale*rWidthBy2, yCenter+m_ScanScale*m_BeamWidth);
  glEnd();
  
  // The South sensors
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(xCenter, yUB);
  glVertex2f(xCenter-m_ScanScale*m_BeamWidth, yUB-m_ScanScale*rWidthBy2);
  glVertex2f(xCenter+m_ScanScale*m_BeamWidth, yUB-m_ScanScale*rWidthBy2);
  glEnd();
  
  // The West sensors
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(xLB, yCenter);
  glVertex2f(xLB-m_ScanScale*rWidthBy2, yCenter-m_ScanScale*m_BeamWidth);
  glVertex2f(xLB-m_ScanScale*rWidthBy2, yCenter+m_ScanScale*m_BeamWidth);
  glEnd();
  
  // The North sensors
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(xCenter, yLB);
  glVertex2f(xCenter-m_ScanScale*m_BeamWidth, yLB+m_ScanScale*rWidthBy2);
  glVertex2f(xCenter+m_ScanScale*m_BeamWidth, yLB+m_ScanScale*rWidthBy2);
  glEnd();
  
  
  m_ScanScale += 0.025;
  if (m_ScanScale > 1.0) m_ScanScale = 0.0;
}

// Purely for demo to have a robot moving in the scene
void Robot::updateRobotPosition()
{
  std::vector<std::vector<int> > dirOffset(4, std::vector<int>(2));
  dirOffset[0][0] = 0;
  dirOffset[0][1] = 1;
  dirOffset[1][0] = 1;
  dirOffset[1][1] = 0;
  dirOffset[2][0] = 0;
  dirOffset[2][1] = -1;
  dirOffset[3][0] = -1;
  dirOffset[3][1] = 0;
  
  std::vector<std::vector<int> >::const_iterator dIter = dirOffset.begin();
  bool done = false;
  
  while (!done)
    {
      std::vector<int> dir = dirOffset[RANDOM_NUMBER_INT(0, 3)];
      int rowNext = m_Row + dir[0];
      int colNext = m_Col + dir[1];
      if (m_Terrain->isTraversable(m_Row, m_Col, rowNext, colNext) &&
          m_RobotPositionServer->setRobotPosition(m_Name, rowNext, colNext))
        {
          done = true;
          setRobotPositionLocal(rowNext, colNext); // local cache for display purposes only
          updateRobotEnergyLevel(m_EnergySources->acquireEnergySource(rowNext, colNext)-0.025);
        }
      ++dIter;
    }
}

double Robot::determineEnergySourceLevel()
{
  int row, col;
  //    readRobotPosition(row, col);
  m_RobotPositionServer->getRobotPosition(m_Name, row, col);
  return m_EnergySources->determineEnergySourceLevel(row, col);
}

double Robot::determineGoalLevel()
{
  int row, col;
  //    readRobotPosition(row, col);
  m_RobotPositionServer->getRobotPosition(m_Name, row, col);
  return m_Goals->determineGoalLevel(row, col);
}

void Robot::receivedMessage (const std::string& sender,
                             const std::string& msg)
{
  std::cout << "Received " << msg << std::endl;
  sleep(1);

  if ((msg == "MoveUp") || (msg == "MoveDown") || (msg == "MoveRight") || 
      (msg == "MoveLeft"))
    moveRobot(sender, msg);
  else if ((msg == "QueryEnergySensor") || (msg == "QueryGoalSensor"))
    querySensorLevel(sender, msg);
  else if (msg == "QueryVisibilitySensor")
    queryVisibility(sender, msg);
  else if (msg == "QueryRobotState")
    queryRobotState(sender, msg);
  else
    std::cout << "Robot:receivedMessage: Unknown message: "
              << msg << " Ignoring" << std::endl;
}

void Robot::getRobotPositionLocal(int& row, int& col)
{
  MutexGuard mg(&m_RobotPositionMutex);
  row = m_Row;
  col = m_Col;
}

void Robot::setRobotPositionLocal(int row, int col)
{
  MutexGuard mg(&m_RobotPositionMutex);
  m_Row = row;
  m_Col = col;
}

const double& Robot::readRobotEnergyLevel()
{
  MutexGuard mg(&m_RobotEnergyLevelMutex);
  return m_EnergyLevel;
}

void Robot::updateRobotEnergyLevel(double energyLevel)
{
  MutexGuard mg(&m_RobotEnergyLevelMutex);
  m_EnergyLevel = std::max(0.0, std::min(1.0, m_EnergyLevel + energyLevel));
}

void Robot::queryRobotState(const std::string& sender, const std::string& msg)
{
  int row, col;
  std::string result=msg+",";
  //    readRobotPosition(row, col);
  m_RobotPositionServer->getRobotPosition(m_Name, row, col);
  double energyLevel = readRobotEnergyLevel();
  std::ostringstream sstr;
  sstr << row << "," << col << "," << energyLevel;
  
  result += sstr.str();
  std::cout << "res: " << result << std::endl;
  
  m_SSWGClient.sendMessage(result, sender);
}

void Robot::querySensorLevel(const std::string& sender, const std::string& msg)
{
  int row, col;
  std::string result=msg+",";
  //    readRobotPosition(row, col);
  m_RobotPositionServer->getRobotPosition(m_Name, row, col);
  
  for (std::vector<std::vector<int> >::const_iterator dIter = m_DirOffset.begin();
       dIter != m_DirOffset.end();)
    {
      std::ostringstream sstr;
      
      if (msg == "QueryEnergySensor")
        sstr << m_EnergySources->determineEnergySourceLevel(row+(*dIter)[0], 
                                                    col+(*dIter)[1]);
      else if (msg == "QueryGoalSensor")
        sstr <<  m_Goals->determineGoalLevel(row+(*dIter)[0], 
                                             col+(*dIter)[1]);
      
      result += sstr.str();
      ++dIter;
      if (dIter == m_DirOffset.end())
        break;
      else
        result += ",";
    }
  std::cout << "res: " << result << std::endl;
  
  m_SSWGClient.sendMessage(result, sender);
}

void Robot::queryVisibility(const std::string& sender, const std::string& msg)
{
  int row, col;
  std::string result=msg+",";
  //    readRobotPosition(row, col);
  m_RobotPositionServer->getRobotPosition(m_Name, row, col);
  
  std::vector<std::vector<int> >::const_iterator dIter = m_DirOffset.begin();
  int currRow = row;
  int currCol = col;
  int iter =  m_DirOffset.size();
  // The last (row, col) pair is the curr location. No need to consider it
  // for visibility
  while (iter > 1)
    {
      std::ostringstream sstr;
      bool noWall = m_Terrain->isTraversable(currRow, currCol, currRow+(*dIter)[0], 
                                             currCol+(*dIter)[1]);
      // If no wall make sure there are no other robots occupying the location.
      if (noWall && m_RobotPositionServer->gridOccupied(currRow+(*dIter)[0],
                                                        currCol+(*dIter)[1]))
        sstr << "-1";
      else
        sstr << noWall;
      
      result += sstr.str();
      ++dIter;
      --iter;
      if (iter > 1) result += ",";
    }
  std::cout << "res: " << result << std::endl;
  
  m_SSWGClient.sendMessage(result, sender);
}

void Robot::moveRobot(const std::string& sender, const std::string& str)
{
  int rowDirOffset, colDirOffset;
  std::string retCode=str;
  
  if (str == "MoveUp") 
    {
      rowDirOffset = -1;
      colDirOffset = 0;
    }
  else if (str == "MoveDown")
    {
      rowDirOffset = 1;
      colDirOffset = 0;
    }
  else if (str == "MoveRight")
    {
      rowDirOffset = 0;
      colDirOffset = 1;
    }
  else if (str == "MoveLeft")
    {
      rowDirOffset = 0;
      colDirOffset = -1;
    }
  else
    std::cout << "moveRobot: Unknown direction." << std::endl;
  
  int rowCurr, colCurr;
  m_RobotPositionServer->getRobotPosition(m_Name, rowCurr, colCurr);
  int rowNext = rowCurr + rowDirOffset;
  int colNext = colCurr + colDirOffset;
  bool traversible = false;
  if ((traversible = m_Terrain->isTraversable(rowCurr, colCurr, rowNext, colNext)) &&
      m_RobotPositionServer->setRobotPosition(m_Name, rowNext, colNext))
    {
      setRobotPositionLocal(rowNext, colNext);// local cache for display purposes only
      updateRobotEnergyLevel(m_EnergySources->acquireEnergySource(rowNext, colNext)-0.025);
      retCode += ",1";
    }
  else if (!traversible)
    {
      std::cout << "moveRobot: Cannot move robot to the desired location due to" 
                << " a fixed obstacle." << std::endl;
      retCode += ",0";
    }
  else
    {
      std::cout << "moveRobot: Cannot move robot to the desired location due to" 
                << " a dynamic obstacle." << std::endl;
      retCode += ",-1";
    }
  
  m_SSWGClient.sendMessage(retCode, sender);
}