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

package gov.nasa.luv;

import java.awt.Color;
import javax.swing.JMenu;
import javax.swing.JSeparator;
import javax.swing.JPopupMenu;
import java.util.Vector;
import java.util.HashMap;
import java.util.Map;
import java.awt.event.ActionEvent;
import static gov.nasa.luv.Constants.*;

/**
 * The LuvBreakPointHandler class provides methods to handle breakpoints set
 * on a Plexil Plan loaded in the Luv application.
 */

public class LuvBreakPointHandler 
{    
      // if break has occured, the causal break point object     
      private LuvBreakPoint breakPoint;    
      
      // collection of all breakpoints
      private HashMap<LuvBreakPoint, Integer> breakPointMap;  
      
      // breakpoints not found in current plan
      private Vector<LuvBreakPoint> unfoundBreakPoints;      
      
      // list of breakpoints to check for repeats
      private Vector<String> breakPointList; 
      
      /**
       * Constructs a LuvBreakPointHandler.
       */
      public LuvBreakPointHandler()
      {
          breakPoint = null;
          breakPointMap = new HashMap<LuvBreakPoint, Integer>();
          unfoundBreakPoints = new Vector<LuvBreakPoint>();
          breakPointList = new Vector<String>();
      }
      
      /**
       * Returns the LuvBreakPoint.
       * @return the LuvBreakPoint
       */
      public BreakPoint getBreakPoint()
      {
          return breakPoint;
      }
      
      /**
       * Returns the HashMap of all LuvBreakPoints and matching row number.
       * @return the HashMap of all LuvBreakPoints and matching row number
       */
      public HashMap<LuvBreakPoint, Integer> getBreakPointMap()
      {
          return breakPointMap;
      }
      
      /**
       * Returns the Vector of all the LuvBreakPoints not found in current Plexil Plan.
       * @return the Vector of all the LuvBreakPoints not found in current Plexil Plan
       */
      public Vector<LuvBreakPoint> getUnfoundBreakPoints()
      {
          return unfoundBreakPoints;
      }
      
      /**
       * Returns whether or not LuvBreakPoints exist in the current Plexil Plan.
       * @return whether or not LuvBreakPoints exist in the current Plexil Plan
       */
      public boolean breakpointsExist()
      {
          return !breakPointList.isEmpty();
      }
      
      /**
       * Sets the specified LuvBreakPoint in the current Plexil Plan.
       * @param bp the LuvBreakPoint to be set
       */
      public void setBreakPoint(LuvBreakPoint bp)
      {
          breakPoint = bp;
      }

      /**
       * Clears the LuvBreakPoint.
       */
      public void clearBreakPoint()
      {
          breakPoint = null;
      }

      /**
       * Adds the specified LuvBreakPoint to the full list of LuvBreakPoints.
       * @param breakPoint the LuvBreakPoint to be added
       * @param model the model on which the LuvBreakPoint was added
       */
      public void addBreakPoint(LuvBreakPoint breakPoint, Model model)
      {
         if (!breakPointList.contains(breakPoint.toString() + " " 
                 + model.getRowNumber()))
         {
             breakPointList.add(breakPoint.toString() + " "
                     + model.getRowNumber());

             breakPointMap.put(breakPoint, model.getRowNumber());
             
             Luv.getLuv().enableRemoveBreaksMenuItem(true);

             Luv.getLuv().getStatusMessageHandler().showStatus("Added break on " 
                     + breakPoint, 5000l);
             Luv.getLuv().getViewHandler().refreshView();       
         }
         else
             Luv.getLuv().getStatusMessageHandler().showStatus("\"" + breakPoint 
                     + "\" breakpoint has already been added", Color.RED, 5000l);
      }

      /**
       * Removes the specified LuvBreakPoint from the full list of LuvBreakPoints.
       * @param breakPoint
       */
      public void removeBreakPoint(BreakPoint breakPoint, Model model)
      {
         breakPoint.unregister();
         breakPointMap.remove(breakPoint);
         breakPointList.remove(breakPoint.toString() + " " + model.getRowNumber());
         
         Luv.getLuv().getStatusMessageHandler().showStatus("Removed break on " 
                 + breakPoint, 5000l);
         Luv.getLuv().getViewHandler().refreshView();
      }

      /**
       * Removes all the LuvBreakPoints.
       */
      public void removeAllBreakPoints()
      {
         for (BreakPoint bp: breakPointMap.keySet())
            bp.unregister();
         breakPointMap.clear();
         breakPointList.clear();
         unfoundBreakPoints.clear();
         breakPoint = null;
         Luv.getLuv().enableRemoveBreaksMenuItem(false);
         Luv.getLuv().getViewHandler().refreshView();
      }
      
      /**
       * Creates a LuvBreakPoint which fires when the specified model state changes at all.
       * 
       * @param model the model on which the LuvBreakPoint fires
       * @return the LuvBreakPoint
       */
      public BreakPoint createChangeBreakpoint(Model model)
      {
         return new LuvBreakPoint(model, MODEL_STATE)
            {
                  public boolean isBreak()
                  {
                     return !model.getProperty(MODEL_STATE).equals(oldValue);
                  }
                  
                  public void onBreak()
                  {
                     reason = model.getModelName() + 
                        " changed from " + oldValue +
                        " to " + model.getProperty(MODEL_STATE) + ".";
                     oldValue = model.getProperty(MODEL_STATE);
                     super.onBreak();
                  }

                  public String toString()
                  {
                     return model.getModelName();
                  }
         };
      }
      
      /**
       * Maps all the current LuvBreakPoints to the new specified Plexil model.
       * @param model the new Plexil model the current LuvBreakPoints will be mapped to
       */
      public void mapBreakPointsToNewModel(Model model)
      {
	    getUnfoundBreakPoints().clear();
         
	    for (Map.Entry<LuvBreakPoint, Integer> pair: getBreakPointMap().entrySet()) 
            {
		LuvBreakPoint bp = pair.getKey();
		int row_number = pair.getValue();

		Model target = model.findChildByRowNumber(row_number);
                model.resetFoundChildFlag();

                if (target != null)
                {
                    bp.setModel(target);
		}
		else
		    getUnfoundBreakPoints().add(bp);
	    }
      }

      /**
       * Creates a LuvBreakPoint which fires when the model state changes to a specified state.
       * 
       * @param model the model on which the LuvBreakPoint fires
       * @param propertyTitle the name of the the previous model state
       * @param targetProperty the name of the current model state
       * @param targetValue the name of the target model state the LuvBreakPoint will fire upon
       * @return the LuvBreakPoint
       */
      public BreakPoint createTargetPropertyValueBreakpoint(
         Model model, final String propertyTitle, 
         final String targetProperty, final String targetValue)
      {
         return new LuvBreakPoint(model, targetProperty)
            {
                  public boolean isBreak()
                  {
                     String newValue = model.getProperty(targetProperty);
                     if (newValue != null && 
                         !newValue.equals(oldValue) && 
                         newValue.equals(targetValue))
                     {
                        return true;
                     }
                     else 
                        oldValue = newValue;
                     
                     return false;
                  }
                  
                  public void onBreak()
                  {
                     reason = propertyTitle + " changed to " + 
                        model.getProperty(targetProperty) + ".";
                     oldValue = model.getProperty(targetProperty);
                     super.onBreak();
                  }

                  public String toString()
                  {
                     return propertyTitle + " changed to " + targetValue;
                  }
         };
      }
      
      /**
       * Construct a node popup menu when the user right-clicks on the specified 
       * Plexil model/node.
       * 
       * @param model the Plexil model the user wants to set a LuvBreakPoint on
       * @return the pop up menu
       */
      public JPopupMenu constructNodePopupBreakPointMenu(final Model model)
      {
         // get the model name
         final String name = model.getModelName();

         // construct the node popup menu         
         JPopupMenu popup = new JPopupMenu("Node Popup Menu");
         
         // get the break points for this model       
         final Vector<LuvBreakPoint> bps = getBreakPoints(model);

         // add node state change breakpoint
         popup.add(new LuvAction(
                      "Add Break Point for " + name + " State Change",
                      "Add a break point any time " + name + " changes state.")
            {
                  public void actionPerformed(ActionEvent e)
                  {
                     createChangeBreakpoint(model);
                  }
            });
         
         // add target state break points menu
         JMenu stateMenu = new JMenu(
            "Add Break Point for " + name + " State");
         stateMenu.setToolTipText(
            "Add a break point which pauses execution when " + name + 
            " reaches a specified state.");
         popup.add(stateMenu);
         for (final String state: NODE_STATES)
            stateMenu.add(new LuvAction(
                             state,
                             "Add a break point when " + name + 
                             " reaches the " + state + " state.")
               {
                     public void actionPerformed(ActionEvent e)
                     {
                        createTargetPropertyValueBreakpoint(
                           model, name + " state", MODEL_STATE, state);
                     }
               });

         // add target outcome break points menu
         JMenu outcomeMenu = new JMenu(
            "Add Break Point for " + name + " Outcome");
         outcomeMenu.setToolTipText(
            "Add a break point which pauses execution when " + name + 
            " reaches a specified outcome.");
         popup.add(outcomeMenu);
         for (final String outcome: NODE_OUTCOMES)
            outcomeMenu.add(new LuvAction(
                               outcome,
                               "Add a break point when " + name + 
                               " reaches the " + outcome + " outcome.")
               {
                     public void actionPerformed(ActionEvent e)
                     {
                        createTargetPropertyValueBreakpoint(
                           model, name + " outcome", MODEL_OUTCOME, outcome);
                     }
               }); 

         // add target failure type break points menu
         JMenu failureTypeMenu = new JMenu(
            "Add Break Point for " + name + " Failure Type");
         failureTypeMenu.setToolTipText(
            "Add a break point which pauses execution when " + name + 
            " reaches a specified failure type.");
         popup.add(failureTypeMenu);
         for (final String failureType: NODE_FAILURE_TYPES)
            failureTypeMenu.add(new LuvAction(
                                   failureType,
                                   "Add a break point when " + name + 
                                   " reaches the " + failureType +
                                   " failure type.")
               {
                     public void actionPerformed(ActionEvent e)
                     {
                        createTargetPropertyValueBreakpoint(
                           model, name + " failure type", 
                           MODEL_FAILURE_TYPE, failureType);
                     }
               }); 

         // if we got any add enable/disable & remove item for each one
         //if (bps.size() > 0 && !Luv.getLuv().getIsExecuting())
         if (bps.size() > 0)
         {
            // add the breakpoints
            popup.add(new JSeparator());
            for (final LuvBreakPoint bp: bps)
            {
               String action = bp.isEnabled() ? "Disable" : "Enable";
               popup.add(new LuvAction(
                            action + " " + bp,
                            action + " the breakpoint " + 
                            bp + ".")
                  {
                        public void actionPerformed(ActionEvent e)
                        {
                           bp.setEnabled(!bp.isEnabled());
                           if (!bp.isEnabled())
                               bp.reserveBreakStatus(true);
                           Luv.getLuv().getViewHandler().refreshView();
                        }
                  }); 
            }

            // add the breakpoints
            popup.add(new JSeparator());
            for (final BreakPoint bp2: bps)
            {
               popup.add(new LuvAction(
                            "Remove " + bp2,
                            "Permanently remove the breakpoint " + 
                            bp2 + ".")
                  {
                        public void actionPerformed(ActionEvent e)
                        {
                           removeBreakPoint(bp2, model);
                        }
                  }); 
            }
         }

         // if there is more then one break point add a remove all item
         if (bps.size() > 1)
         {
            // add the remove all action           
            popup.add(new JSeparator());
            popup.add(new LuvAction(
                         "Remove All Break Points From " + name,
                         "Permanently remove all breakpoints from " + name + ".")
               {
                     public void actionPerformed(ActionEvent e)
                     {
                        for (final BreakPoint bp3: bps)
                           removeBreakPoint(bp3, model);
                     }
               });
         }
               
         Luv.getLuv().updateBlockingMenuItems();        

         // return our freshly created popup menu
         return popup;
      }

      /**
       * Returns all the LuvBreakPoints for the specified Plexil model.
       * @param model the Plexil model that contains the current LuvBreakPoints
       * @return the Vector of LuvBreakPoints
       */
      public Vector<LuvBreakPoint> getBreakPoints(Model model)
      {
         Vector<LuvBreakPoint> bps = new Vector<LuvBreakPoint>();
         for (LuvBreakPoint bp: breakPointMap.keySet())
            if (model == bp.getModel())
               bps.add(bp);
         return bps;
      }
}
