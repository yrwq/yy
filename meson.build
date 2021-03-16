project('yy', 'c')

src = [
  'src/editor.c',
  'src/term/mode.c',
  'src/term/input.c',
  'src/main.c'
]

inc = include_directories('src')

executable(
  'yy',
  src,
  include_directories : [inc],
)
