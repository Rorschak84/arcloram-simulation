#include "Seed.hpp"

// we don't adopt the real time windows for the moment, as it is really impractical (a few dowens of ms every minutes...)

std::vector<std::shared_ptr<Node>> Seed::transferOwnership()
{
    std::vector<std::shared_ptr<Node>> allNodes;

    for (auto &node : listNode)
    {
        allNodes.push_back(std::move(node)); // std::move transfers ownership of the shared_ptr (from listNode to allNodes) without copying.
    }
    listNode.clear(); // Clear the seed's copy of the nodes
    return allNodes;
}

void Seed::initializeNodes()
{
    std::string use_case = communicationWindow + "_" + topology;

#if COMMUNICATION_PERIOD == RRC_DOWNLINK || COMMUNICATION_PERIOD == RRC_BEACON

    if (use_case == "RRC_Beacon_Line")
    {
        initialize_RRC_Beacon_Line();
    }
    else if (use_case == "RRC_Beacon_Mesh")
    {
        initialize_RRC_Beacon_Mesh();
    }
    else if (use_case == "RRC_Downlink_Line")
    {
        initialize_RRC_Downlink_Line();
    }
    else if (use_case == "RRC_Downlink_Mesh")
    {
        Log log("RRC_Downlink_Mesh runnning", true);
        logger.logMessage(log);
        initialize_RRC_Beacon_Mesh(); // Provisionning is the same, TDMA may differ, if protocol is fully implemented
    }
    else if (use_case == "RRC_Beacon_Mesh_Self_Healing")
    {
        initialize_RRC_Beacon_Mesh_Self_Healing();
    }
#elif COMMUNICATION_PERIOD == RRC_UPLINK
    if (use_case == "RRC_Uplink_Mesh")
    {
        initialize_RRC_Uplink_Mesh();
    }
    else if (use_case == "RRC_Uplink_Line")
    {
        initialize_RRC_Uplink_Line();
    }
#endif
}
//it's the same actually from a provisionning point of view
#if COMMUNICATION_PERIOD == RRC_DOWNLINK || COMMUNICATION_PERIOD == RRC_BEACON

void Seed::initialize_RRC_Beacon_Mesh()
{
    /*                 -- C2--
                   ---        ---
             ---C2-----------------C2
            ---        ---      ---
        C3                --C2--
          ---          ---     --
             ---C2---            --C2

    *                 ----4 ----
                   ---        ---
             --- 1----------------- 5
            ---       ---      ---
         0               -- 3--
          ---         ---     --
             --- 2---            -- 6
    */

    // create a C3 node
    std::pair<int, int> coordinates = std::make_pair(0, 0);
    auto firstNode = std::make_shared<C3_Node>(0, logger, coordinates, dispatchCv, dispatchCvMutex);

    for (size_t i = 0; i < common::nbComWindows; i++)
    {
        firstNode->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + i * common::lengthTransmissionWindow, WindowNodeState::CanTransmit); // the C3 is only transmitting in this mode

        firstNode->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + (i + 1) * common::lengthTransmissionWindow, WindowNodeState::CanSleep);
    }
    listNode.push_back(firstNode);

    // Create C2 nodes in a mesh configuration
    int nbC2Nodes = 6;
    std::vector<std::pair<int, int>> coordinatesC2 = {std::make_pair(600, 600), std::make_pair(600, -600), std::make_pair(1200, 0),
                                                      std::make_pair(1200, 1200), std::make_pair(1800, 600), std::make_pair(1800, -600)};
        Log log1("Nodes will create", true);
        logger.logMessage(log1);
    for (int i = 1; i < nbC2Nodes + 1; i++)
    {
        std::shared_ptr<C2_Node> node;
        if (i == 1) // to prove energy aware Routing works
        {   
            node = std::make_shared<C2_Node>(i, logger, coordinatesC2[i - 1], dispatchCv, dispatchCvMutex, 10.0); // Create a smart pointer
        }
        else
        {
            node = std::make_shared<C2_Node>(i, logger, coordinatesC2[i - 1], dispatchCv, dispatchCvMutex, 80.0); // Create a smart pointer
        }

        for (size_t i = 0; i < common::nbComWindows; i++)
        {
            node->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + i * common::lengthTransmissionWindow, WindowNodeState::CanCommunicate); // the C3 is only transmitting in this mode

            node->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + (i + 1) * common::lengthTransmissionWindow, WindowNodeState::CanSleep);
        }
        listNode.push_back(node);
    }
    Log log2("Nodes are created", true);
    logger.logMessage(log2);
}

void Seed::initialize_RRC_Beacon_Mesh_Self_Healing()
{

    /*
                   C2    -----   C2---
                    -                  ---
                     -                    C2
                     -               ---
        C3 --------  C2 -------- C2



                    2   -----   4---
                    -                  ---
                     -                    5
                     -               ---
        0 --------  1 -------- 3

        3 is dead -> no activation schedula planned, the network should self-heal and reroute 6 through 5 and 3
    */

    // create a C3 node
    std::pair<int, int> coordinates = std::make_pair(-300, -600);
    auto firstNode = std::make_shared<C3_Node>(0, logger, coordinates, dispatchCv, dispatchCvMutex);

    for (size_t i = 0; i < common::nbComWindows; i++)
    {
        firstNode->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + i * common::lengthTransmissionWindow, WindowNodeState::CanTransmit); // the C3 is only transmitting in this mode

        firstNode->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + (i + 1) * common::lengthTransmissionWindow, WindowNodeState::CanSleep);
    }
    listNode.push_back(firstNode);

    int nbC2Nodes = 5;
    std::vector<std::pair<int, int>> coordinatesC2 =
        {std::make_pair(600, -600), std::make_pair(800, 200), std::make_pair(1700, -600),
         std::make_pair(1700, 600), std::make_pair(2200, 0)};

    for (int i = 1; i < nbC2Nodes + 1; i++)
    {
        std::shared_ptr<C2_Node> node;
        if (i == 3) // this node will be dead
        {
            node = std::make_shared<C2_Node>(i, logger, coordinatesC2[i - 1], dispatchCv, dispatchCvMutex, 0.0, 1,2); // Create a smart pointer
        }
        else
        {

            if (i == 2)
            {
                node = std::make_shared<C2_Node>(i, logger, coordinatesC2[i - 1], dispatchCv, dispatchCvMutex, 60.0, 1,2); // Create a smart pointer
            }
            else if (i == 4)
            {
                node = std::make_shared<C2_Node>(i, logger, coordinatesC2[i - 1], dispatchCv, dispatchCvMutex, 60.0, 2,3); // Create a smart pointer
            }
            else if (i == 5)
            {
                node = std::make_shared<C2_Node>(i, logger, coordinatesC2[i - 1], dispatchCv, dispatchCvMutex, 60.0, 3,3); // Create a smart pointer
            }
            else if (i == 1)
            {
                node = std::make_shared<C2_Node>(i, logger, coordinatesC2[i - 1], dispatchCv, dispatchCvMutex, 60.0, 0,1); // Create a smart pointer
            }
            for (size_t i = 0; i < common::nbComWindows; i++)
            {
                node->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + i * common::lengthTransmissionWindow, WindowNodeState::CanCommunicate); // the C3 is only transmitting in this mode

                node->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + (i + 1) * common::lengthTransmissionWindow, WindowNodeState::CanSleep);
            }
        }

        listNode.push_back(node);
    }
}

void Seed::initialize_RRC_Beacon_Line()
{

    /*

        C3 --------  C2 -------- C2 -------- C2 -------- C2

    */

    // create a C3 node
    std::pair<int, int> coordinates = std::make_pair(0, 0);
    auto firstNode = std::make_shared<C3_Node>(0, logger, coordinates, dispatchCv, dispatchCvMutex);

    for (size_t i = 0; i < common::nbComWindows; i++)
    {
        firstNode->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + i * common::lengthTransmissionWindow, WindowNodeState::CanTransmit); // the C3 is only transmitting in this mode

        firstNode->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + (i + 1) * common::lengthTransmissionWindow, WindowNodeState::CanSleep);
    }

    listNode.push_back(firstNode);

    // Create C2 nodes in a line
    for (int i = 1; i < 5; i++)
    {
        std::pair<int, int> coordinate = std::make_pair(800 * i, 0);
        auto node = std::make_shared<C2_Node>(i, logger, coordinate, dispatchCv, dispatchCvMutex, 80); // Create a smart pointer

        for (size_t i = 0; i < common::nbComWindows; i++)
        {
            node->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + i * common::lengthTransmissionWindow, WindowNodeState::CanCommunicate); // the C3 is only transmitting in this mode

            node->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + (i + 1) * common::lengthTransmissionWindow, WindowNodeState::CanSleep);
        }
        listNode.push_back(node);
    }
}

void Seed::initialize_RRC_Downlink_Line()
{

    /*

        C3 --------  C2 -------- C2 -------- C2 -------- C2

    */

    // create a C3 node
    std::pair<int, int> coordinates = std::make_pair(0, 0);
    auto firstNode = std::make_shared<C3_Node>(0, logger, coordinates, dispatchCv, dispatchCvMutex);

    for (size_t i = 0; i < common::nbComWindows; i++)
    {
        firstNode->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + i * common::lengthTransmissionWindow, WindowNodeState::CanTransmit); // the C3 is only transmitting in this mode
        firstNode->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + (i + 1) * common::lengthTransmissionWindow, WindowNodeState::CanSleep);
    }

    listNode.push_back(firstNode);

    // Create C2 nodes in a line
    for (int i = 1; i < 5; i++)
    {
        std::pair<int, int> coordinate = std::make_pair(800 * i, 0);
        auto node = std::make_shared<C2_Node>(i, logger, coordinate, dispatchCv, dispatchCvMutex, 80.0); // Create a smart pointer

        for (size_t i = 0; i < common::nbComWindows; i++)
        {
            node->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + i * common::lengthTransmissionWindow, WindowNodeState::CanCommunicate); // the C3 is only transmitting in this mode
            node->addActivation(baseTime + (i + 1) * common::lengthSleepingWindow + (i + 1) * common::lengthTransmissionWindow, WindowNodeState::CanSleep);
        }
        listNode.push_back(node);
    }
}
#elif COMMUNICATION_PERIOD == RRC_UPLINK
void Seed::initialize_RRC_Uplink_Mesh()
{
    /*              -- C2--
                ---        ---
          ---C2-----------------C2
         ---        ---      ---
     C3                --C2--
       ---          ---     --
          ---C2---            --C2

 *                 ----4 ----
                ---        ---
          --- 1----------------- 5
         ---       ---      ---
      0               -- 3--
       ---         ---     --
          --- 2---            -- 6

     Odd:1,2,5,6
     Even:3,4
 */

    // create a C3 node
    std::pair<int, int> coordinates = std::make_pair(0, 0);
    auto firstNode = std::make_shared<C3_Node>(0, logger, coordinates, dispatchCv, dispatchCvMutex);

    for (size_t i = 0; i < common::totalNumberOfSlots; i++)
    { // initially sleep
        firstNode->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + i * common::durationDataWindow + i * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanListen);
        firstNode->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + i * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanSleep);
        firstNode->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + (i + 1) * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanTransmit);
        firstNode->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + (i + 1) * common::durationSleepWindowSecondary + (i + 1) * common::durationACKWindow, WindowNodeState::CanSleep);
    }
    listNode.push_back(firstNode);

    // Create C2 nodes in a mesh configuration
    int nbC2Nodes = 6;
    std::vector<std::pair<int, int>> coordinatesC2 = {std::make_pair(600, 600), std::make_pair(600, -600), std::make_pair(1200, 0),
                                                      std::make_pair(1200, 1200), std::make_pair(1800, 600), std::make_pair(1800, -600)};

    // the Id follows the location in the vector
    std::vector<C2_Node_Mesh_Parameter> C2_Parameters;

    C2_Node_Mesh_Parameter node1 = {coordinatesC2[0], 1, 0};
    C2_Parameters.push_back(node1);
    C2_Node_Mesh_Parameter node2 = {coordinatesC2[1], 1, 0};
    C2_Parameters.push_back(node2);
    C2_Node_Mesh_Parameter node3 = {coordinatesC2[2], 2, 1};
    C2_Parameters.push_back(node3);
    C2_Node_Mesh_Parameter node4 = {coordinatesC2[3], 2, 1};
    C2_Parameters.push_back(node4);
    C2_Node_Mesh_Parameter node5 = {coordinatesC2[4], 3, 4};
    C2_Parameters.push_back(node5);
    C2_Node_Mesh_Parameter node6 = {coordinatesC2[5], 3, 3};
    C2_Parameters.push_back(node6);
    for (int i = 1; i < nbC2Nodes + 1; i++)
    {

        auto node = std::make_shared<C2_Node>(i, logger, C2_Parameters[i - 1].coordinates, dispatchCv, dispatchCvMutex, C2_Parameters[i - 1].nextNodeIdInPath, C2_Parameters[i - 1].hopCount); // Create a smart pointer

        for (size_t i = 0; i < common::totalNumberOfSlots; i++)
        {
            node->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + i * common::durationDataWindow + i * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanCommunicate);
            node->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + i * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanSleep);
            node->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + (i + 1) * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanCommunicate);
            node->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + (i + 1) * common::durationSleepWindowSecondary + (i + 1) * common::durationACKWindow, WindowNodeState::CanSleep);
        }
        listNode.push_back(node);
    }
}
void Seed::initialize_RRC_Uplink_Line()
{
    /*

        C3 --------  C2 -------- C2 -------- C2 -------- C2

    */

    // create a C3 node
    // Pattern: Sleep - Listen(Data) - Transmit(ACK) - Sleep (sleep during even slots)

    std::pair<int, int> coordinates = std::make_pair(0, 0);
    auto firstNode = std::make_shared<C3_Node>(0, logger, coordinates, dispatchCv, dispatchCvMutex);

    for (size_t i = 0; i < common::totalNumberOfSlots; i++)
    { // initially sleep
        firstNode->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + i * common::durationDataWindow + i * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanListen);
        firstNode->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + i * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanSleep);
        firstNode->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + (i + 1) * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanTransmit);
        firstNode->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + (i + 1) * common::durationSleepWindowSecondary + (i + 1) * common::durationACKWindow, WindowNodeState::CanSleep);
    }

    listNode.push_back(firstNode);

    // Create C2 nodes in a line
    int hopCount = 1;
    for (int i = 1; i < 5; i++)
    {
        std::pair<int, int> coordinate = std::make_pair(800 * i, 0);

        // Second COnstructor for C2 (as if Beacon Mode has already provided necessary information)
        auto node = std::make_shared<C2_Node>(i, logger, coordinate, dispatchCv, dispatchCvMutex, i - 1, hopCount); // Create a smart pointer
        hopCount++;
        for (size_t i = 0; i < common::totalNumberOfSlots; i++)
        {
            node->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + i * common::durationDataWindow + i * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanCommunicate);
            node->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + i * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanSleep);
            node->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + (i + 1) * common::durationSleepWindowSecondary + i * common::durationACKWindow, WindowNodeState::CanCommunicate);
            node->addActivation(baseTime + (i + 1) * common::durationSleepWindowMain + (i + 1) * common::durationDataWindow + (i + 1) * common::durationSleepWindowSecondary + (i + 1) * common::durationACKWindow, WindowNodeState::CanSleep);
        }
        listNode.push_back(node);
    }
}
#endif