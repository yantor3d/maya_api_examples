import os
import shutil

project_dir = os.path.dirname(os.path.realpath(__file__))

plugin_dir = os.path.join(project_dir, 'plug-ins', 'windows-maya2016')

if not os.path.isdir(plugin_dir):
    os.makedirs(plugin_dir)
    
shutil.copy(
    os.path.join(project_dir, 'src', 'sphereDeformer.cl'),
    os.path.join(project_dir, 'plug-ins', 'windows-maya2016', 'sphereDeformer.cl')
)