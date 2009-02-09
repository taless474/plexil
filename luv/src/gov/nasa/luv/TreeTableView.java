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

import javax.swing.JTree;
import javax.swing.JTable;
import javax.swing.tree.TreePath;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.AbstractTableModel;
import javax.swing.JPopupMenu;

import java.awt.Color;
import java.awt.Component;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseAdapter;

import java.util.ArrayList;
import java.util.Vector;
import java.util.Enumeration;

import java.util.Stack;
import javax.swing.text.Position;
import treetable.TreeTableModel;
import treetable.AbstractTreeTableModel;

import static gov.nasa.luv.Constants.*;
import static java.awt.event.KeyEvent.*;

import treetable.JTreeTable;

public class TreeTableView extends JTreeTable implements View
{
      /** This is the most recent tree view created stored in case it's
       * needed to set expansion state of the items in the new tree */

      private static TreeTableView lastView;
      
      private static ArrayList<String> path = new ArrayList<String>();

      /** swing tree object */

      private JTree tree;

      /** root of tree */

      private Wrapper root;

      /** show or hid node types */

      private static boolean showTextTypes = false;
      
      private static int currentBreakingRow = -1;
      
      private static int row = -1;
      private static int lastRow = -1;

      /** Construct a tree view. 
       *
       * @param name the name of the view
       * @param model model this view is viewing
       */

      public TreeTableView(String name, Model model)
      {
         super(new TreeModel(new Wrapper(model)));
         setAutoResizeMode(AUTO_RESIZE_OFF);
         setRowColors(TREE_TABLE_ROW_COLORS);

         tree = getTree();   

         final JTable table = this;

         // add mouse listener which puts up pop-up menus

         addMouseListener(new MouseAdapter()
            {
            @Override
                  public void mousePressed(MouseEvent e)
                  {
                     if (e.isPopupTrigger())
                        handlePopupEvent(e);
                     else if (e.getClickCount() == 2)
                         handleClickEvent(e);
                  }
            });
         
         root = (Wrapper)tree.getModel().getRoot();
         Wrapper.setView(this);

         // manage handles

         tree.setShowsRootHandles(true);

         // if there is an old version of this view

         if (lastView != null)
         {
            // regain the expand stat from the old view

            regainExpandedState(
               tree, new TreePath(root),
               lastView.tree, new TreePath(lastView.root));

            // set the column widths to the old view

            setPreferredColumnWidths();
         }
         lastView = this;

         // setup the column coloring logic

         setDefaultRenderer(
            TreeModel.cTypes[STATE_COL_NUM], 
            new DefaultTableCellRenderer()
            {
                  public Component getTableCellRendererComponent(
                     JTable table, 
                     Object value, 
                     boolean isSelected, 
                     boolean hasFocus, 
                     int row, 
                     int column)
                  {
                     Component component = super.getTableCellRendererComponent(
                        table, value, isSelected, hasFocus, row, column);
                     
                     String label = (String)value;
                     Color color = Constants.lookupColor(label);
                     setForeground(color != null
                                   ? color
                                   : Color.BLACK);
                     
                     setBackground(isSelected ? table.getSelectionBackground() : getRowColor(row));  

                     return component;
                  }
            });
            
      /*      setDefaultRenderer(
            TreeModel.cTypes[ROW_COL_NUM], 
            new DefaultTableCellRenderer()
            {
                  public Component getTableCellRendererComponent(
                     JTable table, 
                     Object value, 
                     boolean isSelected, 
                     boolean hasFocus, 
                     int row, 
                     int column)
                  {
                     Component component = super.getTableCellRendererComponent(
                        table, value, isSelected, hasFocus, row, column);
                     
                     setForeground(Color.lightGray);
                     setBackground(isSelected ? table.getSelectionBackground() : getRowColor(row)); 

                     return component;
                  }
            });*/

         // set cell renderer to customize icons

         tree.setCellRenderer(new DefaultTreeCellRenderer()
            {
                  public Component getTreeCellRendererComponent(
                     JTree newTree,
                     Object value,
                     boolean isSelected,
                     boolean expanded,
                     boolean leaf,
                     int row,
                     boolean hasFocus) 
                  {
                     Component component = super.getTreeCellRendererComponent(
                        newTree, value, isSelected, expanded, leaf, row, hasFocus);
                     
                     Model model = ((Wrapper)value).model;

                     setIcon(
                        !showTextTypes 
                        ? Constants.getIcon(model.getProperty(NODETYPE_ATTR))
                        : null);
                     
                     Vector<LuvBreakPoint> breakPoints = Luv.getLuv().getLuvBreakPointHandler().getBreakPoints(model);
                     if (breakPoints.size() > 0)
                     {
                        setForeground(lookupColor(MODEL_DISABLED_BREAKPOINTS));
                        for (BreakPoint breakPoint: breakPoints)
                           if (breakPoint.isEnabled())
                           {
                              setForeground(lookupColor(MODEL_ENABLED_BREAKPOINTS));
                           }
                     }
                     
                     setBackgroundNonSelectionColor(
                        isSelected
                        ? table.getSelectionBackground()
                        : getRowColor(row));
                     
                     return component;
                  }
            });
      }
      
      public boolean selectRow(int row)
      {
          if (row > -1)
          {
              tree.setSelectionRow(row);
              return true;
          }
          else
              return false;
      }
      
      public void highlightRow(Model node)
      {
          restartSearch();
          int highlight = findNode(node.getPath(node));    

          if (highlight > -1)
          {
              currentBreakingRow = highlight;
              tree.setSelectionRow(currentBreakingRow);
              lastView.setSelectionBackground(Color.PINK);
              scrollToRow(currentBreakingRow);
          }
      }
      
      public void unHighlightRow()
      {
          if (currentBreakingRow >= 0)
          {
              lastView.setSelectionBackground(getRowColor(currentBreakingRow));
              tree.setSelectionRow(-1);
          }
      }

      /** Display node information from under tool tip
       * 
       * @return tooltip text for element in tree
       */

      @Override public String getToolTipText(MouseEvent event)
      {
          StringBuffer   toolTip     = new StringBuffer();
          TreePath       nodePath    = tree.getPathForLocation(event.getX(), event.getY());       
          
          if (nodePath != null)
          {    
              Model          node        = ((Wrapper)nodePath.getLastPathComponent()).model;
              String         nodeName    = node.getModelName();
          
              toolTip.append("<html>");         
              toolTip.append("<b>NAME</b> " + nodeName);
              toolTip.append("<br><b>TYPE</b>  " + node.getProperty(MODEL_TYPE));
              toolTip.append("<br><hr>");
              toolTip.append("<b>Double-Click</b> on node to view condition information");    
              toolTip.append("<br><b>Right-Click</b> on node to set breakpoints");
          }
          
          return toolTip.length() > 0 ? toolTip.toString() : null;
      }

      /** Ensure that this views properties are propertly reflected in
       * the luv properties.
       *
       * @param properties the luv properties
       */

      public void getViewProperties(Properties properties)
      {
         // handle column widths 

         setPreferredColumnWidths();

         // handle text/icon node types

         showTextTypes = properties.getBoolean(PROP_TTV_TEXT_TYPES);
      }

      /** Allow this view to extract properties from the luv properties.
       *
       * @param properties the luv properties
       */

      public void setViewProperties(Properties properties)
      {
         properties.set(PROP_TTV_TEXT_TYPES, showTextTypes);
      }

      /** Handle popup menu event.
       *
       * @param mouseEvent event which triggered popup menu
       * @param luv current active luv
       */

      public void handlePopupEvent(MouseEvent mouseEvent)
      {
          if (Luv.getLuv().breaksAllowed())
          {          
             TreePath   nodePath  = tree.getClosestPathForLocation(mouseEvent.getX(), mouseEvent.getY());
             Model      node = ((Wrapper)nodePath.getLastPathComponent()).model;
             JPopupMenu popup = new JPopupMenu();

             // construct the popup menu

             popup = Luv.getLuv().getLuvBreakPointHandler().constructNodePopupBreakPointMenu(node);

             // display the popup menu

             popup.show(mouseEvent.getComponent(),mouseEvent.getX(), mouseEvent.getY());
          }
      }
      
      public ArrayList<String> getPathToNode()
      {
          return path;
      }

      /** Handle popup menu event.
       *
       * @param mouseEvent event which triggered popup menu
       * @param luv current active luv
       */

      public void handleClickEvent(MouseEvent mouseEvent)
      {
         TreePath   nodePath    = tree.getClosestPathForLocation(mouseEvent.getX(), mouseEvent.getY());
         Model      node        = ((Wrapper)nodePath.getLastPathComponent()).model;
         
         // save condition window information 
         
         saveConditionWindow(node, nodePath); 
         
         // create information window if node has any additional data to show

         if (node.hasConditions() || node.hasVariables() || node.hasAction())
         {
             Luv.getLuv().getConditionsTab().createConditionTab(node);
             Luv.getLuv().getVariablesTab().createVariableTab(node);  
             Luv.getLuv().getActionTab().createActionTab(node);
             Luv.getLuv().getNodeInfoTabbedWindow().createAndShowGUI(node);
         }
         else
         {
             Luv.getLuv().showStatus("No additional information is available for " + node.getModelName(), 5000);
         }
             
      }
      
      public void saveConditionWindow(Model node, TreePath tp)
      {
         path.clear();
         path.add(node.getModelName());

         while (!node.isRoot())
         {
             path.add(node.getParent().getModelName());
             node = node.getParent();
         } 
         
      }
      
      public boolean isNodeInfoTabbedWindowOpen()
      {
          return Luv.getLuv().getNodeInfoTabbedWindow().isNodeInfoTabbedWindowOpen();
      }
      
      public void closeNodeInfoTabbedWindow()
      {
          Luv.getLuv().getNodeInfoTabbedWindow().closeNodeInfoTabbedWindow();
      }

      /** Focus has been disabled for this view.  This way the view
       * doesn't grab up all the key events that it shouldn't.  This is
       * not the best way to acheive this goal.
       *
       * @return good old false, nothing beats false
       */
      
      @Override public boolean isFocusable()
      {
         return false;
      }

      /** Get the current tree table view if it exists.
       *
       * @return current tree table view, or null if it does not exist
       */

      public static TreeTableView getCurrent()
      {
         return lastView;
      }

      /** Get the current column widths.
       *
       * @return a vector containing the column widths
       */

      public Vector<Integer> getColumnWidths()
      {
         Vector<Integer> widths = new Vector<Integer>();

         Enumeration<TableColumn> columns = getColumnModel().getColumns();
         while (columns.hasMoreElements())
            widths.add(columns.nextElement().getWidth());
         return widths;
      }

      /** Set Preferred the current column widths.
       *
       * @param widths a vector containing the column widths
       */

      public void setPreferredColumnWidths()
      {
          //int row_col = 40;
          int status_col = 130;
          int outcome_col = 70;
          int fail_col = 230;
          int name_col = Luv.getLuv().getRootPane().getWidth() - (status_col + outcome_col + fail_col);
          
          Vector<Integer> widths = new Vector<Integer>();
        //  widths.add(row_col);
          widths.add(name_col);
          widths.add(status_col);
          widths.add(outcome_col);
          widths.add(fail_col);
          
          Enumeration<TableColumn> columns = getColumnModel().getColumns();
          
          for (int width : widths)
          {
              columns.nextElement().setPreferredWidth(width);          
          }
      }

      /** If the previous view was the same plan, then set the expanded
       * state of the new tree to that of the old tree.
       *
       * @param t1  original tree
       * @param tp1 current path in original tree
       * @param t2  new tree
       * @param tp2 current path in new tree
       * @return true if the two plans are the same
       */
      
      public boolean regainExpandedState(JTree t1, TreePath tp1, 
                                         JTree t2, TreePath tp2)
      {
         // get the tree nodes for the paths

         Wrapper w1 =
            (Wrapper)tp1.getLastPathComponent();
         Wrapper w2 =
            (Wrapper)tp2.getLastPathComponent();

         // if the child count differs, then we should stop right here,
         // the two trees are differnt

          if (w1.getChildCount() != w2.getChildCount())
             return false;

          // if the names differ, then we should stop right here, the two
          // trees are differnt

          if (!w1.equals(w2))
             return false;

          // traverse down the tree

         for (int i = 0; i < w1.getChildCount(); ++i)
         {
            if (!regainExpandedState(
                   t1, tp1.pathByAddingChild(w1.getChild(i)),
                   t2, tp2.pathByAddingChild(w2.getChild(i))))
               return false;
         }
         
         // set the expanded state on the new tree to mach the old tree

         if (t2.isExpanded(tp2))
            t1.expandPath(tp1);
         else
            t1.collapsePath(tp1);

         // return true, both trees are the same thus far

         return true;
      }

      /** This class wraps a model object and provides a naming and
       * equivelency testing services. 
       */

      public static class Wrapper
      {            
            Model model;
            Vector<Wrapper> children = new Vector<Wrapper>();
            static TreeTableView view;
            static int changed_row;                

            public Wrapper(Model model)
            {
               this.model = model;

               model.addChangeListener(new Model.ChangeAdapter()
                  {
                        public void propertyChange(Model model, String property)
                        {     
                            changed_row = getChangedRow(model.getPath(model));
                            
                            if (changed_row != -1)
                                ((AbstractTableModel)view.getModel()).fireTableCellUpdated(changed_row, getPropertyNum(property));
                            //((AbstractTableModel)view.getModel()).fireTableCellUpdated(model.getRowNumber(), getPropertyNum(property));
                        }
                  });
                
               addNodesToTree(model); 
            }
            
            
            
            private int getChangedRow(Stack<String> node_path)
            {               
                TreePath test = null;
                JTree t = view.getTree();
                int r = -1;
                
                while (!node_path.empty())
                {
                    test = t.getNextMatch(node_path.pop(), r + 1, Position.Bias.Forward); 

                    r = t.getRowForPath(test);                          
                }

                return r;
            }
            
            private void addNodesToTree(Model model)
            {
                for (Model child: model.getChildren()) 
                {
                    if (!AbstractModelFilter.isModelFiltered(child))
                    {
                        children.add(new Wrapper(child));
                    }
                    else
                    {
                        addNodesToTree(child);
                    }
                }
            }

            public static void setView(TreeTableView view)
            {
               Wrapper.view = view;
            }

            public String toString()
            {
               return showTextTypes
                     ? model.getProperty(MODEL_TYPE) + " " + model.getModelName()
                     : model.getModelName();
            }

            public Model getModel()
            {
               return model;
            }

            public boolean equals(Wrapper other)
            {
               String n1 = model.getModelName();
               String n2 = other.model.getModelName();
               
               if (n1 == null)
                   return false;
               
               if (n2 == null)
                   return false;

               return n1.equals(n2);
            }

            public Vector<Wrapper> getChildren()
            {
               return children;
            }

            public int getChildCount()
            {
               return children.size();
            }

            public Wrapper getChild(int i)
            {
               return children.get(i);
            }
      }

      public static class TreeModel extends AbstractTreeTableModel<Object> 
      {
            // column names
            
            static String[]  cNames = 
            {
             //  ROW_COL_NAME,
               NAME_COL_NAME,
               STATE_COL_NAME,
               OUTCOME_COL_NAME,
               FAILURE_TYPE_COL_NAME,
               
            };
            
            // column types

            static Class[]  cTypes = 
            {
             //  Integer.class,
               TreeTableModel.class,
               String.class, 
               String.class, 
               String.class, 
               
            };

            public TreeModel(Wrapper model)
            {
               super(model);
            }

            //
            //  The TreeTableNode interface. 
            //
            
            public int getChildCount(Object node) 
            { 
               return ((Wrapper)node).getChildCount();
            }
            
            public Object getChild(Object node, int i) 
            {
               return ((Wrapper)node).getChild(i);
            }

            public boolean isLeaf(Object node)
            {
                return ((Wrapper)node).getChildCount() == 0;
            }
            
            public int getColumnCount() 
            {
               return cNames.length;
            }
            
            public String getColumnName(int column) 
            {
               return cNames[column];
            }
            
            public Class getColumnClass(int column) 
            {
               return cTypes[column];
            }

            public Object getValueAt(Object node, int column) 
            {
               Model model = ((Wrapper)node).getModel();
               
              // if (column == ROW_COL_NUM)
                //  return model.getRowNumber() + 1;
               
               if (column == NAME_COL_NUM)
                  return null;

               if (column == STATE_COL_NUM)
                  return model.getProperty(MODEL_STATE);

               if (column == OUTCOME_COL_NUM)
                  return model.getProperty(MODEL_OUTCOME);

               if (column == FAILURE_TYPE_COL_NUM)
                  return model.getProperty(MODEL_FAILURE_TYPE);

               return "<blank>";
            }
      }

      /** get this view */

      public View getView()
      {
          return this;
      }
      
      public void restartSearch()
      {
          row = lastRow = -1;
          tree.clearSelection();
      }
      
      public int showNode(Stack<String> node_path, int next)
      {
          findNode(node_path);
          selectRow(row);
          if (next == 0)
            scrollToRow(0);
          else
            scrollToRow(row);
          return row;
      }
      
      private int findNode(Stack<String> node_path)
      {
          TreePath test = null;
          while (!node_path.empty())
          {
              if (tree.getRowCount() > row + 1)
              {
                  String validate = node_path.peek();
                  test = tree.getNextMatch(node_path.pop(), row + 1, Position.Bias.Forward); 

                  if (node_path.empty() && !test.toString().endsWith(validate + "]"))
                  {
                      node_path.push(validate);
                  }
                  
                  row = tree.getRowForPath(test);    
                      
                  if (!node_path.empty())
                      tree.expandPath(test); 
                  else if (row == lastRow)
                      node_path.push(validate);
              }
              else
                  test = tree.getNextMatch(node_path.pop(), 0, Position.Bias.Forward);                          
          } 
          
          lastRow = tree.getRowForPath(test);
          
          return row;
      }
      
      public void scrollToRow(int row)
      {
          tree.getAutoscrolls();
          int width = Luv.getLuv().getRootPane().getWidth();
          int height = Luv.getLuv().getRootPane().getHeight();
          int start = this.getRowHeight() * (row - 3);
          
          scrollRectToVisible(new Rectangle(0, start, width, height));
      }              
      
      /** Expand all nodes. */

      public void expandAllNodes()
      {
         for (int i = 0; i < tree.getRowCount(); i++)
            tree.expandRow(i);
      }

      /** Collapse all nodes. */

      public void collapseAllNodes()
      {
         for (int i = tree.getRowCount() - 1; i >= 0; i--) 
            tree.collapseRow(i);
      }

      /** Return an array of specalized actions for this view.
       *
       * @return an array of specalized actions for this view.
       */

      public LuvAction[] getViewActions()
      {
         LuvAction[] actions =
            {
               expandAll,
               collapseAll,
               Luv.getLuv().hideOrShowNodes,
               Luv.getLuv().findNode,
            };
         return actions;
      }
      
      /** Action to fully expand tree. */

      LuvAction expandAll = new LuvAction(
         "Expand All", "Expand all tree nodes.", VK_EQUALS)
         {
               public void actionPerformed(ActionEvent e)
               {                   
                   expandAllNodes();
                   Luv.getLuv().getViewHandler().refreshRegexView(Luv.getLuv().getCurrentPlan());
               }
         };

      /** Action to fully collapse tree. */

      LuvAction collapseAll = new LuvAction(
         "Collapse All", "Collapse all tree nodes.", VK_MINUS)
         {
               public void actionPerformed(ActionEvent e)
               {
                   collapseAllNodes();
                   Luv.getLuv().getViewHandler().refreshRegexView(Luv.getLuv().getCurrentPlan());
               }
         };
}
