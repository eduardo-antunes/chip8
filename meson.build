project('chip8', 'cpp',
  default_options: ['cpp_std=c++17']
)

inc_dir = include_directories('include')
sources = files(
  'src/main.cpp',
  'src/emulator.cpp',
  'src/screen.cpp',
  'src/keypad.cpp',
  'src/speakers.cpp',
)

sdl2_dep = dependency('sdl2')
executable('chip8-emulator', sources, 
  include_directories: inc_dir,
  dependencies: sdl2_dep,
)
