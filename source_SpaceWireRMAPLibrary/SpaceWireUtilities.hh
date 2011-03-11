#ifndef SPACEWIREUTILITIES_HH_
#define SPACEWIREUTILITIES_HH_

#include "SpaceWireException.hh"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

/** This class provides utility methods which can be
 * handily used in SpaceWire-related programs.
 */
class SpaceWireUtilities {
public:
	/** Dumps vector<unsigned char> variable as a packet.
	 * The default console is used to display the data.
	 * The vector containt is formed into 8 column (8bytes
	 * per line) and displayed as hexadecimal number.
	 * @param data a reference to a vector which holds packet data to be dumped
	 */
	static void dumpPacket(vector<unsigned char>* data) throw (SpaceWireException);

	/** Dumps vector<unsigned char> variable as a packet.
	 * The consecutive N bytes (given as wordwidth parameter)
	 * are concatenated to one word.
	 * @param data a reference to a vector which holds packet data to be dumped
	 * @param wordwidth the number of bytes which will be concatenated into one word
	 */
	static void dumpPacket(vector<unsigned char>* data,unsigned int wordwidth) throw (SpaceWireException);
	
	/** Dumps vector<unsigned char> variable as a packet.
	 * The output stream can be selected by giving a pointer
	 * to ostream.
	 * The vector containt is formed into 8 column (8bytes
	 * per line) and displayed as hexadecimal number.
	 * @param ofs a reference to output stream
	 * @param data a reference to a vector which holds packet data to be dumped
	 */	
	static void dumpPacket(ostream* ofs,vector<unsigned char>* data) throw (SpaceWireException);
	
	/** Dumps vector<unsigned char> variable as a packet.
	 * The output stream can be selected by giving a pointer
	 * to ostream. The consecutive N bytes (given as wordwidth parameter)
	 * are concatenated to one word.
	 * @param ofs a reference to output stream
	 * @param data a reference to a vector which holds packet data to be dumped
	 * @param wordwidth the number of bytes which will be concatenated into one word
	 */	
	static void dumpPacket(ostream* ofs,vector<unsigned char>* data,unsigned int wordwidth) throw (SpaceWireException);

	/** Converts string like "0xFF" to unsigned char type value.
	 * The prefix "0x" is automatically removed by this method.
	 * @param str input string (1byte)
	 * @return converted 1-byte unsigned char 
	 */ 
	static unsigned char convertStringToUnsignedChar(string str);
	
	/** Print vector containt with address given as an argument.
	 * @param address address to be dumped with data
	 * @param data a reference to a vector which holds data to be dumped
	 */ 
	static void printVectorWithAddress(unsigned int address,vector<unsigned char>* data);

	/** Print vector containt with address given as an argument.
	 * @param address address to be dumped with data
	 * @param data a reference to a vector which holds data to be dumped
	 */ 
	static void printVectorWithAddress2bytes(unsigned int address,vector<unsigned char>* data);

	
public:
	static const unsigned int DumpsPerLine=8;
};
#endif /*SPACEWIREUTILITIES_HH_*/
