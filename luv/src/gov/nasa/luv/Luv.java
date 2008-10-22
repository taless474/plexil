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

import javax.swing.JFrame;
import javax.swing.JMenuBar;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.border.EmptyBorder;

import java.awt.Container;
import java.awt.Color;
import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowAdapter;

import java.io.IOException;

import java.io.File;
import java.util.HashMap;
import javax.swing.JMenu;
import javax.swing.JSeparator;
import static gov.nasa.luv.Constants.*;

import static java.lang.System.*;
import static java.awt.BorderLayout.*;
import static java.awt.event.KeyEvent.*;
import static javax.swing.JFileChooser.*;

/**
 * The containing frame for the Lightweight UE Viewer.
 */

public class Luv extends JFrame
{
    // variables
    
    private static boolean allowBreaks                 = false;        // is current instance of luv have breaks enabled?
    private static boolean execBlocks                  = false;        // does current exec instance block after every step?
    private static boolean executedViaLuvViewer        = false;        // was current instance of luv executed via the viewer itself?
    private static boolean executedViaCommandPrompt    = false;        // was current instance of luv executed via the command prompt?
    private static boolean isExecuting                 = false;        // is instance of luv currently executing?   
    private static boolean atStartScreen               = true;         // is current instance of luv only just started with no plan loaded?
    private static boolean dontLoadScriptAgain         = false;        // is script already loaded? if so, do not waste time loading it again
    private static boolean stopSearchForMissingLibs    = false;        // is library found? if so, stop searching for missing libraries 
    private static boolean openedPlanViaLuvViewer      = false;        // is current instance of luv executing a plan that was opened via the viewer itself?
    private static boolean planPaused                  = false;        // is instance of luv currently paused?    
    private static boolean planStep                    = false;        // is instance of luv currently stepping? 
    private static boolean stopExecution               = false;
    private static boolean cancelPlanLoading           = false;
      
    // handler instances
      
    private static FileHandler                  fileHandler                   = new FileHandler();                 // handles all file operations
    private static StatusMessageHandler         statusMessageHandler          = new StatusMessageHandler();        // handles all status messages
    private static LuvBreakPointHandler         luvBreakPointHandler          = new LuvBreakPointHandler();        // handles all break points
    private static ExecutionViaLuvViewerHandler executionViaLuvViewerHandler  = new ExecutionViaLuvViewerHandler();// handles when user executes plan via Luv Viewer itself
    private static ViewHandler                  viewHandler                   = new ViewHandler();                 // handles all file operations
    private static ConditionHandler             conditionHandler;             // saves node's condition information
      
    private JMenu fileMenu                = new JMenu("File");  
    private JMenu recentRunMenu           = new JMenu("Recent Runs");
    private JMenu runMenu                 = new JMenu("Run");   
    private JMenu viewMenu                = new JMenu("View");
    private JMenu windowMenu              = new JMenu("Windows");
      
    private HashMap<String, String> libraryNames = new HashMap<String, String>();
      
    private DebugWindow luvViewerDebugWindow;   
      
    SocketServer s;
      
    // current working instance of luv
      
    private static Luv theLuv;         

    // persistent properties for luv viewer 

    private Properties properties = new Properties(PROPERTIES_FILE_LOCATION)
	{
	    {    
		define(PROP_FILE_RECENT_COUNT, PROP_FILE_RECENT_COUNT_DEF);
		define(PROP_WIN_LOC,  PROP_WIN_LOC_DEF);
		define(PROP_WIN_SIZE, PROP_WIN_SIZE_DEF);
		define(PROP_WIN_BCLR, PROP_WIN_BCLR_DEF);
		define(PROP_DBWIN_LOC,     PROP_DBWIN_LOC_DEF);
		define(PROP_DBWIN_SIZE,    PROP_DBWIN_SIZE_DEF);
		define(PROP_DBWIN_VISIBLE, PROP_DBWIN_VISIBLE_DEF);
		define(PROP_TOOLTIP_DISMISS, PROP_TOOLTIP_DISMISS_DEF);
		define(PROP_NET_SERVER_PORT,  PROP_NET_SERVER_PORT_DEF);
		define(PROP_NET_RECENT_HOST,  PROP_NET_RECENT_HOST_DEF);
		define(PROP_NET_AUTO_CONNECT, PROP_NET_AUTO_CONNECT_DEF);
		define(PROP_VIEW_HIDE_PLEXILLISP, PROP_VIEW_HIDE_PLEXILLISP_DEF);
		define(PROP_FILE_RECENT_PLAN_DIR, getProperty(PROP_FILE_RECENT_PLAN_BASE + 1, UNKNOWN));
		define(PROP_FILE_RECENT_SCRIPT_DIR, getProperty(PROP_FILE_RECENT_SCRIPT_BASE + 1, UNKNOWN));
	    }
	};

    // entry point for this program

    public static void main(String[] args)
    {
	runApp();
    }
      
    public static void runApp()
    {
	// if we're on a mac, use mac style menus
         
	System.setProperty("apple.laf.useScreenMenuBar", "true");
         
	try {
            new Luv();
	}
	catch (Exception e) {
            e.printStackTrace();
	}
    }
      
    // constructor for luv viewer
      
    public Luv()
    {
	// record instance of self

	theLuv = this;

	// app exits when frame is closed

	setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

	// construct the frame
         
	constructFrame(getContentPane());    
         
	// create title
         
	setTitle();
         
         // luv will be in this state only when you first start it (minimal options available)
         
	startState();

	// start the server listening for events

	s = new SocketServer(properties.getInteger(PROP_NET_SERVER_PORT),
			     new LuvSocketWranglerFactory());

    }


    // Called from PlexilPlanHandler::endElement(),
    // which will be invoked by both Luv (directly)
    // and the exec (via the Luv listener stream)
    public void handleNewPlan(Model plan) throws IOException
    {
	// *** link libraries here ?? ***

	// check if plan is a duplicate
	// *** N.B. This depends on the fact that new plans are always added at the end
	Model other = Model.getRoot().findChildByName(plan.getModelName());
	if (plan == other) {
	    // it's really new
	}
	else if (plan.equivalent(other)) {
	    // it's a duplicate, ignore
	    Model.getRoot().removeChild(plan);
	    plan = other;
	}
	else {
	    // it has same name as original, new one supersedes
	    Model.getRoot().removeChild(other);
	}

	Model.getRoot().planChanged();

	viewHandler.focusView(plan);

	if (isExecuting) {
	    if (openedPlanViaLuvViewer) {
		luvViewerExecutionState();
		addRunToRecentRunList();  // save plan and script to recent run list
	    }
	    else {
		cmdPromptExecutionState();
		conditionHandler = new ConditionHandler((Model) Model.getRoot().clone());
	    }
	}
                        
	if (TreeTableView.getCurrent().isNodeInfoWindowOpen())
	    refreshPopUpNodeWindow();
                        
	// Determine if the Luv Viewer should pause before executing. 
                                                  
	if (isExecuting && execBlocks) {
	    pausedState(); 
	    runMenu.setEnabled(true);
	}
    }

    // Called from PlexilPlanHandler::endElement()
    public void handleNewLibrary(Model library)
    {
	Model.getRoot().planChanged();

	// This causes tree to be displayed
	// viewHandler.resetView();
                        
	if (TreeTableView.getCurrent().isNodeInfoWindowOpen())
	    refreshPopUpNodeWindow();
    }

    public boolean shouldBlock()
    {
	return planPaused && !planStep;
    }

    public void blockViewer()
    {
	if (shouldBlock()) {
	    statusMessageHandler.showStatus((luvBreakPointHandler.getBreakPoint() == null
					     ? "Plan execution paused."
					     : luvBreakPointHandler.getBreakPoint().getReason()) +
					    "  Hit " + 
					    pauseAction.getAcceleratorDescription() +
					    " to resume, or " + 
					    stepAction.getAcceleratorDescription() +
					    " to step.",
					    Color.RED);

	    luvBreakPointHandler.clearBreakPoint();
                        
	    // wait here for user action
	    while (shouldBlock()) {
		try {
		    Thread.sleep(50);
		}
		catch (Exception e) {
		    e.printStackTrace();
		}
	    }
	}
    }
      
    public ViewHandler            getViewHandler()            { return viewHandler; }             // get current view handler

    public FileHandler            getFileHandler()            { return fileHandler; }             // get current file handler
      
    public LuvBreakPointHandler   getLuvBreakPointHandler()   { return luvBreakPointHandler; }    // get current breakpoint handler
      
    public ConditionHandler       getConditionHandler()       { return conditionHandler; }        // get current condition handler
      
    public Properties             getProperties()             { return properties; }              // get persistent properties for luv viewer
      
    public static Luv             getLuv()                    { return theLuv; }                  // get current active instance of luv viewer
      
    //
    // Accessors for local luv boolean variables
    //

    public boolean getExecBlocks()
    {
	return execBlocks;
    }

    //* Sets the member variable and updates the menu state.

    public void setExecBlocks(boolean value)
    {
	execBlocks = value;
	updateBlockingMenuItems();
    }

    public boolean getExecutedViaCommandPrompt()
    {
	return executedViaCommandPrompt;
    }

    public void setExecutedViaCommandPrompt(boolean value)
    {
	executedViaCommandPrompt = value;
    }

    public boolean getOpenedPlanViaLuvViewer()
    {
	return openedPlanViaLuvViewer;
    }

    public void setOpenedPlanViaLuvViewer(boolean value)
    {
	openedPlanViaLuvViewer = value;
    }

    public boolean getStopSearchForMissingLibs()
    {
	return stopSearchForMissingLibs;
    }

    public void setStopSearchForMissingLibs(boolean value)
    {
	stopSearchForMissingLibs = value;
    }

    public boolean getDontLoadScriptAgain()
    {
	return dontLoadScriptAgain;
    }

    public void setDontLoadScriptAgain(boolean value)
    {
	dontLoadScriptAgain = value;
    }

    public boolean getPlanStep()
    {
	return planStep;
    }

    public boolean getIsExecuting()
    {
	return isExecuting;
    }

    public void setIsExecuting(boolean value)
    {
	isExecuting = value;
	updateBlockingMenuItems();
    }

    public boolean isAtStartScreen()
    {
	return atStartScreen;
    }

    public boolean stoppedExecution()
    {
	return stopExecution;
    }

    public void setStopExecution(boolean value)
    {
	stopExecution = value;
    }

    public boolean breaksAllowed()
    {
	return allowBreaks;
    }

    public void setCancelPlanLoading(boolean value)
    {
	cancelPlanLoading = value;
    }


    //
    // State transitions
    //

    public void startState()
    {
	System.out.println("startState()");

	// reset all luv viewer variables
          
	disableAllMenus();
          
	allowBreaks = false;
	execBlocks = false;
	isExecuting = false;   
	atStartScreen = true;
	dontLoadScriptAgain = false;
	stopSearchForMissingLibs = false;
	openedPlanViaLuvViewer = false;         
	executedViaLuvViewer = false;
	planPaused = false;
	cancelPlanLoading = false;
          
	Model.getRoot().clear();  
	conditionHandler = new ConditionHandler((Model) Model.getRoot().clone());
          
	fileHandler.clearPlan(); 
	fileHandler.clearScript(); 

	viewHandler.clearCurrentView();
	statusMessageHandler.clearStatusMessageQ();
	luvBreakPointHandler.clearBreakPoint();
	luvBreakPointHandler.clearBreakPointMap();
	luvBreakPointHandler.clearUnfoundBreakPoints();
          
	// reset all menu items
          
	fileMenu.getItem(OPEN_PLAN_MENU_ITEM).setEnabled(true);
	fileMenu.getItem(OPEN_SCRIPT_MENU_ITEM).setEnabled(true);
	fileMenu.getItem(OPEN_RECENT_MENU_ITEM).setEnabled(true);
        fileMenu.getItem(RELOAD_MENU_ITEM).setEnabled(false);
	fileMenu.getItem(EXIT_MENU_ITEM).setEnabled(true);
	fileMenu.setEnabled(true);
          
	updateBlockingMenuItems();
	allowBreaksAction.putValue(NAME, ENABLE_BREAKS);
          
	windowMenu.getItem(SHOW_LUV_DEBUG_MENU_ITEM).setEnabled(true);
	windowMenu.setEnabled(true);
          
	if (stopExecution && !executedViaCommandPrompt) {
	    fileMenu.getItem(RELOAD_MENU_ITEM).setEnabled(true);
	}
    }
      
    public void readyState()
    {
	System.out.println("readyState()");

	// set only certain luv viewer variables
          
	planPaused = false;
	planStep = false;
	atStartScreen = false;
	stopSearchForMissingLibs = false; 
	isExecuting = false;
	stopExecution = false;        
	executedViaLuvViewer = false;
	cancelPlanLoading = false;
  
	// set certain menu items          
	
	fileMenu.getItem(OPEN_PLAN_MENU_ITEM).setEnabled(true);
	fileMenu.getItem(OPEN_SCRIPT_MENU_ITEM).setEnabled(true);
	fileMenu.getItem(OPEN_RECENT_MENU_ITEM).setEnabled(true);          
	fileMenu.getItem(RELOAD_MENU_ITEM).setEnabled(true);
	fileMenu.getItem(EXIT_MENU_ITEM).setEnabled(true);
	fileMenu.setEnabled(true);
 
	updateBlockingMenuItems();
   
        execAction.putValue(NAME, EXECUTE_PLAN);
        runMenu.getItem(EXECUTE_MENU_ITEM).setEnabled(true);
	runMenu.setEnabled(true);

	if (viewMenu.getMenuComponentCount() > 0) {
	    viewMenu.getItem(EXPAND_MENU_ITEM).setEnabled(true);
	    viewMenu.getItem(COLLAPSE_MENU_ITEM).setEnabled(true);
	    viewMenu.getItem(TOGGLE_TEXT_TYPES_MENU_ITEM).setEnabled(true);
	    viewMenu.getItem(TOGGLE_LISP_NODES_MENU_ITEM).setEnabled(true); 
	    viewMenu.setEnabled(true);
	}
	else
	    viewMenu.setEnabled(false);

	windowMenu.getItem(SHOW_LUV_DEBUG_MENU_ITEM).setEnabled(true);
	windowMenu.setEnabled(true);
    }

    //* Called when we receive EOF on the LuvListener stream. 
    public void finishedExecutionState()
    {
	System.out.println("finishedExecutionState()");

	// set only certain luv viewer variables
          
	planPaused = false;
	planStep = false;
	atStartScreen = false;
	stopSearchForMissingLibs = false; 
	isExecuting = false;
	stopExecution = false;        
	executedViaLuvViewer = false;
	cancelPlanLoading = false;
  
	// set certain menu items         	

	fileMenu.getItem(OPEN_PLAN_MENU_ITEM).setEnabled(true);
	fileMenu.getItem(OPEN_SCRIPT_MENU_ITEM).setEnabled(true);
	fileMenu.getItem(OPEN_RECENT_MENU_ITEM).setEnabled(true);  
        if (executedViaCommandPrompt)
            fileMenu.getItem(RELOAD_MENU_ITEM).setEnabled(false);
        else
            fileMenu.getItem(RELOAD_MENU_ITEM).setEnabled(true);
	fileMenu.getItem(EXIT_MENU_ITEM).setEnabled(true);
	fileMenu.setEnabled(true);
 
	updateBlockingMenuItems();
        
        execAction.putValue(NAME, EXECUTE_PLAN);
	runMenu.getItem(EXECUTE_MENU_ITEM).setEnabled(true);
	runMenu.setEnabled(true);

	if (viewMenu.getMenuComponentCount() > 0)
	    {
		viewMenu.getItem(EXPAND_MENU_ITEM).setEnabled(true);
		viewMenu.getItem(COLLAPSE_MENU_ITEM).setEnabled(true);
		viewMenu.getItem(TOGGLE_TEXT_TYPES_MENU_ITEM).setEnabled(true);
		viewMenu.getItem(TOGGLE_LISP_NODES_MENU_ITEM).setEnabled(true); 
		viewMenu.setEnabled(true);
	    }
	else
	    viewMenu.setEnabled(false);

	windowMenu.getItem(SHOW_LUV_DEBUG_MENU_ITEM).setEnabled(true);
	windowMenu.setEnabled(true);

	showStatus("Execution complete.", Color.BLACK);
    }
      
    private void preExecutionState()
    {
	System.out.println("preExecutionState()");
        
	fileMenu.getItem(OPEN_PLAN_MENU_ITEM).setEnabled(false);
	fileMenu.getItem(OPEN_SCRIPT_MENU_ITEM).setEnabled(false);
	fileMenu.getItem(OPEN_RECENT_MENU_ITEM).setEnabled(false);
	fileMenu.getItem(RELOAD_MENU_ITEM).setEnabled(false);
	runMenu.getItem(BREAK_MENU_ITEM).setEnabled(false);
	runMenu.getItem(EXECUTE_MENU_ITEM).setEnabled(false);
          
	showStatus("Preparing to execute...", Color.lightGray);
    }
      
    private void executionState()
    {
	System.out.println("executionState()");
        
	isExecuting = true;
	stopExecution = false;
        
        updateBlockingMenuItems();        
          
	showStatus("Executing...", Color.GREEN.darker());	
    }

    private void luvViewerExecutionState()
    {
	System.out.println("luvViewerExecutionState()");
        
	executionState();
        
        execAction.putValue(NAME, STOP_EXECUTION);
	
	executedViaLuvViewer = true;
        executedViaCommandPrompt = false;
	openedPlanViaLuvViewer = true;
    }

    private void cmdPromptExecutionState()
    {
	System.out.println("cmdPromptExecutionState()");
        
	executionState();
        
	executedViaCommandPrompt = true; 
        executedViaLuvViewer = false;
	openedPlanViaLuvViewer = false;   
        
        runMenu.getItem(EXECUTE_MENU_ITEM).setEnabled(false);
    }
    
    private void openPlanViaLuvViewerState()
    {
        System.out.println("openPlanViaLuvViewerState()");
        
        openedPlanViaLuvViewer = true;
        executedViaCommandPrompt = false; 
        
        reloadPlanState();
        
        luvBreakPointHandler.clearBreakPoint();
        luvBreakPointHandler.clearBreakPointMap();
        luvBreakPointHandler.clearUnfoundBreakPoints();                  
    }
    
    private void reloadPlanState()
    {
        System.out.println("reloadPlanState()");
        
        clearLibraryNames();
	readyState();
        
        conditionHandler = new ConditionHandler((Model) Model.getRoot().clone());
        
        if(TreeTableView.getCurrent().isNodeInfoWindowOpen())
            refreshPopUpNodeWindow();
    }
    
    public void pausedState()
    {
	System.out.println("pausedState()");

	allowBreaks = true;
	planPaused = true;
	planStep = false;
          
	// *** modify these? see updateBlockingMenuItems()
	runMenu.getItem(PAUSE_RESUME_MENU_ITEM).setEnabled(true);
	runMenu.getItem(STEP_MENU_ITEM).setEnabled(true);
    }
      
    private void stepState()
    {
	System.out.println("stepState()");
        
	allowBreaks = true;
	isExecuting = true;
	planPaused = false;
	planStep = true;
          
	// *** modify these? see updateBlockingMenuItems()
	runMenu.getItem(PAUSE_RESUME_MENU_ITEM).setEnabled(true);
	runMenu.getItem(STEP_MENU_ITEM).setEnabled(true);
    }

    //
    // Sub-states
    //
      
    private void disabledBreakingState()
    {
	System.out.println("disabledBreakingState()");
        
	allowBreaks = false;
        
	luvBreakPointHandler.removeAllBreakpointsAction.actionPerformed(null);
        
	allowBreaksAction.putValue(NAME, ENABLE_BREAKS);
        
	updateBlockingMenuItems();
    }
      
    private void enabledBreakingState()
    {
	System.out.println("enabledBreakingState()");
        
	allowBreaks = true;
        
	allowBreaksAction.putValue(NAME, DISABLE_BREAKS);
        
	updateBlockingMenuItems();
    }

    //* Modify the state of certain menu items based on whether the exec is running and whether it blocks.

    private void updateBlockingMenuItems()
    {
	// Pause/resume not useful if exec isn't listening
	if (isExecuting) {
	    runMenu.getItem(BREAK_MENU_ITEM).setEnabled(false);
	    if (execBlocks) {
		runMenu.getItem(PAUSE_RESUME_MENU_ITEM).setEnabled(allowBreaks);
		runMenu.getItem(STEP_MENU_ITEM).setEnabled(allowBreaks);
	    }
	    else {
		runMenu.getItem(PAUSE_RESUME_MENU_ITEM).setEnabled(false);
		runMenu.getItem(STEP_MENU_ITEM).setEnabled(false);
	    }
	}
	else {
	    runMenu.getItem(PAUSE_RESUME_MENU_ITEM).setEnabled(false);
	    runMenu.getItem(STEP_MENU_ITEM).setEnabled(false);
	    runMenu.getItem(BREAK_MENU_ITEM).setEnabled(true);
	}
    }

      
    public void refreshPopUpNodeWindow()
    {
	Model node = Model.getRoot();

	for (int i = TreeTableView.getCurrent().getPathToInfoWindowNode().size() - 2; i >= 0; i--) {
	    String name = TreeTableView.getCurrent().getPathToInfoWindowNode().get(i);
	    if (node != null)
		node = node.findChildByName(name);
	    else 
		break;
	}

	if (node != null)
	    TreeTableView.getCurrent().resetNodeInfoWindow(node, node.getModelName());
	else
	    TreeTableView.getCurrent().closeNodeInfoWindow();
    }

    // place all visible elements into the container in the main frame of the application.
  
    public void constructFrame(Container frame)
    {
	// set layout and background color

	setLayout(new BorderLayout());
	setBackground(properties.getColor(PROP_WIN_BCLR));
         
	// add view panel with start logo
         
	JLabel startLogo = new JLabel(getIcon(START_LOGO));
	viewHandler.getViewPanel().add(startLogo);
	frame.add(viewHandler.getViewPanel(), CENTER);
         
	// create a menu bar

	JMenuBar menuBar = new JMenuBar();
	setJMenuBar(menuBar);        
	createMenuBar(menuBar);
         
	// create the status bar

	final JLabel statusBar = new JLabel(" ");
	statusBar.setBorder(new EmptyBorder(2, 2, 2, 2));
	frame.add(statusBar, SOUTH);
	statusMessageHandler.startStatusBarThread(statusBar);       
         
	// set size and location of frame

	setLocation(properties.getPoint(PROP_WIN_LOC));
	setPreferredSize(properties.getDimension(PROP_WIN_SIZE));

	// create the debug window
         
	luvViewerDebugWindow = new DebugWindow(this);
	luvViewerDebugWindow.setTitle("Luv Viewer Debug Window");

	// set size and location off frame

	luvViewerDebugWindow.setLocation(properties.getPoint(PROP_DBWIN_LOC));
	luvViewerDebugWindow.setPreferredSize(properties.getDimension(PROP_DBWIN_SIZE));
	luvViewerDebugWindow.pack();
         
         // reset "Show/Hide debug window" menu item when closing window
         
	luvViewerDebugWindow.addWindowListener(new WindowAdapter() {
		public void windowClosing(WindowEvent winEvt) {
		    // Perhaps ask user if they want to save any unsaved files first.
		    luvDebugWindowAction.actionPerformed(null);   
		}
	    });
         
	// make the frame visible
         
	pack();
	setVisible(true);
    }
      
    private void createMenuBar(JMenuBar menuBar)
    {       
	// create file menu
         
	menuBar.add(fileMenu);
	fileMenu.add(openAction);
	fileMenu.add(openScriptAction);
	updateRecentMenu();
	fileMenu.add(recentRunMenu);
	fileMenu.add(reloadAction);
	fileMenu.add(new JSeparator());
	fileMenu.add(exitAction);

	// create and update exec menu
         
	menuBar.add(runMenu);
	runMenu.add(pauseAction);
	runMenu.add(stepAction);
	runMenu.add(allowBreaksAction);
	runMenu.add(execAction);

	// add view menu

	menuBar.add(viewMenu);

	// show window menu
 
	menuBar.add(windowMenu);
	windowMenu.add(luvDebugWindowAction);
    }
      
    // this is called from the ViewHandler
    public void setLuvViewMenu()
    {
         // insert the view menu items

         viewMenu.removeAll(); 
         for(LuvAction action: viewHandler.getCurrentView().getViewActions())
            viewMenu.add(action);
         viewMenu.add(showHidePrlNodes);


         // enable that menu if we actually have menu items
         
         viewMenu.setEnabled(viewMenu.getMenuComponentCount() > 0);
         setLocation(getLocation());

         // size everything

         setPreferredSize(getSize());

         // set the frame title
         
         setTitle();

         // show the new view

         pack();
         repaint();
    }
      
    public void disableAllMenus()
    {
	// disable all file menu elements
        
	fileMenu.getItem(OPEN_PLAN_MENU_ITEM).setEnabled(false);
	fileMenu.getItem(OPEN_SCRIPT_MENU_ITEM).setEnabled(false);
	fileMenu.getItem(OPEN_RECENT_MENU_ITEM).setEnabled(false);
	fileMenu.getItem(RELOAD_MENU_ITEM).setEnabled(false);
	fileMenu.getItem(EXIT_MENU_ITEM).setEnabled(false);
	fileMenu.setEnabled(false);
          
	// disable all run menu elements
        
	runMenu.getItem(PAUSE_RESUME_MENU_ITEM).setEnabled(false);
	runMenu.getItem(STEP_MENU_ITEM).setEnabled(false);
	runMenu.getItem(BREAK_MENU_ITEM).setEnabled(false);
	runMenu.getItem(EXECUTE_MENU_ITEM).setEnabled(false);         
	runMenu.setEnabled(false);
 
	// disable all view menu elements if there are any

	if (viewMenu.getMenuComponentCount() > 0) {
	    viewMenu.getItem(EXPAND_MENU_ITEM).setEnabled(false);
	    viewMenu.getItem(COLLAPSE_MENU_ITEM).setEnabled(false);
	    viewMenu.getItem(TOGGLE_TEXT_TYPES_MENU_ITEM).setEnabled(false);
	    viewMenu.getItem(TOGGLE_LISP_NODES_MENU_ITEM).setEnabled(false);              
	}
	viewMenu.setEnabled(false);
          
	// disable window menu
          
	windowMenu.setEnabled(false);
    }
      
    /** Given a recent plan index, the description used for the recent menu item tooltip. 
     *
     * @param recentIndex the index of the recent plan
     *
     * @return the description of what gets loaded
     */

    public String getRecentMenuDescription(int recentIndex)
    {
	File recentPlan = new File(Luv.getLuv().getFileHandler().getRecentPlanName(recentIndex));
	StringBuffer description = new StringBuffer("Load " + recentPlan.getName());
         
	description.append(".");
	return description.toString();
    }
      
    // Add a file to the recently opened file list. 

    private void addRunToRecentRunList() throws IOException
    {
	// put newest file at the top of the list
          
	File plan = fileHandler.getPlan();
	File script = fileHandler.getScript();

        // only store the run if both files are valid - could be invalid if 
        // executed from terminal instead of the viewer itself
        if (plan != null && script != null)
        {
            String currentPlan = plan.getAbsolutePath();
            String currentScript = script.getAbsolutePath();

            String planName = currentPlan;
            String scriptName = currentScript;

            int count = properties.getInteger(PROP_FILE_RECENT_COUNT);

            for (int i = 1; i <= count && planName != null; ++i) {
                if (planName != null) {

                    planName = (String)properties.setProperty(PROP_FILE_RECENT_PLAN_BASE + i, planName);
                    scriptName = (String)properties.setProperty(PROP_FILE_RECENT_SCRIPT_BASE + i, scriptName);
                    // if this run already existed in the list, we can stop
                    // it already appears at the top

                    if (planName != null && planName.equals(currentPlan))
                        break;
                }
            }

            // update the recent menu

            updateRecentMenu();
        }
    }
      
    /** Update the recently loaded files menu. */

    private void updateRecentMenu()
    {
	recentRunMenu.removeAll();
	int count = properties.getInteger(PROP_FILE_RECENT_COUNT);
	if (fileHandler.getRecentPlanName(1) == null) {
	    recentRunMenu.add("No recent runs");
	}
	else {
	    for (int i = 0; i < count; ++i)
		if (fileHandler.getRecentPlanName(i + 1) != null)
		    recentRunMenu.add(new LoadRecentAction(i + 1, '1' + i, META_MASK));
	}

	// this menu is only enabled when there are items in it
         
	recentRunMenu.setEnabled(recentRunMenu.getMenuComponentCount() > 0);
    }
      
    // set title of the luv viewer

    private void setTitle()
    {          
	setTitle("Luv Viewer");
    }

    // set a program wide property

    public void setProperty(String key, String value)
    {
	properties.setProperty(key, value);
    }

    // get a program wide property

    public String getProperty(String key)
    {
	return properties.getProperty(key);
    }
      
    public void showStatus(String message)
    {
	statusMessageHandler.showStatus(message);           
    }
      
    public void showStatus(String message, Color color)
    {
	statusMessageHandler.showStatus(message, color);
    }
      
    public void showStatus(String message, long autoClearTime)
    {
	statusMessageHandler.showStatus(message, autoClearTime);
    }
      
    public void showStatus(String message, Color color, long autoClearTime)
    {
	statusMessageHandler.showStatus(message, color, autoClearTime);
    }
      
    public void clearStatus()
    {
	statusMessageHandler.clearStatus();
    }

    // exit this program.

    public void exit()
    { 
	System.exit(0); 
    }
      
    private String getCommandLine() throws IOException
    {
        // is command valid?
        boolean valid = true;
        
        // UE command       
	String command = PROP_UE_EXEC;
        
        // instruct UE to use Luv
        command += " -v";

        // allow breaks?
        command += allowBreaks ? command += " -b" : "";	                   
          
	// does plan still exist?          
	if (fileHandler.getPlan().exists())
	{
            command += " " + fileHandler.getPlan(); 
        }
        else
        {
            valid = false;
            command = "Error: " + fileHandler.getPlan() + " does not exist.";
        }
        
        // if plan exists, get script (user may not have loaded it yet)            
        if (valid && fileHandler.getScript() != null)
        {
            if (fileHandler.getScript().exists())
            {
                command += " " +  fileHandler.getScript();
            }
            else
            {
                valid = false;
                command = "Error: " + fileHandler.getScript() + " does not exist.";
            }
        }
        else
        {
            valid = false;
            command = "Error: script does not exist.";
        }
        
        // if plan and script exist, are there libraries to add?
        if (valid && libraryNames.size() > 0)
        {
            for (String libName : libraryNames.values())
            {
                // double check that library still exists
                if (new File(libName).exists())
                {
                    command += " -l ";
                    command += libName.toString();
                }
                else
                {
                    command = "Error: " + libName + " does not exist.";
                    break;
                }
            }
        }

	return command;
    }
      
    public void addLibraryName(String name, String path)
    {
	libraryNames.put(name, path);
    }
      
    public HashMap<String, String> getLibraryNames()
    {
	return libraryNames;
    }
      
    public void clearLibraryNames()
    {
	libraryNames.clear();
    }
    
    public void stopExecution() throws IOException
    {
        System.out.println("stopExecution()");
        
	executionViaLuvViewerHandler.killUEProcess();
	stopExecution = true;
	executedViaLuvViewer = true;
    }
      
      
    /***************** List of Actions ********************/

    /** Action to open and view a plan. */

    LuvAction openAction = 
	new LuvAction("Open Plan",
		      "Open a plan for viewing.",
		      VK_O, 
		      META_MASK)
	{
	    public void actionPerformed(ActionEvent e)
	    {
		// Loading done in the file handler at present
                
		int option = fileHandler.choosePlan();

		if (option == APPROVE_OPTION) {
		    // is the user trying to reload while a plan is already executing?
		    if (isExecuting) {
			try {
			    stopExecution();
			    JOptionPane.showMessageDialog(theLuv, 
                                                          "Stopping execution and opening a new plan", 
                                                          "Stopping Execution", 
                                                          JOptionPane.INFORMATION_MESSAGE);
			}
			catch (IOException ex) {
			    JOptionPane.showMessageDialog(theLuv, 
                                                          "Error stopping execution. Please see Debug Window.", 
                                                          "Error", 
                                                          JOptionPane.ERROR_MESSAGE);
			    System.err.println("Error: " + ex.getMessage());
			}
		    }
                    
                    openPlanViaLuvViewerState();
		}               
	    }
	};
      
    /** Action to load a script for Execution. */
         
    LuvAction openScriptAction = 
	new LuvAction("Open Script", 
		      "Open a script for execution.", 
		      VK_S, 
		      META_MASK)
	{
	    public void actionPerformed(ActionEvent e)
	    {
		int option = fileHandler.chooseScript();
		if (option == APPROVE_OPTION) {
                    // is the user trying to open a script while a plan is already executing?
		    if (isExecuting) {
			try {
			    stopExecution();
			    JOptionPane.showMessageDialog(theLuv,
							  "Stopping execution and opening script",
							  "Stopping Execution",
							  JOptionPane.INFORMATION_MESSAGE);
			}
			catch (IOException ex) {                       
			    JOptionPane.showMessageDialog(theLuv,
							  "Error stopping execution. Please see Debug Window.",
							  "Error",
							  JOptionPane.ERROR_MESSAGE);
			    System.err.println("Error: " + ex.getMessage());
			}
		    }
		}
	    }
	};

    /** Action to reload a plan. */

    LuvAction reloadAction = 
	new LuvAction("Reload Plan",
		      "Reload current plan file.",
		      VK_R, 
		      META_MASK)
	{
	    public void actionPerformed(ActionEvent e)
	    {
                // is the user trying to reload while a plan is already executing?
		if (isExecuting) {
		    try {
			stopExecution();
			JOptionPane.showMessageDialog(theLuv,
						      "Stopping execution and reloading plan",
						      "Stopping Execution",
						      JOptionPane.INFORMATION_MESSAGE);
		    }
		    catch (IOException ex) {
			JOptionPane.showMessageDialog(theLuv,
						      "Error reloading plan. Please see Debug Window.",
						      "Error",
						      JOptionPane.ERROR_MESSAGE);
			System.err.println("Error: " + ex.getMessage());
		    }
		}

                // make sure the user is reloading a plan that was opened through the viewer itself and not the terminal
		if (openedPlanViaLuvViewer) {
                    
                    reloadPlanState();

		    try {
			if (fileHandler.getPlan() == null)
			    fileHandler.loadRecentPlan(1);
			else
			    fileHandler.loadPlan(fileHandler.getPlan());
		    } 
		    catch (IOException ex) {
			JOptionPane.showMessageDialog(theLuv,
						      "Error reloading plan. Please see Debug Window.",
						      "Error",
						      JOptionPane.ERROR_MESSAGE);
			System.err.println("Error: " + ex.getMessage());
		    }
		}
		else {
		    JOptionPane.showMessageDialog(theLuv,
						  "Error: Cannot reload a plan that was originally loaded by command prompt.",
						  "Reload Error",
						  JOptionPane.ERROR_MESSAGE);
		    statusMessageHandler.showStatus("Error: Cannot reload a plan that was originally loaded by command prompt.", 1000);
		}
	    }
	};

    /** Action to show the debugging window. */

    LuvAction luvDebugWindowAction = 
	new LuvAction("Show Luv Viewer Debug Window",
		      "Show window with luv viewer debug text.",
		      VK_L, 
		      META_MASK)
	{
	    public void actionPerformed(ActionEvent e)
	    {
		luvViewerDebugWindow.setVisible(!luvViewerDebugWindow.isVisible());
                   
		if (luvViewerDebugWindow.isVisible())
		    luvDebugWindowAction.putValue(NAME, "Hide Luv Viewer Debug Window");
		else
		    luvDebugWindowAction.putValue(NAME, "Show Luv Viewer Debug Window");
	    }
	};
         
    /** Action to allow breakpoints. */
         
    LuvAction allowBreaksAction =
	new LuvAction(ENABLE_BREAKS,
		      "Select this to allow breakpoints.",
		      VK_F2)
	{
	    public void actionPerformed(ActionEvent e)
	    {
		if (!isExecuting) {
		    allowBreaks = !allowBreaks;

		    if (allowBreaks) {
			enabledBreakingState();
		    }
		    else {
			disabledBreakingState();
		    }
		}
	    }
	};
         
    /** Action to execute a plexil plan. */

    LuvAction execAction = 
	new LuvAction(EXECUTE_PLAN, 
		      "Execute plan currently loaded.",
		      VK_F1)
	{
	    public void actionPerformed(ActionEvent e)
	    {
                try {

                    if (!isExecuting) {
                        if (!executedViaCommandPrompt) {
                            executedViaLuvViewer = true;
                            preExecutionState();
                            String command = getCommandLine();
                            if (!command.contains("Error")) {
                                executionViaLuvViewerHandler.runExec(command);
                            }
                            else {
                                JOptionPane.showMessageDialog(theLuv, 
                                                              command, 
                                                              "Error", 
                                                              JOptionPane.ERROR_MESSAGE);
                                showStatus("Stopped execution", Color.lightGray, 1000);
                                readyState();
                            }
                        }
                        else {
                            JOptionPane.showMessageDialog(theLuv,
						          "Error: Cannot execute a plan that was originally executed by command prompt.",
						          "Execution Error",
						          JOptionPane.ERROR_MESSAGE);
                            statusMessageHandler.showStatus("Error: Cannot execute a plan that was originally executed by command prompt.", 1000);
                        }
                    }
                    else {                       
                        stopExecution();
                    }
                    
                } 
		catch (IOException ex) {
                    JOptionPane.showMessageDialog(theLuv,
						  "Error executing plan. Please see Debug Window.",
						  "Error", 
						  JOptionPane.ERROR_MESSAGE);
                    System.err.println("Error: " + ex.getMessage());
                }
	    }
	};

    LuvAction pauseAction = 
	new LuvAction(PAUSE_OR_RESUME_PLAN, 
		      "Pause or resume an executing plan, if it is blocking.",
		      VK_ENTER)
	{
	    public void actionPerformed(ActionEvent e)
	    {  
		if (isExecuting) {
		    planPaused = !planPaused;

		    statusMessageHandler.showStatus((planPaused ? "Pause" : "Resume") + " requested.", 
						    Color.BLACK, 
						    1000);

		    if (planPaused)
			pausedState();
		    else
			executionState();
                         
		}
	    }
	};

    /** Action to step a paused plexil plan. */

    LuvAction stepAction = 
	new LuvAction(STEP, 
		      "Step a plan, pausing it if is not paused.",
		      VK_SPACE)
	{
	    public void actionPerformed(ActionEvent e)
	    {
		if (isExecuting) {
		    if (!planPaused) {
			pausedState();
			statusMessageHandler.showStatus("Pause requested.", Color.BLACK, 1000);
		    }
		    else {
			stepState();
			statusMessageHandler.showStatus("Step plan.", Color.BLACK, 1000);
		    }
		}                     
	    }
	};
 
    /** Action show node types in different ways. */
      
    LuvAction showHidePrlNodes = 
	new LuvAction("Toggle Plexil Lisp Nodes", 
		      "Show or hide nodes that start with \"plexillisp_\".",
		      VK_P, 
		      META_MASK)
	{
	    RegexModelFilter filter = 
		new RegexModelFilter(properties.getBoolean(PROP_VIEW_HIDE_PLEXILLISP),
				     "^plexilisp_.*");

	    {
		filter.addListener(
				   new AbstractModelFilter.Listener()
				   {
				       @Override public void filterChanged(AbstractModelFilter filter) 
				       {
					   viewHandler.resetView();
				       }
				   });
	    }

	    public void actionPerformed(ActionEvent e)
	    {
		filter.setEnabled(!filter.isEnabled());
		properties.set(PROP_VIEW_HIDE_PLEXILLISP, filter.isEnabled());
	    }
	};

    /** Action to exit the program. */

    LuvAction exitAction = 
	new LuvAction("Exit", "Terminate this program.", VK_ESCAPE)
	{
	    public void actionPerformed(ActionEvent e)
	    {
		Object[] options = 
		    {
                        "Yes",
                        "No",
		    };
                   
		int exitLuv = 
		    JOptionPane.showOptionDialog(theLuv,
						 "Are you sure you want to exit?",
						 "Exit Luv Viewer",
						 JOptionPane.YES_NO_CANCEL_OPTION,
						 JOptionPane.WARNING_MESSAGE,
						 null,
						 options,
						 options[0]);
                   
		if (exitLuv == 0) {                 
		    exit();
		}
	    }
	};
}

