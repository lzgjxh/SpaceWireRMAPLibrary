#ifndef SPACEWIRENETWORK_HH_
#define SPACEWIRENETWORK_HH_

class SpaceWireNetwork {
public:
private:
};

class SpaceWireNode {
public:
private:
};

class SpaceWirePort {
public:
private:
};

class SpaceWireLink {
public:
private:
};


/** Sample Code
 * in which a virtual SpaceWire network is constructed.
 * A path address from a certain node to other nodes can
 * be obtained with getPathAddress(SpaceWireNode from,
 * SpaceWireNode to) method of SpaceWireNetwork class.
 
SpaceWireNetwork network;
SpaceWireNode spc1(0x50);
SpaceWireNode spc2(0x51);
SpaceWireNode spc3(0x52);

SpaceWireNode dio(0x30);
SpaceWireNode adc(0x31);

network.add(spc1);
network.add(spc2);
network.add(spc3);
network.add(new SpaceWireLink(spc1.getPort(0),spc2.getPort(1));
network.add(new SpaceWireLink(spc1.getPort(1),adc.getPort(0));
network.add(new SpaceWireLink(spc1.getPort(2),spc3.getPort(0));
network.add(new SpaceWireLink(spc2.getPort(0),dio.getPort(0));
network.add(new SpaceWireLink(spc2.getPort(2),spc3.getPort(1));

SpaceWirePathAddress pathaddress_spc1todio=network.getPathAddress(spc1,dio);
RMAPDestination spc1todio(dio.getLogicalAddress(),pathaddress_spc1todio.at(0));

mitaina kanjide...


SpaceWireNetworkNode node;
SpaceWireNetworkRouter router;

SpaceWireNetworkSpaceCube : public SpaceWireNetworkRouter
*/

#endif /*SPACEWIRENETWORK_HH_*/
