-- final scene of project

earth = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)

stone = gr.material({64/255*0.5, 120/255*0.5, 211/255*0.5}, {0.3, 0.3, 0.3}, 0)
grass = gr.material({198/255, 204/255, 204/255}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

egg = gr.material({249/255, 208/255, 16/255}, {0.3, 0.3, 0.3}, 20)

red = gr.material({237/255, 2/255, 11/255},{0,0,0},20)
yellow = gr.material({255/255, 202/255, 43/255},{0,0,0},20)
green = gr.material({204/255, 255/255, 0/255},{0,0,0},20)

transp = gr.material({0, 0, 0}, {0.9, 0.9, 0.9}, 0)
mirror = gr.material({0.0, 0.0, 0.0}, {0.9, 0.9, 0.9}, 0)

earthR = 150+2*5

wall_set = gr.node('wall_set')
wall_set:translate(0,0,-13)

-- square window set
w_set = gr.node('wset')
w_set:rotate('X', 30)
w_set:translate(-1.5,3,0)

window = gr.nh_box('window', {0,0,0}, 1)
w_set:add_child(window)
window:set_material(transp)
window:set_transparent(1.5)
window:scale(3,3,1/100)
window:translate(0,0,0)

w1 = gr.nh_box('w1', {0,0,0}, 1)
w_set:add_child(w1)
w1:set_material(grass)
w1:scale(3,1,1/100)
w1:translate(0,-1,0)

w2 = gr.nh_box('w2', {0,0,0}, 1)
w_set:add_child(w2)
w2:set_material(grass)
w2:scale(3,1,1/100)
w2:translate(0,3,0)

w3 = gr.nh_box('w3', {0,0,0}, 1)
w_set:add_child(w3)
w3:set_material(grass)
w3:scale(3,1/100,40)
w3:translate(0,4,0)

w4 = gr.mesh('w4', 'wall.obj')
w_set:add_child(w4)
w4:set_material(grass)
w4:translate(0,-0.7,0)

-- sphere in the middles
s_set = gr.node('sphere_set')
s_set:translate(-1.0,1.5,0)

s1 = gr.nh_sphere('s1', {0,0,0}, 0.1)
s_set:add_child(s1)
s1:set_material(red)
s1:translate(0.0,0.0,-6.5)

s2 = gr.nh_sphere('s2', {0,0,0}, 0.1)
s_set:add_child(s2)
s2:set_material(yellow)
s2:translate(0.0,0.0,-5.5)

s3 = gr.nh_sphere('s3', {0,0,0}, 0.1)
s_set:add_child(s3)
s3:set_material(green)
s3:translate(0.0,0.0,-4.5)

-- dodeca
dodec = gr.mesh('dodec', 'dodeca.obj')
dodec:set_material(stone)
dodec:scale(1.37,1.37,1.37)
dodec:translate(0,0,2)

-- 2 primitives
ellipsoid = gr.ellipsoid('ellipsoid',3*0.5,1*0.5,2*0.5)
ellipsoid:set_material(egg)
ellipsoid:scale(0.5,1,1)
ellipsoid:translate(-1,-1.5,-5)

econe = gr.econe('econe',2*0.5,3*0.5,4*0.5,0.5)
econe:set_material(yellow)
econe:rotate('X', 90)
econe:rotate('Z', 20)
econe:scale(0.4,0.4,0.4)
econe:translate(0.7,0.6,-3)

-- mirror
m1 = gr.mesh('m1', 'plane.obj')
m1:set_material(mirror)
m1:set_mirror()
m1:scale(0.5,0.5,0.5)
m1:rotate('X',60)
m1:rotate('Y',-30)
m1:translate(1.5,-1.5,-8)

-- ##############################################
-- the scene
-- ##############################################

scene = gr.node('scene')
--scene:rotate('X', 23)
--scene:translate(0, -2, -15)

--scene:add_child(w_set)
--scene:add_child(s1)
scene:add_child(wall_set)
wall_set:add_child(dodec)

scene:add_child(ellipsoid)
scene:add_child(econe)

scene:add_child(s_set)

scene:add_child(m1)

for i = 1, 6 do
    a_w_set = gr.node('w_set' .. tostring(i))
    a_w_set:rotate('Z', (i-1) * 60)
    wall_set:add_child(a_w_set)
    a_w_set:add_child(w_set)
end

--scene:add_child(window)
--scene:add_child(w1)
--scene:add_child(w2)

selfR = 90
--earthR = 5
-- THE EARTH
e = gr.sphere('e')
scene:add_child(e)
e:tm_set_material(earth, "real_earth.png")
e:scale(earthR,earthR,earthR)
e:rotate('Z', 180)
e:rotate('Y',selfR)
e:translate(-earthR/1.414,-earthR/1.414,-1.5*earthR)

sun = gr.light({3000, 3000, 10000}, {1, 1, 1}, {1, 0, 0});
l = gr.light({0.5, 0.5, -5}, {1,1,1}, {1, 0, 0})

gr.render(scene,
	  'final_scene_r2.png', 256, 256,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {sun, l})
