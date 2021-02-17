import os, sys
import json

## To sync submodule:
## git submodule update --remote -- src/miniscript
## Last stable godot-cpp commit: eafe6d96226da5ebf02ec35ca1599a45dd794cfc

CONFIG_FILE = 'config.json'

data = {
	'godot-path': '%USERPROFILE%/Desktop/Godot 3.2.2.exe',
}

def main():
	ec = check_submodule_init()
	ec = check_native_bindings()
	ec = update_subdirs()
	ec = check_config_file()
	ec = update_config()
	log('all checks done')


def log(*msg):
	print("[configure.py]", end='')
	for _msg in msg:
		print(' ' + _msg, end='')
	print()

def update_subdirs():
	''' generate intermediate sub directories if not exists '''
	def mkdirs_if_not_exist(path):
		if not os.path.exists(path):
			os.makedirs(path)

def update_config():
	log("updating configuration")
	if sys.platform == 'win32':
		with open('run.bat', 'w') as f:
			f.write('@echo off\n"%s" --path "%s"' % (data['godot-path'],
				os.path.join(os.getcwd(), 'Glitcher/')))


	log("configuration updated")
	return 0

def write_config():
	global data
	with open(CONFIG_FILE, 'w') as f:
		f.write(json.dumps(data, indent=4))
		
def read_config():
	global data
	with open(CONFIG_FILE, 'r') as f:
		data = json.loads(f.read())
		#print(json.dumps(data, indent=4))

def check_config_file():
	log("checking", CONFIG_FILE)
	if os.path.exists(CONFIG_FILE):
		read_config()
		## TODO: validate config file
	else:
		log("generating", CONFIG_FILE)
		write_config()
	return 0
	

def check_submodule_init():
	log("checking submodules")
	if len(os.listdir('src/godot-cpp/')) == 0 or \
		len(os.listdir('src/godot-cpp/godot_headers')) == 0:
		log("updating missing submodules")
		ec = os.system('git submodule update --init --recursive')
		if ec == 0:
			log('submodules updated successfully')
		return ec
	## TODO: update miniscript.
	log("submodules found")
	return 0

def check_native_bindings():
	log("checking native bindings")
	platform = ''
	jobs = '-j6'
	bits = None
	
	if sys.platform == 'win32':
		platform = 'windows'
		bits = ('64',) ## ('32', '64') ask interactivly
	elif sys.platform in ('x11', 'linux', 'linux2'):
		platform = 'linux'
		bits = ('64',)
	else:
		log("Error: osx and other platforms not implemented (TODO)")
		return 1

	gen_bindings = len(os.listdir('src/godot-cpp/src/gen/')) == 1
	compile = not os.path.exists('src/godot-cpp/bin/')
	if gen_bindings or compile:
		log("building native bindings")
		os.chdir('src/godot-cpp/')
		gen_bindings = 'yes' if gen_bindings else 'no'
		for _bits in bits:
			ec = os.system('scons platform=%s bits=%s generate_bindings=%s %s' % (
				platform, _bits, gen_bindings, jobs))
			if ec != 0: break
		if ec == 0:
			log('native bindings compiled successfully')
		os.chdir('../../')

	## Compile miniscript. (Fixme: True because it's constantly changing).
	if True:
		log("building miniscript language")
		sys.path.insert(1, 'src/miniscript/')
		import ms_configure
		os.chdir('src/miniscript/')
		ms_configure.main()
		for _bits in bits:
			ec = os.system('scons bits=%s %s' % (_bits, jobs))
			if ec != 0: break
		if ec == 0:
			log('miniscript building completed successfully')
		os.chdir('../../')

	log("done building native libs")
	return ec


if __name__ == '__main__':
	main()