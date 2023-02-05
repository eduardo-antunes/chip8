project('chip8', 'cpp')

inc_dir = include_directories('include')
sources = files(
  'src/main.cpp',
  'src/emulator.cpp',
  'src/screen.cpp',
  'src/io_handler.cpp',
)

sdl2_dep = dependency('sdl2')
executable('chip8', sources, 
  include_directories: inc_dir,
  dependencies: sdl2_dep,
)
