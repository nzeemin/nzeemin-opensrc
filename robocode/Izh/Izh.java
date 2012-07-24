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
	double targetDirection = -1.0; //which way to move
	double targetAngle = 0.0;
	double targetX = -1.0;
	double targetY = -1.0;
	int targetPauseCount = 0;
	
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
		setAdjustGunForRobotTurn(false);
		setColors(Color.blue, Color.white, Color.gray); // body,gun,radar

		while (true)
		{
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
			}
			
			double bulletPower = 1.0;
			double gunRotate = 0.0;
			double bearing = 0.0;
			double bodyRotate = 0.0;
			
			// Moving
			if (targetPauseCount > 0)
			{
				targetPauseCount--;
			}
			else if (targetX < 0.0)
			{
				for (int i = 0; i < 100; i++) {
					targetAngle = robocode.util.Utils.getRandom().nextDouble() * Math.PI;
					targetDirection = 1.0;  // forward
					if (targetAngle > Math.PI)
						targetDirection = -1.0;  // backward
					targetAngle = Utils.normalRelativeAngle( getHeadingRadians() + targetAngle - Math.PI/2 );
					double stepdist = 60 + robocode.util.Utils.getRandom().nextDouble() * 60;
					targetX = getX() + Math.sin(targetAngle) * stepdist;
					targetY = getY() + Math.cos(targetAngle) * stepdist;
					if (targetX < 27 || targetX > getBattleFieldWidth()-27 || targetY < 27 || targetY > getBattleFieldHeight()-27)
						continue;
					if ((targetX - enemyLastX)*(targetX - enemyLastX) + (targetY - enemyLastY)*(targetY - enemyLastY) < 45*45)
						continue;
					break;
				}
			}
			else  // We are on the way to our target
			{
				if ((getX() - targetX) * (getX() - targetX) + (getY() - targetY) * (getY() - targetY) < 36)  // Distance to target < 6
				{
					targetX = targetY = -1.0;
					targetAngle = 0.0;
					targetPauseCount = robocode.util.Utils.getRandom().nextInt(5);
				}
				else
				{
					double bearingTarget = Math.atan2(targetX - getX(), targetY - getY());
					double speed = 6.0 + robocode.util.Utils.getRandom().nextDouble() * 2.0;
					double goalDirection = (targetDirection < 0) ? bearingTarget + Math.PI : bearingTarget;
					goalDirection += (robocode.util.Utils.getRandom().nextDouble() - 0.5) * 3*Math.PI/180;
					//double maxBodyTurn = Rules.getTurnRateRadians(speed * targetDirection);
					double maxBodyTurn = 8.5 * Math.PI/180;
					//System.out.println(maxBodyTurn/Math.PI*180);
					bodyRotate = Utils.normalRelativeAngle( goalDirection - getHeadingRadians() );
					bodyRotate = Math.max( Math.min( bodyRotate, maxBodyTurn), -maxBodyTurn);
		
					setAhead(speed * targetDirection);
					setTurnRightRadians(bodyRotate);
				}
			}

			if (ticksSinceEnemyLast > 0)
			{
				// Calculate distance to the enemy
				double distance = Math.sqrt((enemyLastX - getX()) * (enemyLastX - getX()) + (enemyLastY - getY()) * (enemyLastY - getY()));
				
				bulletPower = Math.min(3.0, (distance < 300) ? (450 / distance) : 1.5);
				
				// Predict enemy position
				double bulletTravelTime = distance / (20 - (3 * bulletPower));
				//double ticks = ticksSinceEnemyLast + bulletTravelTime;
				enemyPredictX = enemyLastX + bulletTravelTime * enemyLastVelocity * Math.sin(enemyLastHeadingRad);
				enemyPredictY = enemyLastY + bulletTravelTime * enemyLastVelocity * Math.cos(enemyLastHeadingRad);
				
				// Calculate angle to the enemy
				bearing = Math.atan2(enemyPredictX - getX(), enemyPredictY - getY());
				//System.out.print(bearing * 180.0 / Math.PI); System.out.print(" ");
				// Calculate distance to the enemy
				distance = Math.sqrt((enemyPredictX - getX()) * (enemyPredictX - getX()) + (enemyPredictY - getY()) * (enemyPredictY - getY()));
	
				// Targetting
				gunRotate = Utils.normalRelativeAngle( bearing - getGunHeadingRadians() );
				gunRotate = Math.max( Math.min(gunRotate, Rules.GUN_TURN_RATE_RADIANS), -Rules.GUN_TURN_RATE_RADIANS);
				setTurnGunRightRadians(gunRotate);
			}
	
			// Radar
			if (ticksSinceEnemyLast > 0)
			{
				double bearingCurrent = Math.atan2(enemyLastX - getX(), enemyLastY - getY());
				double radarTurnRad = Utils.normalRelativeAngle( bearingCurrent - getRadarHeadingRadians() );
				double extraTurnRad = 7.5 * Math.PI / 180.0;
				radarTurnRad += (radarTurnRad < 0) ? -extraTurnRad : extraTurnRad;
				radarTurnRad = Utils.normalRelativeAngle( radarTurnRad - gunRotate - bodyRotate );  // adjust for robot/gun rotation
				radarTurnRad = Math.max( Math.min(radarTurnRad, Rules.RADAR_TURN_RATE_RADIANS), -Rules.RADAR_TURN_RATE_RADIANS);
				setTurnRadarRightRadians(radarTurnRad);
			}
		
			if ((Math.abs(gunRotate) < 4*Math.PI/180) && getGunHeat() == 0.0)
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
		if (enemyLastX >= 0)
		{
			g.setColor(java.awt.Color.GREEN);
			g.drawLine((int)getX(), (int)getY(), (int)enemyPredictX, (int)enemyPredictY);
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
