-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.0, 0.0, 0.0}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.0, 0.0, 0.0}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.0, 0.0, 0.0}, 25)
grass = gr.material({1, 1, 1}, {0.0, 0.0, 0.0}, 0)
mirror = gr.material({0.1, 0.1, 0.1}, {0.9, 0.9, 0.9}, 0)

scene_root = gr.node('root')

mirror_set = gr.node('mirrorSet')
scene_root:add_child(mirror_set)
mirror_set:translate(0,0,-400)

m1 = gr.mesh('m1', 'plane.obj')
mirror_set:add_child(m1)
m1:set_material(mirror)
m1:scale(600,600,600)
m1:rotate('X',90)

m2 = gr.mesh('m2', 'plane.obj')
mirror_set:add_child(m2)
m2:set_material(mirror)
m2:scale(600,600,600)
m2:translate(0,-600,60)

m5 = gr.mesh('m5', 'plane.obj')
mirror_set:add_child(m5)
m5:set_material(mirror)
m5:scale(600,600,600)
m5:translate(0,600,60)

m3 = gr.mesh('m3', 'plane.obj')
mirror_set:add_child(m3)
m3:set_material(mirror)
m3:scale(600,600,600)
m3:rotate('Z',90)
m3:translate(-600,0,60)

m4 = gr.mesh('m4', 'plane.obj')
mirror_set:add_child(m4)
m4:set_material(mirror)
m4:scale(600,600,600)
m4:rotate('Z',90)
m4:translate(600,0,60)

s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
scene_root:add_child(s1)
s1:set_material(mat1)
s1:translate(0,0,200)

s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
--scene_root:add_child(s2)
s2:set_material(mat1)
s2:translate(0,0,200)

s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
scene_root:add_child(s4)
s4:set_material(mat3)
s4:translate(0,0,200)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
scene_root:add_child(s5)
s5:set_material(mat1)
s5:translate(0,0,200)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.6, 0.6, 0.6}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'mirror.png', 256, 256,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 120,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
