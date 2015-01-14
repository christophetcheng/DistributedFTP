import os


env = Environment(ENV = os.environ, CXX = 'ccache g++', CC='ccache gcc')

env.Append(CXXFLAGS = '-std=c++0x -g')

#env.Append(CXXFLAGS = '-O2')


com = env.Library('bin/com',[ Glob('src/ext/*.c'), Glob('src/ext/*.c'),Glob('src/com/*.cpp') ] )

libs = ['boost_thread', 'boost_regex', 'boost_system', com]

ftpmag = env.Program(target  = 'bin/ftpmag.exe',
			source  = [Glob('src/ftpmag/*.cpp')],
			CPPPATH = ['src/ext','.','src/com'],
			LIBS    = libs,
			LIBPATH = '.'
			)
			
ftpagt = env.Program(target  = 'bin/ftpagt.exe',
			source  = [Glob('src/ftpagt/*.cpp')],
			CPPPATH = ['src/ext','.','src/com'],
			LIBS    = libs,
			LIBPATH = '.'
			)
unittest = 	env.Program(target  = 'bin/unittest.exe',
			source  = [Glob('src/unittest/*.cpp')],
			CPPPATH = ['src/ext','.','src/com'],
			LIBS    = libs,
			LIBPATH = '.'
			)		

all = [ ftpmag, ftpagt, unittest ]
			
