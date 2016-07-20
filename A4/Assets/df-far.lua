-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

tm1 = gr.material({0.9, 0.8, 0.4}, {0.2, 0.2, 0.2}, 25)
tm2 = gr.material({0.9, 0.8, 0.4}, {0.4, 0.4, 0.4}, 25)
tm3 = gr.material({0.9, 0.8, 0.4}, {0.4, 0.4, 0.4}, 25)
tm4 = gr.material({0.9, 0.8, 0.4}, {0.4, 0.4, 0.4}, 25)

mirror = gr.material({0.0, 0.0, 0.0}, {0.9, 0.9, 0.9}, 0)

gold = gr.material({0.9*0.5, 0.8*0.5, 0.4*0.5}, {0.2, 0.2, 0.2}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
red = gr.material({1, 0.1, 0.1}, {0.8, 0.3, 0.3}, 25)

scene = gr.node('scene')
scene:rotate('X',5)

-- the arc
arc = gr.node('arc')
scene:add_child(arc)
arc:translate(0,0,0)
arc:scale(0.25,0.25,0.25)

arc1 = gr.node('arc')
arc1:add_child(arc)
scene:add_child(arc1)
arc1:translate(-1.5,0,3)

arc2 = gr.node('arc')
arc2:add_child(arc)
scene:add_child(arc2)
arc2:translate(1.5,0,-3)

p1 = gr.cube('p1')
arc:add_child(p1)
p1:set_material(gold)
p1:scale(0.8, 4, 0.8)
p1:translate(-2.4, 0, -0.4)

p2 = gr.cube('p2')
arc:add_child(p2)
p2:set_material(gold)
p2:scale(0.8, 4, 0.8)
p2:translate(1.6, 0, -0.4)

s = gr.sphere('s')
arc:add_child(s)
s:set_material(gold)
s:scale(4, 0.6, 0.6)
s:translate(0, 4, 0)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(5, 5, 5)

-- The lights
l1 = gr.light({0,0,10}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 0,10}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'df-far.png', 256, 256, 
	  {0, 0, 10}, {0, 0, -3}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})
