Import('env')
import os

## only for ide
env.PROJECT_NAME = "Glitcher"

## ide debug run target
env.RUN_TARGET = 'Godot 3.2.2.exe --path MiniScript'
if env['platform'] == 'windows':
	env.RUN_TARGET = 'run.bat'
	
## compile the miniscript vm first
##SConscript('src/miniscript/SConscript')

def target_path(name):
	return os.path.join('bin/', name)

## include directories
CPPPATH = [
	Dir('#src/godot-cpp/godot_headers/'),
	Dir('#src/godot-cpp/include/'),
	Dir('#src/godot-cpp/include/core/'),
	Dir('#src/godot-cpp/include/gen/'),
]

LIBPATH = [ 
	Dir("#src/godot-cpp/bin/"),
	Dir("src/miniscript/bin/"),
]
LIBS = [
	'libgodot-cpp.{}.{}.{}{}'.format(
	env['platform'],
	env['target'],
	env['bits'], ## TODO: -> 32, 64, default,... "arch_suffix",
	env['LIBSUFFIX']),

	#'miniscript',
]

## lib sources
SOURCES = [
	Glob("src/test/*.cpp"),
]

library = env.SharedLibrary(
	target  = "bin/testlib",
	source  = SOURCES,
	CPPPATH = CPPPATH,
	LIBPATH = LIBPATH,
	LIBS    = LIBS,
)

## copy compiled binar to gdnative
#from shutil import copyfile
#if env['platform'] == 'windows':
#	copyfile("bin/testlib.dll",
#		str(File("#MiniScript/test/bin/testlib.dll")))
	
	