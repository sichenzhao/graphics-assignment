-- A simple test scene featuring some spherical cows grazing
-- around Stonehenge.  "Assume that cows are spheres..."

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

-- ##############################################
-- the scene
-- ##############################################

scene = gr.node('scene')

cube = gr.mesh('cube', 'cube.obj')
scene:add_child(cube)
cube:scale(100,100,100)
cube:translate(0,0,-400)
cube:set_material(grass)

-- the floor
gr.render(scene,
	  'sample.png', 256, 256,
	  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})
