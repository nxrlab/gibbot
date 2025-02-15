import java.awt.*;
import java.awt.event.*;
import java.util.HashMap;

import javax.swing.*;

/**SpeedometerBox is what it sounds like; shows the linear velocity of the two links with two different colored "number" speedometers
 */
public class SpeedometerBox extends JPanel implements ActionListener{
	
	private Image batteryBar = ImageHandler.getImage("batteryBar.png");
	private final Font ANDALE_BIG;
	private final Font ANDALE_MED; //for 100% percentage to be centered when displayed
	private final Font DIGITAL;
	
	private int batterySpent;
	private final int MAX_BATT = 136; //Will be changed to actual maximum battery value
	
    /**Specified by LayoutContainerPanel parent. Used to set preferred dimensions in constructor*/
	private int height;
	/**Specified by LayoutContainerPanel parent. Used to set preferred dimensions in constructor*/
	private int width;
	
	/**Proportion of horizontal space this box will take up in {@link LayoutContainerPanel}. Strongly recommended not to alter.*/
	private final double SPEEDOMETER_WIDTH_ALLOCATION = 1/3.0;
	/**Proportion of vertical space this box will take up in {@link LayoutContainerPanel}. Strongly recommended not to alter.*/
	private final double SPEEDOMETER_HEIGHT_ALLOCATION = 1/3.25;
    
	/**Color of needle on the speedometer corresponding to first link. If altered, also alter 
	 *{@link Gibbot#ARM1COLOR} with same first three values (RGB) but no fourth parameter (alpha).*/
	public final Color ARM1COLOR = new Color(150, 150, 150);
	/**Color of needle on the speedometer corresponding to second link. If altered, also alter 
	 *{@link Gibbot#ARM2COLOR} with same first three values (RGB) but no fourth parameter (alpha).*/
	public final Color ARM2COLOR = new Color(36, 149, 176);
	
	/**Value of velocity for first link (primary board) that's displayed on the speedometer*/	
	private double arm1vel;
	/**Value of velocity for second link (secondary board) that's displayed on the speedometer*/ 
	private double arm2vel;
 
	
	//THESE EXIST FOR THE BETA TEST
	private int wait_1 = 0;
	private int betaTest[] = new int[50];
	private boolean ARM_2_ON = true;
	private int count = 0;
	private boolean SPEED_LABEL;
	private boolean BATT_LABEL;
	
	/**Constructor sets preferred size to tell layout manager of {@link LayoutContainerPanel} how to draw this panel;
    Initializes chart size variables.
    @param widthOfContainer used to set {@link #width}
    @param heightOfContainer used to set {@link #height}*/
	public SpeedometerBox(int widthOfContainer, int heightOfContainer){
		
		width = (int)(widthOfContainer*SPEEDOMETER_WIDTH_ALLOCATION); //417px
		height = (int)(heightOfContainer*SPEEDOMETER_HEIGHT_ALLOCATION); //238px
		
		ANDALE_BIG = ImageHandler.getAndaleFont().deriveFont(Font.BOLD, (int)(width*0.072));
		ANDALE_MED = ImageHandler.getAndaleFont().deriveFont(Font.BOLD, (int)(width*0.065));
		DIGITAL = ImageHandler.getDigitalFont().deriveFont(Font.BOLD, (int)(width*0.156));
		
		
		setPreferredSize(new Dimension(width, height));
		setBackground(GibbotGUI3.GLOBAL_BG);
		
		GUITimer.addActionListener(this);
		
		
		//FOR BETA TEST
		batterySpent = 0;
		arm1vel = 0; //test case
		arm2vel = 0; //test case
		for(int i = 0; i < betaTest.length / 2 - 1; i++){
			betaTest[i] = i;
		}
		for(int i = betaTest.length / 2; i > 0; i--){
			betaTest[i] = i;
		}
		SPEED_LABEL = true;
		BATT_LABEL = true;
	}
	
	/**Override of {@link javax.swing.JComponent#paintComponent}. super.paintComponent() call fills background color.
    This is what is executed whenever repaint() is called in the program. 
    Calls {@link #drawTab} to draw containing rectangle and shadow, draws chart image, then draws needles in the appropriate place. 
    @param g Graphics context for drawing. Kind of a black box; gets handled in the background somehow
     */ 
	public void paintComponent(Graphics g){
		
		int MAX_BATT_HEIGHT = (int)(height*0.57);
		super.paintComponent(g);
		
		/**Drawing arm1vel and arm2vel*/
		int arm1VelUpperX = (int)(width*0.05);
		int arm1VelUpperY = (int)(height*0.252);
		
		g.setFont(DIGITAL);
		g.setColor(ARM1COLOR);
		g.drawString(arm1vel + "mph", arm1VelUpperX, arm1VelUpperY);
		g.setColor(ARM2COLOR);
		g.drawString(arm2vel + "mph", arm1VelUpperX, (int)(arm1VelUpperY*2.667));
		
		
		/**Drawing battery bar*/
		
		int battBarUpperX = (int)(width*0.720);
		int battBarUpperY = (int)(height*0.063);
		g.drawImage(batteryBar,
				battBarUpperX,
				battBarUpperY,
				(int)(battBarUpperX*0.27),
				(int)(battBarUpperY*10.71),
				this);
				
		g.setFont(ANDALE_BIG);
		if(batterySpent <= MAX_BATT){
			g.setColor(Color.GREEN);
			g.fillRect((int)(battBarUpperX*1.025),
					(int)(battBarUpperY*1.75)+batterySpent,
					(int)(width*0.161),
					MAX_BATT_HEIGHT-batterySpent);
		}
		g.setColor(Color.BLACK);
		int batt_height = (int)(100*batterySpent/MAX_BATT_HEIGHT);
		if(batt_height == 0) { g.setFont(ANDALE_MED); }
		g.drawString(100-batt_height + "%",
				(int)(battBarUpperX*1.05),
				(int)(battBarUpperY*14));
		
		
		if(SPEED_LABEL && BATT_LABEL){
			
		}
		
	}
	
	/**Gets velocity values from {@link GUISerialPort#data}. Called by {@link #actionPerformed}.
	*/
	public void updateForDrawing(){
		

		//HashMap<String, Integer> data = GUISerialPort.getData();
		//above will be used to update drawing with live-updating. Below code is for beta test
		
		if(BananaPanel1.getAnimating()){
			
			SPEED_LABEL = false;
			BATT_LABEL = false;
			
			if(wait_1 % 400 == 0 && batterySpent < MAX_BATT /4){
				batterySpent++;
			}
			
			if(ARM_2_ON && count < betaTest.length){
				arm2vel = betaTest[count++];
			}
			else if(ARM_2_ON && count >= betaTest.length){
				ARM_2_ON = false;
				count = 0;
			}
			else if (!ARM_2_ON && count < betaTest.length){
				arm1vel = betaTest[count++];
			}
			else{
				ARM_2_ON = true;
				count = 0;
			}
			wait_1++;
		}
		else{
			wait_1 = 0;
			arm1vel = 0;
			arm2vel = 0;
			ARM_2_ON = true;
			count = 0;
			SPEED_LABEL = true;
			BATT_LABEL = true;
		}
		
		
		//arm1vel = data.get("Gyroscope_Z_Primary")*Math.PI/180; //convert to radians
		//arm2vel = data.get("Gyroscope_Z_Secondary")*Math.PI/180; //convert to radians
			
	}
		
	
	/**Specifies how to respond to timer events from {@link GUITimer}. This panel uses events as a signal to 
    call {@link #updateForDrawing} and repaint. Only updates for drawing if awake panel is active.
    @param evt The timer event (not important to code but required by {@link java.awt.event#ActionListener} interface)
    */
	public void actionPerformed(ActionEvent evt){
		updateForDrawing();
		repaint();
	}	
}