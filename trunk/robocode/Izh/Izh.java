package nzeemin;
import robocode.*;
import java.awt.Color;

// API help : http://robocode.sourceforge.net/docs/robocode/robocode/Robot.html

/**
 * Izh - a robot by (your name here)
 */
public class Izh extends AdvancedRobot
{
	double moveDirection = -1.0; //which way to move
	int directionChangeCount = 100;
	
	double enemyLastX = -1;
	double enemyLastY = -1;
	double enemyLastHeadingRad = 0;

	public void run() {
		setAdjustRadarForRobotTurn(false);
		setAdjustRadarForGunTurn(false);
		setAdjustGunForRobotTurn(true);
		setColors(Color.blue, Color.white, Color.gray); // body,gun,radar

		while (true) {
			if (getOthers() == 0) {
				doNothing();
				continue;
			}
				
			setTurnRadarRight(45); //keep turning radar

			if (enemyLastX < 0){  // Enemy is not found yet
				doNothing();
				continue;
			}
			
			// Calculate angle to the enemy
			double bearing = Math.atan2(enemyLastX - getX(), enemyLastY - getY());
			//System.out.print(bearing * 180.0 / Math.PI); System.out.print(" ");
			double gunRotate = bearing - getGunHeadingRadians();
			if (gunRotate > Math.PI) gunRotate -= Math.PI * 2;
			if (gunRotate < -Math.PI) gunRotate += Math.PI * 2;
			if (gunRotate > 20.0 * Math.PI / 180.0) gunRotate = 20.0 * Math.PI / 180.0;
			if (gunRotate < -20.0 * Math.PI / 180.0) gunRotate = -20.0 * Math.PI / 180.0;
			setTurnGunRightRadians(gunRotate);
			
			// Calculate distance to the enemy
			double distance = Math.sqrt((enemyLastX - getX()) * (enemyLastX - getX()) + (enemyLastY - getY()) * (enemyLastY - getY()));

			double speed = 1.0 + robocode.util.Utils.getRandom().nextDouble() * 7.0;
			double goalDirection = 0.0;
			if (distance > 350.0)
			{
				goalDirection = bearing;
				goalDirection += (robocode.util.Utils.getRandom().nextDouble() - 0.5) * 0.4 * Math.PI;
			}
			else
			{
				double nextX = getX() + Math.sin(goalDirection) * speed;
				double nextY = getY() + Math.cos(goalDirection) * speed;
				if (nextX < 22 || nextX > getBattleFieldWidth()-22 || nextY < 22 || nextY > getBattleFieldHeight()-22)
					goalDirection = bearing;
				else
				{
					goalDirection = bearing + Math.PI / 2.0 * moveDirection;
					goalDirection += (robocode.util.Utils.getRandom().nextDouble() - 0.5) * 0.4 * Math.PI;
				}
			}
			double bodyRotate = goalDirection - getHeadingRadians();
			if (bodyRotate > Math.PI) bodyRotate -= Math.PI * 2;
			if (bodyRotate < -Math.PI) bodyRotate += Math.PI * 2;
			if (bodyRotate > 9.0 * Math.PI / 180.0) bodyRotate = 9.0 * Math.PI / 180.0;
			if (bodyRotate < -9.0 * Math.PI / 180.0) bodyRotate = -9.0 * Math.PI / 180.0;
			setTurnRightRadians(bodyRotate);

			if (Math.abs(gunRotate) < 0.3 && getGunHeat() == 0.0)
			{
				double pwr = Math.min(3.0, (distance < 200) ? (300 / distance) : 1.25);
				setFire(pwr);
			}

			setAhead(speed);
				
			execute();
			
			directionChangeCount--;
			if (directionChangeCount == 0)
			{
				moveDirection = -moveDirection;
				directionChangeCount = robocode.util.Utils.getRandom().nextInt(60) + 40;
			}
		}
	}

	public void onHitWall(HitWallEvent e){
		//moveDirection = -moveDirection; //reverse direction upon hitting a wall
	}
	
	/**
	 * onScannedRobot: What to do when you see another robot
	 */
	public void onScannedRobot(ScannedRobotEvent e) {
		double absBearing = e.getBearingRadians() + getHeadingRadians();
		enemyLastX = getX() + Math.sin(absBearing) * e.getDistance();
		enemyLastY = getY() + Math.cos(absBearing) * e.getDistance();
		enemyLastHeadingRad = e.getHeadingRadians();
	}
}
																								