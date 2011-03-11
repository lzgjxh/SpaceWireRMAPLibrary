/*

����
�ESpaceWireNetwork���C�u�����ł́A�N���X�̃C���X�^���X����new�ł����s��Ȃ�
�@�E���ׂăq�[�v��ɃI�u�W�F�N�g�C���X�^���X�𐶐�����
�@�Edelete�́Anew�����l���s��(�o�^�����R���e�i�̒��Ȃǂł́A�C���X�^���X��
�@�@����͂��邪�Adelete�͂��Ȃ�)
�ESpaceWireNetwork���C�u�������̃N���X�ɂ́A�ȉ��̃��\�b�h��p��
�@�E�R�s�[�R���X�g���N�^
�@�E������Z�q�̃I�[�o�[���C�h
�@�Eclone()���\�b�h
�@�Edump()���\�b�h
�E���ׂẴI�u�W�F�N�g�Ɂu���O name�v������

�@
*/

#include <string>
#include <stringstream>
using namespace std;

class SpaceWireNetworkObject {
private:
	string name;
public:
	SpaceWireNetworkObject(string newname){
		setName(newname);
	}
	string getName(){
		return name;
	}
	void setName(string newname){
		name=newname;
	}
	virtual void dump(){
		cout << "SpaceWireNetworkObject" << endl;
		cout << "name = " << name << endl;
	}
};

class SpaceWireNetworkLink : public SpaceWireNetworkObject {
private:
	SpaceWireNetworkPort* porta;
	SpaceWireNetworkPort* portb;
public:
	SpaceWireNetworkLink(string newname) : SpaceWireNetworkObject(newname){
		
	}
	
	SpaceWireNetworkLink(string newname,SpaceWireNetworkPort* newporta,SpaceWireNetworkObject* newportb) : SpaceWireNetworkObject(newname){
		setPortA(newporta);
		setPortB(newportb);
	}
	
	void setPortA(SpaceWireNetworkPort* newporta){
		porta=newporta;
	}
	
	SpaceWireNetworkPort* getPortA(){
		return porta;
	}
	
	void setPortB(SpaceWireNetworkPort* newportb){
		portb=newportb;
	}

	SpaceWireNetworkPort* getPortB(){
		return portb;
	}
};

class SpaceWireNetworkTopology : SpaceWireNetworkObject {
private:
	list<SpaceWireNetworkObject*> objectlist;
	list<SpaceWireNetworkLink*> linklist;
public:
	SpaceWireNetworkTopology(string newname) : SpaceWireNetworkObject(newname){
		
	}
	
	void addNode(SpaceWireNetworkObject* object) throw (SpaceWireNetworkException) {
		objectlist.push_back(object);
	}
	
	void connectLinkBetween(SpaceWireNetworkPort* porta,SpaceWireNetworkPort* portb) throw (SpaceWireNetworkException) {
		stringstream ss;
		ss << getName() << "_link_between_" << porta->getName() << "_" << portb->getName();
		SpaceWireNetworkLink* newlink=new SpaceWireNetworkLink(ss.str(),porta,portb);
		linklist.push_back(newlink);
	}
	
	void disconnectLinkBetween(SpaceWireNetworkPort* porta,SpaceWireNetworkPort* portb) throw (SpaceWireNetworkException) {
		SpaceWireNetworkLink* deletedlink=NULL;
		for(unsigned int i=0;i<linklist.size();i++){
			SpaceWireNetworkLink* link=linklist.at(i);
			if( (link->getPortA()==porta && link->getPortB()==portb)
				|| (link->getPortA()==porta && link->getPortB()==portb) ){
				deletedlink=link;
			}
		}
		if(deletedlink!=NULL){
			linklist->remove(deletedlink);
		}
	}
	
};

class SpaceWireNetworkNode : public SpaceWireNetworkObject {
private:
	list<SpaceWireNetworkPort*> portlist;
public:
	SpaceWireNetworkNode(string newname) : SpaceWireNetworkObject(newname) {
		
	}
	
	void addPort(SpaceWireNetworkPort* port){
		portlist.push_back(port);
	}
	
	void deletePort(SpaceWireNetworkPort* port){
		portlist.remove(port);
	}
	
	void deletePort(string deletedobjectname){
		for(unsigned int i=0;i<portlist.size();i++){
			if(portlist.at(i).getName()==deletedobjectname){
				SpaceWireNetworkPort* port=portlist.at(i);
			}
		}
		portlist.remove(port);
	}
	
	unsigned int getNumberOfPort(){
		return portlist.size();
	}
	
	unsigned int getSmallestPortNumber(){
		unsigned int smallestportnumber=portlist.at(0)->getPortnumber();
		for(unsigned int i=0;i<portlist.size();i++){
			if(portlist.at(i)->getPortnumber()<smallestportnumber){
				smallestportnumber=portlist.at(i)->getPortnumber();
			}
		}
		return smallestportnumber;
	}
	
	unsigned int getLargestPortNumber(){
		unsigned int largestportnumber=portlist.at(0)->getPortnumber();
		for(unsigned int i=0;i<portlist.size();i++){
			if(portlist.at(i)->getPortnumber()>largestportnumber){
				largestportnumber=portlist.at(i)->getPortnumber();
			}
		}
		return largestportnumber;
	}
	
	list<SpaceWireNetworkPort*> getPortList(){
		return portlist;
	}
	
	SpaceWireNetworkPort* getPort(unsigned int portnumber){
		for(unsigned int i=0;i<portlist.size();i++){
			if(portlist.at(i)->getPortNumber()==portnumber){
				return portlist.at(i);
			}
		}
	}
};

class SpaceWireNetworkRouter : public SpaceWireNetworkObject {
private:
	list<SpaceWireNetworkPort*> portlist;
	
public:
	
};


class SpaceWireNetworkPort : public SpaceWireNetworkObject {
private:
	unsigned int portnumber;
public:
	SpaceWireNetworkPort(string newname,unsigned int newportnumber)
	 : SpaceWireNetworkObject(newname),portnumber(newportnumber) {
		
	}
	
	unsigned int getPortNumber(){
		return portnumber;
	}
};

class SpaceWireNetworkNodeSpaceCube1Shimafuji3PortIPcore : public SpaceWireNetworkNode {
private:
	
public:
	SpaceWireNetworkNodeSpaceCube1Shimafuji3PortIPcore(string newname) : SpaceWireNetworkNode(newname) {
		SpaceWireNetworkPort* port1=new SpaceWireNetworkPort(newname+"_port1",1);
		SpaceWireNetworkPort* port2=new SpaceWireNetworkPort(newname+"_port2",2);
		SpaceWireNetworkPort* port3=new SpaceWireNetworkPort(newname+"_port3",3);
		addPort(port1);
		addPort(port2);
		addPort(port3);
	}
};

class SpaceWireNetworkRouterSpaceCube