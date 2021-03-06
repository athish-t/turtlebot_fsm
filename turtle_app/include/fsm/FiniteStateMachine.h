#pragma once

#include <unordered_map>
#include <any>
#include <thread>
#include <chrono>

#include "fsm/State.h"

namespace fsm
{

/*
* Class defining the finite state machine. Holds pointer to the active state
*/
class FiniteStateMachine
{
	using UserData = std::unordered_map<std::string, std::any>;

public:
	FiniteStateMachine();

	/*
	* Returns pointer to the current active state
	*/
	const State* getCurrentState() const {return currentState; }

	/*
	* Start the FSM
	*/
	void start();

	/*
	* Stop the FSM
	*/
	void stop();

	/*
	* Set the current state.
	* newState - reference to state singleton
	*/
	void setState(State& newState);

	/*
	* Get modifiable reference to the UserData store
	*/
	UserData& getUserData() {return userData; }

private:
	/*
	* Evalue the transitions of current state and run its behaviour
	*/
	void run();

	State* currentState = nullptr;  // Pointer to the current active state
	UserData userData;  			// Data store that any state in this FSM can access

	std::unique_ptr<std::thread> mainThread;  // Main thread of FSM
	bool isRunning;

};

} // end namespace fsm