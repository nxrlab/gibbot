import java.lang.Math;
import java.awt.*;

/**A Gibbot object contains data on where the robot is on the board, to be translated to user interface.
 Right now its method for updating its own animation is a simulation, but a commented outline is above it
 for hypothetically getting the real coordinates from {@link GUISerialPort}, which also contains a dummy method now.
 All the get methods may not be necessary (last four are currently unused) but they were left just in case.*/
public class Gibbot {
	
	/**GIBBOT IS HARDCODED FOR BETA TEST, WILL BE CONVERTED INTO RELATIVE COORDINATES*/
	
	/**Distance, in pixels, from motor to end of first link*/
	private final int ARM1W = 100;
	/**Distance, in pixels, from motor to end of second link*/
	private final int ARM2W = 100;
	/**Height, in pixels, of first link*/
	private final int ARM1H = 20;
	/**Height, in pixels, of second link*/
	private final int ARM2H = 20;
	/**Initial x-coor of center joint of gibbot animation*/
	private final int STARTX = 250;
	/**Initial y-coor of center joint of gibbot animation*/
	private final int STARTY = 100;
	/**Specifies radius, in pixels, of circles at the joints of the gibbot animation. 
	 *NOTE: if height of arm1 and arm2 are different, this will need to be changed. */
	private final int RADIUS = (ARM1H % 2 == 0 ? ARM1H + 12 : ARM1H + 11);   																			  

	/**Color of the first link in the animation. If altered, also alter 
	 *{@link Speedometer#ARM1COLOR} with same first three values (RGB) and fourth value (alpha) = 100.*/
	public final Color ARM1COLOR = new Color(150, 150, 150);
	/**Color of the second link in the animation. If altered, also alter 
	 *{@link Speedometer#ARM2COLOR} with same first three values (RGB) and fourth value (alpha) = 100.*/
	public final Color ARM2COLOR = new Color(36, 149, 176); 
	
	/**Coordinates of center joint, ends of links*/
	private double pivotX, pivotY, arm1X, arm1Y, arm2X, arm2Y;
	/**Angle between links (currently unused in code)*/
	//private double theta;

    /**Constructor initializes all coordinates.*/
    public Gibbot() {
    	pivotX = STARTX;
    	pivotY = STARTY;
    	arm2X = pivotX+ARM2W;
    	arm2Y = pivotY;
    	arm1X = pivotX-ARM2W;
    	arm1Y = pivotY;
  	
    }
    
    /**Returns x-coor of center joint (motor) in the animation (called by {@link BananaPanel1#paintComponent} 
    to draw a text bubble relative to it, and {@link BananaPanel1#actionPerformed} to check whether to continue
    animation).
    @return pivotX*/
    public double getPivotX(){
		return pivotX;
    }
    
    /**Returns y-coor of center joint (motor) in the animation (called by {@link BananaPanel1#paintComponent} 
    to draw a text bubble relative to it).
    @return pivotY;*/
    public double getPivotY(){
    	return pivotY;
    }
    
    /**Returns x-coor of end of first (initially left-most) link in the animation.
    @return arm1X;*/
    public double getArm1X(){
    	return arm1X;
    }
    
    /**Returns y-coor of end of first (initially left-most) link in the animation.
    @return arm1Y;*/
    public double getArm1Y(){
    	return arm1Y;
    }
    
    /**Returns x-coor of end of second (initially left-most) link in the animation.
    @return arm2X;*/
    public double getArm2X(){ 
    	return arm2X;
    }
    
    /**Returns y-coor of end of second (initially left-most) link in the animation.
    @return arm2Y;*/
    public double getArm2Y(){ 
    	return arm2Y;
    }
    
    /**Returns angle between the two links
    @return theta*/
    /*public double getTheta(){
    	return theta;
    }*/
    	
    
    /**This will update the animation based on real input from the robot*/
     	public void updateRealCoors(){
     	
     		double[] coors = GUISerialPort.getRealCoors();
     		
     		arm1X=(int)coors[0];
     		arm1Y=(int)coors[1];
     		pivotX=(int)coors[2];
     		pivotY=(int)coors[3];
     		arm2X=(int)coors[4];
     		arm2Y=(int)coors[5];
     }
     
     /**Animation simulation. Called by {@link BananaPanel1#actionPerformed}.
      *@param frameCount Stands for time value in motion equations
      *In the future this function's parameters will be double x, double y, double th1, double th2
     */
  
     public void arcMotionUpdate(double frameCount){
    	
    	//Motion code that corresponds with brachiation of the Gibbot
    	/*
    	double x = array[0];
    	double y = array[1];
    	double th1 = array[2];
    	double th2 = array[3];
    	
    	arm1X = x;
    	arm1Y = -y + GUILayeredPane.getScreenHeight();
    	
    	
    	pivotX = arm1X + (ARM1W*Math.sin(-th1));
    	pivotY = arm1Y - (ARM1W*Math.cos(th1));
    	
    	arm2X = pivotX + (ARM2W*Math.sin(-th1-th2));
    	arm2Y = pivotY - (ARM2W*Math.cos(th1+th2));
    	*/
    	
    	//Hardcoded motion code used for conducting beta test
 		if((int)(frameCount/50)%2 == 0){             
 			arm1X += 8;							   
 			arm1Y = Math.sqrt(4*Math.pow(ARM1W,2)-Math.pow((arm1X-arm2X),2))+arm2Y;
 			pivotX += 4;
 			pivotY = Math.sqrt(Math.pow(ARM1W,2)-Math.pow((pivotX-arm2X),2))+arm2Y;
 		}
 		else{
 			arm2X += 8;
 			arm2Y = Math.sqrt(4*Math.pow(ARM1W,2)-Math.pow((arm2X-arm1X),2))+arm1Y;
 			pivotX += 4;
 			pivotY = Math.sqrt(Math.pow(ARM1W,2)-Math.pow((pivotX-arm1X),2))+arm1Y;
 		}
 		
     		
     }	
     			
	/**Draws gibbot animation, mostly to minimize clutter in {@link BananaPanel1#paintComponent}.
	Right now it is drawn as two thick lines and three circles.
	@param g passed from {@link BananaPanel1#paintComponent}
	*/
	public void draw(Graphics g){ 
	
		Graphics2D g2=(Graphics2D)g;
		
		g2.setStroke(new BasicStroke((int)ARM1H));
		g2.setColor(Color.BLACK);
		g2.drawLine((int)arm1X, (int)arm1Y, (int)pivotX, (int)pivotY);
		g2.drawLine((int)arm2X, (int)arm2Y, (int)pivotX, (int)pivotY);
		g2.setStroke(new BasicStroke((int)ARM1H - 7));
		g2.setColor(ARM1COLOR);
		g2.drawLine((int)arm1X, (int)arm1Y, (int)pivotX, (int)pivotY);
		g2.setColor(ARM2COLOR);
		g2.drawLine((int)arm2X, (int)arm2Y, (int)pivotX, (int)pivotY);
		
		g2.setColor(Color.BLACK);
		g2.fillOval((int)pivotX-RADIUS/2, (int)pivotY-RADIUS/2, RADIUS, RADIUS);
		g2.fillOval((int)arm1X-RADIUS/2, (int)arm1Y-RADIUS/2, RADIUS, RADIUS);
		g2.fillOval((int)arm2X-RADIUS/2, (int)arm2Y-RADIUS/2, RADIUS, RADIUS);
		
	}
	
	/**Called by {@link BananaPanel1#actionPerformed} when the animated robot reaches the banana image. Not sure
	if this will be needed when using actual coordinates for animation.
	*/
	public void reset(){ 
		pivotX = STARTX;
    	pivotY = STARTY;
    	arm2X = pivotX+ARM2W;
    	arm2Y = pivotY;
    	arm1X = pivotX-ARM2W;
    	arm1Y = pivotY;
	}
}   