import sys, os, subprocess

if os.uname()[0].find('Darwin') >= 0:
	sys.path.append('/Users/max/Dropbox/bin')
	# libcmd1 = 'g++ -c helper_library.cpp'
	libcmd1 = 'g++ -fPIC -g -c helper_library.cpp'
	libcmd2 = 'gcc -dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,libnumerica.dylib -o libnumerica.dylib helper_library.o'
	# libcmd2 = 'ar -cvq libnumerica.a helper_library.o'
	lncmd1 = 'echo just'
	lncmd2 = 'echo relax'
else:
	sys.path.append("/home/riethmann/Dropbox/bin")
	libcmd1 = 'g++ -fPIC -g -c helper_library.cpp'
	libcmd2 = 'g++ -shared -Wl,-soname,libnumerica.so.1 -o libnumerica.so.1.0 helper_library.o -lc'
	lncmd1 = 'ln -sf libnumerica.so.1.0 libnumerica.so'
	lncmd2 = 'ln -sf libnumerica.so.1.0 libnumerica.so.1'

import builder as b


# lib = b.Target('helper_library.so.1.0')

run = b.Target('go')
file1=b.Component('parser.y','bison -d -o parser.cpp -v -rsolved','parser.cpp')
run.add(file1)
run.add(b.Component('lexer.l','flex -o tokens.cpp','tokens.cpp'))
run.add(b.Component('types.cpp','g++ -g `llvm-config --cxxflags` -c'))
run.add(b.Component('main.cpp', 'g++ -g `llvm-config --cxxflags` -c'))
run.compiler = 'g++'
run.compiler_options = '-g `llvm-config --cxxflags --ldflags --libs core jit nativecodegen` -ldl -L. -lnumerica'

os.system(libcmd1)
os.system(libcmd2)
os.system(lncmd1)
os.system(lncmd2)

sys.exit(run.build())
