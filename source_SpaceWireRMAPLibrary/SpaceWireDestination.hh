#ifndef SPACEWIREDESTINATION_HH_
#define SPACEWIREDESTINATION_HH_

#include <vector>
using namespace std;

/** This class holds information of the destination of
 * a SpaceWire packet such as logical and path address.
 * Packet addressing mode can be designated by filling
 * path addressing field of this class with non-zero vector
 * (use setDestinationPathAddress(vector<unsigned char>) method).
 */  
class SpaceWireDestination {
private:
	unsigned char destinationLogicalAddress;
	vector<unsigned char> destinationPathAddress;
public:
	/** Default constructor.
	 */
	SpaceWireDestination();
	
	/** Constructor. Initializes the destination logical address.
	 * @param address destination logical address
	*/
	SpaceWireDestination(unsigned char address);
	
	/** Constructor. Initializes the destination path address.
	 * @param address destination path address vector
	*/
	SpaceWireDestination(vector<unsigned char> address);
	
	/** Constructor. Initializes the destination logical and path address.
	 * @param logicaladdress destination logical address
	 * @param pathaddress destination path address vector
	*/
	SpaceWireDestination(unsigned char logicaladdress,vector<unsigned char> pathaddress);
	
	/** Setter for destinationLogicalAddress.
	 * @param address destination logical address
	 */
	void setDestinationLogicalAddress(unsigned char address);
	
	/** Getter for destinationLogicalAddress.
	 * @return destination logical address
	 */
	unsigned char getDestinationLogicalAddress();

	/** Setter for destinationPathAddress.
	 * If the path address vector is not empty, Path Addressing mode
	 * will be used in a packet transfer.
	 * @param address destination path address vector
	 */
	void setDestinationPathAddress(vector<unsigned char> address);
	
	/** Setter for destinationPathAddress.
	 * This method changes the addressing mode from Path Addressing
	 * to Logical Addressing by clering path address vector.
	 */
	void clearDestinationPathAddress();

	/** Getter for destinationPathAddress.
	 * @return destination path address vector
	 */
	vector<unsigned char> getDestinationPathAddress();

public:
	static const unsigned char DefaultLogicalAddress=0xFE;
};


#endif /*SPACEWIREDESTINATION_HH_*/
