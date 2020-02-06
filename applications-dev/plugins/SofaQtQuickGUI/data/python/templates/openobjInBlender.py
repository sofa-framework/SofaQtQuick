import bpy
import sys

argv = sys.argv
argv = argv[argv.index("--") + 1:] # get all args after "--"

obj_in = argv[0]


bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete()

bpy.ops.import_mesh.obj(filepath=obj_in, axis_forward='-Z', axis_up='Y')
for area in bpy.context.screen.areas:
    if area.type == 'VIEW_3D':
        ctx = bpy.context.copy()
        ctx['area'] = area
        ctx['region'] = area.regions[-1]

bpy.ops.view3d.view_selected(ctx)
