open Core
open Bonding_lib

(** get the args from the commandline and handle everything from the struct Container creation to the exit. *)
let start (args : Cli.t) =
    Result.(Container.make args >>= Container.create >>= Container.clean_exit)

let _ =
    Result.iter
      ~f:(fun _ -> Caml_threads.Thread.delay 2.)
      (Cli.
         {
           debug = true;
           command = "/bin/bash";
           uid = 0;
           mount_dir = "./mountdir";
         }
      |> start)
