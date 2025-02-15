/**
 * @(#)DebuggerGraphPanel.java
 *
 *
 * @author 
 * @version 1.00 2014/8/18
 */
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class DebuggerGraphPanel extends JPanel implements ActionListener{
	
	private final Color GRAPH_BG = new Color(230, 230, 230);
	Color[] colors = new Color[]{Color.BLACK, new Color(0, 229, 221), Color.RED, new Color(15, 229, 0), Color.MAGENTA, Color.GRAY, 
									new Color(210, 0, 0), Color.BLUE, Color.ORANGE, Color.PINK, new Color(129,215,145), 
										new Color(244,128,90), new Color(140,63,43), new Color(145, 0, 255), new Color(160,172,188), 
										   new Color(169,82,214), new Color(255, 188, 0), new Color(41,118,137), new Color(86, 127, 53)};
		 									
	public final int MARGIN = 20;
	private final String message;
	private final int ID;
	
	private int oldWidth = 0;
	private int n;
	private int[] xvals;

    public DebuggerGraphPanel(String s, int i) {
    	super();
    	setBackground(Color.WHITE);
    	message = s; 
    	ID = i;
    	n = GraphArrayUpdater.getN();
    	DebuggerGUITimer.addActionListener(this);
    	GraphArrayUpdater.marginOfGraphs = MARGIN;
    }
    
    public void paintComponent(Graphics g){
    	
    	int width = getWidth();
    	int height = getHeight();
    	GraphArrayUpdater.currentHeight = height;
    	
    	super.paintComponent(g);
    	
    	g.setColor(GRAPH_BG);
    	g.fillRect(MARGIN, MARGIN + 12, width - 2*MARGIN, height - 2*MARGIN - 12);
    	
    	g.setColor(Color.BLACK);
    	g.drawLine(MARGIN, MARGIN + 12, MARGIN, height - MARGIN);
    	g.drawLine(MARGIN, height - MARGIN, width - MARGIN, height - MARGIN);
    	
    	g.drawString(message, width/2, MARGIN + 9);
    	
    	g.setColor(Color.BLUE);
    	for(int i = 0; i < 19; i++){
    		if(GraphArrayUpdater.graphChoices[i] == ID){
    			g.setColor(colors[i]);
    			g.drawPolyline(xvals, GraphArrayUpdater.yvals[i], n);
    		}
    	}
    	
    }
    
    public void actionPerformed(ActionEvent evt){
    
    	if(getWidth() != oldWidth){
    		oldWidth = getWidth();
    		xvals = new int[n];
    		for(int i = 0; i < n; i++)
    			xvals[i] = MARGIN + i*(oldWidth/(n + 1));
    	}
    	
    	repaint();
    }
    
}