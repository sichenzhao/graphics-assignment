#include "JointNode.hpp"
#include <iostream>

using namespace std;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
    joint_xAngle = init;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
    joint_yAngle = init;
}

void JointNode::set_xAngle(float xAngle){
    joint_xAngle =  xAngle;
    if(joint_xAngle > m_joint_x.max){
        joint_xAngle = m_joint_x.max;
    }
    if(joint_xAngle < m_joint_x.min){
        joint_xAngle = m_joint_x.min;
    }
}

void JointNode::set_yAngle(float yAngle){
    joint_yAngle =  yAngle;
    if(joint_yAngle > m_joint_y.max){
        joint_yAngle = m_joint_y.max;
    }
    if(joint_yAngle < m_joint_y.min){
        joint_yAngle = m_joint_y.min;
    }
}

float JointNode::get_xAngle(){
    return joint_xAngle;
}

float JointNode::get_yAngle(){
    return joint_yAngle;
}
