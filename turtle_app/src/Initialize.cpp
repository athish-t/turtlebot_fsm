#include "Initialize.h"
#include "Navigate.h"
#include "fsm/FiniteStateMachine.h"

namespace fsm
{

State& Initialize::getInstance()
{
	static Initialize singleton;
	return singleton;
}

void Initialize::parseGoals(XmlRpc::XmlRpcValue& xmlGoals, Goals& goals)
{
	ROS_ASSERT(xmlGoals.getType() == XmlRpc::XmlRpcValue::TypeArray);

	for (int i = 0; i < xmlGoals.size(); i++)
	{
		ROS_ASSERT(xmlGoals[i].getType() == XmlRpc::XmlRpcValue::TypeStruct);
		ROS_ASSERT(xmlGoals[i]["id"].getType() == XmlRpc::XmlRpcValue::TypeInt);
		ROS_ASSERT(xmlGoals[i]["coords"].getType() == XmlRpc::XmlRpcValue::TypeArray);
		ROS_ASSERT(xmlGoals[i]["coords"].size() == 3);
		ROS_ASSERT(xmlGoals[i]["coords"][0].getType() == XmlRpc::XmlRpcValue::TypeDouble);
		ROS_ASSERT(xmlGoals[i]["coords"][1].getType() == XmlRpc::XmlRpcValue::TypeDouble);
		ROS_ASSERT(xmlGoals[i]["coords"][2].getType() == XmlRpc::XmlRpcValue::TypeDouble);

		Goal goal;
		goal.id = static_cast<int>(xmlGoals[i]["id"]);
		goal.coordinates = {
			static_cast<double>(xmlGoals[i]["coords"][0]),
			static_cast<double>(xmlGoals[i]["coords"][1]),
			static_cast<double>(xmlGoals[i]["coords"][2])
		};

		goals.push(goal);
	}
}

void Initialize::run(FiniteStateMachine* fsm)
{
	ROS_INFO_STREAM_NAMED(__func__, "In Initialize state");

	ros::NodeHandle nh("~");

	XmlRpc::XmlRpcValue xmlGoals;
	Goals goals;
	std::string image_save_path;

	nh.getParam("image_save_path", image_save_path);
	nh.getParam("turtle_goals", xmlGoals);
	parseGoals(xmlGoals, goals);

	fsm->getUserData().emplace(std::make_pair("image_save_path", image_save_path));
	fsm->getUserData().emplace(std::make_pair("goals", goals));
	fsm->getUserData().emplace(std::make_pair("last_checkpoint_id", 0));

	// Transition
	fsm->setState(Navigate::getInstance());
}

} // end namespace fsm