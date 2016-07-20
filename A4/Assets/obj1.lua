-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -15)

-- the primitives
ellipsoid = gr.ellipsoid('ellipsoid',3,1,2)
scene:add_child(ellipsoid)
ellipsoid:set_material(gold)
ellipsoid:translate(-12,2,-10)

elli2 = gr.ellipsoid('elli2',3,1,2)
scene:add_child(elli2)
elli2:set_material(gold)
elli2:rotate('X', 90)
elli2:translate(-5,3,-10)

elli2 = gr.ellipsoid('elli2',3,1,2)
scene:add_child(elli2)
elli2:set_material(gold)
elli2:rotate('Y', 90)
elli2:translate(1,2,-10)

econe = gr.econe('econe',2,3,4,2)
scene:add_child(econe)
econe:set_material(gold)
econe:rotate('X', 90)
econe:translate(-10,2,2)

ec2 = gr.econe('ec2',2,3,4,2)
scene:add_child(ec2)
ec2:set_material(gold)
ec2:translate(-7,3,2)
ec2:rotate('X', 13)

ec3 = gr.econe('ec3',2,3,4,2)
scene:add_child(ec3)
ec3:set_material(gold)
ec3:rotate('Y', 90)
ec3:translate(-3,2,2)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'obj1.png', 2560, 2560, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})
