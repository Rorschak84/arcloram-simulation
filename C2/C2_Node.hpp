#pragma once
#include "../Node.hpp"
#include "../TCP/packets.hpp"

//TODO:
//instead of having multiple variables, prepare struct or objects that will contain the information needed to pass logic between receive() and transmit()


class C2_Node : public Node {

public :

#if COMMUNICATION_PERIOD == RRC_DOWNLINK|| COMMUNICATION_PERIOD == RRC_BEACON

    C2_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex,double batteryLevel=0)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex,batteryLevel) {
           
            initializeTransitionMap();
            setInitialState(NodeState::Sleeping);

            

    };

    //constructor that simulates a beacon session already happened -> routes are established
        C2_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex,double batteryLevel,int nextNodeId,int hopCount)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex,batteryLevel) {
            
        #if TOPOLOGY == MESH_SELF_HEALING
            //I lack of time, so I will not provision the full initial state that should result from a beacon mode,
            //I will simply display the route but internally, it's as "if" nothing happened. The topology using this constructor is built in a way that we don't see it.
            //TODO: Implement this initial state: nextNodeIdInPath, basePathCost, hopCount....
            this->nextNodeIdInPath=nextNodeId;   

            initializeTransitionMap();

        #endif

        setInitialState(NodeState::Sleeping);
            

    };
#elif COMMUNICATION_PERIOD == RRC_UPLINK
    C2_Node(int id, Logger& logger,std::pair<int, int> coordinates, std::condition_variable& dispatchCv, std::mutex& dispatchCvMutex,uint16_t nextNodeIdInPath,uint8_t hopCount)
    : Node(id, logger, coordinates, dispatchCv, dispatchCvMutex), nextNodeIdInPath(nextNodeIdInPath), hopCount(hopCount) {

            initializeTransitionMap();
            setInitialState(NodeState::Sleeping);

            //decide which slots among the DATA communicating slots will actually be used to transmit information
            if(hopCount%2==0)transmissionSlots=selectRandomEvenSlots(common::maxNodeSlots,common::totalNumberOfSlots);
            else  transmissionSlots=selectRandomOddSlots(common::maxNodeSlots,common::totalNumberOfSlots);

            nbPayloadLeft=initialnbPaylaod;
    };

#else
    #error "Unknown COMMUNICATION_PERIOD mode"
#endif
    int getClassId() const override {
        return 2;
    }
    
    std::string initMessage() const override;
 
    protected:


    bool canTransmitFromListening();
    bool canTransmitFromSleeping();
    bool canTransmitFromTransmitting();
    bool canTransmitFromCommunicating();

    bool canListenFromTransmitting();
    bool canListenFromSleeping();
    bool canListenFromListening();
    bool canListenFromCommunicating();

    bool canSleepFromTransmitting();
    bool canSleepFromListening();
    bool canSleepFromSleeping();
    bool canSleepFromCommunicating();

    bool canCommunicateFromTransmitting();
    bool canCommunicateFromListening();
    bool canCommunicateFromSleeping();
    bool canCommunicateFromCommunicating();

    bool receiveMessage(const std::vector<uint8_t> message, std::chrono::milliseconds timeOnAir) override;


    #if COMMUNICATION_PERIOD == RRC_DOWNLINK

    bool shouldSendBeacon=false;
    std::vector<int> beaconSlots; 
    std::vector<uint16_t> globalIDPacketList;//
    bool canNodeReceiveMessage();
    bool isTransmittingWhileCommunicating=false;   

    //this variable contains the id of the node that is the final receiver of the flooding packet circulating in the network
    //use to bridge the receive function and the transmit function
    std::optional<uint32_t> packetFinalReceiverId ;


    #elif COMMUNICATION_PERIOD == RRC_BEACON

        //This values should be a struct or an object, todo
        bool shouldSendBeacon=false;
        std::optional<uint8_t> hopCount;
        std::optional<uint32_t> lastTimeStampReceived;
        std::vector<int> beaconSlots; 
        std::vector<uint16_t> globalIDPacketList;//
        std::optional<uint8_t> pathCost;//the value that will be sent in the beacon packets
        std::optional<uint8_t> basePathCost; //the value that serves as reference for the most optimal path, found in the beacon received
        std::optional<uint16_t> nextNodeIdInPath;//The Id of the node in the optimal route
        bool canNodeReceiveMessage();
        bool isTransmittingWhileCommunicating=false;

    #elif COMMUNICATION_PERIOD== RRC_UPLINK

        //visualiser
        void displayRouting();// we cannot put this in the constructor as we need to wait for the visualiser to receive all the nodes
        bool routingDisplayed=false;

        //Reception
        bool canNodeReceiveMessage();
        bool isTransmittingWhileCommunicating=false;

        //Variables that should have been provided during beacon phase
        std::optional< uint16_t> nextNodeIdInPath;
        uint8_t hopCount;

        //Data Strategy
        bool isOddSlot=false;
        bool isACKSlot=true;
        unsigned int localIDPacketCounter=0;
        
        std::vector<int> transmissionSlots; //the slots where the node WILL transmit (unless if no data to send), it's fixed
        uint8_t nbPayloadLeft; //the number of payload left to send(initial + forward packet)(represents the data that will be sent, in the simulation, every payload is the same (0xFF...FF)) 
        uint8_t initialnbPaylaod=2; //initial number of payload
        //TODO: use more aliases in the code!! If only I had found this earlier...
        using SenderID = uint16_t;
        using PacketID = uint16_t;
        using PacketList = std::vector<PacketID>; // Alias for a list of Packet IDs
        using PacketMap = std::unordered_map<SenderID, PacketList>; //we need the packet Map to not forward already forwarded data packet
        // Adds a Packet ID to the sender's list
        PacketMap packetsMap; // Data structure to store packets

        //ACK strategy
        bool shouldReplyACK=false;
        uint16_t lastSenderId;
        uint16_t lastLocalIDPacket;

        //to display the number of retransmission in the visualiser
        bool isExpectingACK=false;  
        bool secondSleepWindow=true;     

    #else
        #error "Unknown COMMUNICATION_PERIOD mode"
    #endif

};
