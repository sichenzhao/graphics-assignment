-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

rootnode = gr.node('root')
--rootnode:rotate('y', -90.0)
rootnode:translate(0.0, 0.1, -0.8)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
light_blue = gr.material({0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
pink = gr.material({255/255,192/255,203/255}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('sphere', 'torso')
rootnode:add_child(torso)
torso:set_material(pink)
torso:scale( 0.25*0.7*0.92, 0.25*0.7, 0.25*0.7 )
torso:translate(0.0, -0.2, 0.0);

jtll = gr.joint('jtll', {-90, -90, 60}, {0, 0, 0})
torso:add_child(jtll)
jtll:translate(-0.25*0.7*0.4, -0.1*1.1, 0.0)

leftLeg = gr.mesh('cube', 'leftLeg')
jtll:add_child(leftLeg)
leftLeg:scale(0.1, 0.1, 0.1)
leftLeg:scale(0.7, 1.6, 0.7)
leftLeg:translate(0.0, -0.08, 0.0)
leftLeg:set_material(light_blue)

jtrl = gr.joint('jtrl', {-90, -90, 60}, {0, 0, 0})
torso:add_child(jtrl)
jtrl:translate(0.25*0.7*0.4, -0.1*1.1, 0.0)

rightLeg = gr.mesh('cube', 'rightLeg')
jtrl:add_child(rightLeg)
rightLeg:scale(0.1, 0.1, 0.1)
rightLeg:scale(0.7, 1.6, 0.7)
rightLeg:translate(0.0, -0.08, 0.0)
rightLeg:set_material(light_blue)

jtla = gr.joint('jla', {-120, 20, 120}, {-90, 90, 90})
torso:add_child(jtla)
jtla:translate(-0.25*0.7*0.8, 0.1, 0.0)

leftArm = gr.mesh('sphere', 'leftArm')
jtla:add_child(leftArm)
leftArm:scale( 0.25, 0.25, 0.25 )
leftArm:scale(0.4, 0.1, 0.1)
leftArm:translate(-0.4*0.25*0.7, 0.0, 0.0)
leftArm:set_material(red)

jlla = gr.joint('jlla', {-5, 0, 5}, {-45, 0, 45})
leftArm:add_child(jlla)
jlla:translate(-0.4*0.25*0.7*1.2, 0, 0)

leftHand = gr.mesh('sphere', 'leftHand')
jlla:add_child(leftHand)
leftHand:scale( 0.25, 0.25, 0.25 )
leftHand:scale( 0.25, 0.25, 0.25 )
leftHand:scale( 0.4, 0.4, 0.4 )
leftHand:scale( 0.9, 0.9, 0.4 )
leftHand:translate( -0.035, 0,0)
leftHand:set_material(red)

leftPalm = gr.mesh('sphere', 'leftPalm')
leftHand:add_child(leftPalm)
leftPalm:scale( 0.25, 0.25, 0.25 )
leftPalm:scale( 0.25, 0.25, 0.25 )
leftPalm:scale( 0.4, 0.4, 0.4 )
leftPalm:scale( 0.9, 0.9, 0.4 )
leftPalm:scale( 0.9, 0.9, 1 )
leftPalm:scale( 0.9, 0.9, 1 )
leftPalm:scale( 0.9, 0.9, 1 )
leftPalm:scale( 0.9, 0.9, 1 )
leftPalm:translate( 0,0,0.004)
leftPalm:set_material(pink)

jlf1 = gr.joint('jlf1', {0, 0, 0}, {-90, 0, 90})
leftHand:add_child(jlf1)
jlf1:translate(-0.02, 0,0)

lf1 = gr.mesh('cube', 'lf1')
jlf1:add_child(lf1)
lf1:scale(0.01, 0.005,0.001)
lf1:translate(-0.007, 0, 0.0004)
lf1:set_material(white)

jlf2 = gr.joint('jlf2', {0, 0, 0}, {-90, 0, 90})
leftHand:add_child(jlf2)
jlf2:translate(-0.02, 0,0)
jlf2:rotate('z', 30)

lf2 = gr.mesh('cube', 'lf2')
jlf2:add_child(lf2)
lf2:scale(0.01, 0.005,0.001)
lf2:translate(-0.007, 0, 0.0004)
lf2:set_material(white)

jlf3 = gr.joint('jlf3', {0, 0, 0}, {-90, 0, 90})
leftHand:add_child(jlf3)
jlf3:translate(-0.02, 0,0)
jlf3:rotate('z', -30)

lf3 = gr.mesh('cube', 'lf3')
jlf3:add_child(lf3)
lf3:scale(0.01, 0.005,0.001)
lf3:translate(-0.007, 0, 0.0004)
lf3:set_material(white)

jtra = gr.joint('jra', {-120, -90, 120}, {-90, -90, 70})
torso:add_child(jtra)
jtra:translate(0.25*0.7*0.8, 0.1, 0.0)

rightArm = gr.mesh('sphere', 'rightArm')
jtra:add_child(rightArm)
rightArm:scale( 0.25, 0.25, 0.25 )
rightArm:scale(0.4, 0.1, 0.1)
rightArm:translate(0.4*0.25*0.7, 0.0, 0.0)
rightArm:set_material(red)

jrla = gr.joint('jrla', {-5, 0, 5}, {-45, 0, 45})
rightArm:add_child(jrla)
jrla:translate(0.4*0.25*0.7*1.2, 0, 0)

rightHand = gr.mesh('sphere', 'rightHand')
jrla:add_child(rightHand)
rightHand:scale( 0.25, 0.25, 0.25 )
rightHand:scale( 0.25, 0.25, 0.25 )
rightHand:scale( 0.4, 0.4, 0.4 )
rightHand:scale( 0.9, 0.9, 0.4 )
rightHand:translate( 0.035, 0,0)
rightHand:set_material(red)

rightPalm = gr.mesh('sphere', 'rightPalm')
rightHand:add_child(rightPalm)
rightPalm:scale( 0.25, 0.25, 0.25 )
rightPalm:scale( 0.25, 0.25, 0.25 )
rightPalm:scale( 0.4, 0.4, 0.4 )
rightPalm:scale( 0.9, 0.9, 0.4 )
rightPalm:scale( 0.9, 0.9, 1 )
rightPalm:scale( 0.9, 0.9, 1 )
rightPalm:scale( 0.9, 0.9, 1 )
rightPalm:scale( 0.9, 0.9, 1 )
rightPalm:translate( 0,0,0.004)
rightPalm:set_material(pink)

jrf1 = gr.joint('jrf1', {0, 0, 0}, {-90, 0, 90})
rightHand:add_child(jrf1)
jrf1:translate(0.02, 0,0)

rf1 = gr.mesh('cube', 'rf1')
jrf1:add_child(rf1)
rf1:scale(0.01, 0.005,0.001)
rf1:translate(0.007, 0, 0.0004)
rf1:set_material(white)

jrf2 = gr.joint('jrf2', {0, 0, 0}, {-90, 0, 90})
rightHand:add_child(jrf2)
jrf2:translate(0.02, 0,0)
jrf2:rotate('z', 30)

rf2 = gr.mesh('cube', 'rf2')
jrf2:add_child(rf2)
rf2:scale(0.01, 0.005,0.001)
rf2:translate(0.007, 0, 0.0004)
rf2:set_material(white)

jrf3 = gr.joint('jrf3', {0, 0, 0}, {-90, 0, 90})
rightHand:add_child(jrf3)
jrf3:translate(0.02, 0,0)
jrf3:rotate('z', -30)

rf3 = gr.mesh('cube', 'rf3')
jrf3:add_child(rf3)
rf3:scale(0.01, 0.005,0.001)
rf3:translate(0.007, 0, 0.0004)
rf3:set_material(white)

-- joint torso neck
jtn = gr.joint('jtn', {-15, 15, 15}, {-15, 0, 15})
torso:add_child(jtn)
jtn:translate(0.0, 0.17, 0.0)

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
leftEye:set_material(black)

rightEye = gr.mesh('cube', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.02, 0.01, 0.01)
rightEye:translate(0.02, 0.06, 0.077)
rightEye:set_material(black)

nose = gr.mesh('sphere', 'nose')
head:add_child(nose)
nose:scale(0.02, 0.01, 0.01)
nose:translate(0.0, 0.03, 0.1)
nose:set_material(pink)

return rootnode
