glide1x_res = wrc_generator.process('version1x.rc')
glide2x_res = wrc_generator.process('version2x.rc')
glide3x_res = wrc_generator.process('version3x.rc')

glide_shaders = files([
  'shaders/glide_fixed_function_vert.vert',
  'shaders/glide_fixed_function_frag.frag',
])

glide_common = [
  'glide_device.cpp',
  'glide_multithread.cpp',
  'glide_options.cpp',
  'glide_shaders.cpp',
  'glide_swapchain.cpp',
  'glide_texture.cpp',
  'glide_tmu.cpp',
  'glide_vertex_layout.cpp',
  'glide_utils.cpp',
]

glide1x_src = [
  'glide1x_main.cpp',
]

glide2x_src = [
  'glide2x_main.cpp',
]

glide3x_src = [
  'glide3x_main.cpp',
]

glide_common_lib = static_library(dxvk_name_prefix+'glidecommon', glide_common, glsl_generator.process(glide_shaders),
  dependencies        : [ dxbc_spirv_dep, dxvk_dep ],
  include_directories : [ dxvk_include_path ],
  kwargs              : dxvk_so_version,
)

glide_common_dep = declare_dependency(
  link_with           : [ glide_common_lib ],
  include_directories : [ dxvk_include_path ],
)

if get_option('enable_glide1x')
  glide1x_ld_args      = []
  glide1x_link_depends = []
  glide1x_extra_dep    = []
  glide1x_def          = ''

  if dxvk_is_msvc
    glide1x_def = 'glide1x_msvc'+def_spec_ext
  else
    glide1x_def = 'glide1x'+def_spec_ext
  endif

  if platform == 'windows'
    glide1x_extra_dep    += [ cpp.find_library('version') ]
  else
    glide1x_ld_args      += [ '-Wl,--version-script', join_paths(meson.current_source_dir(), 'glide1x.sym') ]
    glide1x_link_depends += files('glide1x.sym')
  endif

  glide1x_dll = shared_library(dxvk_name_prefix+'glide', glide1x_src, glide1x_res,
    dependencies        : [ dxbc_spirv_dep, dxvk_dep, glide1x_extra_dep, glide_common_dep ],
    include_directories : [ dxvk_include_path ],
    install             : true,
    vs_module_defs      : glide1x_def,
    link_args           : glide1x_ld_args,
    link_depends        : [ glide1x_link_depends ],
    kwargs              : dxvk_so_version,
  )

  glide1x_dep = declare_dependency(
    link_with           : [ glide1x_dll ],
    include_directories : [ dxvk_include_path ],
  )

  if platform != 'windows'
    pkg.generate(glide1x_dll,
      filebase: dxvk_pkg_prefix + 'glide1x',
      subdirs:  'dxvk',
    )
  endif
endif

if get_option('enable_glide2x')
  glide2x_ld_args      = []
  glide2x_link_depends = []
  glide2x_extra_dep    = []
  glide2x_def          = ''

  if dxvk_is_msvc
    glide2x_def = 'glide2x_msvc'+def_spec_ext
  else
    glide2x_def = 'glide2x'+def_spec_ext
  endif

  if platform == 'windows'
    glide2x_extra_dep    += [ cpp.find_library('version') ]
  else
    glide2x_ld_args      += [ '-Wl,--version-script', join_paths(meson.current_source_dir(), 'glide2x.sym') ]
    glide2x_link_depends += files('glide2x.sym')
  endif

  glide2x_dll = shared_library(dxvk_name_prefix+'glide2x', glide2x_src, glide2x_res,
    dependencies        : [ dxbc_spirv_dep, dxvk_dep, glide2x_extra_dep, glide_common_dep ],
    include_directories : [ dxvk_include_path ],
    install             : true,
    vs_module_defs      : glide2x_def,
    link_args           : glide2x_ld_args,
    link_depends        : [ glide2x_link_depends ],
    kwargs              : dxvk_so_version,
  )

  glide2x_dep = declare_dependency(
    link_with           : [ glide2x_dll ],
    include_directories : [ dxvk_include_path ],
  )

  if platform != 'windows'
    pkg.generate(glide2x_dll,
      filebase: dxvk_pkg_prefix + 'glide2x',
      subdirs:  'dxvk',
    )
  endif
endif

if get_option('enable_glide3x')
  glide3x_ld_args      = []
  glide3x_link_depends = []
  glide3x_extra_dep    = []
  glide3x_def          = ''

  if dxvk_is_msvc
    glide3x_def = 'glide3x_msvc'+def_spec_ext
  else
    glide3x_def = 'glide3x'+def_spec_ext
  endif

  if platform == 'windows'
    glide3x_extra_dep    += [ cpp.find_library('version') ]
  else
    glide3x_ld_args      += [ '-Wl,--version-script', join_paths(meson.current_source_dir(), 'glide3x.sym') ]
    glide3x_link_depends += files('glide3x.sym')
  endif

  glide3x_dll = shared_library(dxvk_name_prefix+'glide3x', glide3x_src, glide3x_res,
    dependencies        : [ dxbc_spirv_dep, dxvk_dep, glide2x_extra_dep, glide_common_dep ],
    include_directories : [ dxvk_include_path ],
    install             : true,
    vs_module_defs      : glide3x_def,
    link_args           : glide3x_ld_args,
    link_depends        : [ glide3x_link_depends ],
    kwargs              : dxvk_so_version,
  )

  glide3x_dep = declare_dependency(
    link_with           : [ glide3x_dll ],
    include_directories : [ dxvk_include_path ],
  )

  if platform != 'windows'
    pkg.generate(glide3x_dll,
      filebase: dxvk_pkg_prefix + 'glide3x',
      subdirs:  'dxvk',
    )
  endif
endif
