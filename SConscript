Import('env')
import os
	
env.PROJECT_NAME = "MiniScript"

def target_path(name):
	return os.path.join('bin/', name)
	
## ide debug run target
env.RUN_TARGET = 'Godot 3.2.2.exe'
env.RUN_ARGS = '--path MiniScript'

## include directories
env.Append(CPPPATH=[
	Dir('#src/godot-cpp/godot_headers/'),
	Dir('#src/godot-cpp/include/'),
	Dir('#src/godot-cpp/include/core/'),
	Dir('#src/godot-cpp/include/gen/'),
])

env.Append(LIBPATH="#src/godot-cpp/bin")
env.Append(LIBS=['libgodot-cpp.{}.{}.{}{}'.format(
	env['platform'],
	env['target'],
	env['bits'], ## TODO: -> 32, 64, default,... "arch_suffix",
	env['LIBSUFFIX'])
])

## lib sources
env.SOURCES = [
	Glob("src/*.cpp"),
]

## only for ide
env.ALL_SOURCES = []
env.ALL_SOURCES += env.SOURCES

library = env.SharedLibrary(target="bin/" + 'libgdexample', source=env.SOURCES)

