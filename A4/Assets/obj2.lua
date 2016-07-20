-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

tm1 = gr.material({0.9, 0.8, 0.4}, {0.2, 0.2, 0.2}, 25)
tm2 = gr.material({0.9, 0.8, 0.4}, {0.2, 0.2, 0.2}, 25)
tm3 = gr.material({0.9, 0.8, 0.4}, {0.2, 0.2, 0.2}, 25)
tm4 = gr.material({0.9, 0.8, 0.4}, {0.2, 0.2, 0.2}, 25)

grass = gr.material({80/255, 168/255, 227/255}, {0.05, 0.05, 0.2}}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)

scene = gr.node('scene')
scene:rotate('X', 43)
scene:translate(0, -2, -15)

-- sphere
s1 = gr.sphere('s1')
scene:add_child(s1)
s1:tm_set_material(tm1, "grid.png")
s1:scale(1.5,1.5,1.5)
s1:rotate('Z', 180)
s1:translate(-2,4,-2)

s2 = gr.sphere('s2')
scene:add_child(s2)
s2:tm_set_material(tm2, "canada.png")
s2:scale(1.5,1.5,1.5)
s2:rotate('Z', 180)
s2:rotate('X', -45)
s2:translate(2,4,-2)

s3 = gr.sphere('s3')
scene:add_child(s3)
s3:tm_set_material(tm3, "china.png")
s3:scale(1.5,1.5,1.5)
s3:rotate('Z', 180)
s3:rotate('Y', 90)
s3:translate(-2,4,4)

s4 = gr.sphere('s4')
scene:add_child(s4)
s4:tm_set_material(tm4, "earth.png")
s4:scale(1.5,1.5,1.5)
s4:rotate('Z', 180)
s4:translate(2,4,4)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, 20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'obj2.png', 1024, 1024, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})
