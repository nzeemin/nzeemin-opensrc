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
			
			if (ticksSinceEnemyLast < 0 || ticksSinceEnemyLast > 4)
			{  // Enemy is not found yet, or lost
				setTurnRadarRight(45);
				execute();
				continue;
			}

			// Calculate distance to the enemy
			double distance = Math.sqrt((enemyLastX - getX()) * (enemyLastX - getX()) + (enemyLastY - getY()) * (enemyLastY - getY()));
			
			double bulletPower = Math.min(3.0, (distance < 200) ? (300 / distance) : 1.25);
			
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
			double gunRotate = Utils.normalRelativeAngle( bearing - getGunHeadingRadians() );
			gunRotate = Math.max( Math.min(gunRotate, Rules.GUN_TURN_RATE_RADIANS), -Rules.GUN_TURN_RATE_RADIANS);
			//if (gunRotate > 20.0 * Math.PI / 180.0) gunRotate = 20.0 * Math.PI / 180.0;
			//if (gunRotate < -20.0 * Math.PI / 180.0) gunRotate = -20.0 * Math.PI / 180.0;
			setTurnGunRightRadians(gunRotate);

			double speed = 1.5 + robocode.util.Utils.getRandom().nextDouble() * 6.5;
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
			double bodyRotate = Utils.normalRelativeAngle( goalDirection - getHeadingRadians() );
			bodyRotate = Math.max( Math.min( bodyRotate, 9.0 * Math.PI / 180.0), -9.0 * Math.PI / 180.0);
			//if (bodyRotate > 9.0 * Math.PI / 180.0) bodyRotate = 9.0 * Math.PI / 180.0;
			//if (bodyRotate < -9.0 * Math.PI / 180.0) bodyRotate = -9.0 * Math.PI / 180.0;
			setTurnRightRadians(bodyRotate);

			if (Math.abs(gunRotate) < Rules.GUN_TURN_RATE_RADIANS && getGunHeat() == 0.0)
			{
				setFire(bulletPower);
			}

			setAhead(speed);
				
			execute();
			
			ticksSinceEnemyLast++;
			
			directionChangeCount--;
			if (directionChangeCount == 0)
			{
				moveDirection = -moveDirection;
				directionChangeCount = robocode.util.Utils.getRandom().nextInt(60) + 40;
			}
		}
	}
	
	/*public void onPaint(Graphics2D g) {
		if (ticksSinceEnemyLast >= 0)
		{
			//g.setColor(java.awt.Color.BLUE);
			//g.drawLine((int)getX(), (int)getY(), (int)enemyLastX, (int)enemyLastY);
			g.setColor(java.awt.Color.RED);
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
																								