fips_begin_app(AssetServer windowed)
    fips_src(. GROUP_FOLDERS)
    fips_resources(${FIPS_PROJECT_DIR}/work/assets)
    fips_deps(LauncherBase)
	point_proj(NAME AssetServer)
fips_end_app()