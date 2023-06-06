(lang dune 3.7)

(name bonding)

(generate_opam_files true)

(source
 (github muqiuhan/bonding))

(authors "Muqiu Han")

(maintainers "Muqiu Han")

(license MIT)

(documentation https://github.com/muqiuhan/bonding)

(package
 (name bonding)
 (synopsis "A simple container written in OCaml")
 (description "A simple container written in OCaml")
 (depends ocaml dune)
 (tags
  (container docker)))

; See the complete stanza docs at https://dune.readthedocs.io/en/stable/dune-files.html#dune-project
