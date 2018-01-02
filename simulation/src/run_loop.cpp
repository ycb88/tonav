//
// Created by Tomas Krejci on 10/14/17.
//

#include "run_loop.h"

#include <limits>

#include "vio_simulation.h"

void RunLoop::setSimulationLength(double time) {
    simulation_length_ = time;
}

void RunLoop::run() {
    double last_simulation_update_time = std::numeric_limits<double>::min();
    while (!queue_.empty() && !should_stop_) {
        Item item = queue_.top();
        queue_.pop();
        
        double time = item.time;
        if (!std::isnan(simulation_length_) && time > simulation_length_) {
            break;
        }
        RunLoopCallback *callback = item.callback;
        callback->runLoopCallback(time);
        
        if (last_simulation_update_time != time) {
            if (simulation_ != nullptr) {
                simulation_->runLoopCallback(time);
            }
            last_simulation_update_time = time;
        }
    }
}

void RunLoop::stop() {
    should_stop_ = true;
}

void RunLoop::registerSimulationForUpdates(VioSimulation *simulation) {
    simulation_ = simulation;
}

void RunLoop::registerCallback(double time, RunLoopCallback *callback) {
    Item item;
    item.time = time;
    item.callback = callback;
    queue_.push(item);
}

bool RunLoop::Item::operator>(const Item &other) const {
    return time > other.time;
}

bool RunLoop::isRunning() const {
    return should_stop_;
}
