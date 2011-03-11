#ifndef SPACEWIREPACKET_HH_
#define SPACEWIREPACKET_HH_

#include "SpaceWireDestination.hh"

#include <vector>
using namespace std;

/** SpaceWirePacket class holds a packet structure and
 * its destination. This class can be inherited to create
 * a packet class for the upper layer protocol such as RMAP.
 */
class SpaceWirePacket {
protected:
	vector<unsigned char>* packet;
	SpaceWireDestination destination;

public:
	/** Default constructor.
	 */
	SpaceWirePacket();

	/** Destructor.
	 */
	virtual ~SpaceWirePacket();

	/** Setter for the destination of the packet.
	 * @param newdestination destination of this packet
	 */
	void setDestination(SpaceWireDestination newdestination);

	/** Getter for the destination of the packet.
	 * @return destination of this packet
	 */
	SpaceWireDestination getDestination();

	/** Setter for the packet structure.
	 * This method can be overridden by the child class to
	 * perform, eg., more practical packet interpretation.
	 * @param newpacket a reference to a vector which contains packet data
	 */
	virtual void setPacket(vector<unsigned char>* newpacket);

	/** Setter for the packet structure.
	 * This method replaces internal packet buffer (vector) to
	 * new one given as an argument. The memory space for the new
	 * buffer should be reserved by "new" operator. Deletion of the
	 * new buffer will be done in this method when this is called
	 * again with another buffer pointer, or in the destructor of
	 * this class when the instance will be deleted.
	 * @param newpacketbuffer a reference to a vector instance reserved on the heap space
	 */
	void setPacketBuffer(vector<unsigned char>* newpacketbuffer);

	/** Getter for the packet structure.
	 * This method can be overridden by the child class to
	 * perform, eg., more practical packet creation.
	 * @return a reference to a vector which contains packet data
	 */
	virtual vector<unsigned char>* getPacket();

	/** Getter for packet buffer pointer.
	 * @return a reference to a packet buffer pointer
	 */
	virtual vector<unsigned char>* getPacketBuffer();
};
#endif /*SPACEWIREPACKET_HH_*/
