-- A simple scene with five spheres

red = gr.material({1,0,0}, {0,0,0}, 25)
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
grass = gr.material({1, 1, 1}, {0.0, 0.0, 0.0}, 0)
--transp = gr.material({0, 0, 0}, {0.5, 0.5, 0.5}, 0)
transp = gr.material({0, 0, 0}, {0.9, 0.9, 0.9}, 0)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, -400}, 25)
--scene_root:add_child(s1)
s1:set_material(red)

b1 = gr.nh_box('b1', {-5000, -5000, 0}, 5000)
scene_root:add_child(b1)
b1:set_material(mat1)
--b1:set_transparent(1.5)

s2 = gr.nh_sphere('s2', {0,0,0}, 50)
--scene_root:add_child(s2)
s2:set_material(transp)
s2:set_transparent(1.5)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.8, 0.8, 0.8}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'table.png', 256, 256,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
