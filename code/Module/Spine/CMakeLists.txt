fips_begin_lib(Spine)
    fips_include_directories(.)
    fips_src(. GROUP_FOLDERS)
    fips_deps(AzCore)
fips_end_lib()