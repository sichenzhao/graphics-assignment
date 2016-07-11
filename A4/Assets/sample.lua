-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
grass = gr.material({1, 1, 1}, {0.0, 0.0, 0.0}, 0)

scene_root = gr.node('root')
scene_root:translate(0,0,-1000)

-- A small stellated dodecahedron.

plane = gr.mesh( 'plane', 'cow.obj' )
scene_root:add_child(plane)
plane:set_material(grass)
plane:scale(1, 1, 1)
plane:translate(0,0,1000)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'sample.png', 256, 256,
	  {0, 0, 8}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
