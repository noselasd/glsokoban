env = Environment()
env.SourceSignatures("timestamp")
#env['CC'] = 'gcc4'
#env['CC'] = '/opt/intel_cc_80/bin/icc'
sources = [
'camera.c',
'frame.c',
'init.c',
'input.c',
'main.c',
'material.c',
'maincamera.c',
'levels.c',
'textures.c',
'quat.c',
'sky.c',
'timer.c',
'vector.c',
'ftfont.c'
]
#env['CCFLAGS'] = ['-g','-Wbrief']
#env['LINKFLAGS'] += '-i-static'
env['CCFLAGS'] = ['-Wall', '-Wundef','-Wendif-labels','-Wcast-align', '-Wwrite-strings', '-Wstrict-prototypes'
,'-Wmissing-prototypes', '-Wnested-externs', '-Winline', '-Wdisabled-optimization','-W','-Wno-unused','-Wshadow','-Wpointer-arith','-W','-g','-O2','-std=gnu89']
env['CPPPATH'] = ['/usr/pkg/include','/usr/X11R6/include']
env['LIBS'] = ['GL','GLU','m','SDL_image']

fonttool = env.Clone()
fonttool.ParseConfig('freetype-config --libs --cflags')

env.ParseConfig ('sdl-config --libs --cflags')
env.Program('glsokoban',sources)

fonttool.Program('fonttool',['fonttool.c'])
fnr=env.Command('LucidaSR.font', 'LucidaSansRegular.ttf', './fonttool $SOURCE $TARGET 28')
env.Depends(fnr,'fonttool')

