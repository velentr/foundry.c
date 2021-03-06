# List of modules that can be built into objects
modules = ['binheap', 'blkalloc', 'bresenham', 'fixpt', 'graph', 'htable',
           'kmp', 'list', 'pheap', 'rbtree', 'vector']

# Common CFLAGS to use for every build
cflags = '-std=c99 -pedantic -pipe -Wall -Wextra -Wno-unused-function -I. '
# CFLAGS to use for optimized builds
optflags = '-O3 -DNDEBUG -DNVALGRIND -march=native -flto'
# CFLAGS to use for debug builds
dbgflags = '-O0 -g -fsanitize=address -fsanitize=undefined -fsanitize=leak ' \
        '-fno-sanitize-recover'

# Create environments for different builds
opt_env = Environment(CCFLAGS = cflags + optflags)
dbg_env = Environment(CCFLAGS = cflags + dbgflags,
                      LINKFLAGS = '-fsanitize=address -fsanitize=undefined')

# Create optimized and debug objects for each module
for m in modules:
    opt_env.Object(m + '.c')
    dbg_env.Object(m + '.dbg.o', m + '.c')

# Add a test in directory 'tests/<t>' using modules 'mods'
test_progs = []
def add_test(t, mods):
    tests = Glob('tests/' + t + '/*.c')
    libs = []
    for m in mods:
        libs.append(m + '.dbg.o')
    for test in tests:
        test_progs.append(dbg_env.Program(str(test).replace('.c', '.test'),
                                  libs + [test]))

# Add all the tests in the 'tests' directory
add_test('binheap', ['binheap', 'vector'])
add_test('blkalloc', ['blkalloc', 'list'])
add_test('bresenham', ['bresenham'])
add_test('fixpt', ['fixpt'])
add_test('kmp', ['kmp'])
add_test('list', ['list'])
add_test('pheap', ['list', 'pheap'])
add_test('rbtree', ['rbtree'])
add_test('vector', ['vector'])

# Alias for running all tests with 'scons test'
dbg_env.AlwaysBuild(dbg_env.Alias('test', test_progs,
                                  'scripts/testbench.pl tests/*/*.test'))

AlwaysBuild(Alias('docs', [], 'sphinx-build -b html docs/ docs/_build/'))

