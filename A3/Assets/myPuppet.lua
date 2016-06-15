-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

rootnode = gr.node('root')
--rootnode:rotate('y', -90.0)
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
pink = gr.material({255/255,192/255,203/255}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('sphere', 'torso')
rootnode:add_child(torso)
torso:set_material(pink)
torso:scale( 0.25, 0.25, 0.25 )
torso:scale(0.7,0.7,0.7);
torso:translate(0.0, -0.2, 0.0);

-- joint torso neck
jtn = gr.joint('jtn', {-15, 15, 15}, {-15, 0, 15})
torso:add_child(jtn)
jtn:translate(0.0, 0.17, 0.0);

neck = gr.mesh('sphere', 'neck')
jtn:add_child(neck)
neck:scale( 0.25, 0.25, 0.25 )
neck:scale(0.1,0.1,0.1)
neck:set_material(blue)

jnh = gr.joint('jnh', {-95,0,5}, {-70,0,70})
neck:add_child(jnh)

head = gr.mesh('sphere', 'head')
jnh:add_child(head)
head:scale( 0.25, 0.25, 0.25 )
head:scale(0.4, 0.4, 0.4)
head:set_material(red)

jhlr = gr.joint('jhlr', {-45,0,45}, {0,0,0})
head:add_child(jhlr)
jhlr:translate(-0.08,0.06,0)

leftEar = gr.mesh('sphere', 'leftEar')
jhlr:add_child(leftEar)
leftEar:scale(0.0059, 0.058, 0.0001)
leftEar:set_material(blue)

jhrr = gr.joint('jhrr', {-45,0,45}, {0,0,0})
head:add_child(jhrr)
jhrr:translate(0.08,0.06,0)

rightEar = gr.mesh('sphere', 'rightEar')
jhrr:add_child(rightEar)
rightEar:scale(0.0059, 0.058, 0.0001)
rightEar:set_material(blue)

leftEye = gr.mesh('cube', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.02, 0.01, 0.01)
leftEye:translate(-0.02, 0.06, 0.077)
leftEye:set_material(blue)

rightEye = gr.mesh('cube', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.02, 0.01, 0.01)
rightEye:translate(0.02, 0.06, 0.077)
rightEye:set_material(blue)

nose = gr.mesh('sphere', 'nose')
head:add_child(nose)
nose:scale(0.02, 0.01, 0.01)
nose:translate(0.0, 0.03, 0.1)
nose:set_material(pink)

leftShoulder = gr.mesh('sphere', 'leftShoulder')
torso:add_child(leftShoulder)
leftShoulder:scale( 0.25, 0.25, 0.25 )
leftShoulder:scale(0.2, 0.2, 0.2)
leftShoulder:translate(-0.4, 0.35, 0.0)
leftShoulder:set_material(blue)

leftArm = gr.mesh('cube', 'leftArm')
torso:add_child(leftArm)
leftArm:scale( 0.25, 0.25, 0.25 )
leftArm:scale(0.4, 0.1, 0.1)
leftArm:rotate('z', 50);
leftArm:translate(-0.8, 0.0, 0.0)
leftArm:set_material(red)

rightShoulder = gr.mesh('sphere', 'rightShoulder')
torso:add_child(rightShoulder)
rightShoulder:scale( 0.25, 0.25, 0.25 )
rightShoulder:scale(0.2, 0.2, 0.2)
rightShoulder:translate(0.4, 0.35, 0.0)
rightShoulder:set_material(blue)

rightArm = gr.mesh('cube', 'rightArm')
torso:add_child(rightArm)
rightArm:scale( 0.25, 0.25, 0.25 )
rightArm:scale(0.4, 0.1, 0.1)
rightArm:rotate('z', -50);
rightArm:translate(0.8, 0.0, 0.0)
rightArm:set_material(red)

leftHip = gr.mesh('sphere', 'leftHip')
torso:add_child(leftHip)
leftHip:scale( 0.25, 0.25, 0.25 )
leftHip:scale(0.21, 0.21, 0.21)
leftHip:translate(-0.38, -0.5, 0.0)
leftHip:set_material(blue)

rightHip = gr.mesh('sphere', 'rightHip')
torso:add_child(rightHip)
rightHip:scale( 0.25, 0.25, 0.25 )
rightHip:scale(0.21, 0.21, 0.21)
rightHip:translate(0.38, -0.5, 0.0)
rightHip:set_material(blue)

leftLeg = gr.mesh('cube', 'leftLeg')
leftHip:add_child(leftLeg)
leftLeg:scale( 0.25, 0.25, 0.25 )
leftLeg:scale(0.5,4,0.5)
leftLeg:translate(0,-2.8,0)
leftLeg:set_material(red)

rightLeg = gr.mesh('cube', 'rightLeg')
rightHip:add_child(rightLeg)
rightLeg:scale( 0.25, 0.25, 0.25 )
rightLeg:scale(0.5,4,0.5)
rightLeg:translate(0,-2.8,0)
rightLeg:set_material(red)


return rootnode
