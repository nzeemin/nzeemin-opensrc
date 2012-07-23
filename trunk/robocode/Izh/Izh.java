package nzeemin;
import robocode.*;
import robocode.util.Utils;
import java.awt.Color;
//import java.awt.Graphics2D;

// API help : http://robocode.sourceforge.net/docs/robocode/robocode/Robot.html

/**
 * Izh - a robot by (your name here)
 */
public class Izh extends AdvancedRobot
{
	double moveDirection = -1.0; //which way to move
	double targetAngle = 0.0;
	double targetX = -1.0;
	double targetY = -1.0;
	int directionChangeCount = 100;
	
	double enemyLastX = -1;
	double enemyLastY = -1;
	double enemyLastHeadingRad = 0;
	double enemyLastVelocity = 0;
	int ticksSinceEnemyLast = -1;
	
	double enemyPredictX = -1;
	double enemyPredictY = -1;

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
			
			if (ticksSinceEnemyLast >= 4)
			{
				ticksSinceEnemyLast = -1;  // We lost the enemy
				//System.out.println("LOST the enemy");
			}
			if (ticksSinceEnemyLast < 0)
			{  // Enemy is not found yet, or lost
				enemyLastX = enemyLastY = -1;
				enemyLastHeadingRad = enemyLastVelocity = 0;
				ticksSinceEnemyLast = -1;
				setTurnRadarRight(45);
				//execute();
				//continue;
			}
			
			double bulletPower = 1.0;
			double gunRotate = 0.0;

			if (ticksSinceEnemyLast > 0)
			{
				// Calculate distance to the enemy
				double distance = Math.sqrt((enemyLastX - getX()) * (enemyLastX - getX()) + (enemyLastY - getY()) * (enemyLastY - getY()));
				
				bulletPower = Math.min(3.0, (distance < 200) ? (300 / distance) : 1.25);
				
				// Predict enemy position
				double bulletTravelTime = distance / (20 - (3 * bulletPower));
				//double ticks = ticksSinceEnemyLast + bulletTravelTime;
				enemyPredictX = enemyLastX + bulletTravelTime * enemyLastVelocity * Math.sin(enemyLastHeadingRad);
				enemyPredictY = enemyLastY + bulletTravelTime * enemyLastVelocity * Math.cos(enemyLastHeadingRad);
				
				// Calculate angle to the enemy
				double bearing = Math.atan2(enemyPredictX - getX(), enemyPredictY - getY());
				//System.out.print(bearing * 180.0 / Math.PI); System.out.print(" ");
				// Calculate distance to the enemy
				distance = Math.sqrt((enemyPredictX - getX()) * (enemyPredictX - getX()) + (enemyPredictY - getY()) * (enemyPredictY - getY()));
	
				// Radar
				double radarTurnRad = Utils.normalRelativeAngle( bearing - getRadarHeadingRadians() );
				double extraTurnRad = 20.0 * Math.PI / 180.0;
				radarTurnRad += (radarTurnRad < 0 ? -extraTurnRad : extraTurnRad);
				setTurnRadarRightRadians(radarTurnRad);
				//System.out.print("Radar turn ");  System.out.println(radarTurnRad / Math.PI * 180.0);
	
				// Targetting
				gunRotate = Utils.normalRelativeAngle( bearing - getGunHeadingRadians() );
				gunRotate = Math.max( Math.min(gunRotate, Rules.GUN_TURN_RATE_RADIANS), -Rules.GUN_TURN_RATE_RADIANS);
				setTurnGunRightRadians(gunRotate);
			}
			
			// Moving
			if (targetX < 0.0)
			{
				while (true) {
					targetAngle = robocode.util.Utils.getRandom().nextDouble() * Math.PI;
					moveDirection = 1.0;  // forward
					if (targetAngle > Math.PI/2)
					{
						targetAngle += Math.PI/2;
						moveDirection = -1.0;  // backward
					}
					targetAngle = Utils.normalRelativeAngle( getHeadingRadians() + targetAngle - Math.PI/4 );
					double stepdist = 40 + robocode.util.Utils.getRandom().nextDouble() * 80;
					targetX = getX() + Math.sin(targetAngle) * stepdist;
					targetY = getY() + Math.cos(targetAngle) * stepdist;
					if (targetX < 25 || targetX > getBattleFieldWidth()-25 || targetY < 25 || targetY > getBattleFieldHeight()-25)
						continue;
					break;
				}
			}
			else  // We are on the way to our target
			{
				if ((getX() - targetX) * (getX() - targetX) + (getY() - targetY) * (getY() - targetY) < 25)  // Distance to target < 5
				{
					targetX = targetY = -1.0;
					targetAngle = 0.0;
				}
				else
				{
					double bearingTarget = Math.atan2(targetX - getX(), targetY - getY());
					double speed = 4.5 + robocode.util.Utils.getRandom().nextDouble() * 3.5;
					double goalDirection = (moveDirection < 0) ? bearingTarget + Math.PI : bearingTarget;
					goalDirection += (robocode.util.Utils.getRandom().nextDouble() - 0.5) * 0.3 * Math.PI;

					double bodyRotate = Utils.normalRelativeAngle( goalDirection - getHeadingRadians() );
					bodyRotate = Math.max( Math.min( bodyRotate, 9.0 * Math.PI / 180.0), -9.0 * Math.PI / 180.0);
					setTurnRightRadians(bodyRotate);
		
					setAhead(speed * moveDirection);
				}
			}
		
			if (Math.abs(gunRotate) < Rules.GUN_TURN_RATE_RADIANS && getGunHeat() == 0.0)
			{
				setFire(bulletPower);
			}
				
			execute();
			
			if (ticksSinceEnemyLast >= 0)
				ticksSinceEnemyLast++;
		}
	}
	
	/*public void onPaint(java.awt.Graphics2D g) {
		if (targetX >= 0)
		{
			g.setColor(java.awt.Color.RED);
			g.drawLine((int)getX(), (int)getY(), (int)targetX, (int)targetY);
		}
	}*/
	
	/**
	 * onScannedRobot: What to do when you see another robot
	 */
	public void onScannedRobot(ScannedRobotEvent e) {
		//System.out.println("onScannedRobot");
		double absBearing = e.getBearingRadians() + getHeadingRadians();
		enemyLastX = getX() + Math.sin(absBearing) * e.getDistance();
		enemyLastY = getY() + Math.cos(absBearing) * e.getDistance();
		enemyLastHeadingRad = e.getHeadingRadians();
		enemyLastVelocity = e.getVelocity();
		ticksSinceEnemyLast = 0;
	}
}
																								