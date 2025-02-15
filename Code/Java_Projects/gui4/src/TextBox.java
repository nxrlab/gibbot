
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

/**TextBox displays information about the Gibbot with easter eggs about the project */

public class TextBox extends JPanel{
	
	/**Font used to write*/
	private final Font ANDALE_SMALL;
	
	/**Specified by LayoutContainerPanel parent. Used to set preferred dimensions in constructor*/
	private int height;
	/**Specified by LayoutContainerPanel parent. Used to set preferred dimensions in constructor*/
	private int width;
	
	
	/**Proportion of horizontal space this box will take up in {@link LayoutContainerPanel}.*/
	private final double TEXT_WIDTH_ALLOCATION = .23;
	/**Proportion of vertical space this box will take up in {@link LayoutContainerPanel}.*/
	private final double TEXT_HEIGHT_ALLOCATION = 1/3.45;
	
    /**Constructor sets preferred size to tell layout manager of {@link LayoutContainerPanel} how to draw this panel;
    Initializes some graphics elements including wireless signal.
    @param widthOfContainer used to set {@link #width}
    @param heightOfContainer used to set {@link #height}*/
    public TextBox(int widthOfContainer, int heightOfContainer) {
    	
    	width = (int)(widthOfContainer*TEXT_WIDTH_ALLOCATION);
    	height = (int)(heightOfContainer*TEXT_HEIGHT_ALLOCATION);
    	
    	ANDALE_SMALL = ImageHandler.getAndaleFont().deriveFont(Font.BOLD, (int)(width*0.07));
    	
    	setPreferredSize(new Dimension(width, height));
    	setBackground(GibbotGUI3.GLOBAL_BG);
    	setFont(ANDALE_SMALL);
  
    	
    }
    
    /**Override of {@link javax.swing.JComponent#paintComponent}. super.paintComponent() call fills background color.
     *This is what is executed whenever repaint() is called in the program. 
     *Calls {@link #drawTextBubble} to draw everything. If images need to be relocated/resized, change parameters of this call.
     *@param g Graphics context for drawing. Kind of a black box; gets handled in the background somehow */  	
    public void paintComponent(Graphics g){
    	
    	int xCoor = (int)(width*0.0347);
    	int yCoor = (int)(height*0.16);
    	
    	super.paintComponent(g);
    	g.drawString("Name: Gibbot", xCoor, yCoor);
    	g.drawString("Age : 7 years old", xCoor, (int)(yCoor*1.86));
    	g.drawString("Home: NxR Lab @ NU", xCoor, (int)(yCoor*2.72));
    	g.drawString("Hobbies:Eating bananas,", xCoor, (int)(yCoor*3.572));
    	g.drawString("Swinging around,", (int)(xCoor*11.8), (int)(yCoor*4.43));
    	g.drawString("Grooming friends", (int)(xCoor*11.8), (int)(yCoor*5.29));
    	
    }

}