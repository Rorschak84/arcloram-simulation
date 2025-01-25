#include "Clock.hpp"
#include <string>
#include "TCP/packets.hpp"
void Clock::start(){
        
        /*
        begins a background thread that repeatedly calls tick() at fixed intervals (tickInterval). 
        The loop sleeps between iterations to ensure consistent timing.
        */

        running = true;
        lastProcessedTime = currentTimeInMilliseconds();
        Log startingLog("time start: "+std::to_string(lastProcessedTime), true);
        logger.logMessage(startingLog);
        
        clockThread= std::thread([this]() {
            auto nextTick = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

            // Log printNextTick("nextTIckeValue:"+std::to_string (nextTick.time_since_epoch().count()),true);
            // logger.logMessage(printNextTick);

            while (running) {
                
                
                nextTick += tickInterval;
            //   Log printNextTick2("nextTIckeValue:"+std::to_string( nextTick.time_since_epoch().count()),true);
            //   logger.logMessage(printNextTick2);
                tick();
                if(nextTick<std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())){
                   // throw std::runtime_error("Missed tick-Unpredictable Behaviour: PLease Increase Tick Interval ");
                    //when this happens, increase the tick interval
                    // Log missedTick("Missed tick", true);
                    // logger.logMessage(missedTick);
                }
                else{
                    std::this_thread::sleep_until(nextTick);
                }
            }
        });
    }

void Clock::stop(){
        running = false;
        if (clockThread.joinable()) {
        clockThread.join();
    }
    }

void Clock::tick() {
    compteurTick++;
    sf::Packet tickPacketReceiver;
    tickPacket tickPacket(compteurTick);
    tickPacketReceiver<<tickPacket;
    logger.sendTcpPacket(tickPacketReceiver);

    //Collect the callbacks during the last processed time and the current time
    //execute them in the order they were scheduled

    //std::lock_guard<std::mutex> lock(callbackMutex); It's useless as the map has finished been initialized, and will not be modified before reading it

    int64_t currentTime = currentTimeInMilliseconds();

    // Debugging: Print current and last processed times

    //Log tickLog("Current Time: "+std::to_string(currentTime)+", Last Processed Time: "+std::to_string(lastProcessedTime), true);
   // logger.logMessage(tickLog);
   
    // Collect callbacks to execute between lastProcessedTime and currentTime
    std::vector<CallbackType> callbacksToExecute;
    //TODO: this is not efficient, since our event are synchronous, we should not check for every ms
    
    for (int64_t time = lastProcessedTime; time <= currentTime; ++time) {
        auto range = scheduledCallbacks.equal_range(time);

        // Debugging: Check if any callbacks are found
        if (range.first == range.second) {
            // Log noCallbacks("No callbacks found for time: "+std::to_string(time), true);
            // logger.logMessage(noCallbacks);
           
        } else {

            Log callbacks("During tick: "+std::to_string(compteurTick)/*+ ", callbacks found for time: "+std::to_string(time)*/, true);
            logger.logMessage(callbacks);
            for (auto it = range.first; it != range.second; ++it) {
                callbacksToExecute.push_back(it->second); // Collect the callback
            }    
        }
    }

    // Update last processed time
    lastProcessedTime = currentTime;

    // Execute collected callbacks outside of the iteration loop
    for (const auto& callback : callbacksToExecute) {

        //Maybe make a mechanism where we launch all the callbacks and we join them afterwards.
        //maybe also freeze the clock during that time, we are not doing a real time system
        //but that means nothing is in detached mode.
        callback();
    }
}

void Clock::scheduleCallback(int64_t activationTime, CallbackType callback){
        //put the callback in the list of events at the given time
        //for one time stamp, there can multiple events (one for each node)
        //emplace and move are used to avoid copying the callback
        
        scheduledCallbacks.emplace(activationTime, std::move(callback));
    }

int64_t Clock::currentTimeInMilliseconds(){
        //returns the time that passed since the "epoch" (1st January 1970) in ms
        auto now = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    }