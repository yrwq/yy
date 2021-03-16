project('yy', 'c')

src = [
  'src/term/mode.c',
  'src/term/input.c',
  'src/main.c'
]

inc = include_directories('src')

# tree_sitter_inc = include_directories('tree_sitter/include')

executable(
  'yy',
  src,
  include_directories : [inc],
  # objects: ['tree_sitter/libtree-sitter.a']
)
