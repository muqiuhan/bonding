open Core
open Bonding_lib

(** get the args from the commandline and handle everything from the struct Container creation to the exit. *)
let start (args : Cli.t) =

    Result.(
      Container.check_linux_version ()
      >>= (fun _ -> Container.make args)
      >>= Container.create
      >>= Container.start
      >>= Container.clean_exit)

let _ =
    Cli.{debug = true; command = "/bin/bash"; uid = 0; mount_dir = "./mountdir"}
    |> start
