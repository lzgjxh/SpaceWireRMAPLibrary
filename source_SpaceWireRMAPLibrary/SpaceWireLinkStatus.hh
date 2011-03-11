#ifndef SPACEWIRELINKSTATUS_HH_
#define SPACEWIRELINKSTATUS_HH_

#include <string>
#include <iostream>
using namespace std;

/** This class handles the status of SpaceWire link.
 * SpaceWireIF class and its child classes store their
 * link status in this class. Users can tell link properties
 * such as link speed and timeout duration to SpaceWireIF classes,
 * by setting new values to this class.
 */
class SpaceWireLinkStatus {
private:
	double linkSpeed;
	bool linkEstablished;
	double timeout;
	string comment;
public:
	/** Default Constructor.
	 * Link speed is set to default value.
	 * In the initialization stage, usually, link is not established.
	 */
	SpaceWireLinkStatus();
	
	/** Setter for link status.
	 * @param mode link status (true=established)
	 */
	void setLinkEstablishment(bool mode);
	
	/** Getter for link status.
	 * @return true if link is established
	 */
	bool isLinkEstablished();
	
	/** Setter for link speed.
	 * @param speedInMHz connection speed
	 */
	void setLinkSpeed(double speedInMHz);
	
	/** Getter for link speed.
	 * @return connection speed in MHz
	 */
	double getLinkSpeed();
	
	/** Setter for timeout duration.
	 * @param timeoutInMilliSec time out duration
	 */
	void setTimeout(double timeoutInMilliSec);
	
	/** Getter for timeout duration.
	 * @return time out duration
	 */
	double getTimeout();
	
	/** Setter for comment.
	 */
	void setComment(string newcomment);
	
	/** Getter for comment.
	 */
	string getComment();
	
	/** Dump detailed information.
	 */
	void dump();
	
public:
	static const double DefaultLinkSpeed;
};


#endif /*SPACEWIRELINKSTATUS_HH_*/
