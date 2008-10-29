from distutils.core import setup
import py2exe

setup(console=[{'script':'WorldsServer.py', 'icon_resources':[(1, 'WorldsServer.ico')]}])