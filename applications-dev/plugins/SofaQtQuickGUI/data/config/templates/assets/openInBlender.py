import bpy
import sys

argv = sys.argv
argv = argv[argv.index("--") + 1:] # get all args after "--"


file_type = argv[0]
mesh = argv[1]


bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete()

print(file_type)
if file_type == "obj":
    bpy.ops.import_mesh.obj(filepath=mesh, axis_forward='-Z', axis_up='Y')
elif file_type == "stl":
    bpy.ops.import_mesh.stl(filepath=mesh, axis_forward='-Z', axis_up='Y')
else:
    print("ERROR: Cannot load asset in blender: could not find"
          "a loader in blender for this file type")
for area in bpy.context.screen.areas:
    if area.type == 'VIEW_3D':
        ctx = bpy.context.copy()
        ctx['area'] = area
        ctx['region'] = area.regions[-1]

bpy.ops.view3d.view_selected(ctx)
