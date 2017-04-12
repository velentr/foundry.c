# List of modules that can be built into objects
modules = ['binheap', 'bresenham', 'htable', 'kmp', 'list', 'pheap', 'rbtree',
           'vector']

# Common CFLAGS to use for every build
cflags = '-ansi -pedantic -pipe -Wall -Wextra -Werror -Wno-unused-function '
# CFLAGS to use for optimized release builds
relflags = '-O3 -DNDEBUG -march=native -flto '
# CFLAGS to use for debug builds
dbgflags = '-O0 -g '

# Create environments for different builds
rel_env = Environment(CC = 'gcc', CCFLAGS = cflags + relflags)
dbg_env = Environment(CC = 'gcc', CCFLAGS = cflags + dbgflags)

# Create release and debug objects for each module
for m in modules:
    rel_env.Object(m + '.c')
    dbg_env.Object(m + '.dbg.o', m + '.c')

# Add a test in directory 'tests/<t>' using modules 'mods'
def add_test(t, mods):
    tests = Glob('tests/' + t + '/*.c')
    libs = []
    for m in mods:
        libs.append(m + '.dbg.o')
    for test in tests:
        Program(str(test).replace('.c', '.test'), libs + [test],
                CCFLAGS = dbgflags + '-I.')

# Add all the tests in the 'tests' directory
add_test('binheap', ['binheap', 'vector'])
add_test('bresenham', ['bresenham'])
add_test('kmp', ['kmp'])
add_test('list', ['list'])
add_test('pheap', ['list', 'pheap'])
add_test('rbtree', ['rbtree'])

