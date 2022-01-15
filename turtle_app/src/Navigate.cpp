#include "Navigate.h"
#include "CameraCapture.h"
#include "fsm/FiniteStateMachine.h"

#include <actionlib/client/simple_action_client.h>
#include <move_base_msgs/MoveBaseGoal.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

namespace fsm
{

State& Navigate::getInstance()
{
	static Navigate singleton;
	return singleton;
}

void Navigate::run(FiniteStateMachine* fsm)
{
	ROS_INFO_STREAM_NAMED(__func__, "In Navigate state");

	auto& goals = std::any_cast<Goals&>(fsm->getUserData().at("goals"));
	if (goals.size() == 0) {
		ROS_INFO_STREAM_NAMED(__func__, "No remaining goals");
		return;
	}

	const auto& goal = goals.front();
	ROS_INFO_STREAM_NAMED(__func__, "Next goal: " << goal[0] << " " << goal[1] << " " << goal[2]);

	actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> navActionClient("move_base", true);
	navActionClient.waitForServer();
	move_base_msgs::MoveBaseGoal navGoal;
	navGoal.target_pose.header.frame_id = "map";
	navGoal.target_pose.pose.position.x = goal[0];
	navGoal.target_pose.pose.position.y = goal[1];
	tf2::Quaternion quat;
	quat.setRPY(0, 0, goal[2]);
	quat.normalize();
	navGoal.target_pose.pose.orientation = tf2::toMsg(quat);
	navActionClient.sendGoal(navGoal);
	bool finished_before_timeout = navActionClient.waitForResult();

	goals.pop();

	// Transition
	if (goals.size() != 0) {
		fsm->setState(CameraCapture::getInstance());
	}
}

} // end namespace fsm