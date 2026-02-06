project(
    'cpp-game',
    'cpp',
    default_options: [
        'cpp_std=c++23',
        'warning_level=3',
        'werror=false',
    ],
)

srcs = files(
    'src/main.cpp',
    'src/wire.cpp',
    'src/physics/jolt_physics.cpp',
)

executable(
    'cpp-game',
    srcs,
    include_directories: [
        include_directories('src'),
    ],
    dependencies: [
        dependency('raylib'),
        dependency('glfw3'),
        dependency('flecs'),
        dependency('joltphysics'),
    ],
    cpp_args: ['-march=native'],
    install: false
)
