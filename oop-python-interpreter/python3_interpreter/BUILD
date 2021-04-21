#load(":build.bzl","src_gen")
COPTS=['-Ipython3_interpreter/include/parser','-Ipython3_interpreter/include/common','-Ipython3_interpreter/include/pgen','-Ipython3_interpreter/include/asdl','-Ipython3_interpreter/include/python','-Ipython3_interpreter/include/object']
INCLUDES=glob(['include'])

#src_gen(
#    name="pgensrc",
#    srcs=['Grammar/Grammar'],
#    deps=[':pgenmain'],
#    executable=':pgenmain',
#    inputs=['Grammar/Grammar'],
#    #outputs=['include/pgen/graminit.h','src/pgen/graminit.cc']
#)
cc_library(
    name='opt',
    includes=INCLUDES,
    hdrs=glob([i+'/**/*.h' for i in INCLUDES]),
)
cc_binary(
    name='main',
    srcs=['src/main.cc'],
    deps=[':common',':parser',':pgen',':python',':object'],
    copts=COPTS
)
cc_binary(
    name='asdlmain',
    srcs=['src/asdlmain.cc'],
    deps=[':common',':parser',':pgen',':asdl'],
    copts=COPTS
)
cc_binary(
    name='pgenmain',
    srcs=['src/pgenmain.cc'],
    deps=[':common',':parser'],
    copts=COPTS
)
cc_library(
    name='common',
    srcs=[],
    hdrs=glob(['include/common/*.h']),
    copts=COPTS
)
cc_library(
    name='pgen',
    srcs=glob(['src/pgen/*.cc']),
    hdrs=glob(['include/pgen/*.h']),
    deps=[':common',':parser'],
    copts=COPTS
)
cc_library(
    name='python',
    srcs=glob(['src/python/*.cc']),
    hdrs=glob(['include/python/*.h']),
    deps=[':common',':pgen',':parser',':object'],
    copts=COPTS
)
cc_library(
    name='parser',
    srcs=glob(['src/parser/*.cc']),
    hdrs=glob(['include/parser/*.h']),
    deps=[':common'],
    copts=COPTS
)
cc_library(
    name='object',
    srcs=glob(['src/object/*.cc']),
    hdrs=glob(['include/object/*.h']),
    deps=[':common'],
    copts=COPTS
)
cc_library(
    name='asdl',
    srcs=glob(['src/asdl/*.cc']),
    hdrs=glob(['include/asdl/*.h']),
    deps=[':common'],
    copts=COPTS
)
