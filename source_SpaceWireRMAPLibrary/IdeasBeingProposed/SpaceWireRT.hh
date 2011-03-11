#ifndef SPACEWIRERT_HH_
#define SPACEWIRERT_HH_

class SpaceWireRTEngine {
public:
private:
};

class SpaceWireRTChannel {
public:
private:
};

class SpaceWireRTBuffer {
public:
private:
};

class SpaceWireRTException {
public:
private:
};

/** Sample Code
 * 

-----------------
SpaceWireRTEngine* rtengine=new SpaceWireRTEngine();
rtengine->start();

SpaceWireRTChannel* channel=rtengine->createChannel(SpaceWireDestination(xx));
channel->connect();

vector<unsinged char> sentdata;
channel->send(sentdata);
vector<unsinged char> receiveddata=channel->receive();
-----------------

-----------------
SpaceWireRTEngine* rtengine=new SpaceWireRTEngine();
rtengine->start();

SpaceWireRTChannel* channel=rtengine->acceptChannel(SpaceWireDestination(xx));

vector<unsinged char> sentdata;
channel->send(sentdata);
vector<unsinged char> receiveddata=channel->receive();
-----------------


channel no connection ga kakuritsu sareru madeha,
send ya receive wo invoke shitemo, wait ni hairu
or timeout de exception ni naru.

connected ka douka ha, BACK ga jushin sarete iruka
douka de check dekiru node, sonotameno method mo
youi shite oku.

#endif /*SPACEWIRERT_HH_*/
