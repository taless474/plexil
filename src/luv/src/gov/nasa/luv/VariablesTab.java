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

import gov.nasa.luv.Model.ChangeAdapter;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;

/** 
 * The VariablesTab class provides methods for displaying a Plexil Model's local 
 * variable information. 
 */

public class VariablesTab extends JPanel 
{ 
    private static VariablesTab variablesPane;
    private Model model;   
    private int rows;
    private String info[][];
    private JTable table;   
    
    public VariablesTab() {}
    
    /** 
     * Constructs a VariablesTab with the specified Plexil Model.
     *
     * @param model model on which the VariablesTab represents
     */
    
    public VariablesTab(Model model) 
    {       
        super(new GridLayout(1,0));
        
        this.model = model;


        String[] columnNames = {"In/InOut",
                                "Name",
                                "Type",
                                "Value",
        };
        
        rows = 1000;
        info = new String[rows][4];
        table = new JTable(info, columnNames);
        table.setAutoResizeMode(JTable.AUTO_RESIZE_SUBSEQUENT_COLUMNS);
        table.getColumnModel().getColumn(0).setPreferredWidth(50);
        table.getColumnModel().getColumn(1).setPreferredWidth(200);
        table.getColumnModel().getColumn(2).setPreferredWidth(100);
        table.getColumnModel().getColumn(3).setPreferredWidth(550);
        table.setPreferredScrollableViewportSize(new Dimension(900, 300));
        table.setShowGrid(false);
        table.setGridColor(Color.GRAY);
        
        refreshTable();
        
        JScrollPane scrollPane = new JScrollPane(table);
        add(scrollPane);
        model.addChangeListener(new ChangeAdapter() {
        	public void variableAssigned(Model model, String variableName) {
        		refreshTable();
        	}
        });
        setOpaque(true);
    }
    
    private void refreshTable() {
        int row = 0;
        int col = 0;
        for (Variable original : model.getVariableList())
        {
            if (original != null)
            {
                info[row][col] = original.getInOut(); 
                col++;
                info[row][col] = original.getName(); 
                col++;
                info[row][col] = original.getType(); 
                col++;
                info[row][col] = original.getValue(); 
                
                col = 0;
                ++row;
            }
        }
        table.addNotify();
    }
    
    /** 
     * Returns the current instance of the VariablesTab. 
     *
     * @return the current instance of the VariablesTab
     */
    
    public static VariablesTab getCurrentVariablesTab()
    {
        return variablesPane;
    }
    
    /** 
     * Creates an instance of a VariablesTab with the specified model. 
     *
     * @param model the model on which to create an VariablesTab
     */

    public static void open(Model model) 
    {       
        variablesPane = new VariablesTab(model);
    }
}
