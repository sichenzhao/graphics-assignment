-- A simple scene with five spheres

red = gr.material({1,0,0}, {0,0,0}, 25)
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
grass = gr.material({1, 1, 1}, {0.0, 0.0, 0.0}, 0)
--transp = gr.material({0, 0, 0}, {0.5, 0.5, 0.5}, 0)
transp = gr.material({0, 0, 0}, {0.9, 0.9, 0.9}, 0)

scene_root = gr.node('root')
scene_root:translate(0,0,-1000)

s1 = gr.nh_sphere('s1', {0, 0, -400}, 25)
scene_root:add_child(s1)
s1:set_material(red)
s1:translate(0,0,1000)

bsize = 200
b1 = gr.nh_box('b1', {-bsize/2, -bsize/2, 0}, bsize)
scene_root:add_child(b1)
b1:set_material(transp)
b1:set_transparent(1.5)
b1:rotate('Y', 5)
b1:translate(0,0,1000)

s2 = gr.nh_sphere('s2', {0,0,0}, 50)
--scene_root:add_child(s2)
s2:set_material(transp)
s2:set_transparent(1.5)
s2:translate(0,0,1000)

plane = gr.mesh( 'plane', 'bplane.obj' )
scene_root:add_child(plane)
plane:set_material(mat2)
plane:scale(1, 1, 1)
plane:translate(0,-200,1000 - 500)
plane:rotate('X', 90)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.8, 0.8, 0.8}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'obj4-cube-move.png', 256, 256,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
