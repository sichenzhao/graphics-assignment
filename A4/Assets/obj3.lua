-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

tm1 = gr.material({0.9, 0.8, 0.4}, {0.2, 0.2, 0.2}, 25)
tm2 = gr.material({0.9, 0.8, 0.4}, {0.4, 0.4, 0.4}, 25)
tm3 = gr.material({0.9, 0.8, 0.4}, {0.4, 0.4, 0.4}, 25)
tm4 = gr.material({0.9, 0.8, 0.4}, {0.4, 0.4, 0.4}, 25)

mirror = gr.material({0.0, 0.0, 0.0}, {0.9, 0.9, 0.9}, 0)

gold = gr.material({0.9, 0.8, 0.4}, {0.4, 0.4, 0.4}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(0, -2, -10)

m1 = gr.mesh('m1', 'plane.obj')
scene:add_child(m1)
m1:set_material(mirror)
m1:set_mirror()
m1:scale(10,2,30)
m1:rotate('X',90)
m1:rotate('Y',-30)
m1:translate(0,4,-8)

-- sphere
s1 = gr.sphere('s1')
scene:add_child(s1)
s1:tm_set_material(tm1, "grid.png")
s1:scale(1.5,1.5,1.5)
s1:rotate('Z', 180)
s1:translate(-2,4,-2)

-- the arc
arc = gr.node('arc')
scene:add_child(arc)
arc:translate(0,0,0)
arc:scale(0.5,0.5,0.5)

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
plane:scale(30, 30, 30)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, 20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'obj3.png', 1024, 1024, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})
