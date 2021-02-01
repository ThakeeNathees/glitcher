import os, sys
import json

CONFIG_FILE = 'config.json'

data = {
	'godot-path': '',
}

def main():
	ec = check_submodule_init()
	ec = check_native_bindings()
	log('all checks done')


def log(*msg):
	print("[configure.py]", end='')
	for _msg in msg:
		print(' ' + _msg, end='')
	print()

def write_config():
	with open(CONFIG_FILE, 'w') as f:
		f.write(json.dumps(data, indent=4))
		
def read_config():
	with open(CONFIG_FILE, 'r') as f:
		data = json.loads(f.read())
		print(json.dumps(data, indent=4))

def check_submodule_init():
	log("checking submodules")
	if len(os.listdir('src/godot-cpp/')) == 0 or \
		len(os.listdir('src/godot-cpp/godot_headers')) == 0:
		log("updating missing submodules")
		ec = os.system('git submodule update --init --recursive')
		if ec == 0:
			log('submodules updated successfully')
		return ec
	log("submodules found")
	return 0

def check_native_bindings():
	log("checking native bindings")
	gen_bindings = len(os.listdir('src/godot-cpp/src/gen/')) == 1
	compile = not os.path.exists('src/godot-cpp/bin/')
	if gen_bindings or compile:
		log("building native bindings")
		
		platform = ''
		jobs = '-j6'
		bits = None
		
		if sys.platform == 'win32':
			platform = 'windows'
			bits = ('32', '64')
		elif sys.platform in ('x11', 'linux', 'linux2'):
			platform = 'linux'
			bits = '64'
		else:
			log("Error: osx and other platforms not implemented (TODO)")
			return 1
			
		os.chdir('src/godot-cpp/')
		gen_bindings = 'yes' if gen_bindings else 'no'
		for _bits in bits:
			ec = os.system('scons platform=%s bits=%s generate_bindings=%s %s' % (
				platform, _bits, gen_bindings, jobs))
			if ec != 0: break
		os.chdir('../../')
		
		if ec == 0:
			log('native bindings compiled successfully')
		return ec
		
	log("native bindings found")
	return 0

if __name__ == '__main__':
	main()