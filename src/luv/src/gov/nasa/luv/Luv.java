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

import java.io.IOException;
import javax.swing.JFrame;
import javax.swing.JMenuBar;
import javax.swing.JLabel;
import javax.swing.border.EmptyBorder;
import javax.swing.JMenu;
import javax.swing.JSeparator;
import java.awt.Container;
import java.awt.Color;
import java.awt.BorderLayout;
import java.util.ArrayList;
import static gov.nasa.luv.Constants.*;
import static java.lang.System.*;
import static java.awt.BorderLayout.*;
import static javax.swing.JFileChooser.*;

/**
 * The Luv class is the starting point of the Luv application and creates a 
 * containing frame for the Lightweight Universal Executive Viewer.
 */
public class Luv extends JFrame {

    // boolean variables to help determine Luv state
    private static boolean allowBreaks;
    private static boolean planPaused;
    private static boolean planStep;
    private static boolean isExecuting;
    private static boolean extendedViewOn;
    private boolean newPlan;
    private boolean shouldHighlight;

    // class instances to manage Luv features
    private static FileHandler fileHandler;
    private static StatusMessageHandler statusMessageHandler;
    private static LuvBreakPointHandler luvBreakPointHandler;
    private static ExecutionHandler executionHandler;
    private static ViewHandler viewHandler;
    private static LuvStateHandler luvStateHandler;
    private static HideOrShowWindow hideOrShowWindow;
    private DebugWindow debugWindow;
    private CreateCFGFileWindow createCFGFileWindow;
    private SourceWindow sourceWindow;
    private RegexModelFilter regexFilter;

    // Luv menus
    private JMenu fileMenu;
    private JMenu recentRunMenu;
    private JMenu runMenu;
    private JMenu viewMenu;
    private JMenu debugMenu;

    // the current model/plan
    private Model currentPlan;

    // current working instance of luv
    private static Luv theLuv;   
    
    // persistent properties for luv viewer
    private Properties properties;

    /** Entry point for the Luv application. */
    public static void main(String[] args) {
        runApp();
    }

    /** Creates a new instance of the Luv application. */
    private static void runApp() {
        // if we're on a mac, use mac style menus
        System.setProperty("apple.laf.useScreenMenuBar", "true");

        try {
            new Luv();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /** 
     * Constructs Luv and initializes the many features of the Luv application
     * such as creating the main viewing window, putting Luv in a 'start state'
     * and initializing a SocketServer to start listening for events from the 
     * Universal Executive.
     */
    public Luv() throws IOException {
        init();

        viewHandler.showModelInViewer(currentPlan);

        constructFrame(getContentPane());

        luvStateHandler.startState();

        new LuvSocketServer(properties.getInteger(PROP_NET_SERVER_PORT));

    }

    private void init() {
        theLuv = this;

        currentPlan = new Model("dummy");

        properties = new Properties(PROPERTIES_FILE_LOCATION) {

            {
                define(PROP_FILE_RECENT_COUNT, PROP_FILE_RECENT_COUNT_DEF);
                define(PROP_WIN_LOC, PROP_WIN_LOC_DEF);
                define(PROP_WIN_SIZE, PROP_WIN_SIZE_DEF);
                define(PROP_WIN_BCLR, PROP_WIN_BCLR_DEF);
                define(PROP_DBWIN_LOC, PROP_DBWIN_LOC_DEF);
                define(PROP_DBWIN_SIZE, PROP_DBWIN_SIZE_DEF);

                define(PROP_NODEINFOWIN_LOC, PROP_NODEINFOWIN_LOC_DEF);
                define(PROP_NODEINFOWIN_SIZE, PROP_NODEINFOWIN_SIZE_DEF);
                define(PROP_FINDWIN_LOC, PROP_FINDWIN_LOC_DEF);
                define(PROP_FINDWIN_SIZE, PROP_FINDWIN_SIZE_DEF);
                define(PROP_HIDESHOWWIN_LOC, PROP_HIDESHOWWIN_LOC_DEF);
                define(PROP_HIDESHOWWIN_SIZE, PROP_HIDESHOWWIN_SIZE_DEF);
                define(PROP_CFGWIN_LOC, PROP_CFGWIN_LOC_DEF);
                define(PROP_CFGWIN_SIZE, PROP_CFGWIN_SIZE_DEF);

                define(PROP_NET_SERVER_PORT, PROP_NET_SERVER_PORT_DEF);
                define(PROP_FILE_RECENT_PLAN_DIR,
                        getProperty(PROP_FILE_RECENT_PLAN_BASE + 1, UNKNOWN));
                define(PROP_FILE_RECENT_SCRIPT_DIR,
                        getProperty(PROP_FILE_RECENT_SCRIPT_BASE + 1, UNKNOWN));

                define(PROP_HIDE_SHOW_LIST,
                        getProperty(PROP_HIDE_SHOW_LIST, UNKNOWN).equals(UNKNOWN) ? "" : getProperty(PROP_HIDE_SHOW_LIST));
                define(PROP_SEARCH_LIST,
                        getProperty(PROP_SEARCH_LIST, UNKNOWN).equals(UNKNOWN) ? "" : getProperty(PROP_SEARCH_LIST));
            }
        };

        allowBreaks = false;
        planPaused = false;
        planStep = false;
        isExecuting = false;
        extendedViewOn = true;
        newPlan = false;
        shouldHighlight = true;
        
        fileHandler = new FileHandler();
        statusMessageHandler = new StatusMessageHandler();
        luvBreakPointHandler = new LuvBreakPointHandler();
        executionHandler = new ExecutionHandler();
        luvStateHandler = new LuvStateHandler();
        viewHandler = new ViewHandler();
        hideOrShowWindow = new HideOrShowWindow();
        debugWindow = new DebugWindow();

        createCFGFileWindow = new CreateCFGFileWindow();
        sourceWindow = new SourceWindow();
        regexFilter = new RegexModelFilter(true);
        fileMenu = new JMenu("File");
        recentRunMenu = new JMenu("Recent Runs");
        runMenu = new JMenu("Run");
        viewMenu = new JMenu("View");
        debugMenu = new JMenu("Debug");
    }

    private void constructFrame(Container frame) {
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
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

        // save preferred window sizes when Luv application closes
        Runtime.getRuntime().addShutdownHook(new Thread() {

            public void run() {
                properties.set(PROP_WIN_LOC, getLocation());
                properties.set(PROP_WIN_SIZE, getSize());
                properties.set(PROP_DBWIN_LOC, debugWindow.getLocation());
                properties.set(PROP_DBWIN_SIZE, debugWindow.getSize());
            }
        });

        setLocation(properties.getPoint(PROP_WIN_LOC));
        setPreferredSize(properties.getDimension(PROP_WIN_SIZE));

        regexFilter.extendedPlexilView();
        regexFilter.updateRegexList();

        setTitle();
        pack();
        setVisible(true);
    }

    /** Handles the Plexil plan being loaded into the Luv application. 
     *  Called from PlexilPlanHandler.endElement(), which will be invoked 
     *  by both Luv (directly) and the Universal Executive (via the Luv listener stream).
     *
     * @param plan the Plexil plan to be loaded into the Luv application
     */
    public void handleNewPlan(Model plan) {
        // *** N.B. This depends on that new plans are always added at the end
        Model other = Model.getRoot().findChildByName(plan.getModelName());

        if (plan == other) {
            // brand new 'plan' loaded
            NodeInfoWindow.closeNodeInfoWindow();
        } else if (plan.equivalent(other) && !newPlan) {
            // same 'plan' loaded, so use 'other' previous plan
            Model.getRoot().removeChild(plan);
            plan = other;
        } else {
            // new 'plan' has same root name as 'other' previous plan,
            // but new 'plan' supersedes
            Model.getRoot().removeChild(other);
        }

        if (!plan.equivalent(currentPlan) || newPlan) {
            luvBreakPointHandler.removeAllBreakPoints();
            currentPlan = plan;
            Model.getRoot().planChanged();
            viewHandler.showModelInViewer(currentPlan);
        }

        luvStateHandler.preExecutionState();

        if (isExecuting) {
            luvStateHandler.executionState();
            LoadRecentAction.addRunToRecentRunList();
        }

        setTitle();

        // Determine if the Luv Viewer should pause before executing.
        if (isExecuting && allowBreaks) {
            luvStateHandler.pausedState();
            runMenu.setEnabled(true);
        }
    }

    /** 
     * Pauses the execution of the Plexil plan by the Universal Executive
     * when directed to by the user. 
     */
    public void blockViewer() {
        if (shouldBlock()) {
            statusMessageHandler.showStatus("Plan execution is paused. " +
                    LuvActionHandler.pauseAction.getAcceleratorDescription() +
                    " to resume, or " +
                    LuvActionHandler.stepAction.getAcceleratorDescription() +
                    " to step",
                    Color.RED);

            if (luvBreakPointHandler.getBreakPoint() != null && shouldHighlight) {
                TreeTableView.getCurrent().highlightRow(luvBreakPointHandler.getBreakPoint().getModel());
            }

            luvBreakPointHandler.clearBreakPoint();

            // wait here for user action
            while (shouldBlock()) {
                try {
                    Thread.sleep(50);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            TreeTableView.getCurrent().unHighlightRow();
            shouldHighlight = true;
        }
    }

    /** Returns the current instance of the Luv application. 
     *  @return the current instance of the Luv application */
    public static Luv getLuv() {
        return theLuv;
    }

    /** Returns the current instance of the Luv ViewHandler.
     *  @return the current instance of the Luv ViewHandler */
    public ViewHandler getViewHandler() {
        return viewHandler;
    }

    /** Returns the current instance of the Luv FileHandler.
     *  @return the current instance of the Luv FileHandler */
    public FileHandler getFileHandler() {
        return fileHandler;
    }

    /** Returns the current instance of the Luv ExecutionHandler.
     *  @return the current instance of the Luv ExecutionHandler */
    public ExecutionHandler getExecutionHandler() {
        return executionHandler;
    }

    /** Returns the current instance of the Luv LuvBreakPointHandler.
     *  @return the current instance of the Luv LuvBreakPointHandler */
    public LuvBreakPointHandler getLuvBreakPointHandler() {
        return luvBreakPointHandler;
    }

    /** Returns the current instance of the Luv StatusMessageHandler.
     *  @return the current instance of the Luv StatusMessageHandler */
    public StatusMessageHandler getStatusMessageHandler() {
        return statusMessageHandler;
    }

    /** Returns the current instance of the Luv LuvStateHandler.
     *  @return the current instance of the Luv LuvStateHandler */
    public LuvStateHandler getLuvStateHandler() {
        return luvStateHandler;
    }

    /** Returns the current instance of the Luv HideOrShowWindow.
     *  @return the current instance of the Luv HideOrShowWindow */
    public HideOrShowWindow getHideOrShowWindow() {
        return hideOrShowWindow;
    }

    /** Returns the current instance of the Luv DebugWindow.
     *  @return the current instance of the Luv DebugWindow */
    public DebugWindow getDebugWindow() {
        return debugWindow;
    }

    /** Returns the current instance of the Luv DebugCFGWindow.
     *  @return the current instance of the Luv DebugCFGWindow */
    public CreateCFGFileWindow getCreateCFGFileWindow() {
        return createCFGFileWindow;
    }

    /** Returns the current instance of the Luv SourceWindow.
     *  @return the current instance of the Luv SourceWindow */
    public SourceWindow getSourceWindow() {
        return sourceWindow;
    }

    /** Returns the current instance of the Luv RegexModelFilter.
     *  @return the current instance of the Luv RegexModelFilter */
    public RegexModelFilter getRegexModelFilter() {
        return regexFilter;
    }

    /** Returns the current instance of the Luv Properties.
     *  @return the current instance of the Luv Properties */
    public Properties getProperties() {
        return properties;
    }

    /** Returns the current instance of the Luv Model.
     *  @return the current instance of the Luv Model */
    public Model getCurrentPlan() {
        return currentPlan;
    }

    /** Returns the current instance of the Luv viewMenu.
     *  @return the current instance of the Luv viewMenu */
    public JMenu getViewMenu() {
        return viewMenu;
    }

    /** Returns the current instance of the Luv fileMenu.
     *  @return the current instance of the Luv fileMenu */
    public JMenu getFileMenu() {
        return fileMenu;
    }

    /** Returns the current instance of the Luv runMenu.
     *  @return the current instance of the Luv runMenu */
    public JMenu getRunMenu() {
        return runMenu;
    }

    /** Returns the current instance of the Luv debugMenu.
     *  @return the current instance of the Luv debugMenu */
    public JMenu getDebugMenu() {
        return debugMenu;
    }

    /** Returns the current instance of the Luv recentRunMenu.
     *  @return the current instance of the Luv recentRunMenu */
    public JMenu getRecentRunMenu() {
        return recentRunMenu;
    }

    /** Returns whether the current Plexil plan is stepping.
     *  @return the current instance of planStep */
    public boolean getPlanStep() {
        return planStep;
    }

    /** Returns whether the current Plexil plan is paused.
     *  @return the current instance of planPaused */
    public boolean getPlanPaused() {
        return planPaused;
    }

    /** Returns whether the current Plexil plan is executing.
     *  @return the current instance of isExecuting */
    public boolean getIsExecuting() {
        return isExecuting;
    }

    public boolean getIsExtendedViewOn() {
        return extendedViewOn;
    }

    /** Returns whether Luv currently allows breaks.
     *  @return the current instance of allowBreaks */
    public boolean breaksAllowed() {
        return allowBreaks;
    }

    /** Returns whether the currently executing Plexil plan should pause. 
     *  @return the whether the flag for pausing a plan is set and the flag 
     *  for stepping a plan is not set */
    public boolean shouldBlock() {
        return planPaused && !planStep;
    }

    /** Sets the flag that indicates whether or not the current Plexil plan is 
     *  executing. Updates items under the Run menu accordingly.
     *  @param value sets the flag that indicates whether the current Plexil 
     *  plan is executing or not
     */
    public void setIsExecuting(boolean value) {
        isExecuting = value;
        updateBlockingMenuItems();
    }

    /** Sets the flag that indicates whether or not the current Plexil plan is 
     *  paused. 
     *  @param value sets the flag that indicates whether the current Plexil 
     *  plan is paused or not
     */
    public void setIsPaused(boolean value) {
        planPaused = value;
    }

    /** Sets the flag that indicates whether or not the current Plexil plan is 
     *  stepping. 
     *  @param value sets the flag that indicates whether the current Plexil 
     *  plan is stepping or not
     */
    public void setIsStepped(boolean value) {
        planStep = value;
    }

    /** Sets the flag that indicates whether a new Plexil plan has been loaded. 
     *  @param value sets the flag that indicates whether there is a new Plexil 
     *  plan
     */
    public void setNewPlan(boolean value) {
        newPlan = value;
    }

    /** Sets the flag that indicates whether the Luv application is currently
     *  allowing breaks. Updates items under the Run menu accordingly.
     *  @param value sets the flag that indicates whether breaks are allowed
     */
    public void setBreaksAllowed(boolean value) {
        allowBreaks = value;
        updateBlockingMenuItems();
    }

    /** Sets the flag that indicates whether the Luv application should 
     *  highlist rows in pink in the model tree.
     *  @param value sets the flag that indicates whether rows can be highlighted
     */
    public void setShouldHighlight(boolean value) {
        shouldHighlight = value;
    }

    public void setExtendedViewOn(boolean value) {
        extendedViewOn = value;
    }

    /** Modifies the state of certain menu items based on whether the Universal 
     *  Executive is running and whether it blocks.
     * */
    public void updateBlockingMenuItems() {
        // Pause/resume not useful if exec isn't listening            
        if (isExecuting) {
            if (allowBreaks) {
                runMenu.getItem(PAUSE_RESUME_MENU_ITEM).setEnabled(true);
                runMenu.getItem(STEP_MENU_ITEM).setEnabled(true);
            } else {
                runMenu.getItem(PAUSE_RESUME_MENU_ITEM).setEnabled(false);
                runMenu.getItem(STEP_MENU_ITEM).setEnabled(false);
            }
            runMenu.getItem(BREAK_MENU_ITEM).setEnabled(false);
            runMenu.getItem(REMOVE_BREAKS_MENU_ITEM).setEnabled(false);
        } else {
            runMenu.getItem(PAUSE_RESUME_MENU_ITEM).setEnabled(false);
            runMenu.getItem(STEP_MENU_ITEM).setEnabled(false);
            runMenu.getItem(BREAK_MENU_ITEM).setEnabled(true);

            if (luvBreakPointHandler.breakpointsExist()) {
                runMenu.getItem(REMOVE_BREAKS_MENU_ITEM).setEnabled(true);
            } else {
                runMenu.getItem(REMOVE_BREAKS_MENU_ITEM).setEnabled(false);
            }
        }
    }

    /** Enables or Disables the Remove Breaks menu item.
     *  @param value to enable or disable the Remove Breaks menu item
     * */
    public void enableRemoveBreaksMenuItem(boolean value) {
        if (runMenu.getComponents().length > 0) {
            runMenu.getItem(REMOVE_BREAKS_MENU_ITEM).setEnabled(value);
        }
    }

    private void createMenuBar(JMenuBar menuBar) {
        menuBar.add(fileMenu);
        fileMenu.add(LuvActionHandler.openPlanAction);
        fileMenu.add(LuvActionHandler.openScriptAction);
        LoadRecentAction.updateRecentMenu();
        fileMenu.add(recentRunMenu);
        fileMenu.add(LuvActionHandler.reloadAction);
        fileMenu.add(new JSeparator());
        fileMenu.add(LuvActionHandler.exitAction);

        menuBar.add(runMenu);
        runMenu.add(LuvActionHandler.pauseAction);
        runMenu.add(LuvActionHandler.stepAction);
        runMenu.add(LuvActionHandler.allowBreaksAction);
        runMenu.add(LuvActionHandler.removeAllBreaksAction);
        runMenu.add(new JSeparator());
        runMenu.add(LuvActionHandler.execAction);

        menuBar.add(viewMenu);
        viewMenu.add(LuvActionHandler.expandAll);
        viewMenu.add(LuvActionHandler.collapseAll);
        viewMenu.add(LuvActionHandler.hideOrShowNodes);
        viewMenu.add(LuvActionHandler.findNode);
        viewMenu.add(new JSeparator());
        viewMenu.add(LuvActionHandler.extendedViewAction);
       // viewMenu.add(new JSeparator());
       // viewMenu.add(LuvActionHandler.viewSourceAction);
        

        menuBar.add(debugMenu);
        debugMenu.add(LuvActionHandler.luvDebugWindowAction);
        debugMenu.add(LuvActionHandler.createDebugCFGFileAction);
        debugMenu.add(LuvActionHandler.aboutWindowAction);
    }

    /** Sets the title of the Luv application. */
    public void setTitle() {
        if (currentPlan != null && !currentPlan.getPlanName().equals(UNKNOWN)) {
            String title = "Luv Viewer  -  " + currentPlan.getPlanName();

            if (!currentPlan.getScriptName().equals(UNKNOWN)) {
                title += " + " + currentPlan.getScriptName();
            }

            setTitle(title);
        } else if (isExecuting) {
            setTitle("Luv Viewer  -  Remote Execution");
        } else {
            setTitle("Luv Viewer");
        }
    }

    /** Sets a program wide property 
     *  
     *  @param key
     *  @param value
     */
    public void setProperty(String key, String value) {
        properties.setProperty(key, value);
    }

    /** Sets a program wide property 
     * 
     *  @param key
     *  @param value
     */
    public void setProperty(String key, ArrayList<String> value) {
        properties.define(key, value);
    }

    /** Returns a program wide property
     * 
     *  @param key
     *  @return the program wide property
     */
    public String getProperty(String key) {
        return properties.getProperty(key, UNKNOWN);
    }
}