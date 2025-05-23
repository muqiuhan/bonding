add_requires("plog")

target("logging")
  set_kind("static")
  set_languages("c++23")
  set_warnings("all")
  set_optimize("smallest")
  set_warnings("all", "error")
  add_files("*.cpp")
  add_includedirs("private")
  add_includedirs("public", { public = true })
  add_packages("plog")